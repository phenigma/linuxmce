<?php
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
 * fckeditor.php: PHP pages integration.
 *
 * Authors:
 *   Frederico Caldeira Knabben (fckeditor@fredck.com)
 */

class FCKeditor
{
	var $ToolbarSet ;
	var $Value ;
	var $CanUpload ;
	var $CanBrowse ;
	var $BasePath ;

	function FCKeditor()
	{
		$this->ToolbarSet = '' ;
		$this->Value = '' ;
		$this->CanUpload = true ;
		$this->CanBrowse = true ;
		$this->BasePath = '/pluto_home/javascripts/fckeditor/' ;
	}
	
	function CreateFCKeditor($instanceName, $width, $height)
	{
		echo $this->ReturnFCKeditor($instanceName, $width, $height) ;
		//$this->ReturnFCKeditor($instanceName, $width, $height) ;
	}
	
	function ReturnFCKeditor($instanceName, $width, $height)
	{

//		$grstr = htmlentities( $this->Value ) ;
		$grstr = htmlspecialchars( $this->Value ) ;

		$strEditor = "" ;
		
		if ( $this->IsCompatible() )
		{
			$sLink = $this->BasePath . "fckeditor.html?FieldName=$instanceName" ;

			if ( $this->ToolbarSet != '' )
				$sLink = $sLink . "&Toolbar=$this->ToolbarSet" ;

			if ( $this->CanUpload != 'none' )
			{
				if ($this->CanUpload == true)
					$sLink = $sLink . "&Upload=true" ;
				else
					$sLink = $sLink . "&Upload=false" ;
			}

			if ( $this->CanBrowse != 'none' )
			{
				if ($this->CanBrowse == true)
					$sLink = $sLink . "&Browse=true" ;
				else
					$sLink = $sLink . "&Browse=false" ;
			}

			$strEditor .= "<IFRAME src=\"$sLink\" width=\"$width\" height=\"$height\" frameborder=\"no\" scrolling=\"no\"></IFRAME>" ;
			$strEditor .= "<INPUT type=\"hidden\" name=\"$instanceName\" value=\"$grstr\">" ;
		}
		else
		{
			$strEditor .= "<TEXTAREA name=\"$instanceName\" rows=\"4\" cols=\"40\" style=\"WIDTH: $width; HEIGHT: $height\" wrap=\"virtual\">$grstr</TEXTAREA>" ;
		}
		
		return $strEditor;
	}
	
	function IsCompatible()
	{
		$sAgent = $_SERVER['HTTP_USER_AGENT'] ;

		if ( is_integer( strpos($sAgent, 'MSIE') ) && is_integer( strpos($sAgent, 'Windows') ) && !is_integer( strpos($sAgent, 'Opera') ) )
		{
			$iVersion = (int)substr($sAgent, strpos($sAgent, 'MSIE') + 5, 1) ;
			return ($iVersion >= 5) ;
		} else {
			return FALSE ;
		}
	}
}
?>