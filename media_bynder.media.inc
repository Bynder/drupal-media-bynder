<?php

/**
 * @file
 * Media module integration for the Media: Bynder module.
 */

/**
 * Implements hook_media_browser_plugin_info().
 */
function media_bynder_media_browser_plugin_info() {
  $info['media_bynder'] = array(
    'title' => t('Bynder'),
    'class' => 'MediaBynderBrowser',
  );

  return $info;
}
