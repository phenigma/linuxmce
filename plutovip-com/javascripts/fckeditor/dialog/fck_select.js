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
 * fck_select.js: Scripts for the fck_select.html page.
 *
 * Authors:
 *   Frederico Caldeira Knabben (fckeditor@fredck.com)
 */

function Select( combo )
{
	var iIndex = combo.selectedIndex ;
	
	oListText.selectedIndex		= iIndex ;
	oListValue.selectedIndex	= iIndex ;
	
	var oTxtText	= document.getElementById( "txtText" ) ;
	var oTxtValue	= document.getElementById( "txtValue" ) ;

	oTxtText.value	= oListText.value ;
	oTxtValue.value	= oListValue.value ;
}

function Add()
{
	var oTxtText	= document.getElementById( "txtText" ) ;
	var oTxtValue	= document.getElementById( "txtValue" ) ;
	
	AddComboOption( oListText, oTxtText.value, oTxtText.value ) ;
	AddComboOption( oListValue, oTxtValue.value, oTxtValue.value ) ;
	
	oListText.selectedIndex = oListText.options.length - 1 ;
	oListValue.selectedIndex = oListValue.options.length - 1 ;
	
	oTxtText.value	= '' ;
	oTxtValue.value	= '' ;
	
	oTxtText.focus() ;
}

function Modify()
{
	var iIndex = oListText.selectedIndex ;
	
	if ( iIndex < 0 ) return ;

	var oTxtText	= document.getElementById( "txtText" ) ;
	var oTxtValue	= document.getElementById( "txtValue" ) ;
	
	oListText.options[ iIndex ].innerText	= oTxtText.value ;
	oListText.options[ iIndex ].value		= oTxtText.value ;
	
	oListValue.options[ iIndex ].innerText	= oTxtValue.value ;
	oListValue.options[ iIndex ].value		= oTxtValue.value ;

	oTxtText.value	= '' ;
	oTxtValue.value	= '' ;
	
	oTxtText.focus() ;
}

function Move( steps )
{
	ChangeOptionPosition( oListText, steps ) ;
	ChangeOptionPosition( oListValue, steps ) ;
}

function Delete()
{
	RemoveSelectedOptions( oListText ) ;	
	RemoveSelectedOptions( oListValue ) ;	
}

function SetSelectedValue()
{
	var iIndex = oListValue.selectedIndex ;
	if ( iIndex < 0 ) return ;
	
	var oTxtValue = document.getElementById( "txtSelValue" ) ;

	oTxtValue.value = oListValue.options[ iIndex ].value ;
}

// Add a new option to a SELECT object (combo or list)
function AddComboOption(combo, optionText, optionValue)
{
	var oOption = document.createElement("OPTION") ;

	combo.options.add(oOption) ;

	oOption.innerText = optionText ;
	oOption.value     = optionValue ;
	
	return oOption ;
}

// Moves the selected option by a number of steps (also negative)
function ChangeOptionPosition( combo, steps )
{
	var iActualIndex = combo.selectedIndex ;
	
	if ( iActualIndex < 0 ) return ;
	
	var iFinalIndex = iActualIndex + steps ;
	
	if ( iFinalIndex < 0 ) iFinalIndex = 0 ;
	if ( iFinalIndex > ( combo.options.lenght - 1 ) ) iFinalIndex = combo.options.lenght - 1 ;
	
	var oOption = combo.options[ iActualIndex ] ;
	
	combo.options.remove(iActualIndex) ;
	
	combo.add( oOption, iFinalIndex ) ;
}

// Remove all selected options from a SELECT object
function RemoveSelectedOptions(combo)
{
	// Save the selected index
	var iSelectedIndex = combo.selectedIndex ;

	var oOptions = combo.options ;

	// Remove all selected options	
	for ( var i = oOptions.length - 1 ; i >= 0 ; i-- )
	{
		if (oOptions[i].selected) oOptions.remove(i) ;
	}
	
	// Reset the selection based on the original selected index
	if ( combo.options.length > 0 )
	{
		if ( iSelectedIndex >= combo.options.length ) iSelectedIndex = combo.options.length - 1 ;
		combo.selectedIndex = iSelectedIndex ;
	}
}

// Add a new option to a SELECT object (combo or list)
function AddComboOption(combo, optionText, optionValue , documentObject)
{
	var oOption ;
	
	if ( documentObject )
		oOption = documentObject.createElement("OPTION") ;
	else
		oOption = document.createElement("OPTION") ;
	
	combo.options.add(oOption) ;
	
	oOption.innerText = optionText ;
	oOption.value     = optionValue ;
	
	return oOption ;
}