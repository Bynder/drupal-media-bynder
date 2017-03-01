<?php

/**
 * @file
 * utility functions for the media_bynder module
 */

use \Bynder\Api\BynderApiFactory;

/**
 * Initialize the Bynder REST client.
 */
function media_bynder_init_api()
{

    $settings = array(
        'consumerKey' => variable_get('media_bynder_oauth_consumer', ''),
        'consumerSecret' => variable_get('media_bynder_oauth_consumer_secret', ''),
        'token' => variable_get('media_bynder_oauth_token', ''),
        'tokenSecret' => variable_get('media_bynder_oauth_token_secret', ''),
        'baseUrl' => variable_get('media_bynder_url', '')
    );
    return BynderApiFactory::create($settings);
}

function media_bynder_get_object_by_id($id)
{

    $bynderApi = media_bynder_init_api();
    $assetBank = $bynderApi->getAssetBankManager();
    return $assetBank->getMediaInfo($id)->wait();
}

/**
 * Create the embed url for a fragment.
 *
 * @param string $fragment_id idHash of entity.
 * @param string $type web, mini, thul
 * @param array $type effect
 * @todo add support for type and effect
 */

function media_bynder_embed_url($idHash, $image_style = false)
{
    $bynderEntity = db_select('bynder_media_entity', 'b')
        ->fields('b')
        ->condition('b.bynder_hash_id', $idHash, '=')
        ->execute()
        ->fetchAll();
    if (!empty($bynderEntity)) {
        $bynder_styles = unserialize(variable_get('media_bynder_derivatives'));
        $imageDerivatives = unserialize($bynderEntity[0]->derivatives);
        $styleName = str_replace('bynder_', '', $image_style);

        // Process custom Bynder derivatives
        if (in_array($styleName, $bynder_styles)) {
            if (isset($imageDerivatives[$styleName])) {
                return $imageDerivatives[$styleName];
            }
        }
        // Check if we have the webimage default derivative for that image.
        elseif(isset($imageDerivatives['webimage'])){
            return $imageDerivatives['webimage'];
        }
    }
    // If everything else fails we default to image not found.
    else {
        return $GLOBALS['base_url'] . '/' . drupal_get_path('module',
            'media_bynder') . '/assets/no-image.png';
    }
}

/**
 * Parse the uri.
 */
function media_bynder_idHash_from_uri($uri)
{
    $parts = explode('/', $uri);
    foreach ($parts as $part_key => $part) {
        if ($part == 'i') {
            if (count($parts) >= ($part_key + 1)) {
                return $parts[$part_key + 1];
            }
        }
    }
    return false;
}

function media_bynder_id_from_uri($uri)
{
    $parts = explode('/', $uri);
    foreach ($parts as $part_key => $part) {
        if ($part == 'f') {
            if (count($parts) >= ($part_key + 1)) {
                return $parts[$part_key + 1];
            }
        }
    }
    return false;
}

function media_bynder_id_from_url($url)
{
    $parts = explode('/', $url);
    foreach ($parts as $part_key => $part) {
        if ($part == 'l') {
            if (count($parts) >= ($part_key + 1)) {
                return $parts[$part_key + 1];
            }
        }
    }
    return false;
}

function isValid($type, $val)
{
    switch ($type) {
        case "uuid":
            return preg_match("/^[0-9a-fA-F]{8}-?[0-9a-fA-F]{4}-?[0-9a-fA-F]{4}-?[0-9a-fA-F]{4}-?[0-9a-fA-F]{12}$/",
                $val);
        case "idHash":
            return preg_match("/[a-f0-9]{16}/", $val);
        case "bynder_drupal_url":
            return preg_match("/bynder:\/\/f\/[0-9a-fA-F]{8}-?[0-9a-fA-F]{4}-?[0-9a-fA-F]{4}-?[0-9a-fA-F]{4}-?[0-9a-fA-F]{12}\/i\/[a-f0-9]{16}/",
                $val);
        case "id":
            return preg_match("/^[0-9a-f]{16}$/", $val);
    }
    return true;
}

function validateId($str)
{
    if (isValid('id', $str)) {
        return $str;
    } else {
        error_log("Invalid ID: " . $str);
        return false;
    }
}

function validateUUID($str)
{
    if (isValid("uuid", $str)) {
        return $str;
    } else {
        error_log("Invalid UUID: " . $str);
        return false;
    }
}

function validateIdHash($str)
{
    if (isValid('idHash', $str)) {
        return $str;
    } else {
        error_log("Invalid IDHash: " . $str);
        return false;
    }
}

function media_bynder_uri_parse_style_url($uri)
{
    $idHash = media_bynder_idHash_from_uri($uri);
    return variable_get('media_bynder_url', '') . '/l/' . $idHash . '/original/';
}

function validateBynderDrupalUrl($str)
{
    if (isValid('bynder_drupal_url', $str)) {
        return $str;
    } else {
        error_log("Invalid Bynder Drupal URL: " . $str);
        return false;
    }
}

function customResolutionUrl($file, $width, $height, $effect, $prefix = false)
{
    if (isset($file['idHash'])) {
        $idHash = $file['idHash'];
    } else {
        return false;
    }
    if (!$prefix) {
        $prefix = 'drupal_custom';
    }

    $bynder_url = variable_get('media_bynder_cdn_url', false);
    if (!$bynder_url) {
        $bynder_url = variable_get('media_bynder_url', '');
    }
    $url = $bynder_url . '/additional/';
    $effect = str_replace('bynder_custom_resolution_', '', $effect);
    if ($effect == 'resize') {
        return $url . $prefix . '/media/' . $idHash . '?w=' . $width . '&h=' . $height;
    } else {
        return $url . $prefix . '/media/' . $idHash . '?w=' . $width . '&h=' . $height . '&' . $effect . '=1';
    }
}

