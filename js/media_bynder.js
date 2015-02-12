(function ($) {
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

            var opts = {
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
            };
            var spinner = new Spinner(opts).spin($image[0]);

            var id = $image.attr('data-id');
            var idHash = $image.attr('data-idHash');

            $.ajax({
                url: '/admin/content/media/add/media_bynder',
                type: 'GET',
                dataType: 'json',
                data: {
                    id: id,
                    idHash: idHash
                },
                success: function(data, textStatus) {
                    $image.removeClass('loading');
                    spinner.stop();
                    $(".alert").find('span.text').text(data.message);
                    $(".alert").removeClass('alert-warning').removeClass('alert-success').removeClass('alert-error');
                    $(".alert").addClass('alert-' + data.type);
                    $(".alert").fadeIn();
                    setTimeout(function() {
                        $(".alert").fadeOut(1000);
                    }, 2500);
                    console.log("success", data, textStatus);
                },
                error: function(errorObj, textStatus, errorThrown) {
                    $image.removeClass('loading');
                    spinner.stop();
                    $(".alert").find('span.text').text(data.message);
                    $(".alert").removeClass('alert-warning').removeClass('alert-success').removeClass('alert-error');
                    $(".alert").addClass('alert-' + data.type);
                    $(".alert").fadeIn();
                    setTimeout(function() {
                        $(".alert").fadeOut(1000);
                    }, 2500);
                    console.log("success", data, textStatus);
                    console.log("error", errorObj, textStatus, errorThrown);
                }
            });
        });
    });
})(jQuery);
