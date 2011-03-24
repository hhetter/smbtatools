// Place your application-specific JavaScript functions and classes here
// This file is automatically included by javascript_include_tag :defaults

function getDomains () {
    $.ajax({
        url: "get_domains",
        type: "get"
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
function getShares() {
    $.ajax({
        url: "get_shares",
        type: "get",
        data: {
            domain : $("#domain").attr('value')
            }
    });
}
function refreshShares() {
    $.ajax({
        url: "refresh_shares",
        type: "get",
        data: {
            domain : $("#domain").attr('value'),
            share:    $("#share").attr('value')
            }
    });
    setTimeout("refreshShares()",300000);
}
function domOnSelectChange(){
    var selected = $("#domains option:selected");
    $("#domain").val(selected.val());
}
function shareOnSelectChange(){
    var selected = $("#shares option:selected");
    $("#share").val(selected.val());
}
function startFunction(){
    var selected = $("#domfunc option:selected");
    selected = selected.val();
    $.ajax({
        url: "../" + selected +  "/initial",
        type:"get",
        data:{
            domain:$("#domain").attr('value')
        }
    });
}
$(document).ready(function(){
});

