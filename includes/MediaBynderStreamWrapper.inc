<?php

/**
 * @file
 * media_youtube/includes/MediaBynderStreamWrapper.inc
 *
 * Create a Bynder Stream Wrapper class for the Media/Resource module.
 */
class MediaBynderStreamWrapper extends MediaReadOnlyStreamWrapper
{

    public function interpolateUrl()
    {
        return @media_bynder_uri_parse_style_url($this->uri);
    }

    /**
     * Function getWebimagePath().
     * @return string
     *   The path to the image.
     */
    public function getWebimagePath($image_style)
    {
        return @media_bynder_embed_url($this->parameters['i'], $image_style);
    }
}
