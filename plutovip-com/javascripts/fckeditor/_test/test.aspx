<%@ Register TagPrefix="FredCK" Namespace="FredCK" Assembly="FredCK.FCKeditor" %>
<%@ Page language="c#" AutoEventWireup="false" %>
<%--
 * FCKeditor - The text editor for internet
 * Copyright (C) 2003-2004 Frederico Caldeira Knabben
 *
 * Licensed under the terms of the GNU Lesser General Public License
 * (http://www.opensource.org/licenses/lgpl-license.php)
 *
 * For further information go to http://www.fredck.com/FCKeditor/ 
 * or contact fckeditor@fredck.com.
 *
 * test.aspx: ASPX page to test the editor ASP.NET integration.
 *   This page uses the ASP.NET web user control 
 *   "FredCK.FCKeditor.dll". You can found it at
 *   http://www.fredck.com/FCKeditor/ 
 *
 * Authors:
 *   Frederico Caldeira Knabben (fckeditor@fredck.com)
--%>
<script runat="server">
protected override void OnLoad(EventArgs e)
{
	FCKeditor1.Config["StyleNames"] = ";Style 1;Style 2; Style 3" ;
}
</script>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN" >
<html>
	<head>
		<title>FCKeditor - ASP.NET Test</title>
		<meta name="CODE_LANGUAGE" Content="C#">
		<link href="../css/fck_editorarea.css" type="text/css" rel="stylesheet">
		<meta name="ROBOTS" content="NOINDEX, NOFOLLOW">
	</head>
	<body>
		<form id="Test" method="post" runat="server">
			<FredCK:FCKeditor id="FCKeditor1" height="300" runat="server" Value='<P align="center">This is just a <B>test</B>.</P>'
				BasePath="../"></FredCK:FCKeditor><br>
			<br>
			<input type="submit" value="Submit Data">
			<br>
			<br>
			<hr width="100%" size="1">
			<%=FCKeditor1.Value%>
		</form>
	</body>
</html>
