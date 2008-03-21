<?
function slimServer($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/slimServer.lang.php');
	
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	//$dbADO->debug=true;
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
	
	
	if($action=='form'){
	
		$ip=$_SERVER['REMOTE_ADDR'];
		
		$data=exec_batch_command('sudo -u root /usr/pluto/bin/Network_DisplaySettings.sh --all',1);
		$ret=parse_ini_str($data);

		$out.='<iframe src="http://'.$_SERVER['HTTP_HOST'].':9000/" style="width:98%;height:600px;"></iframe>';
	
	}else{
	// process area
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=slimServer&error=$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST");
			exit();
		}	

		
		header('Location: index.php?section=slimServer');
		exit();
	}

	$output->setMenuTitle($TEXT_ADVANCED_CONST.' |');
	$output->setPageTitle($TEXT_SLIMSERVER_CONST);
	$output->setNavigationMenu(array($TEXT_SLIMSERVER_CONST=>'index.php?section=slimServer'));	
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_SLIMSERVER_CONST);
	$output->output();
}
?>
