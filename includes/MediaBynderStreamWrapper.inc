<?php
/**
 * @file
 * media_youtube/includes/MediaBynderStreamWrapper.inc
 *
 * Create a Bynder Stream Wrapper class for the Media/Resource module.
 */

class MediaBynderStreamWrapper extends MediaReadOnlyStreamWrapper{

    public function interpolateUrl() {
        $parts = explode('/', $this->uri);

        foreach ($parts as $part => $part_key) {
            if ($part_key == 'i'){
                if (count($parts) >= ($part + 1)) {
                    return media_bynder_embed_url($parts[$part + 1]);
                }
            }
        }
        return FALSE;
    }

    /**
     * Function getOriginalThumbnailPath().
     * @return string
     *   The path to the thumbnail.
     */
    public function getOriginalThumbnailPath() {
        $parts = $this->get_parameters();
        $url = media_bynder_embed_url($parts['i'], 'thul');
        return $url;
    }

    /**
     * Function getOriginalThumbnailPath().
     * @return string
     *   The path to the thumbnail.
     */
    public function getWebimagePath() {
        $parts = $this->get_parameters();
        $url = media_bynder_embed_url($parts['i'], 'web');
        return $url;
    }

}
