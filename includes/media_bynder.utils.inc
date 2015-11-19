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
    if (!$type) {
        $type = "web";
    }
    $base_url = variable_get('media_bynder_url', '');
    if (substr($base_url, -1) != '/') {
        $base_url .= '/';
    }

    if (validateId($fragment_id) == FALSE) {
        return FALSE;
    }

    $base_url .= 'm/' . $fragment_id . '/web-drupal.jpg';
    $query = [];

    if ($effects){
        foreach($effects as $effect_key => $effect){
            if ($effect_key == 'image_resize'){
                $query['h'] = $effect['height'];
                $query['w'] = $effect['width'];
            }
        }
    }
    if (count($query) == 0) {
        return $base_url;
    } else {
        return $base_url . '?' . http_build_query($query);
    }
}

/**
 * Parse the style out of the uri.
 */
function media_bynder_uri_parse_style_url($uri) {
    if (validateBynderDrupalUrl($uri) == FALSE) {
        return FALSE;
    }

    $target = file_uri_target($uri);
    $parts = explode('/', $target);
    if(isset($parts[1]) && isset($parts[4]) && isset($parts[6])){
        $data = array(
            "style" => $parts[1],
            "f" => $parts[4],
            "i" => $parts[6],
            "effects" => []
        );
        $image_styles = image_styles();
        $image_style = $image_styles[$data['style']];
        foreach($image_style['effects'] as $effect){
            $data['effects'][$effect['name']] = $effect['data'];
        }
        return $data;
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


function isValid($type, $val) {
    switch($type){
        case "uuid":
            return preg_match("/^[0-9a-fA-F]{8}-?[0-9a-fA-F]{4}-?[0-9a-fA-F]{4}-?[0-9a-fA-F]{4}-?[0-9a-fA-F]{12}$/", val);
        case "idHash":
            return preg_match("/[a-f0-9]{16}/", $val);
        case "bynder_drupal_url":
            return preg_match("/bynder:\/\/f\/[0-9a-fA-F]{8}-?[0-9a-fA-F]{4}-?[0-9a-fA-F]{4}-?[0-9a-fA-F]{4}-?[0-9a-fA-F]{12}\/i\/[a-f0-9]{16}/", $val);
        case "id":
            return preg_match("/^[0-9a-f]{16}$/", $val);
    }
    return true;
};

function validateId($str) {
    if (isValid('id', $str)) {
        return $str;
    } else {
        error_log("Invalid ID: " . $str);
        return FALSE;
    }
};

function validateUUID($str) {
    if (isValid("uuid", $str)) {
        return $str;
    } else {
        error_log("Invalid UUID: " . $str);
        return FALSE;
    }
};

function validateIdHash($str) {
    if (isValid('idHash', $str)) {
        return $str;
    } else {
        error_log("Invalid IDHash: " . $str);
        return FALSE;
    }
};

function validateBynderDrupalUrl($str) {
    if (isValid('bynder_drupal_url', $str)) {
        return $str;
    } else {
        error_log("Invalid Bynder Drupal URL: " . $str);
        return FALSE;
    }
};
