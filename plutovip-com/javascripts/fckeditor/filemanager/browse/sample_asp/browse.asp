<%@LANGUAGE="VBSCRIPT" CODEPAGE="1252"%>
<%
'###
 ' FCKeditor - The text editor for internet
 ' Copyright (C) 2003-2004 Frederico Caldeira Knabben
 '
 ' Licensed under the terms of the GNU Lesser General Public License
 ' (http://www.opensource.org/licenses/lgpl-license.php)
 '
 ' For further information go to http://www.fredck.com/FCKeditor/ 
 ' or contact fckeditor@fredck.com.
 '
 ' browse.asp: Sample browse system.
 '
 ' Authors:
 '   Andi Barnet
'###
%>
<%
Response.Expires = 0
Response.Expiresabsolute = Now() - 1
Response.AddHeader "pragma","no-cache"
Response.AddHeader "cache-control","private"
Response.CacheControl = "no-cache"
%>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<title>Image Browser</title>
<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
</head>
<body>
<SCRIPT language=javascript>
function getit(URL)
{
window.opener.setImage(URL) ;
window.close() ;
}
</script>
<%
Dim sMapImg
sMapImg = Server.MapPath( "/UserImages/" )
Dim Item
Dim IMGI
Dim SERIES
Set IMGI = Server.CreateObject("Scripting.FileSystemObject")
Set SERIES = IMGI.GetFolder(sMapImg)
For Each Item In SERIES.Files
Response.Write("<a href=""JavaScript:getit('/UserImages/" & Item.Name &"'); "">" & Item.Name & "</a><br>")
Next
Set SERIES = Nothing
Set IMGI = Nothing
%>
</body>
</html>
