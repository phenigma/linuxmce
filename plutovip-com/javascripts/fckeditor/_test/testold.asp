<!--
 * FCKeditor - The text editor for internet
 * Copyright (C) 2003-2004 Frederico Caldeira Knabben
 *
 * Licensed under the terms of the GNU Lesser General Public License
 * (http://www.opensource.org/licenses/lgpl-license.php)
 *
 * For further information go to http://www.fredck.com/FCKeditor/ 
 * or contact fckeditor@fredck.com.
 *
 * testold.asp: ASP page to test the editor ASP integration with
 *              ASP version 4 and above (old versions).
 *
 * Authors:
 *   Frederico Caldeira Knabben (fckeditor@fredck.com)
-->
<!-- #INCLUDE file="../fckeditorold.asp" -->
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN" >
<html>
	<head>
		<title>FCKeditor - ASP Test Page - Old Versions</title>
		<meta name="ROBOTS" content="NOINDEX, NOFOLLOW">
	</head>
	<body>
		<form action="TestSubmit.asp" target="_blank" method="post" language="javascript">
			Normal Text Field<br>
			<input type="text" name="NormalInput" value="My 'normal' &quot;field&quot; value"><br>
			<br>
			Normal Textarea<br>
			<textarea name="NormalTextArea" rows="2" cols="20"></textarea>
			<br>
			<br>
			FCKeditor - Default Toolbar Set<br>
<%
FCKeditorBasePath = "/FCKeditor/"		' "/FCKeditor/" is the default value so this line could be deleted.
CreateFCKeditor "EditorDefault", "100%", 150, "", "This is same <B>sample text</B>."
%>
			<br>
			FCKeditor - Accessibility Toolbar Set<br>
<%
FCKCanUpload = False
FCKCanBrowse = False
CreateFCKeditor "EditorAccessibility", "80%", 120, "Accessibility", "This is another test. <BR><B>The &quot;Second&quot; row.</B>"
%>
			<br>
			<br>
			FCKeditor - Basic Toolbar Set<br>
<%
FCKCanUpload = Null
FCKCanBrowse = Null
CreateFCKeditor "EditorBasic", 300, 80, "Basic", "<P align=""center"">Another test.</P>"
%>
			<br>
			<br>
			<input type="submit" value="Submit Data">
			<br>
		</form>
	</body>
</html>
