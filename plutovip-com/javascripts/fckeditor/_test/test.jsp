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
<%@ page language="java" import="com.fredck.FCKeditor.*" %>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN" >
<html>
	<head>
		<title>FCKeditor - JSP Test Page</title>
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

<%
FCKeditor oFCKeditor;
oFCKeditor = new FCKeditor(request,"EditorDefault") ;
oFCKeditor.setBasePath("/FCKeditor/");		// '/FCKeditor/' is the default value so this line could be deleted.
oFCKeditor.setValue("This is some <B>sample text</B>.");
oFCKeditor.getConfig().put("StyleNames",";Style 1;Style 2; Style 3");
oFCKeditor.getConfig().put("ToolbarFontNames",";Arial;Courier New;Times New Roman;Verdana");
out.println(oFCKeditor.create());
%>

			<br>
			FCKeditor - Accessibility Toolbar Set<BR>
		
<%
oFCKeditor = new FCKeditor(request,"EditorAccessibility") ;
oFCKeditor.setWidth("80%");
oFCKeditor.setHeight("120");
oFCKeditor.setToolbarSet("Accessibility");
oFCKeditor.setValue("This is another test. <BR><B>The &quot;Second&quot; row.</B>");
oFCKeditor.setCanUpload(false);	// Overrides fck_config.js default configuration
oFCKeditor.setCanBrowse(false);	// Overrides fck_config.js default configuration
out.println(oFCKeditor.create());
%>
		
			<br>
			<br>
			FCKeditor - Basic Toolbar Set<BR>
			
<%
oFCKeditor = new FCKeditor(request,"EditorBasic", "300", "80", "Basic", "<P align=\"center\">Another test.</P>") ;
out.println(oFCKeditor.create());
%>
			<br>
			<br>
			<input type="submit" value="Submit Data">
			<br>
		</form>
	</body>
</html>