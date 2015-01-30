<?php

/**
 * @file
 * media_bynder/includes/media_bynder.variables.inc
 * Variable defaults for Media: bynder.
 */

define('MEDIA_BYNDER_NAMESPACE', 'media_bynder_');
define('MEDIA_BYNDER_PAGER', '25');
define('MEDIA_BYNDER_RESTPOINT', '/api/v4');

function media_bynder_variable_get($name, $default = NULL) {
    if (!isset($default)) {
        $default = media_bynder_variable_default($name);
    }
    $variable_name = MEDIA_BYNDER_NAMESPACE . $name;
    return variable_get($variable_name, $default);
}

function media_bynder_variable_set($name, $value) {
    $variable_name = MEDIA_BYNDER_NAMESPACE . $name;
    return variable_set($variable_name, $value);
}

function media_bynder_variable_del($name){
    $variable_name = MEDIA_BYNDER_NAMESPACE . $name;
    variable_del($variable_name);
}

function media_bynder_variable_default($name = NULL) {
    static $defaults;

    if (!isset($defaults)) {
        $defaults = array();
    }

    if (!isset($name)) {
        return $defaults;
    }

    if (isset($defaults[$name])) {
        return $defaults[$name];
    }
}

function media_bynder_variable_name($name){
    return MEDIA_BYNDER_NAMESPACE . $name;
}
