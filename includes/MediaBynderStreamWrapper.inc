<?php
/**
 * @file
 * media_youtube/includes/MediaBynderStreamWrapper.inc
 *
 * Create a Bynder Stream Wrapper class for the Media/Resource module.
 */

class MediaBynderStreamWrapper extends MediaReadOnlyStreamWrapper{

    public function interpolateUrl() {
        $this->base_url = variable_get('media_bynder_url', '');

        if ($parameters = $this->get_parameters()) {
            return media_bynder_embed_url($parameters['i']);
        } else {
            $parameters = media_bynder_uri_parse_style_url($this->uri);
            if ($parameters) {
                $build_url = media_bynder_embed_url($parameters['i'], $parameters['style'], $parameters['effects']);
                if (validateBynderDrupalUrl($build_url)) {
                    return $build_url;
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
