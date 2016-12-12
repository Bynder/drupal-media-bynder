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

function media_bynder_get_object_by_id($id) {
    $rest_client = media_bynder_rest_client();
    return json_decode($rest_client->getMediaObject($id), TRUE);
}

/**
 * Create the embed url for a fragment.
 *
 * @param string $fragment_id idHash of entity.
 * @param string $type web, mini, thul
 * @param array $type effect
 * @todo add support for type and effect
 */

function media_bynder_embed_url($fragment_id, $type=FALSE, $effects=FALSE, $idHash=FALSE) {
    $url = '';
     if (!$type) {
        $type = 'bynder_custom';
    }

    if(isset($fragment_id) && !$idHash) {
        $media_item = media_bynder_get_object_by_id($fragment_id);
    } else {
        $media_item = ['idHash' => $fragment_id];
    }

    if ($effects) {
        $url = customResolutionUrl($media_item, $effects['data']['width'], $effects['data']['height'], $effects['name']);
    } else if (isset($media_item['thumbnails'][$type])) {
        $url = $media_item['thumbnails'][$type];
    }
    if (!$url) {
        //fallback image
        $url = $GLOBALS['base_url'] . '/' . drupal_get_path('module', 'media_bynder') . '/assets/no-image.png';
    }
    return $url;
}

/**
 * Parse the uri.
 */
function media_bynder_idHash_from_uri($uri) {
    $parts = explode('/', $uri);
    foreach ($parts as $part_key => $part) {
        if ($part == 'i') {
            if (count($parts) >= ($part_key + 1)) {
                return $parts[$part_key + 1];
            }
        }
    }
    return FALSE;
}

function media_bynder_id_from_uri($uri) {
    $parts = explode('/', $uri);
    foreach ($parts as $part_key => $part) {
        if ($part == 'f') {
            if (count($parts) >= ($part_key + 1)) {
                return $parts[$part_key + 1];
            }
        }
    }
    return FALSE;
}

function media_bynder_id_from_url($url) {
    $parts = explode('/', $url);
    foreach ($parts as $part_key => $part) {
        if ($part == 'l') {
            if (count($parts) >= ($part_key + 1)) {
                return $parts[$part_key + 1];
            }
        }
    }
    return FALSE;
}

function isValid($type, $val) {
    switch($type) {
        case "uuid":
            return preg_match("/^[0-9a-fA-F]{8}-?[0-9a-fA-F]{4}-?[0-9a-fA-F]{4}-?[0-9a-fA-F]{4}-?[0-9a-fA-F]{12}$/", $val);
        case "idHash":
            return preg_match("/[a-f0-9]{16}/", $val);
        case "bynder_drupal_url":
            return preg_match("/bynder:\/\/f\/[0-9a-fA-F]{8}-?[0-9a-fA-F]{4}-?[0-9a-fA-F]{4}-?[0-9a-fA-F]{4}-?[0-9a-fA-F]{12}\/i\/[a-f0-9]{16}/", $val);
        case "id":
            return preg_match("/^[0-9a-f]{16}$/", $val);
    }
    return true;
}

function validateId($str) {
    if (isValid('id', $str)) {
        return $str;
    } else {
        error_log("Invalid ID: " . $str);
        return FALSE;
    }
}

function validateUUID($str) {
    if (isValid("uuid", $str)) {
        return $str;
    } else {
        error_log("Invalid UUID: " . $str);
        return FALSE;
    }
}

function validateIdHash($str) {
    if (isValid('idHash', $str)) {
        return $str;
    } else {
        error_log("Invalid IDHash: " . $str);
        return FALSE;
    }
}

function media_bynder_uri_parse_style_url($uri) {
    $idHash = media_bynder_idHash_from_uri($uri);
    return variable_get('media_bynder_url', '')  .  '/l/' .  $idHash . '/original/';
}

function validateBynderDrupalUrl($str) {
    if (isValid('bynder_drupal_url', $str)) {
        return $str;
    } else {
        error_log("Invalid Bynder Drupal URL: " . $str);
        return FALSE;
    }
}


function customResolutionUrl($file, $width, $height, $effect, $prefix=FALSE) {
    if (isset($file['idHash'])) {
        $idHash = $file['idHash'];
    } else {
        return FALSE;
    }
    if (!$prefix) {
        $prefix = 'drupal_custom';
    }

    $bynder_url = variable_get('media_bynder_cdn_url', false);
    if(!$bynder_url) {
        $bynder_url = variable_get('media_bynder_url', '');
    }
    $url =  $bynder_url . '/additional/';
    $effect = str_replace('bynder_custom_resolution_', '', $effect);
    if ($effect == 'resize') {
        return $url . $prefix . '/media/' . $idHash . '?w=' . $width . '&h=' . $height;
    } else {
        return $url . $prefix . '/media/' . $idHash . '?w=' . $width . '&h=' . $height . '&' . $effect . '=1';
    }
}

