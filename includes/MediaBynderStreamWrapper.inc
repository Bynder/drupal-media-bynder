<?php
/**
 * @file
 * media_youtube/includes/MediaBynderStreamWrapper.inc
 *
 * Create a Bynder Stream Wrapper class for the Media/Resource module.
 */

class MediaBynderStreamWrapper extends MediaReadOnlyStreamWrapper{

    public function interpolateUrl() {
        $this->base_url = media_bynder_variable_get('url');

        if ($parameters = $this->get_parameters()) {
            return media_bynder_embed_url($parameters['i']);
        } else {
            return FALSE;
        }
    }

    /**
     * Function getOriginalThumbnailPath().
     * @return string
     *   The path to the thumbnail.
     */
    public function getOriginalThumbnailPath()
    {
        $parts = $this->get_parameters();
        $url = media_bynder_embed_url($parts['i'], 'thul');
        return $url;
    }

    /**
     * Function getOriginalThumbnailPath().
     * @return string
     *   The path to the thumbnail.
     */
    public function getWebimagePath()
    {
        $parts = $this->get_parameters();
        $url = media_bynder_embed_url($parts['i'], 'web');
        return $url;
    }

    // /**
    //  * Determine thumbnail path.
    //  * @return string
    //  *   Path to thumbnail.
    //  */
    // public function getLocalThumbnailPath()
    // {
    //     $parts = $this->get_parameters();

    //     $local_path = file_default_scheme() . '://todo/' . check_plain($parts['f']) . '.jpg';
    //     if (!file_exists($local_path)) {
    //         $dirname = drupal_dirname($local_path);
    //         file_prepare_directory($dirname, FILE_CREATE_DIRECTORY | FILE_MODIFY_PERMISSIONS);
    //         $response = drupal_http_request($this->getOriginalThumbnailPath());
    //         if (!isset($response->error)) {
    //             file_unmanaged_save_data($response->data, $local_path, TRUE);
    //         } else {
    //             @copy($this->getOriginalThumbnailPath(), $local_path);
    //         }
    //     }
    //     return $local_path;
    // }
}
