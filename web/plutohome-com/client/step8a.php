<?
// dedicated core/hybrid - kickstart CD

$out .= '
<table align="center" width="100%" border="0">
	<tr>
		<td align="left" class="insidetable"><img src="images/titles/installation_wizard.gif"></td>
		<td align="right" class="insidetable" width="150"><a href="index.php?section=wizard&step=7">&lt;&lt; Previous</a></td>
	</tr>
	<tr>
		<td align="center" colspan="2" bgcolor="#DADDE4"><b>Step 8 of 8: Download</b></td>
	</tr>
	<tr class="normaltext">
		<td colspan="2" align="left">You\'re done!  During the installation you will be asked for your device code: <b>'.$rowDevice['PK_Device'].'</b> &nbsp; and  &nbsp;
		activation code: <b>'.$_SESSION['ActivationCode'].'</b> so the installer can lookup the settings you chose.</p>
		
		<p>Until that happens you can go back to a prior page to change options.  But once your system is up and running, 
		you will make all future changes on the <a href="support/index.php?section=document&docID=35">Pluto Admin web site</a> that gets installed on your '.$_SESSION['coreName'].'.  
		The settings you chose here are only used during the initial install.</p></td>
	</tr>
	<tr class="normaltext">
		<td colspan="2" align="left"><p>Download the Kick-Start CD using the link below--it\'s an ISO image.  You will need CD burning software that can write an ISO image to a blank CD--most can do this.
		Then boot your computer with the CD in the drive.  Note: You may need to change your BIOS settings if your computer does not boot from the CD by default.</p>
		<p>When asked about partitioning the hard drive, you can just accept the default values and then "Write changes to disk".  
		If you understand partioning, you may want to change the values.  By default it will give you an 8GB partition for the Pluto system, 
		and the rest for your media & data files.  That way you can later re-install the Pluto system from scratch without losing your data.</p>
		<p>If you have any problems, you can checkout Pluto\'s general support forums, or use our email or live chat support.</p>
		<p><b>IMPORTANT NOTE:</b> During installation you may see a warning that starts with "You are attempting to install a kernel version that is the same as the version..." 
			followed by a lot of text, and then "Please Hit return to continue".
			The warning is harmless and will be supressed in future releases.</p>
		<p><b><a href="http://plutohome.com/download/cds/pldebsrg.iso" onClick="self.location=\'index.php?section=wizard&action=kick-start%20cd&step=8\';">DOWNLOAD KICK-START CD</a></b>  Alternatively get it by anonymous ftp at ftp.plutohome.com directory: "cds" file: "pldebsrg.iso"</p>
		</td>
	</tr>
	<tr class="normaltext">
		<td valign="top" colspan="2"><b>FYI - Advanced users:</b> can get a list of all the software packages that will be installed <a href="#" onClick="javascript:windowOpen(\'checkDependancies.php?device='.$rowDevice['PK_Device'].'\',\'width=1024,height=768,toolbars=true,scrollbars=1\');">here</a></td>
	</tr>';
	if($disklessMDs==0){
	}else{
		$out.=$mediaDirectorLines;	
	}
	$out.='
	<tr class="normaltext">
		<td colspan="2"><p>Media directors with network boot are plug-and-play.  Just enable network boot in the BIOS and put it before the hard drive.  
			<a href="support/index.php?section=document&docID=144">details</a></p>
		<p><b>Orbiters:</b><br>Once the '.$_SESSION['coreName'].' is running, open the <a href="support/index.php?section=document&docID=35">Pluto Admin website</a> in a web browser
		on any devices you want to use as Orbiters (PDA\'s/WebPad\'s/Touch-screen or regular PC\'s).  It includes links to install the Orbiter softare.  Bluetooth
		mobile phones are plug-and-play, the software is installed automatically when they come within range of a media director that has bluetooth.</td>
	</tr>';
	$out.='
	<tr class="normaltext">
		<td valign="top">&nbsp;</td>
	</tr>
</table>';
?>
