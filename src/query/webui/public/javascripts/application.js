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
    getDomains();
}
function domOnClickChange(){
    var selected = $("#domainlist option:selected");
    $("#dom_diff").hide();
    $("#domain").val(selected.val());
    getShares();
    if ($("div#files").length)
        $("#files").remove();
}
function shareOnClickChange(){
    var selected1 = $("#sharelist option:selected");
    var selected2 = $("#userlist option:selected");
    $("#share_diff").hide();
    $("#user_diff").hide();
    $("#share").val(selected1.val());
    $("#user").val(selected2.val());
    getFiles();

}
function fileOnClickChange(){
    var selected = $("#filelist option:selected");
    $("#file_diff").hide();
    $("#file").val(selected.val());
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
            }
            i = i+1;
            if (i==3)
                i = 0;
    }, 120000);
});
