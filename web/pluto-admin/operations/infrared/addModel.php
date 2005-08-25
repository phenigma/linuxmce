<?
function addModel($output,$dbADO,$mediadbADO)
{

	/* @var $dbADO ADOConnection */
	global $PlutoHomeHost;
	
	$deviceID=(int)@$_REQUEST['deviceID'];
	$userID=(int)@$_SESSION['userID'];
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$step=$_REQUEST['step'];
	$publicADO=$dbADO;
	
	$mID=@$_REQUEST['mID'];
	$dcID=@$_REQUEST['dcID'];

	getDeviceCategoryChildsArray($GLOBALS['rootAVEquipment'],$dbADO);
	$avDeviceCategories=cleanArray($GLOBALS['childsDeviceCategoryArray']);
	
	if(isset($_REQUEST['dcID']) && !in_array($dcID,$avDeviceCategories)){
		include('operations/addBasicModel.php');
	}else{
		if(file_exists('operations/infrared/addModel_step'.$step.'.php'))
			include('operations/infrared/addModel_step'.$step.'.php');
		else
			Header('Location: index.php');
	}		
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}
?>