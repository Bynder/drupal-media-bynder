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
      variable_get('media_bynder_url', '') . '/api/v4',
      variable_get('media_bynder_oauth_consumer', ''),
      variable_get('media_bynder_oauth_consumer_secret', ''),
      variable_get('media_bynder_oauth_token', ''),
      variable_get('media_bynder_oauth_token_secret', '')
    );
}

/**
 * Create the embed url for a fragment.
 *
 * @param string $fragment_id idHash of entity.
 * @param string $type web, mini, thul
 * @param array $type effect
 * @todo add support for type and effect
 */
function media_bynder_embed_url($fragment_id, $type=false, $effects=false, $name = null){
    $current_media = variable_get('current_media');

    if(isset($current_media)) {
        if ($current_media['idHash'] == $fragment_id) {
            return $current_media['thumbnails']['thul'];
        }
    }
     return FALSE;
}

/**
 * Parse the style out of the uri.
 */
function media_bynder_uri_parse_style_url($uri) {
    $fragment_id = media_bynder_uri_parse_url($uri)['i'];
    $current_media = variable_get('current_media');

    if(isset($current_media)) {
        if ($current_media['idHash'] == $fragment_id) {
            return $current_media['thumbnails']['thul'];
        }
    } else {
        return FALSE;
    }
}


/**
 * Parse the uri.
 */
function media_bynder_uri_parse_url($uri) {
    if(!validateBynderDrupalUrl($uri)){
        return FALSE;
    }
    $target = file_uri_target($uri);
    $parts = explode('/', $target);
    if(isset($parts[1]) && isset($parts[3])) {
        return array(
            "f" => $parts[3],
            "i" => $parts[1]
        );
    }else{
        return FALSE;
    }
}


function isValid($type, $val){
    switch($type){
        case "uuid":
            return preg_match("/^[0-9a-fA-F]{8}-?[0-9a-fA-F]{4}-?[0-9a-fA-F]{4}-?[0-9a-fA-F]{4}-?[0-9a-fA-F]{12}$/", val);
        case "idHash":
            return preg_match("/[a-f0-9]{16}/", $val);
        case "bynder_drupal_url":
            return preg_match("/bynder:\/\/f\/[0-9a-fA-F]{8}-?[0-9a-fA-F]{4}-?[0-9a-fA-F]{4}-?[0-9a-fA-F]{4}-?[0-9a-fA-F]{12}\/i\/[a-f0-9]{16}/", $val);
    }
    return true;
};

function validateUUID($str){
    if (isValid("uuid", $str)) {
        return $str;
    } else {
        error_log("Invalid UUID: " . $str);
        return FALSE;
    }
};

function validateIdHash($str){
    if (isValid('idHash', $str)) {
        return $str;
    } else {
        error_log("Invalid IDHash: " . $str);
        return FALSE;
    }
};

function validateBynderDrupalUrl($str){
    if (isValid('bynder_drupal_url', $str)) {
        return $str;
    } else {
        error_log("Invalid Bynder Drupal URL: " . $str );
        return FALSE;
    }
};
