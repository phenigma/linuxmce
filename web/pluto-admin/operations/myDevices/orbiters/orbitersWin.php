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
	<a href="fdownload.php?filepath=installers/OrbiterInstaller.msi" target="_blank">Windows XP/2000</a><br><br>
	<a href="fdownload.php?filepath=installers/OrbiterCEInstaller.exe" target="_blank">Windows CE</a><br><br>
		
<b>Orbiter CE binaries</b><br>
<a href="fdownload.php?filepath=orbiter/binaries/AYGSHELL.DLL" target="_blank">Download AYGSHELL.DLL</a><br>
<a href="fdownload.php?filepath=orbiter/binaries/PthreadsCE.dll" target="_blank">Download PthreadsCE.dll</a><br>
<a href="fdownload.php?filepath=orbiter/binaries/logo.gif" target="_blank">Download logo.gif</a><br>
<a href="fdownload.php?filepath=orbiter/binaries/Orbiter.MD5" target="_blank">Download Orbiter.MD5</a><br>
<a href="fdownload.php?filepath=bin/OrbiterCE.exe" target="_blank">Download OrbiterCE.exe</a><br><br>		
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
