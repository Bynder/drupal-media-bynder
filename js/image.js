/* Try to reload the image 5 times with a 1 second pause.
Do this by replacing to image source.
After 5 tries just display the alt text to contact Bynder. */

(function ($) {
	var retries = 5;
	$( document).ready(function(){
		$('.field-items .field-item img').one('error', function() {
			var $image = $(this);
			$image.attr('title', 'The image could not be retrieved, please contact Bynder Support');
			$image.attr('alt', 'The image could not be retrieved, please contact Bynder Support');
			if (typeof $image !== 'undefined') {
				if (typeof $image.attr('src') !== 'undefined') {
					$image.attr('src', retryToLoadImage($image));
				}
			}
		});
	});

	function retryToLoadImage($img) {
		var $newImg = $('<img>');
		var $src = ($img.attr('src')) || '';
		$newImg.attr('src', $src);
		$newImg.one('error', function() {
			window.setTimeout(function(){
				if (retries > 0) {
					retries--;
					retryToLoadImage($newImg);
				}
			}, 1000);
		});

		$newImg.one('load', function() {
			return $newImg.attr('src');
		});
	}
})(jQuery);
