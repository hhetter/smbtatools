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
        url: "get_shares_and_users",
        type: "get",
        data: {
            domain : $("#domain").attr('value')
            }
    });
}
function getFiles(){
    $.ajax({
        url: "get_files",
        type: "get",
        data: {
            domain: $("#domain").attr('value'),
            share: $("#share").attr('value'),
            user: $("#user").attr('value')
        }
    });
}
function refreshShares() {
    $.ajax({
        url: "refresh_shares_and_users",
        type: "get",
        data: {
            domain : $("#domain").attr('value'),
            share:    $("#share").attr('value'),
            user:      $("#user").attr('value')
            }
    });
    setTimeout("refreshShares()",300000);
}
function refreshFiles(){
    $.ajax({
        url: "refresh_files",
        type: "get",
        data: {
            domain: $("#domain").attr('value'),
            share:   $("#share").attr('value'),
            user:     $("#user").attr('value')
        }
    });
    setTimeout("refreshFiles()", 300000);
}
function globalOnSelectChange(){
    getDomains();
}
function domOnSelectChange(){
    var selected = $("#domains option:selected");
    $("#domain").val(selected.val());
    getShares();
    if ($("div#files").length)
        $("#files").remove();
}
function shareOnSelectChange(){
    var selected1 = $("#shares option:selected");
    var selected2 = $("#users option:selected");
    $("#share").val(selected1.val());
    $("#user").val(selected2.val());
    getFiles();
}
function userOnSelectChange(){
    var selected = $("#users option:selected");
    $("#user").val(selected.val());
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

