<%@ page import="java.util.Enumeration"%>
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
 * testsubmit.asp: Lists the submited data from a form POST.
 *
 * Authors:
 *   Pritpal Dhaliwal
-->
<%
	Enumeration params = request.getParameterNames();
%>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN" >
<HTML>
	<HEAD>
		<TITLE>FCKeditor - Posted Data</TITLE>
		<meta name="ROBOTS" content="NOINDEX, NOFOLLOW">
		<STYLE TYPE="text/css">
			body, td 
			{
				FONT-SIZE: 12px; 
				FONT-FAMILY: Arial, Verdana, Sans-Serif;
			}
		</STYLE>
	</HEAD>
	<BODY>
		<table width="100%" border="1" cellspacing="0" bordercolor="#999999">
			<tr style="FONT-WEIGHT: bold; COLOR: #dddddd; BACKGROUND-COLOR: #999999">
				<td>Field</td>
				<td>Value</td>
			</tr>
			<%
			String parameter = null;
			while( params.hasMoreElements() ){
				parameter = (String) params.nextElement();
			%>
			<tr>
				<td valign="top" nowrap><b><%=parameter%></b></td>
				<td width="100%"><%=request.getParameter(parameter)%></td>
			</tr>
			<%
			}
			%>
		</table>
	</BODY>
</HTML>