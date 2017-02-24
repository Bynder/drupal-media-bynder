<?php

/**
 * @file
 * Manages the Bynder media browser functionality
 */

const BYNDER_SEARCH_PAGE_SIZE = 10;

/**
 * Create a form to add media from Bynder.
 */
function media_bynder_add($form, &$form_state = array(), $redirect = null)
{
    $form['#attached']['library'][] = array('media_bynder', 'media_bynder_administration');
    $form['#attached']['css'][] = array(
        'type' => 'external',
        'data' => '//maxcdn.bootstrapcdn.com/font-awesome/4.1.0/css/font-awesome.css',
    );
    drupal_process_attached($form);

    $id = media_bynder_parameter($_POST, 'id');

    if ($id !== null) {
        media_bynder_submit_mob($form, $form_state);
        return;
    }

    $data['bynder_search'] = media_bynder_generate_search($form_state);
    $data['selected_asset'] = array(
        '#type' => 'hidden',
        '#default_value' => false
    );

    $selected_asset = media_bynder_parameter($_POST, 'selected_asset');
    if ($selected_asset) {
        $id = media_bynder_id_from_uri($selected_asset);
        $result = media_bynder_save_image($id, $form);
        if (!$result['success']) {
            drupal_set_message(t($result['message']), 'error');
        } else {
            drupal_goto('/media/browser', array(
                'query' => array(
                    'render' => 'media-popup',
                    'fid' => $result['fid']
                )
            ));
        }
    }
    return array('bynder' => $data);
}

function media_bynder_save_image($id, $form = null)
{
    $selected_media = media_bynder_get_object_by_id($id);
    $idHash = $selected_media['idHash'];

    $uri = file_stream_wrapper_uri_normalize("bynder://f/$id/i/$idHash");
    $file = file_uri_to_object($uri, true);

    if ($selected_media['type'] == 'image') {
        $file->filemime = 'image/jpeg';
    }
    $file->filesize = 0;
    $file->filename = $selected_media['name'];

    if ($file === false) {
        return array('success' => 0, 'message' => 'An error occurred and no file was added to your Library.');
    } elseif (!property_exists($file, 'fid')) {
        $file = file_save($file);
        field_attach_submit('media', $file, $form, $form_state);
        $file->file = array();
        $file->file[LANGUAGE_NONE] = array();
        $file->file[LANGUAGE_NONE][0] = (array)$file + array(
                'display' => true
            );
        $file->is_new = true;
        field_attach_insert('media', $file);
        return array('success' => 1, 'message' => 'Successfully added media to Library', 'fid' => $file->fid);
    } else {
        return array('success' => 1, 'message' => 'Already exists in Library', 'fid' => $file->fid);
    }
}

/**
 * Generate search page.
 */
function media_bynder_generate_search($state)
{
    global $filter_labels;
    global $meta_property_prefix;

    $meta_property_prefix = 'property_';

    $data = array('#type' => 'fieldset');

    $bynderApi = media_bynder_init_api();
    $assetBank = $bynderApi->getAssetBankManager();
    $meta_properties = $assetBank->getMetaProperties()->wait();

    $filter_labels = array(
        'brandId' => t('Brands'),
        'tags' => t('Tags'),
        'type' => t('Type'),
        'orientation' => t('Orientation')
    );
    foreach ($meta_properties as $meta_property) {
        $filter_labels[$meta_property_prefix . $meta_property['name']] = $meta_property['label'];
        foreach ($meta_property['options'] as $meta_property_option) {
            $filter_labels[$meta_property_option['name']] = $meta_property_option['label'];
        }
    }

    $brands = media_bynder_brands();
    $search = media_bynder_search_string($state);
    $filters = media_bynder_filters_json($state);

    if (isset($_POST['filters'])) {
        $filters = json_decode(filter_xss($_POST['filters']))->filters;
    }

    if (isset($_POST['source_filter'])) {
        if ($_POST['source_filter'] == 'bynder_remove_all_facets_filter_key') {
            $filters = array();
        }
    }

    $selected_facets = media_bynder_selected_facets($state);
    $results = media_bynder_search_results($search, $filters, $meta_properties, BYNDER_SEARCH_PAGE_SIZE);
    $available_facets = $results['count'];

    // Create search section
    $data['search'] = array(
        '#type' => 'fieldset',
        '#title' => t('Search'),
        '#collapsible' => false,
        '#collapsed' => false,
        '#attributes' => array(
            'class' => array(
                'bynder-search-form',
            ),
        ),
    );
    $data['search']['field'] = array(
        '#type' => 'textfield',
        '#default_value' => isset($search) ? $search : '',
        '#description' => "<p class='alert alert-warning' style='display:none'><span class='text'></span></p>",
    );
    $data['search']['button'] = array(
        '#type' => 'button',
        '#value' => t('Search'),
        '#submit' => array(
            'media_bynder_add_test'
        )
    );

    $data['selected_facets'] = media_bynder_generate_selected_facets($filters);

    $data['facets'] = media_bynder_generate_facets($available_facets, $selected_facets, $brands, $meta_properties);

    $data['filters'] = array(
        '#type' => 'hidden',
        '#default_value' => (json_encode($filters) != '[]') ? json_encode($filters) : '{"filters": []}'
    );

    // Create the results section
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
function media_bynder_parameter($container, $key, $default = null)
{
    if (isset($container[$key])) {
        return $container[$key];
    }
    return $default;
}

/**
 * Return entered search string.
 */
function media_bynder_search_string($state)
{
    $search = null;

    if (isset($state['values']['field'])) {
        $search = $state['values']['field'];
    } elseif (isset($_GET['search'])) {
        $search = check_plain(filter_xss($_GET['search']));
    }
    return $search;
}

/**
 * Return entered filters.
 */
function media_bynder_filters_json($state)
{
    $filters = array();

    if (isset($state['values']['filters'])) {
        $filters_json = json_decode($state['values']['filters']);
    }
    if (isset($filters_json->filters)) {
        $filters = $filters_json->filters;
    }
    return $filters;
}

/**
 * Return selected facets.
 */
function media_bynder_selected_facets($state)
{
    $selected_facets = array();

    if (isset($state['values']['search'])) {
        $selected_facets = array_merge($selected_facets, array(
            "keyword" => (array)$state['values']['search']
        ));
    }

    if (isset($state['values']['filters'])) {
        $filters_json = json_decode($state['values']['filters']);
        $filters = $filters_json->filters;

        foreach ($filters as $filter) {
            $selected_facets = array_merge($selected_facets, array(
                $filter->key => $filter->value
            ));
        }
    }

    return $selected_facets;
}

/**
 * Search with selected facets and a search term.
 */
function media_bynder_search_facets($selected_facets, $search_term)
{
    $query = array();
    foreach ($selected_facets as $selected_facet_group_name => $selected_facet_group) {
        $query[$selected_facet_group_name] = implode(",", $selected_facet_group);
    }

    if (isset($search) && strlen($search) > 0) {
        $query['keyword'] = urlencode($search_term);
    }
    $query['count'] = 1;

    $bynderApi = media_bynder_init_api();
    $assetBank = $bynderApi->getAssetBankManager();
    $assets = $assetBank->getMediaList($query)->wait();

    $arr_facets_json = $assets['count'];

    if ($arr_facets_json) {
        return $arr_facets_json;
    } else {
        drupal_set_message("Error fetching media", 'error');
        return false;
    }
}

/**
 * Parse the selected facets.
 */
function media_bynder_parse_selected_facets($selected_facets)
{
    if (!$selected_facets) {
        return false;
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
            foreach ($facets as $facet) {
                $selected_facet_list['#items'][] = $facet_category . ' => ' . $facet;
            }
        }

        return $selected_facet_list;
    }

    return false;
}

function media_bynder_generate_selected_facets($selected_facets)
{
    global $filter_labels;
    $brands = media_bynder_brands();
    if (empty($selected_facets)) {
        return array();
    }

    $facets_list = array(
        '#theme' => 'item_list',
        '#type' => 'ul',
        '#title' => t('Selected Facets'),
        '#attributes' => array(
            'class' => 'selected_facet_list facet_list'
        )
    );
    foreach ($selected_facets as $selected_facet) {
        $selected_facet_key = $selected_facet->key;
        $selected_facet_value = $selected_facet->value;

        if ($selected_facet_key == 'brandId') {
            $facet_name = media_bynder_get_brand_name($brands, $selected_facet_value);
        } else {
            $facet_name = $selected_facet_value;
        }
        if (!isset($filter_labels[$selected_facet_key])) {
            $filter_labels[$selected_facet_key] = t($selected_facet_key);
        }

        $facet_category = array(
            'data' => "<span>" . filter_xss($facet_name) . "<span class='expand'><i class='fa fa-times'></i></span></span>",
            'class' => array(
                'facet_title'
            ),
            'data-filter-key' => filter_xss($selected_facet->key)
        );
        $facets_list['#items'][] = $facet_category;
    }

    $facet_remove_all = array(
        'data' => "<span id='clear-all'>Clear facets<span class='expand'><i class='fa fa-times'></i></span></span>",
        'class' => array(
            'facet_title'
        ),
        'data-filter-key' => 'bynder_remove_all_facets_filter_key'
    );

    $facets_list['#items'][] = $facet_remove_all;

    return $facets_list;
}

/**
 * Generate facets.
 */
function media_bynder_generate_facets($facets, $selected_facets, $brands)
{
    global $filter_labels;
    if (empty($facets)) {
        return array();
    }

    $facets_list = array(
        '#theme' => 'item_list',
        '#type' => 'ul',
        '#title' => t('Facets'),
        '#prefix' => '<div class="normal-facet-list-wrapper">',
        '#suffix' => '</div>',
        '#attributes' => array(
            'class' => 'normal_facet_list facet_list'
        )
    );

    foreach ($facets as $facet_category_name => $facet_category) {
        if ($facet_category_name == 'categoryId' ||
            $facet_category_name == 'subBrandId'
        ) {
            continue;
        }
        //Hide single item filters.
        if (count($facet_category) < 2) {
            continue;
        }

        if (!isset($filter_labels[$facet_category_name])) {
            $filter_labels[$facet_category_name] = t($facet_category_name);
        }
        $icons = array(
            'brandId' => 'archive',
            'tags' => 'tags',
            'type' => 'file',
            'orientation' => 'image'
        );
        $children = media_bynder_generate_facets_link($facet_category, $facet_category_name, $selected_facets, $brands);
        $facet_category = array(
            'data' => "<span><i class='fa fa-" . filter_xss(isset($icons[$facet_category_name]) ? $icons[$facet_category_name] : 'caret-right') . "'></i> " . filter_xss($filter_labels[$facet_category_name]) . "<span class='expand'><i class='fa fa-angle-down'></i></span></span>",
            'id' => preg_replace('/\s+/', '-', $facet_category_name),
            'class' => array(
                'facet_title'
            ),
            'children' => $children
        );
        if (count($children)) {
            $facets_list['#items'][] = $facet_category;
        }
    }
    return $facets_list;
}

/**
 * Generate links for facets.
 */
function media_bynder_generate_facets_link($facet_category, $facet_category_name, $selected_facets, $brands)
{
    global $filter_labels;
    $facet_links = array();

    foreach ($facet_category as $facet_id => $facet_count) {
        if ($facet_category_name == 'brandId') {
            $facet_name = media_bynder_get_brand_name($brands, $facet_id);
        } else {
            $facet_name = $facet_id;
        }
        if (!isset($filter_labels[$facet_name])) {
            $filter_labels[$facet_name] = $facet_name;
        }
        $filter_key = $facet_category_name;
        $filter_value = $facet_id;

        $active = media_bynder_facet_active($selected_facets, array($facet_category_name => array($facet_id)));
        $active_class = $active ? "active" : "";
        $link = "";
        $link .= '<a class="filter-url ' . filter_xss($active_class) . '" href="#" data-filter-key="' . filter_xss($filter_key) . '" data-filter-value="' . filter_xss($filter_value) . '">' . filter_xss($filter_labels[$facet_name]) . '</a>';
        $link .= '<span class="count">(' . filter_xss($facet_count) . ')</span>';
        $facet_links[] = $link;
    }
    return $facet_links;
}

function media_bynder_get_brand_name($brands, $brand_id)
{
    foreach ($brands as $brand) {
        if ($brand['id'] == $brand_id) {
            return $brand['name'];
        }
    }
    return "";
}

function media_bynder_query_facet_link($facet, $facet_value, $selected_facets, $deactivate = false)
{
    if (!$deactivate) {
        $selected_facets = array_merge_recursive(
            array($facet => array($facet_value)),
            $selected_facets
        );
    } else {
        if (isset($selected_facets[$facet])) {
            $selected_facets[$facet] = array_diff(explode(',', $selected_facets[$facet][0]), array($facet_value));
        }
    }
    $csv_facets = array();
    foreach (array_filter($selected_facets) as $selected_facet_key => $selected_facet) {
        $csv_facets[$selected_facet_key] = implode(',', $selected_facet);
    }
    $value = http_build_query($csv_facets);
    return $value;
}

function media_bynder_facet_active($selected_facets, $current_facet)
{
    if (isset($selected_facets[key($current_facet)])) {
        return in_array($current_facet[key($current_facet)][0],
            explode(',', $selected_facets[key($current_facet)][0]));
    } else {
        return false;
    }
}

/**
 * Parse the facet link.
 */
function media_bynder_parse_facet_link($query)
{
    $pattern = '/\+\([A-Za-z0-9 ]*\)/';
    preg_match($pattern, $query, $matches);
    $query = urlencode($query);
    if (!empty($matches)) {
        $query = $query . '&search=' . substr(trim($matches[0]), 2, -1);
    }

    return $query;
}

function media_bynder_remove_hidden_facets($available_facets, $selected_facets)
{
    foreach ($selected_facets as $selected_facet_key => $selected_facet) {
        $value = $selected_facet[0];
        $isset = isset($available_facets[$selected_facet_key][$value]);
        if (!$isset) {
            unset($selected_facets[$selected_facet_key]);
        }
    }
    return $selected_facets;
}

/**
 * Get brands.
 */
function media_bynder_brands()
{

    $bynderApi = media_bynder_init_api();
    $assetBank = $bynderApi->getAssetBankManager();
    $result = $assetBank->getBrands()->wait();

    if ($result) {
        return $result;
    } else {
        drupal_set_message(t('Could not retrieve search results:') . filter_xss($result->error), 'error');
    }
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
function media_bynder_search_results($search, $selected_facets, $meta_properties, $nr_of_results = 5)
{
    $start_index = 0;
    $query = array();

    if (isset($_GET['page'])) {
        $current_page = (int)filter_xss($_GET['page']);
        $current_page += 1;
        $start_index = $current_page * $nr_of_results;
    }
    $selected_facets_meta_properties = [];
    foreach ($meta_properties as $meta_property) {
        foreach ($meta_property['options'] as $meta_property_option) {
            $selected_facets_meta_properties[$meta_property['name']][$meta_property_option['name']] = $meta_property_option['id'];
        }
    }

    if (isset($selected_facets)) {
        foreach ($selected_facets as $facet) {
            $facets = $facet->value;
            $facet_category = $facet->key;

            if (isset($selected_facets_meta_properties[$facet_category])) {
                $meta_facets = [];
                foreach (explode(',', $facets[0]) as $facet) {
                    $meta_facets[] = $selected_facets_meta_properties[$facet_category][$facet];
                }
                $query['propertyOptionId'] = implode(',', $meta_facets);
            } else {
                $query[$facet_category] = $facets;
            }
        }
    }

    if (isset($search) && strlen($search) > 0) {
        $query['keyword'] = urlencode($search);
    }

    $bynderApi = media_bynder_init_api();
    $assetBank = $bynderApi->getAssetBankManager();

    $page = 1;
    if (isset($start_index) && $start_index > 0) {
        $page = $start_index / $nr_of_results;
    }
    $query = $query + array(
            'page' => $page,
            'limit' => $nr_of_results,
            'count' => 1
        );

    $result = $assetBank->getMediaList($query)->wait();
    if ($result) {
        if (isset($result['count']['total'])) {
            $result['totalNrOfResults'] = $result['count']['total'];
        }
    } else {
        drupal_set_message(t('Could not retrieve search results:') . filter_xss($result->error), 'error');
        return false;
    }
    return $result;
}

/**
 * Organize results from search engine to visualize them.
 */
function media_bynder_generate_results($results)
{
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
            if (!empty($media_item['thumbnails']['thul'])) {
                $output['#items'][] = media_bynder_generate_result_current($media_item);
            }
        }
    } else {
        return false;
    }

    return $output;
}

/**
 * Show search result.
 */
function media_bynder_generate_result_current($media_item)
{
    if (!isset($media_item['thumbnails']['thul'])) {
        return false;
    }

    if (!isset($media_item['idHash'])) {
        return "image has no idHash";
    }

    return l('<div class="image-holder"><img src="' . $media_item['thumbnails']['thul'] . '"/></div>
      <span class="media_object_type_' . $media_item['type'] . '">' . t($media_item['name']) . '</span>',
        'admin/content/media/add/media_bynder/', array(
            'attributes' => array(
                'class' => 'bynder-image',
                'rel' => $media_item['id'],
                'data-id' => $media_item['id'],
                'data-idHash' => $media_item['idHash']
            ),
            'html' => true,
            'query' => array(
                'id' => $media_item['id'],
                'idHash' => $media_item['idHash']
            )
        ));
}

/**
 * Generate paging functionality.
 */
function media_bynder_generate_pager($num_results, $element = 0, $parameters = array())
{
    if (isset($_POST['search'])) {
        $_GET['search'] = filter_xss(check_plain($_POST['search']));
    }
    pager_default_initialize($num_results, BYNDER_SEARCH_PAGE_SIZE, $element);
    return array(
        '#markup' => theme('pager', array(
            'parameters' => $parameters,
            'element' => $element,
         ))
    );
}

/**
 * Submit the selected media object.
 */
function media_bynder_submit_mob(&$form, &$form_state)
{
    $id = media_bynder_parameter($_POST, 'id');
    $idHash = media_bynder_parameter($_POST, 'idHash');

    $redirect = 'media_bynder_add';

    $result = media_bynder_save_image($id, $idHash, $form);
    if (!$result['success']) {
        echo '{"success": 0, "type": "error", "message": "' . $result['message'] . '"}';
    } else {
        echo '{"success": 1, "type": "success", "message": "' . t($result['message']) . '"}';
    }
    exit;
}