<?php

/**
 * @file
 * Functions for administration of the module
 *
 * Callback for /admin/configure/media/media_bynder.
 */

/**
 * Page callback: Current posts settings.
 *
 * @see current_posts_menu()
 */
function media_bynder_form($form, &$form_state)
{
    $form['media_bynder_settings'] = array(
        '#type' => 'fieldset',
        '#title' => t('Bynder API'),
        '#description' => t('You will first need to apply for an API Developer Key'),
        '#collapsible' => true
    );

    $form['media_bynder_settings']['media_bynder_url'] = array(
        '#type' => 'textfield',
        '#title' => t('Bynder url'),
        '#default_value' => variable_get('media_bynder_url', ''),
        '#size' => 50,
        '#description' => t('The Bynder url.'),
        '#required' => true
    );
    $form['media_bynder_settings']['media_bynder_oauth_consumer'] = array(
        '#type' => 'textfield',
        '#title' => t('OAuth Consumer Token'),
        '#default_value' => variable_get('media_bynder_oauth_consumer', ''),
        '#size' => 50,
        '#description' => t('The consumer token to be used by the module to communicate with Bynder.'),
        '#required' => true
    );
    $form['media_bynder_settings']['media_bynder_oauth_consumer_secret'] = array(
        '#type' => 'textfield',
        '#title' => t('OAuth Consumer Secret'),
        '#default_value' => variable_get('media_bynder_oauth_consumer_secret', ''),
        '#size' => 50,
        '#description' => t('The consumer secret to be used by the module to communicate with Bynder.'),
        '#required' => true
    );

    $form['media_bynder_settings']['media_bynder_oauth_token'] = array(
        '#type' => 'textfield',
        '#title' => t('OAuth Token'),
        '#default_value' => variable_get('media_bynder_oauth_token', ''),
        '#size' => 50,
        '#description' => t('The token to be used by the module to communicate with Bynder.'),
        '#required' => true
    );

    $form['media_bynder_settings']['media_bynder_oauth_token_secret'] = array(
        '#type' => 'textfield',
        '#title' => t('OAuth Token Secret'),
        '#default_value' => variable_get('media_bynder_oauth_token_secret', ''),
        '#size' => 50,
        '#description' => t('The token secret to be used by the module to communicate with Bynder.'),
        '#required' => true
    );

    $form['media_bynder_settings']['media_bynder_cdn_url'] = array(
        '#type' => 'textfield',
        '#title' => t('Cloudfront CDN url'),
        '#default_value' => variable_get('media_bynder_cdn_url', ''),
        '#size' => 50,
        '#description' => t('Optional cloudfront CDN url.'),
        '#required' => false
    );

    return system_settings_form($form);
}

/**
 * Implements validation from the Form API.
 */
function media_bynder_form_validate($form, &$form_state)
{
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
function media_bynder_validate_url($url)
{
    return filter_var($url, FILTER_VALIDATE_URL);
}
