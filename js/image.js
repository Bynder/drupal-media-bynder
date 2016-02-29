/* Try to reload the image with a 3 second interval.
Do this by replacing to image source. */
jQuery(document).ready(function() {
    $retries = 3;
    jQuery('.field-items .field-item img').error(function() {
        $img = jQuery(this);
        setTimeout(function() {
            $img.attr('src', $img.attr('src'));
        }, 3000);
        $retries--;
    if(retries < 0){
        jQuery('.field-items .field-item img').unbind('error');
    }
    });
});
