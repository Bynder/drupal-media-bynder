(function ($) {
    function showAlert(message, type) {
        $('#edit-bynder-search .alert').find('span.text').text(message);
        $('#edit-bynder-search .alert').removeClass('alert-warning').removeClass('alert-success').removeClass('alert-error');
        $('#edit-bynder-search .alert').addClass('alert-' + type);
        $('#edit-bynder-search .alert').fadeIn();
        setTimeout(function() {
            $('#edit-bynder-search .alert').fadeOut(1000);
        }, 2500);
    }
    $(document).ready(function(){
        $('#edit-bynder-search .facet_list > .facet_title').click(function() {
            $(this).find('.expand i').toggleClass('fa-angle-down').toggleClass('fa-angle-up');
            $(this).find(' > .item-list').slideToggle(300);
        });

        $('#edit-bynder-search .result_list .bynder-image').click(function(e){
            e.preventDefault();
            e.stopPropagation();
            var $image = $(e.currentTarget);
            $image.addClass('loading');

            var spinner = new Spinner({
                lines: 13,
                length: 20,
                width: 10,
                radius: 30,
                corners: 1,
                rotate: 0,
                direction: 1,
                color: '#000',
                speed: 1,
                trail: 60,
                shadow: false,
                hwaccel: false,
                className: 'spinner',
                zIndex: 2e9,
                top: '50%',
                left: '50%'
            }).spin($image[0]);

            var id = $image.attr('data-id');
            var idHash = $image.attr('data-idHash');

            $.ajax({
                url: '/?q=admin/content/media/add/media_bynder',
                type: 'POST',
                dataType: 'json',
                data: {
                    id: id,
                    idHash: idHash
                },
                success: function(data) {
                    $image.removeClass('loading');
                    spinner.stop();
                    showAlert(data.message, data.type);
                }
            });
        });
    });
})(jQuery);
