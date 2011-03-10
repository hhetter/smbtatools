// Place your application-specific JavaScript functions and classes here
// This file is automatically included by javascript_include_tag :defaults

function getDomains () {
  $.ajax({
    url: "get_domains",
    type: "get"});
}
function refreshDomains() {
  $.ajax({
    url: "refresh_domains",
    type: "get"});
  setTimeout("refreshDomains()",5000);
}
$(document).ready(function(){
});
