<?
// windows installer

$out .= '
<table align="center" width="100%" border="0">
	<tr>
		<td align="left" class="normaltext"><img src="images/install_wizard.gif"></td>
		<td align="right" class="normaltext"><a href="index.php?section=wizard&step=7">&lt;&lt; Previous</a></td>
	</tr>
	<tr>
		<td align="center" colspan="2" bgcolor="#DADDE4"><b>Step 8 of 8: Download</b></td>
	</tr>
	<tr class="normaltext">
		<td colspan="2" align="left">You\'re done!  During the installation you will be asked for your device code: <b>'.$rowDevice['PK_Device'].'</b> &nbsp; and  &nbsp;
		activation code: <b>'.$_SESSION['ActivationCode'].'</b> so the installer can lookup the settings you chose.</p>
		
		<p>Until that happens you can go back to a prior page to change options.  But once your system is up and running, 
		you will make all future changes on the <a href="support/index.php?section=document&docID=35">Pluto Admin web site</a> that gets installed on your '.$_SESSION['coreName'].'.  
		The settings you chose here are only used during the initial install.</p>
		<p><b><a href="http://plutohome.com/download/installers/windows/PlutoHomeInstaller.exe" onClick="self.location=\'index.php?section=wizard&action=win%20installer&step=8\';">DOWNLOAD WINDOWS INSTALLER</a></b> &nbsp; This will install the Pluto '.$_SESSION['coreName'].'
		software.  The same installer can also be used on any Windows PC\'s you want to use as media directors.  Run the same installer and use the same activation code, but when prompted
		specify the device ID of the media director instead.  Because your '.$_SESSION['coreName'].' is not dedicated, you cannot use network boot for the media directors--any
		media directors must have the Pluto software installed.</td>
	</tr>';
	$out.=$mediaDirectorLines;	
	$out.='
	<tr class="normaltext">
		<td colspan="2">
		<p><b>Orbiters:</b><br>Once the '.$_SESSION['coreName'].' is running, open the <a href="support/index.php?section=document&docID=35">Pluto Admin website</a> in a web browser
		on any devices you want to use as Orbiters (PDA\'s/WebPad\'s/Touch-screen or regular PC\'s).  It includes links to install the Orbiter softare.  Bluetooth
		mobile phones are plug-and-play, the software is installed automatically when they come within range of a media director that has bluetooth.</td>
	</tr>
</table>';

?>
