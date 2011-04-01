// Place your application-specific JavaScript functions and classes here
// This file is automatically included by javascript_include_tag :defaults

function getDomains () {
    $("#spinner_getdomains").show();
    $.ajax({
        url: "get_domains",
        type: "get",
        complete: function(){$("#spinner_getdomains").hide();},
    });
}
function refreshDomains() {
    $("#spinner_domains").show();
    $.ajax({
        url: "refresh_domains",
        type: "get",
        complete: function(){$("#spinner_domains").hide();},
        data: {
            domain : $("#domain").attr('value')
            }
    });
//    setTimeout("refreshDomains()",300000);
}
function getShares() {
    $("#spinner_getshares").show();
    $.ajax({
        url: "get_shares_and_users",
        type: "get",
        complete: function(){$("#spinner_getshares").hide();},
        data: {
            domain : $("#domain").attr('value')
            }
    });
}
function getFiles(){
    $("#spinner_getfiles").show();
    $.ajax({
        url: "get_files",
        type: "get",
        complete: function(){$("#spinner_getfiles").hide();},
        data: {
            domain: $("#domain").attr('value'),
            share: $("#share").attr('value'),
            user: $("#user").attr('value'),
            file:   $("#file").attr('value')
        }
    });
  //$("#files").oneTime(  6000, "files", refreshFiles());
}

function refreshShares() {
    $("#spinner_shares").show();
    $.ajax({
        url: "refresh_shares_and_users",
        type: "get",
        complete: function(){$("#spinner_shares").hide();},
        data: {
            domain : $("#domain").attr('value'),
            share:    $("#share").attr('value'),
            user:      $("#user").attr('value')
            }
    });
 //   setTimeout("refreshShares()",300000);
}
function refreshFiles(){
    $("#spinner_files").show();
    $.ajax({
        url: "refresh_files",
        type: "get",
        complete: function(){$("#spinner_files").hide();},
        data: {
            domain: $("#domain").attr('value'),
            share:   $("#share").attr('value'),
            user:     $("#user").attr('value'),
            file:       $("#file").attr('value')
        }
    });
  //$("#files").oneTime(  6000, "files", refreshFiles());
}
function globalOnSelectChange(){
    getDomains();
}
function domOnClickChange(){
    var selected = $("#domainlist option:selected");
    $("#domain").val(selected.val());
    getShares();
    if ($("div#files").length)
        $("#files").remove();
 //   $(document).stopTime("files");
}
function shareOnClickChange(){
   // $("#files").stopTime("files");
    var selected1 = $("#sharelist option:selected");
    var selected2 = $("#userlist option:selected");
    $("#share").val(selected1.val());
    $("#user").val(selected2.val());
    getFiles();
//    $(document).everyTime(  5000, "files", function(i){refreshFiles();}, 0);
}
function userOnClickChange(){
    var selected = $("#userlist option:selected");
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
var i = 0;
$(document).ready(function(){
    setInterval(function()
    {
            if (($("#domains").length) && (i == 0)) {
                refreshDomains();
            }
            if (($("#shares").length) && (i==1)) {
                refreshShares();
            }
            if (($("#files").length) && (i==2)) {
                refreshFiles();
            //$("#bla").append('<br/>test');
            }
            i = i+1;
            if (i==3)
                i = 0;
    }, 120000);
});
