<?php

/**
 * @file media_bynder/includes/MediaBynderBrowser.inc
 *
 * Definition of MediaBynderBrowser.
 */

/**
 * Browser plugin for Bynder files.
 */
class MediaBynderBrowser extends MediaBrowserPlugin
{
    /**
     * Implements MediaBrowserPluginInterface::access().
     */
    public function access($account = null)
    {
        return media_internet_access($account);
    }

    /**
     * Implements MediaBrowserPlugin::view().
     */
    public function view()
    {
        $build = array();
        $build['form'] = drupal_get_form('media_bynder_add', $this->params);

        return $build;
    }
}
