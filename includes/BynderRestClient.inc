<?php

/**
 * @file
 * A Rest Client class for interaction with the Bynder REST-API
 */

class BynderRestClient {

    public $baseRestUrl;
    protected $options = array();
    public $error = '';

    public function __construct($baseRestUrl, $oauth_consumer,
                                $oauth_consumer_secret, $oauth_token,
                                $oauth_token_secret) {
        $this->baseRestUrl = $baseRestUrl;
        $this->oauth = array(
            'oauth_consumer' => $oauth_consumer,
            'oauth_consumer_secret' => $oauth_consumer_secret,
            'oauth_token' => $oauth_token,
            'oauth_token_secret' => $oauth_token_secret
        );
        $this->options = array(
            CURLOPT_RETURNTRANSFER => 1,
            CURLOPT_COOKIEJAR => "/tmp/cookie.txt",
            CURLOPT_SSL_VERIFYPEER => FALSE,
            CURLOPT_SSL_VERIFYHOST => FALSE,
            CURLOPT_HEADER => false
        );
    }

    public function test() {
        print_r($this->getMetaProperties());
        exit;
    }

    protected function initCurl() {
        $curl = curl_init();
        foreach ($this->options as $option => $value) {
            curl_setopt($curl, $option, $value);
        }
        return $curl;
    }

    protected function initOAUTH() {
        $oauth_consumer = new OAuthConsumer(
            $this->oauth['oauth_consumer'],
            $this->oauth['oauth_consumer_secret']
        );
        $oauth_token = new OAuthConsumer(
            $this->oauth['oauth_token'],
            $this->oauth['oauth_token_secret']
        );
        $sig_method = new OAuthSignatureMethod_HMAC_SHA1();
        $parsed = parse_url($this->options[CURLOPT_URL]);
        $params = array();
        if (isset($parsed['query'])) {
            parse_str($parsed['query'], $params);
        }

        $req_req = OAuthRequest::from_consumer_and_token(
            $oauth_consumer,
            $oauth_token,
            "GET",
            $this->options[CURLOPT_URL],
            $params
        );
        $req_req->sign_request($sig_method, $oauth_consumer, $oauth_token);

        $oauth_header = $req_req->to_header();
        $this->options[CURLOPT_HTTPHEADER] = array(
            $oauth_header,
            'Expect:'
        );
    }

    protected function execute() {
        $this->initOAUTH();
        $curl = $this->initCurl();

        $response = curl_exec($curl);
        $http_code = curl_getinfo($curl, CURLINFO_HTTP_CODE);
        $this->error = curl_error($curl);
        curl_close($curl);

        if (!$response) {
            return FALSE;
        } elseif ($http_code != 200) {
            $this->error = $response;
            return $http_code;
        } else {
            return $response;
        }
    }

    public function search($query, $start_index, $nr_of_results) {
        $url = $this->baseRestUrl . '/media/?count=1' . $query;

        if (isset($start_index) && $start_index > 0) {
            $page = $start_index/$nr_of_results;
            $url .= '&page=' . ($page + 1);
        }
        if (isset($nr_of_results) && $nr_of_results) {
            $url .= '&limit=' . $nr_of_results;
        }
        $this->options[CURLOPT_URL] = $url;
        return $this->execute();
    }

    public function brands() {
        $url = $this->baseRestUrl . '/brands/';
        $this->options[CURLOPT_URL] = $url;
        return $this->execute();
    }

    public function customResolutionUrl($file, $width, $height, $prefix=FALSE) {
        $retries = 5;
        if(isset($file['thumbnails']) && isset($file['idHash'])) {
            $thumbnails = $file['thumbnails'];
            $idHash  = $file['idHash'];
        } else {
            return FALSE;
        }

        if ($prefix) {
            $url = variable_get('media_bynder_url', '') . '/additional/';
            return $url . $prefix . '/media/' . $idHash . '?w=' . $width . '&h=' . $height;
        }

        foreach ($thumbnails as $prefix => $thumbnail_url) {
            $url = variable_get('media_bynder_url', '') . '/additional/';
            $url .= $prefix . '/media/' . $idHash . '?w=' . $width . '&h=' . $height;
            $this->options[CURLOPT_URL] = $url;
            $result = $this->execute();

            if ($result == 202) {
                while($retries > 0 && $result == 202) {
                    $result = $this->execute();
                    $retries--;
                    sleep(2);
                }
            } else if (!is_numeric($result) || $result == 302) {
                return $url;
            } else {
                $this->error = 'statuscode $result: for prefix $prefix.';
            }
        }
        return FALSE;
    }

    public function getMediaObject($id) {
        $this->options[CURLOPT_URL] = $this->baseRestUrl . '/media/' . $id . '/';
        return $this->execute();
    }

    public function getFacets($query) {
        $this->options[CURLOPT_URL] = $this->baseRestUrl . '/media/?count=1' . $query;
        $result = $this->execute();
        $result = json_decode($result);

        return $result->count;
    }

    public function getAll() {
        $this->options[CURLOPT_URL] = $this->baseRestUrl . '/media/?count=1';
        $result = $this->execute();
        $result = json_decode($result, TRUE);

        return $result;
    }

    public function getMetaProperties() {
        $this->options[CURLOPT_URL] = $this->baseRestUrl . '/metaproperties/';
        $result = $this->execute();

        $result = json_decode($result);
        $meta_properties = $this->limitMetaProperties($result);
        return $meta_properties;
    }
    private function limitMetaProperties($result) {
        $limited_meta_properties = [];
        foreach($result as $meta_property) {
            $limited_meta_property = [];
            $limited_meta_property['id'] = $meta_property->id;
            $limited_meta_property['label'] = $meta_property->label;
            $limited_meta_property['name'] = $meta_property->name;
            $limited_meta_property['zindex'] = $meta_property->zindex;

            foreach($meta_property->options as $option){
                $limited_meta_property['options'][] = array(
                    "id" => $option->id,
                    "label" => $option->label,
                    "name" => $option->name
                );
            }
            if(isset($limited_meta_property['options']) && count($limited_meta_property['options']) > 0) {
                $limited_meta_properties[] = $limited_meta_property;
            }
        }
        return $limited_meta_properties;
    }
}
