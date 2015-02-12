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
            $parameters = media_bynder_uri_parse_style_url($this->uri);
            if($parameters){
                return media_bynder_embed_url($parameters['i'], $parameters['style']);
            }
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

}
