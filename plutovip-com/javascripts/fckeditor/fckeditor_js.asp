<%
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
 * fckeditor_js.asp: ASP integration - JavaScript version.
 *
 * Authors:
 *   Frederico Caldeira Knabben (fckeditor@fredck.com)
 *   Dimiter Naydenov (dimitern@netagesolutions.com)
 */

	function FCKeditor()
	{
		this.ToolbarSet = "" ;
		this.Value = "" ;
		this.CanUpload = "none" ;
		this.CanBrowse = "none" ;
		this.BasePath = "/FCKeditor/" ;
	}

	FCKeditor.prototype.CreateFCKeditor = function FCKeditor_CreateFCKeditor(instanceName,width,height)
	{
		var sLink;
		
		if (this.IsCompatible())
		{
			sLink = this.BasePath + "fckeditor.html?FieldName=" + instanceName;

			if (this.ToolbarSet != "")
			{
				sLink += "&Toolbar=" + this.ToolbarSet;
			}

			if (this.CanUpload != "none")
			{
				if (this.CanUpload == true)
				{
					sLink += "&Upload=true";
				}
				else
				{
					sLink += "&Upload=false";
				}
			}

			if (this.CanBrowse != "none")
			{
				if (this.CanBrowse == true)
				{
					sLink += "&Browse=true";
				}
				else
				{
					sLink += "&Browse=false";
				}
			}

			Response.Write("<IFRAME src=\""+ sLink + "\" width=\"" + width + "\" height=\"" + height + "\" frameborder=\"no\" scrolling=\"no\"></IFRAME>");
			Response.Write("<INPUT type=\"hidden\" name=\"" + instanceName + "\" value=\"" + Server.HTMLEncode(this.Value) + "\">");
		} else {
			Response.Write("<TEXTAREA name=\"" + instanceName + "\" rows=\"4\" cols=\"40\" style=\"WIDTH: " + width + "; HEIGHT: " + height + "\" wrap=\"virtual\">" + Server.HTMLEncode(this.Value) + "</TEXTAREA>");
		}
	}
	
	FCKeditor.prototype.IsCompatible = function FCKeditor_IsCompatible()
	{
		var sAgent = String(Request.ServerVariables("HTTP_USER_AGENT"));
		var iVersion;

		if ((sAgent.indexOf("MSIE") > 0) && (sAgent.indexOf("Windows") > 0) && (sAgent.indexOf("Opera") == -1))
		{
			iVersion = parseInt(sAgent.replace(/^.*MSIE\s(\d).*$/gi,"$1"));
			return (iVersion >= 5);
		} else {
			return false;
		}
	}
%>