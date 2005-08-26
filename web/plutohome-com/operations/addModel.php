<?
function addModel($output)
{
	global $dbPlutoAdminServer,$dbPlutoAdminUser,$dbPlutoAdminPass,$dbPlutoAdminDatabase;
	include_once('include/plutoAdminUtils.inc.php');
	$mID=@$_REQUEST['mID'];
	$dcID=@$_REQUEST['dcID'];
	
	$publicADO = &ADONewConnection('mysql');
	$publicADO->NConnect($dbPlutoAdminServer,urlencode($dbPlutoAdminUser),urlencode($dbPlutoAdminPass),urlencode($dbPlutoAdminDatabase));


	/* @var $dbADO ADOConnection */
	global $PlutoHomeHost;
	$userID=(int)@$_SESSION['userID'];
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$step=$_REQUEST['step'];
	
	if($userID==0){
		$back=(isset($_REQUEST['mID']))?urlencode("section=addModel&step=1&mID=$mID&dcID=$dcID"):urlencode("section=addModel&step=$step&dtID={$_REQUEST['dtID']}");
		header("Location: index.php?section=login&back=".$back."&error=You must be logged in order to add new models");
		exit();
	}
	if(file_exists('operations/addModel_step'.$step.'.php'))
		include('operations/addModel_step'.$step.'.php');
	else
		Header('Location: index.php');
		
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}
?>