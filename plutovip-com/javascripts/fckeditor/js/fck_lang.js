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
 * fck_lang.js: Handles multi language functionality.
 *
 * Authors:
 *   Frederico Caldeira Knabben (fckeditor@fredck.com)
 */

var lang = new Object() ;

var AvailableLangs = new Object() ;

AvailableLangs.LanguageCodes = [
	'ar','bs','ca','cs','da','de','en','es','fi','fr',
	'gr','he','hu','is','it','jp','ko','lt','nl','no',
	'pl','pt-br','ro','ru','sk','sl','sr','sv','tr','zh-cn',
	'zh-tw' 
] ;

for ( var i = 0 ; i < AvailableLangs.LanguageCodes.length ; i++ )
	AvailableLangs[ AvailableLangs.LanguageCodes[i] ] = true ;

AvailableLangs.GetActiveLanguage = function()
{
	if ( config.AutoDetectLanguage && navigator.userLanguage )
	{
		var sUserLang = navigator.userLanguage.toLowerCase() ;
		
		if ( sUserLang.length >= 5 )
		{
			sUserLang = sUserLang.substr(0,5) ;
			if ( this[sUserLang] ) return sUserLang ;
		}
		
		if ( sUserLang.length >= 2 )
		{
			sUserLang = sUserLang.substr(0,2) ;
			if ( this[sUserLang] ) return sUserLang ;
		}
	}
	
	return config.DefaultLanguage ;
}

document.write('<script src="lang/' + AvailableLangs.GetActiveLanguage() + '.js" type="text/javascript"><\/script>') ;

AvailableLangs.GetEntry = function( entryCode, defaultValue )
{
	return ( lang[ entryCode ] || defaultValue || entryCode ) ;
}

function LangEntry( entryCode, defaultValue )
{
	return AvailableLangs.GetEntry( entryCode, defaultValue ) ;
}

AvailableLangs.TranslatePage = function( targetDocument )
{
	// Gets all INPUT elements and translate then values
	var aInputs = targetDocument.getElementsByTagName("INPUT") ;
	for ( i = 0 ; i < aInputs.length ; i++ )
	{
		if ( aInputs[i].fckLang && lang[ aInputs[i].fckLang ] )
			aInputs[i].value = lang[ aInputs[i].fckLang ] ;
	}

	// Gets all INPUT elements and translate then values
	var aInputs = targetDocument.getElementsByTagName("BUTTON") ;
	for ( i = 0 ; i < aInputs.length ; i++ )
	{
		if ( aInputs[i].fckLang && lang[ aInputs[i].fckLang ] )
			aInputs[i].title = lang[ aInputs[i].fckLang ] ;
	}

	// Gets all SPAN elements and translate then cotents
	var aSpans = targetDocument.getElementsByTagName("SPAN") ;
	for ( i = 0 ; i < aSpans.length ; i++ )
	{
		if ( aSpans[i].fckLang && lang[ aSpans[i].fckLang ] )
			aSpans[i].innerText = lang[ aSpans[i].fckLang ] ;
	}

	// Gets all LABEL elements and translate then cotents
	var sLabels = targetDocument.getElementsByTagName("LABEL") ;
	for ( i = 0 ; i < sLabels.length ; i++ )
	{
		if ( sLabels[i].fckLang && lang[ sLabels[i].fckLang ] )
			sLabels[i].innerText = lang[ sLabels[i].fckLang ] ;
	}
		
	// Gets all OPTION elements and translate then cotents
	var aOptions = targetDocument.getElementsByTagName("OPTION") ;
	for ( i = 0 ; i < aOptions.length ; i++ )
	{
		if ( aOptions[i].fckLang && lang[ aOptions[i].fckLang ] )
			aOptions[i].innerText = lang[ aOptions[i].fckLang ] ;
	}
}