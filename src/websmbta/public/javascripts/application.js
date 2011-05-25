// Place your application-specific JavaScript functions and classes here
// This file is automatically included by javascript_include_tag :defaults

function getDomains () {
    $("#spinner_getdomains").show();
    $.ajax({
        url: "get_domains",
        type: "get",
        complete: function(){$("#spinner_getdomains").hide();}
    });
}
function refreshDomains() {
    $("#spinner_domains").show();
    var checksum;
    $('#domainlist option').each(function(){
        checksum += $(this).val();
    });
    checksum = $.md5(checksum);
    $.ajax({
        url: "refresh_domains",
        type: "get",
        complete: function(){
            $("#spinner_domains").hide();
            var new_checksum;
            $('#domainlist option').each(function(){
                new_checksum += $(this).val();
            });
            new_checksum = $.md5(new_checksum);
            if (checksum != new_checksum)
                $("#dom_diff").show();
        },
        data: {
            checksum: checksum,
            domain : $("#domain").attr('value')
        }
    });
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
}

function refreshShares() {
    $("#spinner_shares").show();
    var checksum_shares;
    var checksum_users;
    $('#sharelist option').each(function(){
        checksum_shares += $(this).val();
    });
    checksum_shares = $.md5(checksum_shares);
    $('#userlist option').each(function(){
        checksum_users += $(this).val();
    });
    checksum_users = $.md5(checksum_users);
    $.ajax({
        url: "refresh_shares_and_users",
        type: "get",
        complete: function(){$("#spinner_shares").hide();
        var new_checksum_shares;
        var new_checksum_users;
            $('#sharelist option').each(function(){
                new_checksum_shares += $(this).val();
            });
            new_checksum_shares = $.md5(new_checksum_shares);
            if (checksum_shares != new_checksum_shares)
                $("#share_diff").show();
       $('#userlist option').each(function(){
                new_checksum_users += $(this).val();
            });
            new_checksum_users = $.md5(new_checksum_users);
            if (checksum_users != new_checksum_users)
                $("#user_diff").show();
        },
        data: {
            domain : $("#domain").attr('value'),
            share:    $("#share").attr('value'),
            user:      $("#user").attr('value')
            }
    });
}
function refreshFiles(){
    $("#spinner_files").show();
     var checksum;
     $('#filelist option').each(function(){
        checksum += $(this).val();
    });
    checksum = $.md5(checksum);
    $.ajax({
        url: "refresh_files",
        type: "get",
        complete: function(){$("#spinner_files").hide();
        var new_checksum;
            $('#filelist option').each(function(){
                new_checksum += $(this).val();
            });
            new_checksum = $.md5(new_checksum);
            if (checksum != new_checksum)
                $("#file_diff").show();
        },
        data: {
            domain: $("#domain").attr('value'),
            share:   $("#share").attr('value'),
            user:     $("#user").attr('value'),
            file:       $("#file").attr('value')
        }
    });
}
function globalOnSelectChange(){
    $("#domain").val("");
    $("#share").val("");
    $("#user").val("");
    $("#file").val("");
    $("#domainlist option:selected").val("");
    $("#sharelist option:selected").val("");
    $("#userlist option:selected").val("");
    $("#filelist option:selected").val("");
    if ($("div#shares_and_users").length){
        $("#files").remove();
        $("#shares_and_users").remove();
        $("#shares_and_users").remove();
    }
    getDomains();
}
function domOnClickChange(){
    var selected = $("#domainlist option:selected");
    $("#dom_diff").hide();
    $("#share").val("");
    $("#user").val("");
    $("#file").val("");
    $("#sharelist option:selected").val("");
    $("#userlist option:selected").val("");
    $("#filelist option:selected").val("");
    $("input#domain").val(selected.val());
    getShares();
    if ($("div#files").length)
        $("#files").remove();
}
function shareOnClickChange(){
    var selected1 = $("#sharelist option:selected");
    var selected2 = $("#userlist option:selected");
    $("#file").val("");
    $("#filelist option:selected").val("");
    $("#share_diff").hide();
    $("#user_diff").hide();
    $("input#share").val(selected1.val());
    $("input#user").val(selected2.val());
    getFiles();

}
function fileOnClickChange(){
    var selected = $("#filelist option:selected");
    $("#file_diff").hide();
    $("input#file").val(selected.val());
}
function configureFunction(){
    var selected = $(".function option:selected").val();
    $("select.objectlist").val("");
    $("select.modelist").val("");
    $("input.number").val("");
    $("input.search").val("");
    if (selected == "search"){
        $("#help").fadeOut();
        $(".function_help").fadeOut();
        $("#total").fadeOut();
        $("#last_activity").fadeOut();
        $("#top").fadeOut();
        $("#usage").fadeOut();
        $("#search").fadeIn();
        $("#search_help").fadeIn();
        $("#throughput").fadeOut();
        $("#list_func").fadeOut();
    }
    else if (selected == "list"){
        $("#help").fadeOut();
        $(".function_help").fadeOut();
        $("#total").fadeOut();
        $("#last_activity").fadeOut();
        $("#top").fadeOut();
        $("#usage").fadeOut();
        $("#search").fadeOut();
        $("#search_help").fadeOut();
        $("#throughput").fadeOut();
        $("#list_func").fadeIn();
        $("#list_help").fadeIn();
    }
    else if (selected == "usage"){
        $("#help").fadeOut();
        $(".function_help").fadeOut();
        $("#search").fadeOut();
        $("#total").fadeOut();
        $("#last_activity").fadeOut();
        $("#top").fadeOut();
        $("#usage").fadeIn();
        $("#usage_help").fadeIn();
        $("#throughput").fadeOut();
        $("#list_func").fadeOut();
    }
    else if (selected == "total"){
        $("#help").fadeOut();
        $(".function_help").fadeOut();
        $("#search").fadeOut();
        $("#usage").fadeOut();
        $("#top").fadeOut();
        $("#last_activity").fadeOut();
        $("#total").fadeIn();
        $("#total_help").fadeIn();
        $("#throughput").fadeOut();
        $("#list_func").fadeOut();
    }
    else if (selected == "last_activity"){
        $(".function_help").fadeOut();
        $("#help").fadeOut();
        $("#search").fadeOut();
        $("#last_activity").fadeIn();
        $("#last_activity_help").fadeIn();
        $("#usage").fadeOut();
        $("#total").fadeOut();
        $("#top").fadeOut();
        $("#throughput").fadeOut();
        $("#list_func").fadeOut();
    }
    else if (selected == "top"){
        $(".function_help").fadeOut();
        $("#help").fadeOut();
        $("#search").fadeOut();
        $("#top").fadeIn();
        $("#top_help").fadeIn();
        $("#usage").fadeOut();
        $("#total").fadeOut();
        $("#last_activity").fadeOut();
        $("#throughput").fadeOut();
        $("#list_func").fadeOut();
    }
    else if (selected == "throughput"){
        $(".function_help").fadeOut();
        $("#help").fadeOut();
        $("#search").fadeOut();
        $("#last_activity").fadeOut();
        $("#usage").fadeOut();
        $("#total").fadeOut();
        $("#top").fadeOut();
        $("#throughput").fadeIn();
        $("#throughput_help").fadeIn();
        $("#list_func").fadeOut();
    }

    else{
        $("#help").fadeIn();
        $("#search").fadeOut();
        $("#usage").fadeOut();
        $("#total").fadeOut();
        $("#top").fadeOut();
        $("#last_activity").fadeOut();
        $("#throughput").fadeOut();
        $("#list_func").fadeOut();
    }
    inspect_functionsettings();
}
function start_function(){
     $("#spinner_functions").show();
     $('input.start').attr('disabled', 'disabled');
    var func = $("select#global_function option:selected").val();
    $.ajax({
        url: "../functions/start_function",
        type: "get",
        complete: function(){$("#spinner_functions").hide();
            reset_functions();
        },
        data: {
            domain: $("#domain").attr('value'),
            share:   $("#share").attr('value'),
            user:     $("#user").attr('value'),
            file:       $("#file").attr('value'),
            search: $("input#search").attr('value'),
            object:  $("#objectlist_" + func + " option:selected").val(),
            mode:   $("#modelist_" + func + " option:selected").val(),
            sort:      $("#sortlist_" + func + " option:selected").val(),
            number:$("#number_" + func).val(),
            timemode: $("select#timelist option:selected").val(),
            func:     $("select#global_function option:selected").val()
        }
    })
}
function reset_functions(){
    $(".function_dialog").fadeOut();
    $(".function").val("");
    $("#help").fadeIn();
}
function inspect_functionsettings(){
    if($("div#search").css("display") != "none"){
        if($("input#search").val().length != 0){
            $(".start").removeAttr("disabled");
        }
        if($("input#search").val().length == 0){
            $(".start").attr("disabled", "true");
        }
    }
    else if($("div#list_func").css("display") != "none"){
        if($("select#objectlist_list option:selected").val() != ""){
            $(".start").removeAttr("disabled");
        }
        else{
            $(".start").attr("disabled", "true");
        }
    }
    else if($("div#total").css("display") != "none"){
        if($("select#modelist_total option:selected").val() != ""){
            $(".start").removeAttr("disabled");
        }
        else{
            $(".start").attr("disabled", "true");
        }
    }
    else if($("div#usage").css("display") != "none"){
        if($("select#modelist_usage option:selected").val() != ""){
            $(".start").removeAttr("disabled");
        }
        else{
            $(".start").attr("disabled", "true");
        }
    }
    else if($("div#top").css("display") != "none"){
        if($("select#objectlist_top").val() != "" & $("select#modelist_top").val() != "" & $("input#number_top").val() != ""){
            $(".start").removeAttr("disabled");
        }
        else{
            $(".start").attr("disabled", "true");
        }
    }
    else if($("div#last_activity").css("display") != "none"){
        if($("input#number_last_activity").val() != ""){
            $(".start").removeAttr("disabled");
        }
        else{
            $(".start").attr("disabled", "true");
        }
    }
    else if($("div#throughput").css("display") != "none"){
        if($("input#number_throughput").val() != "" & $("select#timelist").val() != "" & $("select#modelist_throughput").val() != ""){
            $(".start").removeAttr("disabled");
        }
        else{
            $(".start").attr("disabled", "true");
        }
    }
}
function validNumber(field){
    var d=field.attr('numeric');
        var value=$().val("vaule");
        var orignalValue=value;
        val=val.replace(/[0-9]*/g, "");

       	var msg="Only Integer Values allowed.";

       	if (d=='decimal'){
       	  value=value.replace(/\./, "");
          msg="Only Numeric Values allowed.";
          }

        if (val!=''){
        orignalValue=orignalValue.replace(/([^0-9].*)/g, "")
        $(this).val(orignalValue);
        alert(msg);
        }

    }

function highlightClose(divname){
    $('#close_function_' + divname).css({'opacity' : '1',  'filter':'alpha(opacity=100)'});
}
function resetClose(divname){
    $('#close_function_' + divname).css({'opacity' : '0.5', 'filter':'alpha(opacity=50)'});
}
function closeFunction(divname){
    $('div#' + divname).remove();
}

function highlightRefresh(divname){
    $('#refresh_function_' + divname).css({'opacity' : '1', 'filter':'alpha(opacity=100)'});
}
function resetRefresh(divname){
    $('#refresh_function_' + divname).css({'opacity' : '0.5', 'filter':'alpha(opacity=50)'});
}
function refreshFunction(divname){
  $("#spinner_function_" + divname).show();
  $.ajax({
        url: "../functions/refresh_function",
        type: "get",
        complete: function(){$("#spinner_function_" + divname).hide();
        },
        data: {
            divname: divname,
            cmd : $("#function_" + divname).attr('value')
        }
    })
}
function highlightPrintview(divname){
    $('#printview_function_' + divname).css({'opacity': '1', 'filter':'alpha(opacity=100)'});
}
function resetPrintview(divname){
   $('#printview_function_' + divname).css({'opacity' : '0.5', 'filter':'alpha(opacity=50)'});
}
function openPrintview(divname){
    $.ajax({
        url: "../functions/printview_function",
        type: "get",
        data: {
            func: $("input#function_" + divname).val()
        }
    })
}

var i = 0;
$(document).ready(function(){
$(".number").numeric({ decimal: false, negative: false }, function() { alert("Positive integers only"); this.value = ""; this.focus(); });
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
            }
            i = i+1;
            if (i==3)
                i = 0;
    }, 120000);
});
