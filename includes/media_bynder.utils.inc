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

function media_bynder_get_object_by_hash($idHash) {
    $all_results = variable_get('all_results');
    foreach ($all_results as $current_result) {
        if ($current_result['idHash'] == $idHash) {
            return $current_result;
        }
    }

    $rest_client = media_bynder_rest_client();
    $current_results = $rest_client->getAll()['media'];

    foreach ($current_results as $current_result) {
        if ($current_result['idHash'] == $idHash) {
            return $current_result;
        }
    }
    return FALSE;
}

function media_bynder_get_object_by_id($id) {
    $all_results = variable_get('all_results');
    foreach ($all_results as $current_result) {
        if ($current_result['id'] == $id) {
            return $current_result;
        }
    }

    $rest_client = media_bynder_rest_client();
    $current_results = $rest_client->getAll()['media'];

    foreach ($current_results as $current_result) {
        if ($current_result['id'] == $id) {
            return $current_result;
        }
    }
    return FALSE;
}

/**
 * Create the embed url for a fragment.
 *
 * @param string $fragment_id idHash of entity.
 * @param string $type web, mini, thul
 * @param array $type effect
 * @todo add support for type and effect
 */

function media_bynder_embed_url($fragment_id, $type=FALSE, $effects=FALSE, $media_item=FALSE, $fragment_idHash=FALSE) {
    $url = '';
     if (!$type) {
        $type = "thul";
    }
    if (!$media_item) {
            if (isset($fragment_id)) {
                $media_item = media_bynder_get_object_by_id($fragment_id);
            } else {
                if ($fragment_idHash) {
                    $media_item = media_bynder_get_object_by_hash($fragment_idHash);
                }
            }
    }

    if ($effects) {
        $url = customResolutionUrl($media_item, $effects['width'], $effects['height']);
    } else if (isset($media_item['thumbnails'][$type])) {
        $url = $media_item['thumbnails'][$type];
    } else {
        return FALSE;
    }
    return $url;
}

/**
 * Parse the style out of the uri.
 */
function media_bynder_uri_parse_style_url($uri) {
    $idHash = media_bynder_idHash_from_uri($uri);
    return variable_get('media_bynder_url', '')  .  '/m/' .  $idHash . '/original/';
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

function validateBynderDrupalUrl($str) {
    if (isValid('bynder_drupal_url', $str)) {
        return $str;
    } else {
        error_log("Invalid Bynder Drupal URL: " . $str);
        return FALSE;
    }
}

function customResolutionUrl($file, $width, $height, $prefix=FALSE) {
    if (isset($file['idHash'])) {
        $idHash  = $file['idHash'];
    } else {
        return FALSE;
    }

    if (!$prefix) {
        $prefix = 'thul';
    }
    $url = variable_get('media_bynder_url', '') . '/additional/';
    return $url . $prefix . '/media/' . $idHash . '?w=' . $width . '&h=' . $height;
}

