<?php
/**
 * @file
 * media_youtube/includes/MediaBynderStreamWrapper.inc
 *
 * Create a Bynder Stream Wrapper class for the Media/Resource module.
 */

class MediaBynderStreamWrapper extends MediaReadOnlyStreamWrapper{

    public function interpolateUrl() {
      $id = media_bynder_id_from_uri($this->uri);
      return media_bynder_embed_url($id);
    }

    /**
     * Function getOriginalThumbnailPath().
     * @return string
     *   The path to the thumbnail.
     */
    public function getWebimagePath() {
        $id = media_bynder_id_from_uri($this->uri);
        return media_bynder_embed_url($id);
    }
}