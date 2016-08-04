$j(function() {
    var $mainNavItem = $j('#quicklinks-userbar-menu').attr('class', 'j-nav-create').click(function() {
        $j('#quicklinks-userbar-menu-menu').popover({
            context        : $j(this),
            destroyOnClose : false,
            putBack        : true
        });

        return false;
    });

    // alter the DOM to the new format
    $mainNavItem.find(' > a span').addClass('j-ui-elem').append('<span class="j-ui-elem j-nav-more" />');

//    var $all = $j('.js-pop');
//     // exclude the current popover
//     var $filtered = $all.not($linksMenuPopover);
//     // select the first child of each match and trigger the close event on it
//     $filtered.children().find(' > .eq(0)').trigger('close');

    $j('#quicklinks-userbar-menu-more').remove();
});