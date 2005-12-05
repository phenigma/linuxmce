<?
function orbitersWin($output,$dbADO) {
	global $dbPlutoMainDatabase;
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
//	$dbADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
	
	if ($action == 'form') {
		$out.='
	<div class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
	<div class="confirm"><B>'.(isset($_GET['msg'])?strip_tags($_GET['msg']):'').'</B></div>
	<form action="index.php" method="POST" name="devices">
	<input type="hidden" name="section" value="orbitersWin">
	<input type="hidden" name="action" value="add">	
	<h3  align="left">Download Windows Software for Orbiters</h3>
	<br>
	
	<table>
		<tr>
			<td><B>Processor</B></td>
			<td><B>Operating System</B></td>
			<td>&nbsp;</td>
		</tr>
		<tr bgcolor="#F0F3F8">
			<td>Intel/AMD x86 (normal)</td>
			<td>Windows 98/ME/XP</td>
			<td><a href="fdownload.php?filepath=installers/OrbiterInstaller.msi" target="_blank">(download)</a></td>
		</tr>
		<tr>
			<td>XScale</td>
			<td>Windows CE.NET</td>
			<td><a href="fdownload.php?filepath=installers/Orbiter_CeNet4_XScale.CAB" target="_blank">(download)</a></td>
		</tr>
		<tr bgcolor="#F0F3F8">
			<td>Intel x86</td>
			<td> Windows CE.NET</td>
			<td><a href="fdownload.php?filepath=installers/Orbiter_CeNet4_x86.CAB" target="_blank">(download)</a></td>
		</tr>
	</table>
		
	</form>
	';
	} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=devices&error=You are not authorised to change the installation.");
			exit(0);
		}
		
		header("Location: index.php?section=orbitersWin");		
	}

	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Orbiters');
	$output->output();
}
