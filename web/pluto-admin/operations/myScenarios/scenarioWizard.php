<?php
function scenarioWizard($output,$dbADO) {
/* @var $dbADO ADOConnection */
/* @var $rs ADORecordSet */

$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
$out='';
$installationID = (int)@$_SESSION['installationID'];
$wizard=isset($_REQUEST['wizard'])?(int)$_REQUEST['wizard']:0;
$from=$_REQUEST['from'];
$roomID=(int)@$_REQUEST['roomID'];
$cgID=$_REQUEST['cgID'];
$section=$_REQUEST['section'];

if($action=='form') {
	if($roomID!=0){
		$queryRoom='SELECT Description FROM Room WHERE PK_Room=?';
		$resRoom=$dbADO->Execute($queryRoom,$roomID);
		$rowRoom=$resRoom->FetchRow();
	}
	
	$queryCG='SELECT Description FROM CommandGroup WHERE PK_CommandGroup=?';
	$resCG=$dbADO->Execute($queryCG,$cgID);
	$rowCG=$resCG->FetchRow();

	$out.='
		<div align="left" class="confirm"><B>'.(isset($_GET['msg'])?strip_tags($_GET['msg'].'<br>'):'').'</B></div>	
		<div align="left" class="err"><B>'.strip_tags(@$_GET['err']).'</B></div>	
		<form action="index.php" method="POST" name="scenarioWizard" '.(($wizard==0)?'onSubmit="return validateForm();':'').'">
			<input type="hidden" name="section" value="scenarioWizard">
			<input type="hidden" name="action" value="add">	
			<input type="hidden" name="roomID" value="'.$roomID.'">
			<input type="hidden" name="cgID" value="'.$cgID.'">
			<input type="hidden" name="from" value="'.$from.'">
			<input type="hidden" name="oldWizard" value="'.$wizard.'">
	
			<table>
			<tr bgcolor="#D1D9EA">
				<td>Scenario name: <B>'.$rowCG['Description'].'</B></td>
				<td>Using Wizard: <select name="wizard" onChange="document.scenarioWizard.submit();">
					<option value="0" '.(($wizard==0)?'selected':'').'>Lighting wizard</option>
					<option value="1" '.(($wizard==1)?'selected':'').'>Climate wizard</option>
					<option value="2" '.(($wizard==2)?'selected':'').'>Advanced wizard</option>
				</select>
				</td>
			</tr>
		</table>';
	switch ($wizard){
		case 0:
			$out.=lightingDevicesTable($cgID,$dbADO);
		break;
		case 1:
			$out.=climateDevicesTable($cgID,$dbADO);
		break;
		case 2:
			$out.=advancedCommandGroupCommandsTable($cgID,$section,$dbADO);
		break;
	}
	$out.='
	</form>
	';
}else{	
	// action='add'
	$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$installationID,$dbADO);
	if(!$canModifyInstallation){
		Header('Location: index.php?section=scenarioWizard&error=You are not allowed to modify installation.');
		exit();
	}
	$oldWizard=(int)$_POST['oldWizard'];
	$wizard=(int)$_POST['wizard'];
	
	switch($oldWizard){
		case 0:
			processLightingScenario($cgID,$dbADO);
			$msg='Scenario updated';
		break;
		case 1:
			processClimateScenario($cgID,$dbADO);
			$msg='Scenario updated';
		break;
		case 2:
			processAdvancedScenarios($cgID,$section,$dbADO);
			$isModified=$GLOBALS['isModified'];
			$parametersUpdatedAlert=$GLOBALS['parametersUpdatedAlert'];
			$msg="Command Group ".($isModified?"":"not")." updated! $parametersUpdatedAlert";
		break;	
	}

	setOrbitersNeedConfigure($installationID,$dbADO);
	if($wizard!=$oldWizard){
		if((int)@$_REQUEST['addNewDevice']!=0)
			$sufix='&addNewDevice='.(int)@$_REQUEST['addNewDevice'];
		header("Location: index.php?section=scenarioWizard&roomID=$roomID&cgID=$cgID&from=$from&wizard=$wizard".@$sufix);
		exit();
	}else{
		header("Location: index.php?section=$from&roomID=$roomID&cgID=$cgID&msg=$msg");
		exit();
	}
	
	header("Location: index.php?section=scenarioWizard&roomID=$roomID&cgID=$cgID&from=$from&wizard=$wizard");
}
	switch($from){
		case 'lightingScenarios':
			$backLabel='Lighting Scenarios';
		break;
		case 'climateScenarios':
			$backLabel='Climate Scenarios';
		break;
		case 'mobileScenarios':
			$backLabel='Mobile Orbiter Scenarios';
		break;
		case 'sleepingScenarios':
			$backLabel='Sleeping Scenarios';
		break;
		default:
			$backLabel='Back';
		break;
	}
	$output->setScriptInBody("onLoad=\"javascript:top.treeframe.location.reload();\"");
	$output->setNavigationMenu(array("My Scenarios"=>'index.php?section=myScenarios',$backLabel=>'index.php?section='.$from));
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Lighting Scenarios');
	$output->output();

}
?>
