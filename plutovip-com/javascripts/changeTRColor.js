// Decide browser version
var ns4 = (document.layers)? true:false;
var ns6 = (document.getElementById)? true:false;
var ie4 = (document.all)? true:false;
var ie5 = false;
var ie6 = false;

// Microsoft Stupidity Check(tm).
if (ie4) {
	if (navigator.userAgent.indexOf('MSIE 5')>0) {
		ie5 = true;
	}
	if (navigator.userAgent.indexOf('MSIE 6')>0) {
		ie6 = true;
	}
	if (ns6) {
		ns6 = false;
	}
}


    function formSubmit(objF){
		objF.submit();
	}
    function changePage(pageNumber){
		document.forms["form1"].currentPage.value = pageNumber;
		formSubmit(document.forms["form1"]);
	}
    function ordAsc(paramOrder){
		document.forms["form1"].elements["paramOrder"].value = paramOrder;
		document.forms["form1"].elements["typeOrder"].value = 'asc';
		formSubmit(document.forms["form1"]);
	}
	function ordDesc(paramOrder){
		document.forms["form1"].elements["paramOrder"].value = paramOrder;
		document.forms["form1"].elements["typeOrder"].value = 'desc';
		formSubmit(document.forms["form1"]);
	} 
	function ordOff(paramOrder){
		document.forms["form1"].elements["paramOrder"].value = paramOrder;
		document.forms["form1"].elements["typeOrder"].value = 'off';
		formSubmit(document.forms["form1"]);
	}
	
	function resetOrdering() {	 		
	 		document.forms['form1'].elements['resetOrder'].value=1;
	 		formSubmit(document.forms["form1"]);
	}
	
function changeTRClass(objectID, TRIndex){	    
		if (ie5 || ie6) {
			document.forms["form1"].elements["TRSelected"].value = objectID
			var supportGetElementById = (document.getElementById)?1:0;
			if (supportGetElementById) {
				// restore deselected TR 
				if(currentTR != "0" && currentTR != objectID && document.getElementById(currentTR) != null) {
					document.getElementById(currentTR).style.backgroundColor = OldTRBackground
				}
				//save the class of the new selected TR
				if (TRIndex % 2 == 0) {
						OldTRBackground = lightBackground;
				} else {
						OldTRBackground = darkBackground;
				}
				//change the class of the new selected TR to 'tblActive'
				document.getElementById(objectID).style.backgroundColor = highLightBackground;
				currentTR = objectID;
			}
		}
	}	
	var oldTRBackgroundOnMouseOver;
	function changeTRClassOnMOver(objectTRID) {
		if (ie5 || ie6 ) {
			if (!(currentTR == objectTRID)) {
				oldTRBackgroundOnMouseOver = document.getElementById(objectTRID).style.backgroundColor;
			//window.alert(oldTRBackgroundOnMouseOver)
				document.getElementById(objectTRID).style.backgroundColor = overBackground;
			}
		}
	}
	function recoverTRClassOnMOver(objectTRID) {
		if (ie5 || ie6 ) {
			if (!(currentTR == objectTRID)) {
				document.getElementById(objectTRID).style.backgroundColor = oldTRBackgroundOnMouseOver;
			}
		}
	}
	
function getBGColor(ruleName) { //ruleName it's a CSS class
	var i,j;
	for(i=0; i<document.styleSheets.length; i++) { //all styleSheets defined
			for(j=0; j<document.styleSheets[i].rules.length; j++) {
				if (document.styleSheets[i].rules[j].selectorText == ruleName){
					return document.styleSheets[i].rules[j].style.backgroundColor;
				}
			}
	}
}

//loading bgcolors from .css file 
var lightBackground
var darkBackground
var highLightBackground
var overBackground
if (ie5 || ie6 ) {
	lightBackground = getBGColor(".rowLight");
	darkBackground = getBGColor(".rowDark");
	highLightBackground = getBGColor(".rowHighlight");
	overBackground = getBGColor(".rowHover");
	}