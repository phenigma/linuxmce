<?
function setupDisklessMD($output,$dbADO) {

	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */

	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	
	$out='';

	if ($action == 'form') {
		
		$out= '
		<input type="hidden" name="action" value="add">	
		<h3>Setup diskless Media Director</h3>
		<iframe src="operations/logs/executeLog.php?script=1" width="950" height="600"></iframe>
		
		</form>
		';
	} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=setupDisklessMD&error=You are not authorised to change the installation.");
			exit(0);
		}
		
		header("Location: index.php?section=setupDisklessMD");
		
	}

	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Setup diskless Media Director');
	$output->output();
}
?>