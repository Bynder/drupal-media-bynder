(function ($) {
	$( document ).ready(function(){
		$('img').one('error', function() {
			$(this).attr("src", "/sites/all/modules/drupal-media-bynder-develop/assets/bynder.png");
		});
	});
})(jQuery);
