<?php

/**
 * @file
 * media_bynder/includes/media_bynder.formatters.inc
 * Formatters for Media: Bynder.
 */

/**
 * Implements hook_file_formatter_info().
 * $info: Array of information on file formatters exposed by hook_file_formatter_info() implementations.
 */
function media_bynder_file_formatter_info_alter(&$info) {
    $info['file_image'] = array(
        'label' => t('Bynder Preview Image'),
        'file types' => array(
            'image'
        ),
        'default settings' => array(
            'image_style' => ''
        ),
        'view callback' => 'media_bynder_file_formatter_image_view',
        'settings callback' => 'media_bynder_file_formatter_image_settings',
        'mime types' => array(
            'image/bynder'
        )
    );

    return $info;
}

/**
 * Format image view.
 */
function media_bynder_file_formatter_image_view($file, $display, $langcode) {
    $scheme = file_uri_scheme($file->uri);

    if ($scheme == 'bynder') {
        $wrapper = file_stream_wrapper_get_instance_by_uri($file->uri);
        $image_style = $display['settings']['image_style'];
        $valid_image_styles = image_style_options(FALSE);

            $element = array(
                '#theme' => 'image',
                '#path' => $wrapper->getWebimagePath(),
                '#alt' => isset($file->override['attributes']['alt']) ? $file->override['attributes']['alt'] : $file->filename,
                '#suffix' => '<p class="media_object_type_' . ucfirst($file->type) . '">' . ucfirst($file->type) . '</p>'
            );

        return $element;
    }
}

/**
 * Implements hook_file_formatter_FORMATTER_settings().
 */
function media_bynder_file_formatter_image_settings($form, &$form_state, $settings)
{
    $element = array();
    $element['image_style'] = array(
        '#title' => t('Image style'),
        '#type' => 'select',
        '#options' => image_style_options(FALSE),
        '#default_value' => $settings['image_style'],
        '#empty_option' => t('None (original image)')
    );
    return $element;
}

/**
 * Implements hook_entity_info_alter().
 */
// function media_bynder_entity_info_alter(&$entity_info) {
//   cache_clear_all();
//   $current_media = variable_get('current_media');
//   $custom_view_modes = $current_media['thumbnails'];
//   $entity_info['file']['view modes'] = array();

//   foreach ($custom_view_modes as $custom_view_key => $custom_view_key) {
//     $entity_info['file']['view modes'] += array(
//       $custom_view_key => array(
//         'label' => t($custom_view_key),
//         'custom settings' => TRUE,
//       ),
//     );
//   }
// }
