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
 * upload.jsp: Basic file upload manager for the editor. You have
 *   to have set a directory called "UserImages" in the root folder
 *   of your web site.
 *
 * Authors:
 *   Simone Chiaretta (simone@piyosailing.com)
-->
<%@ page import = "javazoom.upload.*,java.util.*" %>
<jsp:useBean id="upBean" scope="page" class="javazoom.upload.UploadBean" >
  <jsp:setProperty name="upBean" property="folderstore" value="C:\UserImages" />
  <jsp:setProperty name="upBean" property="overwrite" value="true" />
</jsp:useBean>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN" >
<HTML>
	<HEAD>
		<TITLE>FCKeditor - File Upload</TITLE>
	</HEAD>
	<BODY>
		<TABLE height="100%" width="100%">
			<TR>
				<TD align=center valign=middle>
					Upload in progress...
				</TD>
			</TR>
		</TABLE>
	</BODY>
</HTML>
<%
MultipartFormDataRequest mrequest = new MultipartFormDataRequest(request);
Hashtable files = mrequest.getFiles();
if ( (files != null) || (!files.isEmpty()) ) {
	UploadFile file = (UploadFile) files.get("FCKeditor_File");
	upBean.store(mrequest, "FCKeditor_File");
	String sFileURL  = "/UserImages/" + file.getFileName();
	%>
    <SCRIPT language=javascript>
        window.opener.setImage('<%=sFileURL %>') ;
        window.close() ;
    </SCRIPT>

	<%
	}
%>