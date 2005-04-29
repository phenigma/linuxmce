<?
if(!isset($_SESSION['userIsLogged'])){
	header('Location: index.php?section=login');
	exit();
}
function deleteInstallation($output,$dbADO) {
	/* @var $dbADO ADOConnection */
	

	$FK_Users=(isset($_SESSION['userID']))?(int)$_SESSION['userID']:0;
	$installationToDel=(int)@$_REQUEST['dID'];
	
	if($FK_Users==0){
		header('Location: index.php?section=login');
		exit();
	}
	if($installationToDel==0){
		header('Location: index.php?section=myPluto&err=1');
		exit();
	}
	
	$queryInstallations='
		SELECT FK_Installation FROM Installation_Users
		WHERE FK_Users=?';
	$res=$dbADO->Execute($queryInstallations,$FK_Users);
	$installations=array();
	while($row=$res->FetchRow()){
		$installations[]=$row['FK_Installation'];
	}
	if(!in_array($installationToDel,$installations)){
		header('Location: index.php?section=myPluto&err=2');
		exit();
	}
	
	delInstallation($installationToDel,$dbADO);
	
	header('Location: index.php?section=myPluto&msg='.$installationToDel);
	exit();

		
	$output->setNavigationMenu(array("deleteInstallation"=>"index.php?section=deleteInstallation"));

	$out='';
	$output->setScriptCalendar('null');
	$output->setScriptTRColor('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME."::My Pluto");
	$output->output();
}
?>