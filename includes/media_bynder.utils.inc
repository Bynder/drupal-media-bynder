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
* @param array $media_item an array with with the complete selected image data
* @return the same array with an extra array call 'details', which contains width, heigth, filesize and the url if available. Also builds an url for the original public url, stored in '-'.
**/

function media_bynder_media_item_detailed($media_item) {
    $detailed_media_item = array();

    if (isset($media_item['thumbnails'])) {
        foreach ($media_item['thumbnails'] as $media_item_prefix => $media_item_url) {
           try {
                $media_size = @getimagesize($media_item_url);
           } catch (Exception $e) {
                $media_size = [0, 0];
           }

            $media_width = $media_size[0];
            $media_height = $media_size[1];

            if (isset(array_change_key_case(get_headers($media_item_url, TRUE))['content-length'])) {
                $media_filesize = array_change_key_case(get_headers($media_item_url, TRUE))['content-length'];
            } else {
                $media_filesize = 0;
            }

            $detailed_media_item = [
                'width' => $media_width,
                'height' => $media_height,
                'filesize' => $media_filesize,
                'url' => $media_item_url
            ];
            $media_item['details'][$media_item_prefix] = $detailed_media_item;
        }
    }
    return $media_item;
}

function media_bynder_get_object_by_hash($idHash) {
     $rest_client = media_bynder_rest_client();
    $current_results = $url = $rest_client->getAll()['media'];

    foreach ($current_results as $current_result) {
        if ($current_result['idHash'] == $id) {
            return $current_result;
        }
    }
    return FALSE;
}

function media_bynder_get_object_by_id($id) {
    $rest_client = media_bynder_rest_client();
    $current_results = $url = $rest_client->getAll()['media'];

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
        $rest_client = media_bynder_rest_client();
        $url = $rest_client->customResolutionUrl($media_item, $effects['width'], $effects['height']);
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
    if (validateBynderDrupalUrl($uri) == FALSE) {
        return FALSE;
    }

    $target = file_uri_target($uri);
    $parts = explode('/', $target);
    if (isset($parts[1]) && isset($parts[4]) && isset($parts[6])) {
        $data = array(
            "style" => $parts[1],
            "f" => $parts[4],
            "i" => $parts[6],
            "effects" => []
        );
        $image_styles = image_styles();
        $image_style = $image_styles[$data['style']];
        foreach($image_style['effects'] as $effect) {
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
            return preg_match("/^[0-9a-fA-F]{8}-?[0-9a-fA-F]{4}-?[0-9a-fA-F]{4}-?[0-9a-fA-F]{4}-?[0-9a-fA-F]{12}$/", val);
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
};

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
