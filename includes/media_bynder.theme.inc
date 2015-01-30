<?php
error_log("INSTALLED");
/**
 * @file
 * This file contains custom theming logic
 */

//asd98fasdpfas7f9s8aodf9o8as897789a879da978a9807asfdi8sadilyusadflh32hilaf98pua
//fhw3hlsfua8ual8ulaw38ua8wfa3wa3

/**
 * Wraps an element in an extra div.
 */
function theme_media_bynder_my_element_wrapper($element)
{
    $output = '<div class="element-wrapper">';
    $output .= $element['#children'];
    $output .= '</div>';
    return $output;
}

/**
 * Some generic theming function.
 */
function theme_media_bynder_my_custom_html($variables)
{
    $foo    = $variables['foo'];
    $output = '<div class="type-foo clearfix">';
    $output .= $foo;
    $output .= '</div>';
    return $output;
}

/**
 * Show thumbnail for a Bynder file.
 */
function theme_media_bynder_thumbnail($variables)
{
    $label   = '';
    $element = $variables['element'];

    // Wrappers to go around the thumbnail.
    $attributes = array(
        'title' => $element['#file']->filename,
        'class' => 'media-item',
        'data-idHash' => $element['#file']->idHash
    );
    $prefix     = '<div ' . drupal_attributes($attributes) . '><div class="media-thumbnail">';
    $suffix     = '</div></div>';

    // Arguments for the thumbnail link.
    error_log($element['#file']->uri);
    $thumb = '<img class="i can change" src="' . media_bynder_embed_url($element['#file']->idHash, 'thul') . '">';

    $target  = 'file/' . $element['#file']->idHash . '/edit';
    $options = array(
        'query' => drupal_get_destination(),
        'html' => TRUE,
        'attributes' => array(
            'title' => t('Click to edit details LOL' . $element['#file']->idHash)
        )
    );

    // Element should be a field renderable array. This should be improved.
    if (!empty($element['#show_names']) && $element['#file']->filename) {
        $label = '<div class="label-wrapper"><label class="media-filename">' . $element['#file']->filename . '</label></div>';
    }

    $output = $prefix;
    if (!empty($element['#add_link'])) {
        $output .= l($thumb, $target, $options);
    } else {
        $output .= $thumb;
    }
    $output .= $label . $suffix;
    return $output;
}

/**
 * Preprocess function for theme('media_bynder_video').
 */
function media_bynder_preprocess_media_bynder_video(&$variables)
{
    $umid              = substr(strrchr($variables['uri'], "/"), 1);
    $variables['umid'] = $umid;

    $variables['bynder_url']   = media_bynder_variable_get('url');
    $variables['video_width']  = 640;
    $variables['video_height'] = 360;
    return $variables;
}
