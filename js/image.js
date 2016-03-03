/* Try to reload the image with a 3 second interval, max 3 times.
Do this by replacing the image source. */
jQuery(document).ready(function() {
    $retries = 5;
    jQuery('.field-items .field-item img').error(function() {
        $img = jQuery(this);
        reloadImage($img);
    });

    jQuery("iframe").contents().find('div.preview img').error(function() {
        $img = jQuery(this);
        reloadImage($img);
    });

    jQuery("iframe").contents().find('iframe').contents().find('body[contenteditable] img').error(function() {
        $img = jQuery(this);
        reloadImage($img);
    });

    function reloadImage($img) {
        setTimeout(function() {
            $img.attr('src', $img.attr('src'));
        }, 3000);
        $retries--;

        if($retries <= 0) {
            $img.unbind('error');
        }
    }
});
