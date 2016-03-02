/* Try to reload the image with a 3 second interval, max 3 times.
Do this by replacing the image source. */
jQuery(document).ready(function() {
    $retries = 3;
    jQuery('.field-items .field-item img, div > div > div > div > p > img').error(function() {
        $img = jQuery(this);
        setTimeout(function() {
            $img.attr('src', $img.attr('src'));
        }, 3000);
        $retries--;

        if($retries <= 0) {
            $img.unbind('error');
        }
    });
});
