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
 * fck_init.js: Editor initializator.
 *
 * Authors:
 *   Frederico Caldeira Knabben (fckeditor@fredck.com)
 */

// #### URLParams: holds all URL passed parameters (like ?Param1=Value1&Param2=Value2)
var URLParams = new Object() ;

var aParams = document.location.search.substr(1).split('&') ;
for ( i = 0 ; i < aParams.length ; i++ )
{
	var aParam = aParams[i].split('=') ;
	var sParamName  = aParam[0] ;
	var sParamValue = unescape( aParam[1] ) ;

	// Override any configuration with the same name (if exists)
	if ( config[ sParamName ] != null )
	{
		if ( sParamValue == "true" )
			config[ sParamName ] = true ;
		else if ( sParamValue == "false" )
			config[ sParamName ] = false ;
		else if ( ! isNaN( sParamValue ) )
			config[ sParamName ] = parseInt( sParamValue ) ;
		else
			config[ sParamName ] = sParamValue ;
	}

	URLParams[ sParamName ] = sParamValue ;
}

// Override some configurations (Deprecated)
if (URLParams['Upload']) config.ImageUpload  = config.LinkUpload  = ( URLParams['Upload'] == 'true' ) ;
if (URLParams['Browse']) config.ImageBrowser = config.LinkBrowser = ( URLParams['Browse'] == 'true' ) ;

// #### BrowserInfo: holds client informations.
var BrowserInfo = new Object() ;
BrowserInfo.MajorVer = navigator.appVersion.match(/MSIE (.)/)[1] ;
BrowserInfo.MinorVer = navigator.appVersion.match(/MSIE .\.(.)/)[1] ;
BrowserInfo.IsIE55OrMore = BrowserInfo.MajorVer >= 6 || ( BrowserInfo.MajorVer >= 5 && BrowserInfo.MinorVer >= 5 ) ;