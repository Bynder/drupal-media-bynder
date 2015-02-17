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

        // $this->test();
        // exit;
    }

    public function test() {
        $url = $this->baseRestUrl . '/metaproperties/';

        $this->options[CURLOPT_URL] = $url;
        var_dump($this->execute());
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
            return FALSE;
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

    public function getMediaObject($id) {
        $this->options[CURLOPT_URL] = $this->baseRestUrl . '/media/' . $id . '/';
        return $this->execute();
    }

    public function getFacets($query) {
        error_log($this->baseRestUrl . '/media/?count=1' . $query);
        $this->options[CURLOPT_URL] = $this->baseRestUrl . '/media/?count=1' . $query;
        $result = $this->execute();
        $result = json_decode($result);

        return $result->count;
    }
}
