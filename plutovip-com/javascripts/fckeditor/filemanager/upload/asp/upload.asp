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
 ' upload.asp: Sample upload system.
 '
 ' Authors:
 '   Trax
 '   Juanjo
'###
%>
<!--#include file="./class_upload.asp"-->
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN" >
<HTML>
	<HEAD>
		<TITLE>FCKeditor - File Upload by Trax</TITLE>
	</HEAD>
	<BODY>
		<TABLE height="100%" width="100%">
			<TR>
				<TD align=center valign=middle>
<%
		' Files are uploaded to the "UserImages" folder on the web site root.
		sDirUrl		= "/UserImages/"
		
		cSysRootDir = Server.MapPath( sDirUrl )
		If Right( cSysRootDir, 1 ) <> "\" Then
			cSysRootDir = cSysRootDir & "\"
		End If

		Set Uploader = New FileUploader
		Uploader.Upload()

		KolkoSlika = Uploader.Files.Count

		If Uploader.Files.Count > 0 Then
			For Each oFile in Uploader.Files.Items
				sName = Right( CStr( 100000000 + GetNextCounter() ), 8 ) & "_" & oFile.FileName
				oFile.SaveToDisk cSysRootDir & sName
				strSlika = sDirUrl & sName
			Next
		End If

		Set Uploader = Nothing

		Response.Write "<strong>File uploaded. Number of uploaded files: " & cstr(KolkoSlika) & "</strong><br><br>"
		Response.Write "<SCRIPT language=javascript>window.opener.setImage('" + strSlika + "') ; window.close();</" + "SCRIPT>"
%>
				</TD>
			</TR>
		</TABLE>
	</BODY>
</HTML>
<%
' By Juanjo
Function GetNextCounter()
	Dim counter
	counter = 0
	
	Set fso = Server.CreateObject("Scripting.FileSystemObject")
	
	If fso.fileExists(Server.MapPath("./upload.cnt")) then
		Set f1 = fso.openTextFile(Server.MapPath("./upload.cnt"),1,true)
		counter = f1.ReadLine
		f1.close
		If isNumeric(counter) then
			counter = cint(counter) + 1
			Set f1 = fso.openTextFile(Server.MapPath("./upload.cnt"),2)
			f1.write cstr(counter)
			f1.close
		else
			' there is a error in counter file. No numeric
			response.write "ERROR: counter file 'upload.cnt' damaged. Value must be numeric"
		End if
	else
		' counter file does not exists
		response.write "ERROR: counter file 'upload.cnt' missing."
	End if
	
	Set fso = nothing
	
	GetNextCounter = counter
End Function
%>