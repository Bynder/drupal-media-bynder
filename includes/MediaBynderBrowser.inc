<?php

/**
 * @file
 * Definition of MediaBynderBrowser.
 */

/**
 * Browser plugin for Bynder files.
 */
class MediaBynderBrowser extends MediaBrowserPlugin {
  /**
   * Implements MediaBrowserPluginInterface::access().
   */
  public function access($account = NULL) {
    return media_internet_access($account);
  }

  /**
   * Implements MediaBrowserPlugin::view().
   */
  public function view() {
    module_load_include('inc', 'media_bynder', 'includes/media_bynder.admin');

    $build = array();
    $params = $this->params;
    $build['form'] = drupal_get_form('media_bynder_add', $params);

    return $build;
  }
}
