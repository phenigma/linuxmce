/*
 * FCKeditor - The text editor for internet
 * Copyright (C) 2003-2004 Frederico Caldeira Knabben
 *
 * Licensed under the terms of the GNU Lesser General Public License
 * (http://www.opensource.org/licenses/lgpl-license.php)
 *
 * For further information go to http://www.fredck.com/FCKeditor/ 
 * or contact fckeditor@fredck.com.
 *
 * fck_editor.js: Main script that initializes the editor.
 *
 * Authors:
 *   Frederico Caldeira Knabben (fckeditor@fredck.com)
 */

var STATE_STARTING		= 0 ;
var STATE_INITIALIZING	= 1 ;
var STATE_LOADING_DATA	= 2 ;
var STATE_RUNNING		= 3 ;

var eActualState = STATE_STARTING ;

function OnDocumentComplete()
{
	if ( eActualState == STATE_STARTING ) 
	{
		eActualState = STATE_INITIALIZING ;
		
		loadToolbarSet() ;
		loadToolbarSourceSet() ;
	}

	if ( eActualState == STATE_INITIALIZING && ! objContent.Busy)
	{
		objContent.ShowBorders = config.StartupShowBorders ;
		objContent.ShowDetails = config.StartupShowDetails ;
		
		setLinkedField() ;
	}
	
	if ( eActualState == STATE_LOADING_DATA && !objContent.Busy )
	{
		objContent.DOM.body.onpaste		= onPaste ;
		objContent.DOM.body.ondrop		= onDrop ;
		
		objContent.DOM.body.onkeydown = onKeyDown ;
		
		objContent.DOM.body.onfocus				= Styles ;
		objContent.DOM.body.onblur				= Styles;
		objContent.DOM.body.onpropertychange	= Styles;

		objContent.DOM.createStyleSheet(config.EditorAreaCSS) ;

		objContent.BaseURL = config.BaseUrl ;
	}
}

// Method: loadToolbarSet()
// Description: Loads a toobar buttons set from an array inside the Toolbar holder.
// Author: FredCK
function loadToolbarSet()
{
	var sToolBarSet = URLParams["Toolbar"] == null ? "Default" : URLParams["Toolbar"] ;

	// FredCK: Toobar holder (DIV)
	var oToolbarHolder = document.getElementById("divToolbar") ;

	var oToolbar = new TBToolbar() ;
	oToolbar.LoadButtonsSet( sToolBarSet ) ;
	oToolbarHolder.innerHTML = oToolbar.GetHTML() ;
}

function loadToolbarSourceSet()
{
	// FredCK: Toobar holder (DIV)
	var oToolbarHolder = document.getElementById("divToolbarSource") ;

	var oToolbar = new TBToolbar() ;
	oToolbar.LoadButtonsSet( "Source" ) ;
	oToolbarHolder.innerHTML = oToolbar.GetHTML() ;
}

function switchEditMode()
{
	var bSource = (trSource.style.display == "none") ;

	if (bSource) 
	{
		if ( config.EnableXHTML && config.EnableSourceXHTML )
			txtSource.value = getXhtml( objContent.DOM.body ) ;
		else
			txtSource.value = objContent.DOM.body.innerHTML ;
	}
	else
	{
		LoadHTML( txtSource.value ) ;
	}
		
	trEditor.style.display = bSource ? "none" : "inline" ;
	trSource.style.display = bSource ? "inline" : "none" ;

	events.fireEvent('onViewMode', bSource) ;
}

// Gets the actual HTML.
function getValue()
{
	var bWysiwyg = (trSource.style.display == "none") ;

	if (bWysiwyg) 
		return objContent.DOM.body.innerHTML ;
	else
		return txtSource.value ;
}

// setValue(): called from reset() to make a select list show the current font
// or style attributes
function selValue(el, str, text)
{
	//if (!RichEditor.txtView) return;      // Disabled in View Source mode
	for (var i = 0; i < el.length; i++) 
	{
		if (((text || !el[i].value) && el[i].text == str) || ((!text || el[i].value) && el[i].value == str)) 
		{
			el.selectedIndex = i;
			return;
		}
	}
	el.selectedIndex = 0;
}

function LoadHTML( html )
{
	eActualState = STATE_LOADING_DATA ;

	objContent.DocumentHTML = 
		'<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">' +
		'<html><head></head><body>' + html + '</body></html>' ;
	
//	// __tmpFCKRemove__ added and removed to solve DHTML component error when loading "<p><hr></p>"
//	objContent.DOM.body.innerHTML = "<div id=__tmpFCKRemove__>&nbsp;</div>" + html ;
//	objContent.DOM.getElementById('__tmpFCKRemove__').removeNode(true) ;
}

var oLinkedField = null ;
function setLinkedField()
{
	if (! URLParams['FieldName']) return ;
	
	var oFields = parent.document.getElementsByName(URLParams['FieldName']) ;
	
	for ( var i = 0 ; i < oFields.length ; i++ )
	{
		oLinkedField = oFields[i] ;
		if ( oLinkedField.tagName == "INPUT" ) break ;
	}
	
	if (! oLinkedField) return ;

	LoadHTML( oLinkedField.value ) ;
	
	var oForm = oLinkedField.form ;
	
	if (!oForm) return ;

	// Attaches the field update to the onsubmit event
	oForm.attachEvent("onsubmit", setFieldValue) ;
	
	// Attaches the field update to the submit method (IE doesn't fire onsubmit on this case)
	if (! oForm.updateFCKEditor) oForm.updateFCKEditor = new Array() ;
	oForm.updateFCKEditor[oForm.updateFCKEditor.length] = setFieldValue ;
	if (! oForm.originalSubmit)
	{
		oForm.originalSubmit = oForm.submit ;
		oForm.submit = function()
		{
			if (this.updateFCKEditor)
			{
				for (var i = 0 ; i < this.updateFCKEditor.length ; i++)
				{
					this.updateFCKEditor[i]() ;
				}
			}
			this.originalSubmit() ;
		}
	}
}

function setFieldValue()
{
	if (trSource.style.display != "none")
		switchEditMode() ;

	if (config.EnableXHTML)
		oLinkedField.value = getXhtml( objContent.DOM.body ) ;
	else
		oLinkedField.value = objContent.DOM.body.innerHTML ;
}

function onPaste()
{
	if (config.ForcePasteAsPlainText)
	{
		pastePlainText() ;	
		return false ;
	}
	else if (config.AutoDetectPasteFromWord && BrowserInfo.IsIE55OrMore)
	{
		var sHTML = GetClipboardHTML() ;
		var re = /<\w[^>]* class="?MsoNormal"?/gi ;
		if ( re.test( sHTML ) )
		{
			if ( config.ForceCleanPasteFromWord || confirm( lang["PasteWordConfirm"] ) )
			{
				cleanAndPaste( sHTML ) ;
				return false ;
			}
		}
	}
	else
		return true ;
}

function onDrop()
{
	if (config.ForcePasteAsPlainText)
	{
		var sText = HTMLEncode( objContent.DOM.parentWindow.event.dataTransfer.getData("Text") ) ;
		sText = sText.replace(/\n/g,'<BR>') ;
		insertHtml(sText) ;
		return false ;
	}
	else if (config.AutoDetectPasteFromWord && BrowserInfo.IsIE55OrMore)
	{
		// TODO
		// To find a way to get the HTML that is dropped, 
		// clean it and insert it into the document.
		return true ;
	}
	else
		return true ;
}

function onKeyDown()
{
	var oWindow = objContent.DOM.parentWindow ;
	
	if ( oWindow.event.ctrlKey || oWindow.event.altKey || oWindow.event.shiftKey ) 
	{
		oWindow.event.returnValue = true ;
		return ;
	}

	if ( oWindow.event.keyCode == 9 && config.TabSpaces > 0 )	// TAB
	{
		var sSpaces = "" ;
		for ( i = 0 ; i < config.TabSpaces ; i++ )
			sSpaces += "&nbsp;" ;
		insertHtml( sSpaces ) ;
		
		oWindow.event.returnValue = false ;
	}
	else if (  oWindow.event.keyCode == 13 && config.UseBROnCarriageReturn )	// ENTER
	{
		if ( objContent.DOM.queryCommandState( 'InsertOrderedList' ) || objContent.DOM.queryCommandState( 'InsertUnorderedList' ) )
		{
			oWindow.event.returnValue = true ;
			return ;
		}

		insertHtml("<br>&nbsp;") ;
			
		var oRange = objContent.DOM.selection.createRange() ;
		oRange.moveStart('character',-1) ;	
		oRange.select() ;
		objContent.DOM.selection.clear() ;
			
		oWindow.event.returnValue = false ;
	}
}

function Styles()
{
	var allForms = objContent.DOM.body.getElementsByTagName("FORM") ;
	for ( var a=0; a < allForms.length; a++) 
	{
		allForms[a].runtimeStyle.border		= "1px dotted #FF0000" ;
		allForms[a].runtimeStyle.padding	= "2px" ;
	}

	var allInputs = objContent.DOM.body.getElementsByTagName("INPUT");
	for ( var b=0; b < allInputs.length; b++) 
	{
		if (allInputs[b].type.toUpperCase() == "HIDDEN") 
		{
			allInputs[b].runtimeStyle.width				= "20px" ;
			allInputs[b].runtimeStyle.height			= "20px" ;
			allInputs[b].runtimeStyle.border			= "1px dotted #FF0000" ;
			allInputs[b].runtimeStyle.backgroundImage	= "url(" + config.BasePath + "images/editor/hidden.gif)" ;
			allInputs[b].runtimeStyle.fontSize			= "99px" ;
		}
	}
	
	// Anchors
	var allLinks = objContent.DOM.body.getElementsByTagName("A") ;
	for ( var i=0; i < allLinks.length; i++ ) 
	{
		if ( allLinks[i].name.length > 0 && allLinks[i].href.length == 0 ) 
		{
			allLinks[i].runtimeStyle.width				= "20px" ;
			allLinks[i].runtimeStyle.height				= "20px" ;
			allLinks[i].runtimeStyle.textIndent			= "20px" ;
			allLinks[i].runtimeStyle.backgroundRepeat	= "no-repeat" ;
			allLinks[i].runtimeStyle.backgroundImage	= "url(" + config.BasePath + "images/editor/anchor.gif)" ;
		}
	}

}

function dblClickAction()
{
  	// Gets the actual selection.
	var sel = objContent.DOM.selection.createRange() ;
	var oTag ;
	var sTagName ;
	if (objContent.DOM.selection.type != 'Text' && sel.length == 1)
	{
		oTag = sel.item(0) ;
		sTagName = oTag.tagName.toUpperCase() ;
	}
	
	// If over a link or anchor.
	if (checkDecCommand(DECMD_UNLINK) == OLE_TRISTATE_UNCHECKED)
	{
		if ( oTag && oTag.name.length > 0 && oTag.href.length == 0 )
			anchor() ;
		else
			dialogLink() ;
	}
	else if ( sTagName == "TABLE" )
		dialogTable() ;
	else if ( sTagName == "IMG" )
		dialogImage() ;
	else if ( sTagName == "INPUT" )
	{
		switch ( oTag.type )
		{
			case 'button' :
			case 'submit' :
			case 'reset' :
				button() ;
				break ;
			case 'checkbox' :
				checkbox() ;
				break ;
			case 'hidden' :
				hidden() ;
				break;
			case 'radio' :
				radio() ;
				break ;
			case 'image' :
				imageButton() ;
				break ;
			case 'password' :
			case 'text' :
				textfield() ;
				break ;
			case 'file' :
				break ;
		}
	}
	else if ( sTagName == "SELECT" )
		selectField() ;
	else if ( sTagName == "TEXTAREA" )
		textarea() ;
	else if ( sTagName == "FORM" )
		form() ;
	else
		return true ;

	return false ;
}