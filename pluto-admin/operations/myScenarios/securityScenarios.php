<?php
function securityScenarios($output,$dbADO) {
/* @var $dbADO ADOConnection */
/* @var $rs ADORecordSet */

$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
$out='';
$installationID = (int)@$_SESSION['installationID'];
$arrayID = $GLOBALS['ArrayIDForSecurity'];	

$cameraDevices=getDevicesArrayFromCategory($GLOBALS['rootCameras'],$dbADO);
$cameraIDArray=array_keys($cameraDevices);

if($action=='form') {
	
	$queryArray = 'SELECT * FROM Array WHERE PK_Array=?';	
	$resArray = $dbADO->Execute($queryArray,$arrayID);
	$rowArray=$resArray->FetchRow();

	if(isset($_GET['cgDelID']) && (int)$_GET['cgDelID']!=0){
		$cgToDelete=(int)$_GET['cgDelID'];
		deleteCommandGroup($cgToDelete,$dbADO);
		header("Location: index.php?section=securityScenarios");
	}

	
	$out.='<h2>'.$rowArray['Description'].'</h2>';

	$out.='
	<div class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
	<div align="center"><B>'.@$_REQUEST['msg'].'</B></div>
	<form action="index.php" method="POST" name="securityScenarios">
		<input type="hidden" name="section" value="securityScenarios">
		<input type="hidden" name="action" value="add">	
		<input type="hidden" name="roomID" value="">	
	
	<table width="100%" cellpadding="4" cellspacing="0" border="0">';
	$queryRooms='
		SELECT Room.*, Room.Description AS RoomName FROM Room
			INNER JOIN RoomType ON FK_RoomType=PK_RoomType
		WHERE FK_Installation=?
			ORDER BY Room.Description ASC';
	$resRooms=$dbADO->Execute($queryRooms,$installationID);
	$displayedCommandGroups=array();
	while($rowRooms=$resRooms->FetchRow()){
		$out.='
		<tr bgcolor="#D1D9EA">
			<td colspan="2"><B>'.$rowRooms['RoomName'].'</B></td>
		</tr>
		<tr>
			<td width="20">&nbsp;</td>
			<td><input type="checkbox" name="later" value="1"> Button to arm system, disarm system & change masks</td>
		</tr>
		<tr>
			<td width="20">&nbsp;</td>
			<td><input type="checkbox" name="later" value="1"> Button to view all cameras</td>
		</tr>
		<tr>
			<td width="20">&nbsp;</td>
			<td><input type="checkbox" name="later" value="1"> *SOS* Button for emergencies</td>
		</tr>';

		if(count($cameraDevices)>0){
			$cameraStr='<table><tr>';
			foreach($cameraDevices as $cameraID=>$cameraDescription){
				$cameraStr.='<td><input type="checkbox" name="camera_'.$cameraID.'_room_'.$rowRooms['PK_Room'].'"> '.$cameraDescription.'</td>';
			}
			$cameraStr.='</tr></table>';
			$out.='
			<tr>
				<td></td>
				<td align="center"><B>Shortcut buttons to view certain cameras full-screen ?</B></td>
			</tr>
			<tr>
				<td></td>
				<td bgcolor="#EEEEEE" align="center">'.$cameraStr.'</td>
			</tr>
			';
		}
		$out.='			
			<tr>
				<td></td>
				<td><hr><table width="100%">';
		$selectCommandGroups='
			SELECT * 
			FROM CommandGroup
			INNER JOIN CommandGroup_Room ON FK_CommandGroup=PK_CommandGroup
			WHERE FK_Room=? AND FK_Installation=? AND FK_Template=?';
		$resCommandGroups=$dbADO->Execute($selectCommandGroups,array($rowRooms['PK_Room'],$installationID,$GLOBALS['securityScenariosTemplate']));
		if($resCommandGroups->RecordCount()==0){
		$out.='
			<tr>
				<td colspan="3">No security scenarios for this room.</td>
			</tr>';
		}
		while($rowCG=$resCommandGroups->FetchRow()){
			$displayedCommandGroups[]=$rowCG['PK_CommandGroup'];
			$out.='
			<tr bgcolor="#DDDDDD">
				<td><input type="text" name="commandGroup_'.$rowCG['PK_CommandGroup'].'" value="'.$rowCG['Description'].'"></td>
				<td align="center"><a href="index.php?section=securityScenarios&cgID='.$rowCG['PK_CommandGroup'].'&action=edit&roomID='.$rowRooms['PK_Room'].'">Edit</a></td>
				<td align="center"><a href="#" onClick="javascript:if(confirm(\'Are you sure you want to delete this scenario?\'))self.location=\'index.php?section=securityScenarios&cgDelID='.$rowCG['PK_CommandGroup'].'\';">Delete</a></td>
			</tr>
			';
		}
		$out.='</table><input type="button" name="newScenario" value="New quad camera security scenario" onClick="self.location=\'index.php?section=securityScenarios&roomID='.$rowRooms['PK_Room'].'&action=addScenario\'">';
	}
	$out.='		</td>
			</tr>
		</table>
	<input type="hidden" name="displayedCommandGroups" value="'.join(',',$displayedCommandGroups).'">	
	</form>
	';
							
}elseif($action=='addScenario'){
	$roomID=$_REQUEST['roomID'];
	
	$out.='	<form action="index.php" method="POST" name="securityScenarios">
		<input type="hidden" name="section" value="securityScenarios">
		<input type="hidden" name="action" value="add">	
		<input type="hidden" name="roomID" value="'.$roomID.'">	
		<table cellpadding="4" cellspacing="0" border="0">
			<tr>
				<td colspan="2"><h3>New quad camera security scenario</h3></td>
			</tr>
			<tr>
				<td><B>Description: </B></td>
				<td><input type="text" name="description" value=""></td>
			</tr>
			<tr>
				<td><B>Hint: </B></td>
				<td><input type="text" name="hint" value=""></td>
			</tr>
			<tr>
				<td><B>Icon: </B></td>
				<td><select name="icon">
					<option value="0"></option>';
				$queryIcons='SELECT * FROM Icon ORDER BY Description ASC';
				$resIcons=$dbADO->Execute($queryIcons);
				while($rowIcons=$resIcons->FetchRow()){
					$out.='<option value="'.$rowIcons['PK_Icon'].'">'.$rowIcons['Description'].'</option>';
				}
				$out.='	
				</select>
				</td>
			</tr>
			<tr>
				<td colspan="2"><b>Select cameras for quad view:</b></td>
			</tr>';
			$camerasTxt='';
			foreach($cameraDevices as $cameraID=>$cameraDescription){
				$camerasTxt.='<option value="'.$cameraID.'">'.$cameraDescription.'</option>';
			}
			for($i=1;$i<5;$i++){
				$out.='
				<tr>
					<td><B>Camera '.$i.': </B></td>
					<td><select name="camera_'.$i.'">
						<option value="0"></option>
						'.$camerasTxt.'						
					</select></td>
				</tr>';
			}
		$out.='	
			<tr>
				<td align="center" colspan="2"><input type="submit" name="addScenario" value="Add new quad camera security scenario"> <input type="button" name="cancel" value="Cancel" onClick="self.location=\'index.php?section=securityScenarios\'"></td>
			</tr>
		</table>
			<script>
		 		var frmvalidator = new formValidator("securityScenarios");
 				frmvalidator.addValidation("description","req","Please enter a description");			
			</script>		
	</form>
';
}
else{
	// check if the user has the right to modify installation
	$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
	if (!$canModifyInstallation){
		header("Location: index.php?section=securityScenarios&error=You are not authorised to change the installation.");
		exit(0);
	}
	
	$roomID=(int)$_REQUEST['roomID'];
	$description=cleanString($_POST['description']);
	$hint=cleanString($_POST['hint']);
	$icon=((int)$_POST['icon']!=0)?(int)$_POST['icon']:NULL;
	
	$dbADO->debug=true;
	$insertCommandGroup='
		INSERT INTO CommandGroup 
			(FK_Array,FK_Installation,Description,Hint,CanTurnOff,AlwaysShow,CanBeHidden,FK_Template,FK_Icon)
		VALUES
			(?,?,?,?,?,?,?,?,?)';
	$dbADO->Execute($insertCommandGroup,array($arrayID,$installationID,$description,$hint,0,0,0,$GLOBALS['securityScenariosTemplate'],$icon));
	$cgID=$dbADO->Insert_ID();
	
	$insertCG_Room='
		INSERT INTO CommandGroup_Room 
			(FK_CommandGroup, FK_Room)
		VALUES
			(?,?)';
	$dbADO->Execute($insertCG_Room,array($cgID,$roomID));
	$insertCG_C='
		INSERT INTO CommandGroup_Command 
			(FK_CommandGroup,FK_Command,TurnOff,OrderNum)
		VALUES
			(?,?,?,?)';
	$insertCG_C_CP='
		INSERT INTO CommandGroup_Command_CommandParameter 
			(FK_CommandGroup_Command,FK_CommandParameter,IK_CommandParameter)
		VALUES
			(?,?,?)';
	
	
	for($i=1;$i<5;$i++){
		$camera=(int)$_POST['camera_'.$i];
		
		$dbADO->Execute($insertCG_C,array($cgID,$GLOBALS['commandSetVar'],0,0));
		$cg_cID=$dbADO->Insert_ID();
			
		$dbADO->Execute($insertCG_C_CP,array($cg_cID,$GLOBALS['commandVariableNumber'],$GLOBALS['camerasVariableNumbersArray'][$i-1]));
		$dbADO->Execute($insertCG_C_CP,array($cg_cID,$GLOBALS['commandParameterValueToAsign'],$camera));
	}
	
	$dbADO->Execute($insertCG_C,array($cgID,$GLOBALS['commandGotoScreen'],0,4));
	$cg_cID=$dbADO->Insert_ID();
	$dbADO->Execute($insertCG_C_CP,array($cg_cID,$GLOBALS['commandParameterObjectScreen'],$GLOBALS['mnuSecurityCamerasDesignObj']));
	
	header("Location: index.php?section=securityScenarios");
}


	$output->setNavigationMenu(array("My Scenarios"=>'index.php?section=myScenarios',"Security Scenarios"=>'index.php?section=securityScenarios'));
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();

}
?>