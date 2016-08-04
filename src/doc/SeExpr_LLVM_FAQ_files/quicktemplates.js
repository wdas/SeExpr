var quickTemplates = {};

quickTemplates.copyTo = function(params) {
    var menu = $j('div#quicktemplates-copyto-menu').get(0);
    if (!$j(menu).is(":visible")) {
        $j(menu).html(jive.ext.quicktemplates.copyTo(params));
        var pos = $j('li#quicktemplates-copyto').offset();
        menu.style.left = (pos.left-($j(menu).width()+8)) + 'px';
        menu.style.top = (pos.top - 12) + 'px';
    } // end if
    $j('li#quicktemplates-copyto').toggleClass('quicktemplates-copyto-selected');
    $j(menu).toggle();
};

$j(function() {
    if ($j('li#quicktemplates-copyto').length > 0) {
        $j(document.body).append(jive.ext.quicktemplates.menu());
    } // end if
});