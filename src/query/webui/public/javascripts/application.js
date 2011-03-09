// Place your application-specific JavaScript functions and classes here
// This file is automatically included by javascript_include_tag :defaults
function initTrees() {
	$("#mixed").treeview({
		url: "get_domains",
		ajax: {
	          type: "get"
		}
	});
};
setInterval(function(){
  initTrees();
}, 10000);
//function refresh() {
//   initTrees();
//   var t = setTimeout("refresh()",10000);
//};	                                                                                                                                                  
$(document).ready(function(){
        initTrees();
        setInterval();
});
