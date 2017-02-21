<?php
/**
 * @file
 * media_youtube/includes/MediaBynderStreamWrapper.inc
 *
 * Create a Bynder Stream Wrapper class for the Media/Resource module.
 */

class MediaBynderStreamWrapper extends MediaReadOnlyStreamWrapper{

    public function interpolateUrl() {
       return @media_bynder_uri_parse_style_url($this->uri);
    }

    /**
     * Function getOriginalThumbnailPath().
     * @return string
     *   The path to the thumbnail.
     */
    public function getOriginalThumbnailPath() {
        return @media_bynder_embed_url($this->parameters['f']);
    }

    /**
     * Function getWebimagePath().
     * @return string
     *   The path to the image.
     */
    public function getWebimagePath($type=FALSE, $effects=FALSE, $idHash=FALSE) {
        return @media_bynder_embed_url($this->parameters['f'], $type, $effects, $idHash);
    }
}
