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

	$filePath=$GLOBALS['scenariosPicsPath'].$cgID.'.png';
	if(file_exists($filePath)){
		$randNumber=rand(0,99999);
		$scenarioImg='<img src="include/image.php?imagepath='.$filePath.'&rand='.$randNumber.'"><br>';
	}else
		$scenarioImg='&nbsp;';		

	$out.='
		<div align="left" class="confirm"><B>'.stripslashes(@$_GET['msg']).'</B></div>	
		<div align="left" class="err"><B>'.stripslashes(@$_GET['err']).'</B></div>	
		<form action="index.php" method="POST" name="scenarioWizard" '.(($wizard==0)?'onSubmit="return validateForm();"':'').' enctype="multipart/form-data">
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
		</table>
		<table>
			<tr>
				<td align="center" valign="top">'.$scenarioImg.'<input type="file" name="pic" value=""></td>
				<td>';
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
	$out.='</td>
		</tr>
	</table>
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
		case -1:
		case 2:
			$returnHook=processAdvancedScenarios($cgID,$section,$dbADO);
			$isModified=$GLOBALS['isModified'];
			$parametersUpdatedAlert=$GLOBALS['parametersUpdatedAlert'];
			$msg="Command Group ".($isModified?"":"not")." updated! $parametersUpdatedAlert";
		break;	
	}

	// upload scenario picture
	$errNotice='';
	$filePath=$GLOBALS['scenariosPicsPath'].$cgID.'.png';
	if($_FILES['pic']['name']!=''){
		switch($_FILES['pic']['type']){
			case 'image/x-png':
			case 'image/png':
			case 'image/jpg':
			case 'image/pjpeg':
			case 'image/jpeg':
			$invalidType=false;
			break;
			default:
			$invalidType=true;
			break;
		}

		if($invalidType===false){
			if(!move_uploaded_file($_FILES['pic']['tmp_name'],$filePath)){
				$errNotice.=$_FILES['pic']['name'].' wasn\'t uploaded, check the rights for '.$filePath;
			}else{
				// convert to png and/or resize if necessary
				exec('chmod 777 -R '.$filePath);
				$imageinfo = @getimagesize( $filePath );
				if($imageinfo[0]!=160 && $imageinfo[1]!=160){
					$flag=resizeImage($filePath,$filePath,160,160,1);
				}
			}
		}else
		$errNotice.=$_FILES['pic']['name'].' is not a valid PNG file.<br>';
	}
	$errToLink=($errNotice!='')?'&error='.urlencode($errNotice):'';
	
	
	setOrbitersNeedConfigure($installationID,$dbADO);
	if($wizard!=$oldWizard){
		if((int)@$_REQUEST['addNewDevice']!=0)
			$sufix='&addNewDevice='.(int)@$_REQUEST['addNewDevice'];
		header("Location: index.php?section=scenarioWizard&roomID=$roomID&cgID=$cgID&from=$from&wizard=$wizard".@$sufix.$errToLink.'#hook_0');
		exit();
	}else{
		header("Location: index.php?section=scenarioWizard&roomID=$roomID&cgID=$cgID&from=$from&wizard=$wizard&msg=Scenario updated.$errToLink#hook_".@$returnHook);
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
	$output->setTitle(APPLICATION_NAME);
	$output->output();

}
?>
