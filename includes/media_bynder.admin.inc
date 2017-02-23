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

//    $form['media_bynder_settings']['media_bynder_test'] = [
//        '#type' => 'fieldset',
//        '#title' => t('API connection test'),
//        'wrapper' => [
//            '#type' => 'html_tag',
//            '#tag' => 'div',
//            '#attributes' => ['id' => 'connection-test'],
//            '#attached' => ['library' => ['bynder/config_form']],
//        ],
//        'check' => [
//            '#type' => 'button',
//            '#limit_validation_errors' => [],
//            '#value' => t('Test connection'),
//            '#ajax' => ['callback' => '::testConnectionAjaxCallback'],
//        ],
//        'test_connection' => [
//            '#type' => 'checkbox',
//            '#title' => t('Test connection before saving'),
//            '#description' => t("Uncheck to allow saving credentials even if connection to Bynder can't be established."),
//            '#default_value' => TRUE,
//        ],
//    ];



    $form['media_bynder_settings']['media_bynder_derivatives'] = [
        '#type' => 'fieldset',
        '#title' => t('Bynder image derivatives'),
        '#description' => t('Bynder provides "mini", "webimage" and "thul" image sizes by default. Custom derivatives can be configured to better suit specific use-cases. Reload of derivatives can be triggered if any derivatives are missing from the list.'),
        'derivatives_list' => [
            '#theme' => 'item_list',
            '#items' => unserialize(variable_get('media_bynder_derivatives','')),
        ],
        'check' => [
            '#type' => 'submit',
            '#value' => t('Reload derivatives info'),
            '#submit' => ['media_bynder_batch_reload_derivatives'],
        ],
    ];

    return system_settings_form($form);
}

/**
 * Submit callback that will initiate derivaties reload batch.
 */
//function media_bynder_submit_reload_derivatives(array &$form, FormStateInterface $form_state) {
//    $batch = [
//        'title' => t('Reloading derivatives information'),
//        'operations' => [
//            ['media_bynder_batch_get_brands'],
//            ['media_bynder_batch_reload_derivatives'],
//        ],
//        'finished' => ['media_bynder_batch_finish'],
//    ];
//    batch_set($batch);
//}

/**
 * The batch callback.
 */
function media_bynder_batch_reload_derivatives()
{

    $bynderApi = media_bynder_init_api();

    $mediaList = $bynderApi->getAssetBankManager()->getMediaList(array('limit' => 1))->wait();
    $derivativesList = array($mediaList[0]['thumbnails']);

    $batch = array(
        'operations' => array(
            array('media_bynder_batch_reload_derivatives_process', $derivativesList),
        ),
        'finished' => 'media_bynder_batch_reload_derivatives_finished',
        'title' => t('Batch to reload Bynder derivatives information'),
        'init_message' => t('Bynder derivatives batch is starting...'),
        'progress_message' => t('Processed @current out of @total.'),
        'error_message' => t('Fix has encountered an error.')
    );
    batch_set($batch);
}

/**
 * The batch processor.
 */
function media_bynder_batch_reload_derivatives_process($derivativesList, &$context)
{
    $derivativeNames = array_keys($derivativesList);
    variable_set('media_bynder_derivatives', serialize($derivativeNames));
    $context['message'] = "Updating derivatives data.";
}

/**
 * The batch finish handler.
 */
function media_bynder_batch_reload_derivatives_finished($success, $results, $operations)
{
    if ($success) {
        drupal_set_message('Fix is complete!');
    } else {
        $error_operation = reset($operations);
        $message = t('An error occurred while processing %error_operation with arguments: @arguments', array(
            '%error_operation' => $error_operation[0],
            '@arguments' => print_r($error_operation[1], true)
        ));
        drupal_set_message($message, 'error');
    }
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

    // If this is the first time we're configuring credentials also reload
    // information about derivatives.
//    if ($is_initial_save) {
//        $this->submitReloadDerivatives($form, $form_state);
//    }
}

/**
 * Validations
 */
function media_bynder_validate_url($url)
{
    return filter_var($url, FILTER_VALIDATE_URL);
}
