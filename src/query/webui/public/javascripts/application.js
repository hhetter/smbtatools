// Place your application-specific JavaScript functions and classes here
// This file is automatically included by javascript_include_tag :defaults

function getDomains () {
    $.ajax({
        url: "get_domains",
        type: "get"
    });
}
function getShares() {
    $.ajax({
        url: "get_shares",
        type: "get",
        data: {
            domain : $("#domain").attr('value')
            }
    });
}
function refreshDomains() {
    $.ajax({
        url: "refresh_domains",
        type: "get",
        data: {
            domain : $("#domain").attr('value')
            }
    });
    setTimeout("refreshDomains()",300000);
}
function onSelectChange(){
    var selected = $("#domains option:selected");
    $("#domain").val(selected.val());
}
$(document).ready(function(){
});

