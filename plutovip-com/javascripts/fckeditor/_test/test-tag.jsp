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
 * test.jsp: JSP page to test the editor JSP integration.
 *
 * Authors:
 *  Simone Chiaretta (simone@piyosailing.com)
-->
<%@ taglib uri="/WEB-INF/FCKeditor.tld" prefix="FCK" %>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN" >
<html>
	<head>
		<title>FCKeditor - JSP Tags Test Page</title>
		<meta name="ROBOTS" content="NOINDEX, NOFOLLOW">
	</head>
	<body>
		<form action="testsubmit.jsp" target="_blank" method="post" language="javascript">
			Normal Text Field<BR>
			<input type="text" name="NormalInput" value="My 'normal' &quot;field&quot; value"><br>
			<br>
			Normal Textarea<br>
			<textarea name="NormalTextArea" rows="2" cols="20"></textarea>
			<br>
			<br>
			FCKeditor - Default Toolbar Set<BR>
			<FCK:editor id="EditorDefault" basePath="/FCKeditor/">
				<FCK:config name="StyleNames" value=";Style 1;Style 2; Style 3" />
				<FCK:config name="ToolbarFontNames" value=";Arial;Courier New;Times New Roman;Verdana" />
				This is some <B>sample text</B>.</FCK:editor>
			<br>
			FCKeditor - Accessibility Toolbar Set<BR>
			<FCK:editor
				id="EditorAccessibility"
				width="80%"
				height="120"
				toolbarSet="Accessibility"
				canUpload="false"
				canBrowse="false"
			>This is another test. <BR><B>The "Second" row.</B></FCK:editor>
		
			<br>
			<br>
			FCKeditor - Basic Toolbar Set<BR>
			<FCK:editor
				id="EditorBasic"
				width="300"
				height="80"
				toolbarSet="Basic"
			><P align="center">Another test.</P></FCK:editor>
			
			<br>
			<br>
			<input type="submit" value="Submit Data">
			<br>
		</form>
	</body>
</html>