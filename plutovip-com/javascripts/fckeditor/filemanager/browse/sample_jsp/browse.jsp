<%@ page import = "java.io.*"%>
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
 * browse.jsp: Sample server images/files browser for the editor.
 *
 * Authors:
 *   Simone Chiaretta (simone@piyosailing.com)
-->
<%
String imagesDir="/FCKeditor/filemanager/browse/sample_html/images/";
String docsDir="/FCKeditor/_docs/";
String fileType=(String)request.getParameter("type");
String selectedDir="";
String functionName="";
if (fileType.equals("img")) {
	selectedDir=imagesDir;
	functionName="getImage";
	}
else if (fileType.equals("doc")) {
	selectedDir=docsDir;
	functionName="getDoc";
	}
%>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN" >
<HTML>
	<HEAD>
		<TITLE>FCKeditor - Image Browser</TITLE>
		<META name="vs_targetSchema" content="http://schemas.microsoft.com/intellisense/ie5">
		<STYLE type="text/css">
			BODY, TD, INPUT { FONT-SIZE: 12px; FONT-FAMILY: Arial, Helvetica, sans-serif }
		</STYLE>
		<SCRIPT language="javascript">
var sImagesPath  = "<%=selectedDir%>" ;
var sActiveImage = "" ;

function getImage(imageName)
{
	sActiveImage = sImagesPath + imageName ;
	imgPreview.src = sActiveImage ;
}

function ok()
{	
	window.setImage(sActiveImage) ;
	window.close() ;
}

function getDoc(fileName)
{
	window.setImage( sImagesPath + fileName ) ;
	window.close() ;
}
		</SCRIPT>
	</HEAD>
	<BODY>
		<TABLE height="100%" cellspacing="0" cellpadding="0" width="100%" border="0">
			<TR>
				<TD height="100%">
					<TABLE height="100%" cellspacing="5" cellpadding="0" width="100%" border="0">
						<TR>
							<TD align="middle" width="50%">
								Select the image to load<BR>
								<BR>
<%
String imagesFolder=application.getRealPath(selectedDir);
File folder=new File(imagesFolder);
String[] filesImmagini=folder.list();
for (int i=0; i<filesImmagini.length;i++) {
	out.println("<A href=\"javascript:"+functionName+"('"+filesImmagini[i]+"');\">"+filesImmagini[i]+"</A><BR>");
} 
%>								
</TD>
<% if (fileType.equals("img")) {%>
							<TD align="middle" width="50%">
								<IMG src="<%=selectedDir%>spacer.gif" id="imgPreview">
							</TD>
<% } %>
						</TR>
					</TABLE>
				</TD>
			</TR>
			<TR>
				<TD valign="bottom" align="middle">
					<INPUT style="WIDTH: 80px" type="button" value="OK"     onclick="ok();"> &nbsp;&nbsp;&nbsp;&nbsp;
					<INPUT style="WIDTH: 80px" type="button" value="Cancel" onclick="window.close();"><BR>
				</TD>
			</TR>
		</TABLE>
	</BODY>
</HTML>
