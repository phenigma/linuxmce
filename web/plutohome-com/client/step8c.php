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
		<td colspan="2" align="left">You finished the wizard!  All the settings you made have been saved into a database so your system can install and configure itself automatically.  You can go back to a prior page to change options and return to this page at any time.  
			Once your system is up and running, you will make all future changes on the Pluto Admin web site that gets installed on your '.$_SESSION['coreName'].'.  The settings you chose here are only used when you are installing the software, and have no effect once the software is already installed.<br><br></td>
	</tr>
	<tr class="normaltext">
		<td colspan="2" align="left"><b>Your activation code is: '.$_SESSION['ActivationCode'].'</b> &nbsp;&nbsp;&nbsp; You will be asked for this during installation.<br><br>During installation you will be asked for the activation code, and the device ID listed below.  This will be used to lookup the configuration at plutohome.com.  Just be sure to correctly specify the device for each computer you use it on.</td>
	</tr>
	<tr class="normaltext">
		<td colspan="2" align="left">&nbsp;</td>
	</tr>
	<tr class="normaltext">
		<td colspan="2" align="left"><br>Here are the computers you will need software for.  Be sure to make note of the Device numbers and click the appropriate link to download the software.<br><br><B>Core:</B></td>
	</tr>				
	<tr class="normaltext" bgcolor="lightblue">
		<td valign="top"><b>'.$DistroNameOS.'</b></td>
		<td>Device #: <b>'.$_SESSION['deviceID'].'</b></td>
	</tr>
	<tr class="normaltext">
		<td valign="top" colspan="2">Below are your link(s) to the the software for this computer: &nbsp;&nbsp;&nbsp;<b>FYI - Advanced users:</b> can get a list of all the software packages <a href="#" onClick="javascript:windowOpen(\'checkDependancies.php?device='.$rowDevice['PK_Device'].'\',\'width=1024,height=768,toolbars=true,scrollbars=1\');">here</a></td>
	</tr>
	<tr class="normaltext">
		<td valign="top" colspan="2">
		<table width="75%" align="center" border="0">
			<tr class="normaltext" bgcolor="#DADDE4">
				<td width="30%" align="left">- <a href="'.$InstallerURL.'">Windows installer</a></td>
				<td>&nbsp;</td>
			</tr>
		</table>
		</td>
	</tr>
	<tr class="normaltext">
		<td valign="top" colspan="2">Windows installer details.</td>
	</tr>
</table>';

?>
