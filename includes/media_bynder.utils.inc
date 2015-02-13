<?php

/**
 * @file
 * utility functions for the media_bynder module
 */

/**
 * Initialize the Bynder REST client.
 */
function media_bynder_rest_client() {
    return new BynderRestClient(
        media_bynder_variable_get('url') . MEDIA_BYNDER_RESTPOINT,
        media_bynder_variable_get('oauth_consumer'),
        media_bynder_variable_get('oauth_consumer_secret'),
        media_bynder_variable_get('oauth_token'),
        media_bynder_variable_get('oauth_token_secret')
    );
}

/**
 * Create the embed url for a fragment.
 *
 * @param string $fragment_id idHash of entity.
 * @param string $type web, mini, thul
 */
function media_bynder_embed_url($fragment_id, $type){
    if(!$type){
        $type = "web";
    }
    $base_url = media_bynder_variable_get('url');
    if (substr($base_url, -1) != '/') {
        $base_url .= '/';
    }
    $base_url .= 'm/' . $fragment_id . '/' . $type . '-drupal.jpg';

    return $base_url;
}


/**
 * Parse the style out of the uri.
 */
function media_bynder_uri_parse_style_url($uri) {
    $target = file_uri_target($uri);
    $parts = explode('/', $target);
    if(isset($parts[1]) && isset($parts[4]) && isset($parts[6])){
        return array(
            "style" => $parts[1],
            "f" => $parts[4],
            "i" => $parts[6]
        );
    }else{
        return FALSE;
    }
}
