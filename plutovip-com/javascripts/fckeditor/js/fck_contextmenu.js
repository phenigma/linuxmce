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
 * fck_contextmenu.js: Right click support.
 *
 * Authors:
 *   Frederico Caldeira Knabben (fckeditor@fredck.com)
 */

// Contants
var MENU_SEPARATOR = "" ; // Context menu separator

// The last context menu
var ContextMenu = new Array() ;

// Avaliable context menu options
var GeneralContextMenu		= new Array() ;
var TableContextMenu		= new Array() ;
var LinkContextMenu			= new Array() ;
var BulletedListContextMenu	= new Array() ;

function ContextMenuSeparator()
{
	this.Text = MENU_SEPARATOR ;
}

// Class that represents an item on the context menu
function ContextMenuItem(text, command, commandType)
{
	this.Text			= text ;
	this.Command		= command || "void(0)" ;
	this.CommandType	= commandType || TBCMD_DEC ;
	
	if ( this.Text && this.Text.substr( 0, 2 ) == '##' )
		this.Text = AvailableLangs.GetEntry( this.Text.substr( 2 ) ) ;

	
	switch (this.CommandType)
	{
		case TBCMD_DEC :
			this.Command     = "decCommand(" + command + ")" ;
			this.CommandId   = command ;
			break ;
		case TBCMD_DOC :
			this.Command     = "docCommand('" + command + "')" ;
			this.CommandCode = command ;
			break ;
		default :	// TBCMD_CUSTOM
			this.Command     = command ;
			break ;
	}
}


GeneralContextMenu[0] = new ContextMenuItem(lang["Cut"], DECMD_CUT) ;
GeneralContextMenu[1] = new ContextMenuItem(lang["Copy"], DECMD_COPY) ;
GeneralContextMenu[2] = new ContextMenuItem(lang["Paste"], DECMD_PASTE) ;

if ( config.ShowPreviewContextMenu )
{
	GeneralContextMenu[3] = new ContextMenuSeparator() ;
	GeneralContextMenu[4] = new ContextMenuItem( "##Preview", "preview()", TBCMD_CUSTOM) ;
}

LinkContextMenu[0] = new ContextMenuSeparator() ;
LinkContextMenu[1] = new ContextMenuItem(lang["EditLink"], "dialogLink()", TBCMD_CUSTOM) ;
LinkContextMenu[2] = new ContextMenuItem(lang["RemoveLink"], DECMD_UNLINK) ;

TableContextMenu[0]  = new ContextMenuSeparator() ;
TableContextMenu[1]  = new ContextMenuItem(lang["InsertRow"], DECMD_INSERTROW) ;
TableContextMenu[2]  = new ContextMenuItem(lang["DeleteRows"], DECMD_DELETEROWS) ;
TableContextMenu[3]  = new ContextMenuSeparator() ;
TableContextMenu[4]  = new ContextMenuItem(lang["InsertColumn"], DECMD_INSERTCOL) ;
TableContextMenu[5]  = new ContextMenuItem(lang["DeleteColumns"], DECMD_DELETECOLS) ;
TableContextMenu[6]  = new ContextMenuSeparator() ;
TableContextMenu[7]  = new ContextMenuItem(lang["InsertCell"], DECMD_INSERTCELL) ;
TableContextMenu[8]  = new ContextMenuItem(lang["DeleteCells"], DECMD_DELETECELLS) ;
TableContextMenu[9]  = new ContextMenuItem(lang["MergeCells"], DECMD_MERGECELLS) ;
TableContextMenu[10] = new ContextMenuItem(lang["SplitCell"], DECMD_SPLITCELL) ;
TableContextMenu[11] = new ContextMenuSeparator() ;
TableContextMenu[12] = new ContextMenuItem("##TableAutoFormat", "dialogTableAutoFormat(true)", TBCMD_CUSTOM) ;
TableContextMenu[13] = new ContextMenuSeparator() ;
TableContextMenu[14] = new ContextMenuItem(lang["CellProperties"], "dialogTableCell()", TBCMD_CUSTOM) ;
TableContextMenu[15] = new ContextMenuItem(lang["TableProperties"], "dialogTable(true)", TBCMD_CUSTOM) ;

BulletedListContextMenu[0] = new ContextMenuSeparator() ;
BulletedListContextMenu[1] = new ContextMenuItem("##BulletedListProperties", "dialogList()", TBCMD_CUSTOM) ;

function showContextMenu()
{
	// Resets the context menu. 
	ContextMenu = new Array() ;
	
	var i ;
  	var index = 0;
  	
  	// Gets the actual selection.
	var sel = objContent.DOM.selection.createRange() ;
	var oTag ;
	var sTagName ;
	if (objContent.DOM.selection.type != 'Text' && sel.length == 1)
	{
		oTag = sel.item(0) ;
		sTagName = oTag.tagName.toUpperCase() ;
	}
  
	// Always show general menu options
	for ( i = 0 ; i < GeneralContextMenu.length ; i++ )
	{
		ContextMenu[index++] = GeneralContextMenu[i] ;
	}
	
	// If over a link
	if (checkDecCommand(DECMD_UNLINK) == OLE_TRISTATE_UNCHECKED)
	{
		if ( oTag && oTag.name.length > 0 && oTag.href.length == 0 )
		{
			ContextMenu[index++] = new ContextMenuSeparator() ;
			ContextMenu[index++] = new ContextMenuItem("##AnchorProperties", "anchor()", TBCMD_CUSTOM) ;
		}
		else
		{		
			for ( i = 0 ; i < LinkContextMenu.length ; i++ ) 
			{
				ContextMenu[index++] = LinkContextMenu[i] ;
			}
		}
	}
	
	// If inside a table, load table menu options
	if (objContent.QueryStatus(DECMD_INSERTROW) != DECMDF_DISABLED) 
	{
		for ( i = 0 ; i < TableContextMenu.length ; i++ ) 
		{
			ContextMenu[index++] = TableContextMenu[i] ;
		}
	}
	
	// If over an ordered list
	if (checkDecCommand(DECMD_UNORDERLIST) == OLE_TRISTATE_CHECKED)
	{
		for ( i = 0 ; i < BulletedListContextMenu.length ; i++ ) 
		{
			ContextMenu[index++] = BulletedListContextMenu[i] ;
		}	
	}

	if ( sTagName == "TABLE" )
	{
		ContextMenu[index++] = new ContextMenuSeparator() ;
		ContextMenu[index++] = new ContextMenuItem("##TableAutoFormat", "dialogTableAutoFormat()", TBCMD_CUSTOM) ;
		ContextMenu[index++] = new ContextMenuSeparator() ;
		ContextMenu[index++] = new ContextMenuItem("##TableProperties", "dialogTable()", TBCMD_CUSTOM) ;
	}
	else if ( sTagName == "IMG" )
	{
		ContextMenu[index++] = new ContextMenuSeparator() ;
		ContextMenu[index++] = new ContextMenuItem("##ImageProperties", "dialogImage()", TBCMD_CUSTOM) ;
	}
	else if ( sTagName == "INPUT" )
	{
		ContextMenu[index++] = new ContextMenuSeparator() ;
		
		switch ( oTag.type )
		{
			case 'button' :
			case 'submit' :
			case 'reset' :
				ContextMenu[index++] = new ContextMenuItem( "##ButtonProperties", "button()", TBCMD_CUSTOM) ;
				break ;
			case 'checkbox' :
				ContextMenu[index++] = new ContextMenuItem( "##CheckboxProperties", "checkbox()", TBCMD_CUSTOM) ;
				break ;
			case 'hidden' :
				ContextMenu[index++] = new ContextMenuItem( "##HiddenFieldProperties", "hidden()", TBCMD_CUSTOM) ;
				break;
			case 'radio' :
				ContextMenu[index++] = new ContextMenuItem( "##RadioButtonProperties", "radio()", TBCMD_CUSTOM) ;
				break ;
			case 'image' :
				ContextMenu[index++] = new ContextMenuItem( "##ImageButtonProperties", "imageButton()", TBCMD_CUSTOM) ;
				break ;
			case 'password' :
			case 'text' :
				ContextMenu[index++] = new ContextMenuItem( "##FieldProperties", "textfield()", TBCMD_CUSTOM) ;
				break ;
			case 'file' :
				break ;
		}
	}
	else if ( sTagName == "SELECT" )
	{
		ContextMenu[index++] = new ContextMenuSeparator() ;
		ContextMenu[index++] = new ContextMenuItem( "##SelectionFieldProperties", "selectField()", TBCMD_CUSTOM) ;
	}	
	else if ( sTagName == "TEXTAREA" )
	{
		ContextMenu[index++] = new ContextMenuSeparator() ;
		ContextMenu[index++] = new ContextMenuItem( "##TextareaProperties", "textarea()", TBCMD_CUSTOM) ;
	}
	
	// Search for a parent FORM tag.
	var e ;
	
	if ( oTag ) 
		e = oTag ;
	else
		e = sel.parentElement() ;
		
	while ( e )
	{
		if ( e.tagName.toUpperCase() == "FORM" )
		{
			ContextMenu[index++] = new ContextMenuSeparator() ;
			ContextMenu[index++] = new ContextMenuItem( "##FormProperties", "form()", TBCMD_CUSTOM) ;
			break ;
		}
		e = e.parentElement ;
	}

	// Set up the actual arrays that get passed to SetContextMenu
	var menuStrings = new Array() ;
	var menuStates  = new Array() ;
	for ( i = 0 ; i < ContextMenu.length ; i++ )
	{
		menuStrings[i] = ContextMenu[i].Text ;
		
		if (menuStrings[i] != MENU_SEPARATOR) 
			switch (ContextMenu[i].CommandType)
			{
				case TBCMD_DEC :
					menuStates[i] = checkDecCommand(ContextMenu[i].CommandId) ;
					break ;
				case TBCMD_DOC :
					menuStates[i] = checkDocCommand(ContextMenu[i].CommandCode) ;
					break ;
				default :
					menuStates[i] = OLE_TRISTATE_UNCHECKED ;
					break ;
			}
		else
			menuStates[i] = OLE_TRISTATE_CHECKED ;
	}
  
	// Set the context menu
	objContent.SetContextMenu(menuStrings, menuStates);
}

function contextMenuAction(itemIndex)
{
	eval(ContextMenu[itemIndex].Command) ;
}