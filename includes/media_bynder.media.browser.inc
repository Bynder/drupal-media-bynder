<?php
include_once 'media_bynder.admin.inc';

/**
* Implements hook_media_browser_plugin_view().
*/
function media_bynder_media_browser_plugin_view($plugin_name, $params) {
    $path = drupal_get_path('module', 'media_bynder');

    switch ($plugin_name) {
        case 'media_bynder':
            return array(
                '#title' => t('Bynder'),
                'filter' => drupal_get_form('media_bynder_browser_filter_form')
            );
    }
}

function media_bynder_browser_filter_form($form, &$form_state) {
    module_load_include('inc', 'media', 'includes/media.browser');

    $data['bynder_search'] = media_bynder_generate_search($form_state);

    return array('bynder' => $data);
}
