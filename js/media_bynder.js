(function ($) {
    $(document).ready(function(){
        $('#edit-bynder-search .facet_list > .facet_title').click(function() {
            $(this).find('.expand i').toggleClass('fa-angle-down').toggleClass('fa-angle-up');
            $(this).find(' > .item-list').slideToggle(300);
        });
    });
})(jQuery);
