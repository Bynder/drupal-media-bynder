<?php

/**
 * @file
 * media_bynder/includes/media_bynder.formatters.inc
 * Formatters for Media: Bynder.
 */

/**
 * Implements hook_file_formatter_info().
 */
function media_bynder_file_formatter_info_alter(&$info)
{
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

    $info['file_field_image'] = array(
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

    $info['file_field_file_video'] = array(
        'label' => t('Bynder Preview Image'),
        'file types' => array(
            'video'
        ),
        'default settings' => array(
            'image_style' => 'thumbnail'
        ),
        'view callback' => 'media_bynder_file_formatter_video_view',
        'settings callback' => 'media_bynder_file_formatter_video_settings',
        'mime types' => array(
            'video/bynder'
        )
    );

    $info['file_field_media_large_icon'] = array(
        'label' => t('Bynder Preview Image'),
        'file types' => array(
            'video'
        ),
        'default settings' => array(
            'image_style' => ''
        ),
        'view callback' => 'media_bynder_file_formatter_image_view',
        'settings callback' => 'media_bynder_file_formatter_image_settings',
        'mime types' => array(
            'video/bynder'
        )
    );

    return $info;
}

/**
 * Format image view.
 */
function media_bynder_file_formatter_image_view($file, $display, $langcode)
{
    $scheme = file_uri_scheme($file->uri);

    if ($scheme == 'bynder') {
        $wrapper            = file_stream_wrapper_get_instance_by_uri($file->uri);
        $image_style        = $display['settings']['image_style'];
        $valid_image_styles = image_style_options(FALSE);
        // @TODO: If autosubmit is removed and we allow view modes that insert
        // images in the WYSIWYG, add file->overrides handling.
        if (empty($image_style) || !isset($valid_image_styles[$image_style])) {
            $element = array(
                '#theme' => 'image',
                '#path' => $wrapper->getWebimagePath(),
                '#alt' => isset($file->override['attributes']['alt']) ? $file->override['attributes']['alt'] : $file->filename,
                '#suffix' => '<p class="media_object_type_' . ucfirst($file->type) . '">' . ucfirst($file->type) . '</p>'
            );
        } else {
            $element = array(
                '#theme' => 'image',
                '#path' => $wrapper->getWebimagePath(),
                '#alt' => isset($file->override['attributes']['alt']) ? $file->override['attributes']['alt'] : $file->filename,
                '#suffix' => '<p class="media_object_type_' . ucfirst($file->type) . '">' . ucfirst($file->type) . '</p>'
            );
        }

        return $element;
    }
}

/**
 * Implements hook_file_formatter_FORMATTER_settings().
 */
function media_bynder_file_formatter_image_settings($form, &$form_state, $settings)
{
    $element                = array();
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
 * Format the video view.
 */
function media_bynder_file_formatter_video_view($file, $display, $langcode)
{
    $scheme = file_uri_scheme($file->uri);

    if ($scheme == 'bynder') {
        $element = array(
            '#theme' => 'media_bynder_video',
            '#uri' => $file->uri,
            '#options' => array()
        );

        return $element;
    }
}

/**
 * Video formatting.
 */
function media_bynder_file_formatter_video_settings($form, &$form_state, $settings)
{
    $element                = array();
    $element['video_style'] = array(
        '#title' => t('Video style'),
        '#type' => 'select',
        '#options' => image_style_options(FALSE),
        '#default_value' => $settings['video_style'],
        '#empty_option' => t('None (original video)')
    );
    return $element;
}
