(function ($) {
    'use strict';

    var spinner;

    var showAlert = function(message, type) {
        var alert = $('#edit-bynder-search .alert');
            alert.find('span.text').text(message);
            alert.removeClass('alert-warning').removeClass('alert-success').removeClass('alert-error');
            alert.addClass('alert-' + type);
            alert.fadeIn();
        setTimeout(function() {
            alert.fadeOut(1000);
        }, 2500);
    };

    var showSpinner = function(element) {
        spinner = new Spinner({
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
        }).spin(element);
    };

    // Public variables/methods
    var isValid = function(type, val){
        switch(type){
            case "uuid":
                return /^[0-9a-fA-F]{8}-?[0-9a-fA-F]{4}-?[0-9a-fA-F]{4}-?[0-9a-fA-F]{4}-?[0-9a-fA-F]{12}$/.test(val);
            case "idHash":
                return /[a-f0-9]{16}/.test(val);
        }
        return true;
    };

    var validateUUID = function(str){
        if (isValid("uuid", str)) {
            return str;
        } else {
            throw "Invalid UUID: "+str;
        }
    };

    var validateIdHash = function(str){
        if (isValid("idHash", str)) {
            return str;
        } else {
            throw "Invalid IDHash: "+str;
        }
    };

    $(document).ready(function(){
        $('#edit-bynder-search .normal_facet_list > .facet_title').click(function() {
            var $filters = $(this).siblings();
            $filters.find('.expand i.fa-angle-up')
                .addClass('fa-angle-down')
                .removeClass('fa-angle-up');
            $filters.find('.item-list:visible').slideUp(300);

            $(this).find('.expand i').toggleClass('fa-angle-down').toggleClass('fa-angle-up');
            $(this).find(' > .item-list').slideToggle(300);
        });

        $('#edit-bynder-search .selected_facet_list > .facet_title').click(function(e) {
            var link = $(e.currentTarget);
            var filters_input = $('#edit-bynder-search input[name="filters"]');

            var filter_key = link.data('filter-key');
            var current_filters = JSON.parse(filters_input.val() || '{}');
            current_filters.filters = current_filters.filters || [];

            var exists = ($.grep(current_filters.filters, function(e, i){
                return e.key == filter_key;
            }).length);
            if(exists){
                //Remove current filter from list
                current_filters.filters = $.grep(current_filters.filters, function(e, i){
                    return e.key != filter_key;
                });
                link.removeClass('active');
            }
            filters_input.val(JSON.stringify(current_filters));
            link.parent().fadeOut(300, function() { $(this).remove(); });

            var input = $('<input>', {type: 'hidden', name: 'source_filter', value: $(this).attr('data-filter-key')});
            $('#media-bynder-add').append($(input));
            if ($(this).attr('data-filter-key') == 'bynder_remove_all_facets_filter_key'){
                $('#edit-bynder-search input[name="filters"]').val('{"filters":[]}');
            }

            $('#media-bynder-add').submit();
        });

        $('#edit-bynder-search .filter-url').click(function(e) {
            e.preventDefault();

            //go back to the first page
            var old_form_action = $('#media-bynder-add').attr('action');
            var new_form_action = old_form_action.replace(/(page=)[^\&]+/, '$1' + '0');
            $('#media-bynder-add').attr('action', new_form_action);

            var link = $(e.currentTarget);
            var filter_key = link.data('filter-key');
            var filter_value = link.data('filter-value');

            var filters_input = $('#edit-bynder-search input[name="filters"]');

            var current_filters = JSON.parse(filters_input.val() || '{}');
            current_filters.filters = current_filters.filters || [];

            var exists = ($.grep(current_filters.filters, function(e, i){
                return e.key == filter_key;
            }).length);
            if (exists) {
                 //Remove current filter from list
                current_filters.filters = $.grep(current_filters.filters, function(e, i){
                    return e.key != filter_key;
                });
                link.removeClass('active');
            }

            // Add current filter to list
            current_filters.filters.push({
                key: filter_key,
                value: filter_value,
            });
            link.addClass('active');

            filters_input.val(JSON.stringify(current_filters));
            $('#media-bynder-add').submit();
        });

        $('#edit-bynder-search .result_list .bynder-image').click(function(e){
            e.preventDefault();
            e.stopPropagation();
            var $image = $(e.currentTarget);
            $image.addClass('loading');

            showSpinner($image[0]);

            var id = $image.attr('data-id');
            var idHash = $image.attr('data-idHash');

            var media_library_mode = $('body').hasClass('page-admin-content-media-add-media-bynder');

            if(media_library_mode){
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
            }else{
                $('input[name="selected_asset"]').val('bynder://f/' + validateUUID(id) + '/i/' + validateIdHash(idHash));
                $('#media-bynder-add').submit();
            }
        });

        $('#media-bynder-add').submit(function() {
            var media_library_mode = $('body').hasClass('page-admin-content-media-add-media-bynder');
            if(media_library_mode){
                showSpinner($('#edit-bynder-search')[0]);
            }
        });
    });
})(jQuery);
