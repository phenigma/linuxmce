<?
function wizard($output,$dbADO) {
	global $domain, $downloadDirectory;
	/* @var $dbADO ADOConnection */
	/* @var $output Template */
	
	if ($_SESSION['userIsLogged']!="yes") {
		header("Location: index.php");
	}

	$FK_Users=(isset($_SESSION['userID']))?(int)$_SESSION['userID']:0;
	$step=(int)$_REQUEST['step'];
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';

	$PK_Installation=(isset($_REQUEST['instid']))?(int)$_REQUEST['instid']:0;
	if($PK_Installation!=0){
		$queryInstallations='
			SELECT * FROM Installation
				INNER JOIN Installation_Users ON FK_Installation=PK_Installation
			WHERE PK_Installation=? AND FK_Users=?';
		$resInstallations=$dbADO->Execute($queryInstallations,array($PK_Installation,$FK_Users));
		if($resInstallations){
			$rowInstallations=$resInstallations->FetchRow();
			if($PK_Installation!=@$_SESSION['installationID'])
				unset($_SESSION['deviceID']);

			$_SESSION['installationID']=$PK_Installation;
			$_SESSION['installationDescription']=$rowInstallations['Description'];
			$_SESSION['ActivationCode']=$rowInstallations['ActivationCode'];
		}else{
			unset($_SESSION['installationID']);
			unset($_SESSION['installationDescription']);
			unset($_SESSION['ActivationCode']);
			header("Location: index.php?section=myPluto");
		}
	}

	$installationID=(isset($_SESSION['installationID']))?$_SESSION['installationID']:0;
	if(file_exists('client/step'.$step.'.php'))
		include('client/step'.$step.'.php');
	else
		Header('Location: index.php');
	


	$output->setNavigationMenu(array("Installation wizard"=>"index.php?section=wizard&step=1"));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME."::Installation wizard step ".$step);
	$output->output();
}
?>
