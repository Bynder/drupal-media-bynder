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
function media_bynder_file_formatter_image_settings($form, &$form_state, $settings) {
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

function media_bynder_field_info_alter(&$info) {
   $info['image']['default widget'] = 'media_bynder';
}

/**
 * Implements hook_image_default_styles().
 * Add an example image style for drupal-media-bynder
 */

function media_bynder_image_default_styles() {
    variable_set('image_style_preview_image', drupal_get_path('module', 'image') . '/sample.png');
    $styles = array();

    $styles['bynder_drupal_example'] = array(
        'label' => t('Bynder Example (400x400)'),
        'effects' => array(
            array(
                'name' => 'bynder_custom_resolution_resize',
                'data' => array(
                    'width' => 400,
                    'height' => 400
                ),
                'weight' => 0,
            ),
        ),
    );
    return $styles;
}

/**
 * Implements hook_image_effect_info().
 * declare effect callbacks
 */
function media_bynder_image_effect_info() {
    return array(
        'bynder_custom_resolution_crop' => array(
            'label' => t('Bynder custom resolution crop'),
            'help' => t('Creates a call to the public Bynder function in /additional/, make sure this is properly configured or ask the Bynder support team.<br>Please note that in order for this function to work you need to call this effect only as it uses an external image from Bynder. Due to this external nature additional drupal effects can\'t be applied'),
            'effect callback'=> 'media_bynder_custom_resolution_effect',
            'dimensions callback' => 'media_bynder_custom_resolution_dimensions',
            'form callback'=> 'image_resize_form',
        ),
        'bynder_custom_resolution_extend' => array(
            'label' => t('Bynder custom resolution extend'),
            'help' => t('Creates a call to the public Bynder function in /additional/, make sure this is properly configured or ask the Bynder support team.<br>Please note that in order for this function to work you need to call this effect only as it uses an external image from Bynder. Due to this external nature additional drupal effects can\'t be applied'),
            'effect callback'=> 'media_bynder_custom_resolution_effect',
            'dimensions callback' => 'media_bynder_custom_resolution_dimensions',
            'form callback'=> 'image_resize_form',
        ),
        'bynder_custom_resolution_resize' => array(
            'label' => t('Bynder custom resolution resize'),
             'help' => t('Creates a call to the public Bynder function in /additional/, make sure this is properly configured or ask the Bynder support team.<br>Please note that in order for this function to work you need to call this effect only as it uses an external image from Bynder. Due to this external nature additional drupal effects can\'t be applied'),
             'effect callback'=> 'media_bynder_custom_resolution_effect',
             'dimensions callback' => 'media_bynder_custom_resolution_dimensions',
             'form callback'=> 'image_resize_form',
         )
    );
}

/**
 * Implements callback 'effect callback'
 * will return the image from the API, instead of modifying the image as intended
 */
function media_bynder_custom_resolution_effect(&$image, $data) {
    if (!media_bynder_custom_resolution($image)) {
        watchdog('image', 'The image %file could not be colorize because the imagefilter() function is not available in this PHP installation.', array('%file' => $image->source), WATCHDOG_ERROR);
        return FALSE;
    }
    return TRUE;
}

/**
 * Filter effect function
 */
function media_bynder_custom_resolution(stdClass $image) {
    return imagefilter($image->resource, null);
}

function media_bynder_custom_resolution_dimensions(array &$dimensions, array $data) {
    if ($dimensions['width'] && $dimensions['height']) {
        $dimensions['width'] = $data['width'];
        $dimensions['height'] = $data['height'];
    }
}

/**
 * Implements hook_preprocess_image()
 * Will edit the Path to match the API call if the bynder_custom_resolution effect is present in the active style
 */
function media_bynder_preprocess_image(&$vars) {
    if (array_key_exists('uri', $vars)) {
        $vars['path'] = $vars['uri'];
    }

    if (!isset($vars['style_name'])) {
        return FALSE;
    }

    $image_styles = image_styles();

    foreach ($image_styles as $image_style_key => $image_style) {
        if ($vars['style_name'] == $image_style_key) {
            foreach ($image_style['effects'] as $effect_key => $effect) {
                if ($effect['name'] == 'bynder_custom_resolution_crop' || $effect['name'] == 'bynder_custom_resolution_extend' || $effect['name'] == 'bynder_custom_resolution_resize') {
                    $fragment_id = media_bynder_id_from_uri($vars['path']);
                    $vars['path'] = media_bynder_embed_url($fragment_id, 'thul', $effect);
                    break;
                }
            }
        }
    }
    return FALSE;
}

/**
 * Implements hook_preprocess_image_style().
 * Change the URI of the image to the real URL
 */
function media_bynder_preprocess_image_style(&$variables) {
    $variables['uri'] = $variables['path'];
}

/**
 * Implements hook_field_formatter_info().
 * Add a new formatter for field type "image"
 */
function media_bynder_field_formatter_info() {
    return array(
        'media_bynder_absolute_url' => array(
            'label' => t('Absolute URL'),
            'field types' => array('image'),
            'multiple values' => FIELD_BEHAVIOR_DEFAULT,
            'settings' => array(
                'tooltip' => t('Bynder function that returns the image URL.'),
                'image_style' => 'bynder_drupal_example'
            )
        )
    );
}

/**
 * Implements hook_field_formatter_settings_form().
 */
function media_bynder_field_formatter_settings_form($field, $instance, $view_mode, $form, &$form_state) {
    $settings = $instance['display'][$view_mode]['settings'];
    $options = array();

    foreach (image_styles() as $image_style) {
        $options[$image_style['name']] = $image_style['label'];
    }

    $element = array();

    $element['image_style'] = array(
        '#type' => 'select',
        '#title' => t('Image Style'),
        '#description' => t('Select your Image Style, for which to get the absolute URL'),
        '#default_value' => $settings['image_style'],
        '#options' => $options
    );

    return $element;
}

/**
 * Implements hook_field_formatter_settings_summary.
 * The summary for the "manage display view"
 */
function media_bynder_field_formatter_settings_summary($field, $instance, $view_mode) {
    $display = $instance['display'][$view_mode];
    $settings = $display['settings'];
    $summary = t('Return the absolute URL with <b> @style </b> Image Style',
        array('@style' => $settings['image_style'])
    );

    return $summary;
}

/**
 * Implements hook_field_formatter_view().
 * Returns the absolute URL for the custom calls
 */
function media_bynder_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
    if ($display['type'] == 'media_bynder_absolute_url') {
        $settings = $display['settings'];
        //global Drupal function
        $effects = image_styles()[$settings['image_style']];

        foreach ($effects as $effect) {
            if (is_array($effect)) {
                //array is not 0-based
                $effect = reset($effect);
                $effect_name = $effect['name'];

                if (strpos($effect_name, 'bynder') !== false) {
                    $effect_width = $effect['data']['width'];
                    $effect_height = $effect['data']['height'];

                    if (!empty($items)) {
                        $media_item = array(
                            'idHash' => media_bynder_idHash_from_uri($items[0]['uri'])
                        );

                        $element = array();
                        // global function from media_bynder.utils.inc
                        $image_url = customResolutionUrl($media_item, $effect_width, $effect_height, $effect_name);
                        $element[0]['#markup'] = $image_url;

                        return $element;
                    }
                }
            }
        }
    }
    return FALSE;
}
