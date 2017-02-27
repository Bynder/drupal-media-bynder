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
            '#items' => unserialize(variable_get('media_bynder_derivatives', '')),
        ],
        'check' => [
            '#type' => 'submit',
            '#value' => t('Reload derivatives info'),
            '#submit' => ['media_bynder_batch_reload_derivatives'],
        ],
    ];

    $form['media_bynder_settings']['media_bynder_derivatives']['media_bynder_custom_derivative'] = array(
        '#type' => 'textfield',
        '#title' => t('Custom derivative name'),
        '#default_value' => variable_get('media_bynder_custom_derivative', 'drupal_custom'),
        '#size' => 50,
        '#description' => t('Custom derivative name set up in Bynder config, allows cropping and resizing.'),
        '#required' => false
    );
    return system_settings_form($form);
}

/**
 * The batch callback.
 */
function media_bynder_batch_reload_derivatives()
{
    variable_set('image_style_preview_image', drupal_get_path('module', 'image') . '/sample.png');
    $batch = array(
        'operations' => array(
            array('media_bynder_get_media_list', array()),
            array('media_bynder_batch_reload_derivatives_process', array()),
            array('media_bynder_batch_clean_image_styles', array()),
        ),
        'finished' => 'media_bynder_batch_reload_derivatives_finished',
        'title' => t('Batch to reload Bynder derivatives information'),
        'init_message' => t('Bynder derivatives batch is starting...'),
        'progress_message' => t('Processed @current out of @total.'),
        'error_message' => t('Fix has encountered an error.')
    );
    batch_set($batch);
}

function media_bynder_get_media_list(&$context)
{
    $context['message'] = "Fetching Media List.";
    $bynderApi = media_bynder_init_api();
    $mediaList = $bynderApi->getAssetBankManager()->getMediaList(array('limit' => 1))->wait();
    $derivatives = array_keys($mediaList[0]['thumbnails']);
    $context['results']['oldDerivatives'] = unserialize(variable_get('media_bynder_derivatives', array()));
    $context['results']['derivativesList'] = $derivatives;
}

/**
 * The batch processor.
 */
function media_bynder_batch_reload_derivatives_process(&$context)
{
    $context['message'] = "Updating derivatives data.";
    $derivatives = $context['results']['derivativesList'];
    $oldDerivatives = $context['results']['oldDerivatives'];
    $newDerivatives = array_diff((array)$derivatives, (array)$oldDerivatives);

    $context['results']['toRemove'] = array_diff((array)$oldDerivatives, (array)$derivatives);

    if (!empty($newDerivatives)) {
        $customDerivative = variable_get('media_bynder_custom_derivative');
        foreach ($newDerivatives as $derivative) {
            if (!image_style_load('bynder_' . $derivative)) {
                $context['message'] = "Creating image style for derivative: $derivative";
                $style = image_style_save(
                    array(
                        'name' => 'bynder_' . $derivative,
                        'label' => t('Bynder derivative: ' . $derivative)
                    )
                );

                if ($derivative == $customDerivative) {
                    $effect = array(
                        'name' => 'bynder_custom_resolution_resize',
                        'data' => array(
                            'width' => 400,
                            'height' => 400
                        ),
                        'isid' => $style['isid'],
                    );
                    image_effect_save($effect);
                }
            }
        }
    }
    variable_set('media_bynder_derivatives', serialize($derivatives));
}

function media_bynder_batch_clean_image_styles(&$context)
{
    foreach ($context['results']['toRemove'] as $name) {
        image_style_delete("bynder_$name", 'thumbnail');
    }

}

/**
 * The batch finish handler.
 */
function media_bynder_batch_reload_derivatives_finished($success, $results, $operations)
{
    if ($success) {
        drupal_set_message('Finished setting up image styles!');
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
