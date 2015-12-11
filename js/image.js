/* Try to reload the image 5 times with a 1 second pause.
Do this by replacing to image source.
After 5 tries display the alt text to contact Bynder. */

(function ($) {
	var retries = 5;
	$( document).ready(function(){
		$('img').bind('error', function() {
			var $image = $(this);
			var $imgSrc = $image.attr('src');
			if (retries > 0 && $imgSrc !== undefined) {
				window.setTimeout(function(){
					$image.attr('src', $imgSrc);
					retries--;
				}, 1000);
			} else {
				$image.unbind('error');
				$image.attr('title', 'The image could not be retrieved, please contact Bynder Support');
				$image.attr('alt', 'The image could not be retrieved, please contact Bynder Support');
			}
		});
	});
})(jQuery);
