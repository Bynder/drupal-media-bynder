<?php

/**
 * @file
 * Functions for administration of the module
 *
 * Callback for /admin/configure/media/media_bynder.
 */

include_once 'BynderRestClient.inc';

/**
 * Create a form to add media from Bynder.
 */
function media_bynder_add($form, &$form_state = array(), $redirect = NULL) {
    module_load_include('inc', 'media', 'includes/media.browser');

    $form['#attached']['library'][] = array('media_bynder', 'media_bynder_administration');

    $form['#attached']['css'][] = array(
        'type' => 'external',
        'data' => '//maxcdn.bootstrapcdn.com/font-awesome/4.1.0/css/font-awesome.css',
    );

    drupal_process_attached($form);

    $id = media_bynder_parameter($_POST, 'id');
    if ($id !== NULL) {
        media_bynder_submit_mob($form, $form_state);
        return;
    }

    $data['bynder_search'] = media_bynder_generate_search($form_state);

    return array('bynder' => $data);
}

/**
 * Generate search page.
 */
function media_bynder_generate_search($state) {
    $data = array('#type' => 'fieldset');

    $rest_client = media_bynder_rest_client();
    $meta_properties = $rest_client->getMetaProperties();

    $brands = media_bynder_brands();
    $search = media_bynder_search_string($state);
    $selected_facets = media_bynder_selected_facets($state);
    $results = media_bynder_search_results($search, $selected_facets, $meta_properties);
    $available_facets = $results['count'];

    foreach($meta_properties as $meta_property) {
        $available_facets[$meta_property['name']] = [];
        foreach($meta_property['options'] as $meta_property_option) {
            $available_facets[$meta_property['name']][$meta_property_option['name']] = '-';
        }
    }

    $selected_facets = media_bynder_remove_hidden_facets($available_facets, $selected_facets);

    $data['search'] = array(
        '#type' => 'textfield',
        '#title' => t('Search'),
        '#default_value' => $search !== NULL ? $search : '',
        '#description' => "<p class='alert alert-warning' style='display:none'><span class='text'></span></p>",
    );

    $data['facets'] = media_bynder_generate_facets($available_facets, $selected_facets, $brands, $meta_properties);

    $data['button'] = array(
        '#type' => 'button',
        '#value' => t('Search'),
        '#submit' => array(
            'media_bynder_add_test'
        )
    );

    $data['results'] = media_bynder_generate_results($results);

    if (isset($results['totalNrOfResults'])) {
        $data['total_results'] = array(
            '#type' => 'html_tag',
            '#tag' => 'h3',
            '#attributes' => array(
                'class' => 'total_results'
            ),
            '#value' => t('Total Results: ') . $results['totalNrOfResults']
        );
        $data['pager'] = media_bynder_generate_pager(
            $results['totalNrOfResults'], 0, array('search' => $search)
        );
    }

    return $data;
}

/**
 * Retrieve parameter from container.
 */
function media_bynder_parameter($container, $key, $default = NULL) {
    if (isset($container[$key])) {
        return $container[$key];
    }
    return $default;
}

/**
 * Return entered search string.
 */
function media_bynder_search_string($state) {
    $search = NULL;

    if (isset($state['values']['search'])) {
        $search = $state['values']['search'];
    } elseif (isset($_GET['search'])) {
        $search = check_plain(filter_xss($_GET['search']));
    }
    return $search;
}

/**
 * Return selected facets.
 */
function media_bynder_selected_facets($state) {
    $selected_facets = array();

    if (isset($state['values']['search'])) {
        $selected_facets = array_merge($selected_facets, array(
            "keyword" => (array) $state['values']['search']));
    }

    if (isset($_GET['brandId'])) {
        $selected_facets = array_merge($selected_facets, array(
            "brandId" => (array) $_GET['brandId']));
    }
    if (isset($_GET['subBrandId'])) {
        $selected_facets = array_merge($selected_facets, array(
            "subBrandId" => (array) $_GET['subBrandId']));
    }
    if (isset($_GET['tags'])) {
        $selected_facets = array_merge($selected_facets, array(
            "tags" => (array) $_GET['tags']));
    }
    if (isset($_GET['type'])) {
        $selected_facets = array_merge($selected_facets, array(
            "type" => (array) $_GET['type']));
    }
    if (isset($_GET['orientation'])) {
        $selected_facets = array_merge($selected_facets, array(
            "orientation" => (array) $_GET['orientation']));
    }
    foreach($_GET as $query_param_key => $query_param){
        if(!isset($selected_facets[$query_param_key])){
            $selected_facets = array_merge($selected_facets, array(
            $query_param_key => (array) $query_param));
        }
    }

    return $selected_facets;
}

/**
 * Search with selected facets and a search term.
 */
function media_bynder_search_facets($selected_facets, $search_term) {
    $full_query = "";
    foreach ($selected_facets as $selected_facet_group_name => $selected_facet_group) {
        $full_query .= "&" . $selected_facet_group_name . '=' . implode(",", $selected_facet_group);
    }

    if (isset($search_term) && strlen($search_term) > 0) {
        if (strlen($full_query) > 0) {
            $full_query .= ' ';
        }
        $full_query .= '&keyword=' . $search_term;
    }

    $rest_client = media_bynder_rest_client();
    $arr_facets_json = $rest_client->getFacets($full_query);

    if ($arr_facets_json) {
        return $arr_facets_json;
    } else {
        drupal_set_message($rest_client->error, 'error');
        return FALSE;
    }
}

/**
 * Parse the selected facets.
 */
function media_bynder_parse_selected_facets($selected_facets) {
    if (!$selected_facets) {
        return FALSE;
    }

    if (!empty($selected_facets)) {
        $selected_facet_list = array(
            '#theme' => 'item_list',
            '#type' => 'ul',
            '#title' => t('Selected Facets'),
            '#attributes' => array(
                'class' => 'selected_facet_list'
            )
        );

        foreach ($selected_facets as $facet_category => $facets) {
            foreach($facets as $facet){
                $selected_facet_list['#items'][] = $facet_category . ' => ' . $facet;
            }
        }

        return $selected_facet_list;
    }

    return FALSE;
}

/**
 * Generate facets.
 */
function media_bynder_generate_facets($facets, $selected_facets, $brands, $meta_properties) {
    if (empty($facets)) {
        return array();
    }

    $facets_list = array(
        '#theme' => 'item_list',
        '#type' => 'ul',
        '#title' => t('Facets'),
        '#attributes' => array(
            'class' => 'facet_list'
        )
    );

    $meta_property_labels = [];
    $meta_property_icons = [];
    foreach($meta_properties as $meta_property){
        $meta_property_labels[$meta_property['name']] = $meta_property['label'];
        $meta_property_icons[$meta_property['name']] = 'caret-right';
    }

    foreach ($facets as $facet_category_name => $facet_category) {
        if($facet_category_name == 'categoryId'){
            continue;
        }
        $children = media_bynder_generate_facets_link($facet_category, $facet_category_name, $selected_facets, $brands, $meta_properties);
        $labels = array(
            'brandId' => t('Brands'),
            'subBrandId' => t('Sub-brands'),
            'tags' => t('Tags'),
            'type' => t('Type'),
            'orientation' => t('Orientation')
        );
        $labels = array_merge($meta_property_labels, $labels);
        $icons = array(
            'brandId' => 'archive',
            'subBrandId' => 'briefcase',
            'tags' => 'tags',
            'type' => 'file',
            'orientation' => 'image'
        );
        $icons = array_merge($meta_property_icons, $icons);
        $facet_category = array(
            'data' => "<span><i class='fa fa-" . filter_xss($icons[$facet_category_name]) . "'></i> " . filter_xss($labels[$facet_category_name]) . "<span class='expand'><i class='fa fa-angle-down'></i></span></span>",
            'id' => $facet_category_name,
            'class' => array(
                'facet_title'
            ),
            'children' => $children
        );
        if(count($children)){
            $facets_list['#items'][] = $facet_category;
        }
    }
    return $facets_list;
}

/**
 * Generate links for facets.
 */
function media_bynder_generate_facets_link($facet_category, $facet_category_name, $selected_facets, $brands, $meta_properties) {
    $facet_links = array();

    $meta_property_labels = [];
    foreach($meta_properties as $meta_property) {
        foreach($meta_property['options'] as $meta_property_option) {
            $meta_property_labels[$meta_property_option['name']] = $meta_property_option['label'];
        }
    }

    foreach ($facet_category as $facet_id => $facet_count) {
        if($facet_category_name == 'brandId'){
            $facet_name = media_bynder_get_brand_name($brands, $facet_id);
        }elseif(isset($meta_property_labels[$facet_id])){
            $facet_name = $meta_property_labels[$facet_id];
        }else{
            $facet_name = $facet_id;
        }
        $activate_href_link = media_bynder_query_facet_link($facet_category_name, $facet_id, $selected_facets);
        $deactivate_href_link = media_bynder_query_facet_link($facet_category_name, $facet_id, $selected_facets, true);
        $active = media_bynder_facet_active($selected_facets, array($facet_category_name => array($facet_id)));
        $active_class = $active ? "active" : "";
        $href_link = ($active)? $deactivate_href_link : $activate_href_link;
        $link = "";
        $link .= '<a class="' . filter_xss($active_class) . '" href="?' . filter_xss($href_link) . '">' . filter_xss($facet_name) . '</a>';
        $link .= '<span class="count">(' . filter_xss($facet_count) . ')</span>';
        $facet_links[] = $link;
    }
    return $facet_links;
}

function media_bynder_get_brand_name($brands, $brand_id) {
    foreach($brands as $brand){
        if($brand['id'] == $brand_id){
            return $brand['name'];
        }
    }
    return "";
}

function media_bynder_query_facet_link($facet, $facet_value, $selected_facets, $deactivate=false){
    if(!$deactivate){
        $selected_facets = array_merge_recursive(
            array($facet => array($facet_value)),
            $selected_facets
        );
    }else{
        if(isset($selected_facets[$facet])){
            $selected_facets[$facet] = array_diff(explode(',', $selected_facets[$facet][0]), array($facet_value));
        }
    }
    $csv_facets = array();
    foreach(array_filter($selected_facets) as $selected_facet_key => $selected_facet){
        $csv_facets[$selected_facet_key] = implode(',', $selected_facet);
    }
    $value = http_build_query($csv_facets);
    return $value;
}

function media_bynder_facet_active($selected_facets, $current_facet){
    if(isset($selected_facets[key($current_facet)])){
        return in_array($current_facet[key($current_facet)][0],
                        explode(',', $selected_facets[key($current_facet)][0]));
    }else{
        return false;
    }
}

/**
 * Parse the facet link.
 */
function media_bynder_parse_facet_link($query) {
    $pattern = '/\+\([A-Za-z0-9 ]*\)/';
    preg_match($pattern, $query, $matches);
    $query = urlencode($query);
    if (!empty($matches)) {
        $query = $query . '&search=' . substr(trim($matches[0]), 2, -1);
    }

    return $query;
}

function media_bynder_remove_hidden_facets($available_facets, $selected_facets) {
    foreach($selected_facets as $selected_facet_key => $selected_facet){
        $value = $selected_facet[0];
        $isset = isset($available_facets[$selected_facet_key][$value]);
        if(!$isset){
            unset($selected_facets[$selected_facet_key]);
        }
    }
    return $selected_facets;
}

/**
 * Get brands.
 */
function media_bynder_brands() {
    $rest_client = media_bynder_rest_client();
    $result = $rest_client->brands();

    if ($result) {
        $arr_json = json_decode($result, TRUE);
    } else {
        drupal_set_message(t('Could not retrieve search results:') . filter_xss($rest_client->error), 'error');
    }

    return $arr_json;
}

/**
 * Perform the search.
 *
 * @param unknown $search
 *   the query entered.
 * @param unknown $selected_facets
 *   the selected facets.
 *
 * @return mixed
 *   the search results.
 */
function media_bynder_search_results($search, $selected_facets, $meta_properties) {
    $start_index = 0;
    $nr_of_results = 25;
    $query = '';

    if (isset($_GET['page'])) {
        $current_page = (int) filter_xss($_GET['page']);
        $start_index = $current_page * $nr_of_results;
    }
    $selected_facets_meta_properties = [];
    foreach($meta_properties as $meta_property){
        foreach($meta_property['options'] as $meta_property_option){
            $selected_facets_meta_properties[$meta_property['name']][$meta_property_option['name']] = $meta_property_option['id'];
        }
    }

    if (isset($selected_facets)) {
        foreach ($selected_facets as $facet_category => $facets) {
            if(isset($selected_facets_meta_properties[$facet_category])){
                $meta_facets = [];
                foreach(explode(',', $facets[0]) as $facet){
                    $meta_facets[] = $selected_facets_meta_properties[$facet_category][$facet];
                }
                $query .= '&propertyOptionId=' . implode(',', $meta_facets);
            }else{
                if($facet_category != 'q'){
                    $query .= '&' . $facet_category . '=' . implode(',', $facets);
                }
            }
        }
    }

    $rest_client = media_bynder_rest_client();
    $result = $rest_client->search($query, $start_index, $nr_of_results);

    if ($result) {
        $arr_json = json_decode($result, TRUE);

        //Set total count;
        if(isset($arr_json['count']['brandId'])){
            $total_count = 0;
            foreach($arr_json['count']['brandId'] as $brand_count){
                $total_count += $brand_count;
            }
            $arr_json['totalNrOfResults'] = $total_count;
        }
    } else {
        drupal_set_message(t('Could not retrieve search results:') . filter_xss($rest_client->error), 'error');
    }

    return $arr_json;
}

/**
 * Organize results from search engine to visualize them.
 */
function media_bynder_generate_results($results) {
    if (isset($results['totalNrOfResults'])) {
        unset($results['totalNrOfResults']);
    }

    if (isset($results['startIndex'])) {
        unset($results['startIndex']);
    }

    $results = $results['media'];

    $output = array(
        '#theme' => 'item_list',
        '#type' => 'ul',
        '#attributes' => array(
            'class' => 'result_list'
        )
    );

    if (!empty($results)) {
        foreach ($results as $media_item) {
            $output['#items'][] = media_bynder_generate_result_current($media_item);
        }
    } else {
        return FALSE;
    }

    return $output;
}

/**
 * Show search result.
 */
function media_bynder_generate_result_current($media_item) {
    if (!isset($media_item['thumbnails']['thul'])) {
        return FALSE;
    }

    $variables = array();
    $variables['path'] = $media_item['thumbnails']['thul'];

    return l('<div class="image-holder"><img src="' . $media_item['thumbnails']['thul'] . '"/></div>
      <span class="media_object_type_' . $media_item['type'] . '">' . t($media_item['name']) . '</span>', 'admin/content/media/add/media_bynder/', array(
        'attributes' => array(
            'class' => 'bynder-image',
            'rel' => $media_item['id'],
            'data-id' => $media_item['id'],
            'data-idHash' => $media_item['idHash'] //fragmentId
        ),
        'html' => TRUE,
        'query' => array(
            'id' => $media_item['id'], //fragmentId
            'idHash' => $media_item['idHash'] //fragmentId
        )
    ));
}

/**
 * Generate paging functionality.
 */
function media_bynder_generate_pager($num_results, $element = 0, $parameters = array()) {
    if (isset($_POST['search'])) {
        $_GET['search'] = filter_xss(check_plain($_POST['search']));
    }

    pager_default_initialize($num_results, 25, $element);
    return array(
        '#markup' => theme('pager', array(
            'parameters' => $parameters,
            'element' => $element
        ))
    );
}


/**
 * Submit the selected media object.
 */
function media_bynder_submit_mob(&$form, &$form_state) {
    $id = media_bynder_parameter($_POST, 'id');
    $idHash = media_bynder_parameter($_POST, 'idHash');

    $redirect = 'media_bynder_add';

    $rest_client = media_bynder_rest_client();
    $result = $rest_client->getMediaObject($id);

    if ($result) {
        $arr_json = json_decode($result, TRUE);
    } else {
        echo '{"success": 0, "type": "error", "message": "' . $rest_client->error . '"}';
        exit;
    }

    $uri = file_stream_wrapper_uri_normalize("bynder://f/$id/i/$idHash");
    $file = file_uri_to_object($uri, TRUE);

    if ($arr_json['type'] == 'image') {
        $file->filemime = 'image/jpeg';
    }

    $file->filesize = @filesize($uri);
    $file->filename = $arr_json['name'];

    if ($file === FALSE) {
        echo '{"success": 0, "type": "error", "message": "' . t('An error occurred and no file was added to your Library.') . '"}';
    } elseif (!property_exists($file, 'fid')) {
        $file = file_save($file);
        field_attach_submit('media', $file, $form, $form_state);
        $file->file = array();
        $file->file[LANGUAGE_NONE] = array();
        $file->file[LANGUAGE_NONE][0] = (array) $file + array(
            'display' => TRUE
        );
        $file->is_new = TRUE;
        field_attach_insert('media', $file);
        echo '{"success": 1, "type": "success", "message": "' . t('Successfully added media to Library') . '"}';
    } else {
        echo '{"success": 0, "type": "warning", "message": "' . t('Already exists in Library') . '"}';
    }
    exit;
}

/**
 * Page callback: Current posts settings.
 *
 * @see current_posts_menu()
 */
function media_bynder_form($form, &$form_state) {
    $form['media_bynder_settings'] = array(
        '#type' => 'fieldset',
        '#title' => t('Bynder API'),
        '#description' => t('You will first need to apply for an API Developer Key'),
        '#collapsible' => TRUE
    );

    $form['media_bynder_settings']['media_bynder_url'] = array(
        '#type' => 'textfield',
        '#title' => t('Bynder url'),
        '#default_value' => variable_get('media_bynder_url', ''),
        '#size' => 50,
        '#description' => t('The Bynder url.'),
        '#required' => TRUE
    );
    $form['media_bynder_settings']['media_bynder_oauth_consumer'] = array(
        '#type' => 'textfield',
        '#title' => t('OAuth Consumer Token'),
        '#default_value' => variable_get('media_bynder_oauth_consumer', ''),
        '#size' => 50,
        '#description' => t('The consumer token to be used by the module to communicate with Bynder.'),
        '#required' => TRUE
    );
    $form['media_bynder_settings']['media_bynder_oauth_consumer_secret'] = array(
        '#type' => 'textfield',
        '#title' => t('OAuth Consumer Secret'),
        '#default_value' => variable_get('media_bynder_oauth_consumer_secret', ''),
        '#size' => 50,
        '#description' => t('The consumer secret to be used by the module to communicate with Bynder.'),
        '#required' => TRUE
    );

    $form['media_bynder_settings']['media_bynder_oauth_token'] = array(
        '#type' => 'textfield',
        '#title' => t('OAuth Token'),
        '#default_value' => variable_get('media_bynder_oauth_token', ''),
        '#size' => 50,
        '#description' => t('The token to be used by the module to communicate with Bynder.'),
        '#required' => TRUE
    );

    $form['media_bynder_settings']['media_bynder_oauth_token_secret'] = array(
        '#type' => 'textfield',
        '#title' => t('OAuth Token Secret'),
        '#default_value' => variable_get('media_bynder_oauth_token_secret', ''),
        '#size' => 50,
        '#description' => t('The token secret to be used by the module to communicate with Bynder.'),
        '#required' => TRUE
    );

    return system_settings_form($form);
}

/**
 * Implements validation from the Form API.
 */
function media_bynder_form_validate($form, &$form_state) {
    if ($form_state['values']['media_bynder_url']) {
        $form_state['values']['media_bynder_url'] = trim($form_state['values']['media_bynder_url']);
        if (!media_bynder_validate_url($form_state['values']['media_bynder_url'])) {
            form_set_error('media_bynder_url', t('Bynder url is not a valid url.'));
        }
    }
}

/**
 * Validations
 */
function media_bynder_validate_url($url) {
    return filter_var($url, FILTER_VALIDATE_URL);
}
