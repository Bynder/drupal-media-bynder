<?php
/**
 * @file
 * media_bynder/includes/MediaBynderBrowser.inc
 *
 * Definition of MediaBynderBrowser.
 */

require_once 'media_bynder.admin.inc';
/**
 * Media browser plugin for displaying a specific view and display.
 */
class MediaBynderBrowser extends MediaBrowserPlugin
{

    /**
     * Implements MediaBrowserPluginInterface::access().
     */
    public function access($account = NULL)
    {
        return file_entity_access('create', $account);
    }

    /**
     * Implements MediaBrowserPlugin::view().
     */
    public function view()
    {
        $build['form'] = drupal_get_form('media_bynder_browser_add', $this->params['types'], $this->params['multiselect']);
        return $build;
    }
}
