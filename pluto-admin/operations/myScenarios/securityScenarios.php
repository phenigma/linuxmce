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
	$selectCameraCG='
		SELECT CommandGroup.*,IK_CommandParameter 
		FROM CommandGroup 
			INNER JOIN CommandGroup_Command ON FK_CommandGroup=PK_CommandGroup
			INNER JOIN CommandGroup_Command_CommandParameter ON FK_CommandGroup_Command=PK_CommandGroup_Command
		WHERE FK_Template=? AND FK_Installation=? AND FK_CommandParameter=?';
	$resCameraCG=$dbADO->Execute($selectCameraCG,array($GLOBALS['SecurityViewOneCameraTemplate'],$installationID,$GLOBALS['commandParameterValueToAsign']));
	$camerasCGArray=array();
	while($rowCameraCG=$resCameraCG->FetchRow()){
		$camerasCGArray[$rowCameraCG['IK_CommandParameter']]=$rowCameraCG['PK_CommandGroup'];
	}

	$queryRooms='
		SELECT Room.*, Room.Description AS RoomName FROM Room
			INNER JOIN RoomType ON FK_RoomType=PK_RoomType
		WHERE FK_Installation=?
			ORDER BY Room.Description ASC';
	$resRooms=$dbADO->Execute($queryRooms,$installationID);
	if($resRooms->RecordCount()==0){
		$out.='There are no rooms in this installation. Go to <a href="index.php?section=rooms">rooms</a> section to add rooms.';
	}
	$displayedCommandGroups=array();
	$displayedRooms=array();
	while($rowRooms=$resRooms->FetchRow()){
		$queryCG_T='
			SELECT * 
			FROM CommandGroup_Room
				INNER JOIN CommandGroup ON FK_CommandGroup=PK_CommandGroup
			WHERE
				FK_Room=? AND FK_Template=?';
		$resArmDisarm=$dbADO->Execute($queryCG_T,array($rowRooms['PK_Room'],$GLOBALS['SecurityArmDisarmTemplate']));
		$resViewAll=$dbADO->Execute($queryCG_T,array($rowRooms['PK_Room'],$GLOBALS['SecurityViewCamerasTemplate']));
		$resSOS=$dbADO->Execute($queryCG_T,array($rowRooms['PK_Room'],$GLOBALS['SecuritySOSTemplate']));
		$out.='
			<input type="hidden" name="oldArmDisarm_'.$rowRooms['PK_Room'].'" value="'.(($resArmDisarm->RecordCount()>0)?'1':'0').'">
			<input type="hidden" name="oldViewAll_'.$rowRooms['PK_Room'].'" value="'.(($resViewAll->RecordCount()>0)?'1':'0').'">
			<input type="hidden" name="oldSOS_'.$rowRooms['PK_Room'].'" value="'.(($resSOS->RecordCount()>0)?'1':'0').'">';
		$displayedRooms[]=$rowRooms['PK_Room'];
		$out.='
		<tr bgcolor="#D1D9EA">
			<td colspan="2"><B>'.$rowRooms['RoomName'].'</B></td>
		</tr>
		<tr>
			<td width="20">&nbsp;</td>
			<td><input type="checkbox" name="armDisarm_'.$rowRooms['PK_Room'].'" value="1" '.(($resArmDisarm->RecordCount()>0)?'checked':'').'> Button to arm system, disarm system & change masks</td>
		</tr>
		<tr>
			<td width="20">&nbsp;</td>
			<td><input type="checkbox" name="viewAll_'.$rowRooms['PK_Room'].'" value="1" '.(($resViewAll->RecordCount()>0)?'checked':'').'> Button to view all cameras</td>
		</tr>
		<tr>
			<td width="20">&nbsp;</td>
			<td><input type="checkbox" name="sos_'.$rowRooms['PK_Room'].'" value="1" '.(($resSOS->RecordCount()>0)?'checked':'').'> *SOS* Button for emergencies</td>
		</tr>';

		if(count($cameraDevices)>0){
			$cameraStr='<table><tr>';
			foreach($cameraDevices as $cameraID=>$cameraDescription){
				if(isset($camerasCGArray[$cameraID])){
					$queryCG_R='SELECT * FROM CommandGroup_Room WHERE FK_CommandGroup=? AND FK_Room=?';
					$resCG_R=$dbADO->Execute($queryCG_R,array($camerasCGArray[$cameraID],$rowRooms['PK_Room']));
					$cg_room_checked=($resCG_R->RecordCount()>0)?'checked':'';
				}else 
					$cg_room_checked='';
				$out.='<input type="hidden" name="oldCamera_'.$cameraID.'_room_'.$rowRooms['PK_Room'].'" value="'.(($cg_room_checked=='checked')?1:0).'">';
				$cameraStr.='<td><input type="checkbox" name="camera_'.$cameraID.'_room_'.$rowRooms['PK_Room'].'" value="1" '.$cg_room_checked.'> '.$cameraDescription.'</td>';
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
				<td align="center"><a href="index.php?section=securityScenarios&cgID='.$rowCG['PK_CommandGroup'].'&action=editScenario">Edit</a></td>
				<td align="center"><a href="#" onClick="javascript:if(confirm(\'Are you sure you want to delete this scenario?\'))self.location=\'index.php?section=securityScenarios&cgDelID='.$rowCG['PK_CommandGroup'].'\';">Delete</a></td>
			</tr>
			';
		}
		$out.='</table><input type="button" name="newScenario" value="New quad camera security scenario" onClick="self.location=\'index.php?section=securityScenarios&roomID='.$rowRooms['PK_Room'].'&action=addScenario\'">';
	}
	$out.='		</td>
			</tr>';
		if(count($displayedRooms)!=0){
			$out.='
			<tr>
				<td colspan="2" align="center"><input type="submit" name="updateScenario" value="Update"></td>
			</tr>';
		}
			$out.='
		</table>
	<input type="hidden" name="displayedRooms" value="'.join(',',$displayedRooms).'">	
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
}elseif($action=='editScenario'){

	$cgID=$_REQUEST['cgID'];
	$queryCG='SELECT * FROM CommandGroup WHERE PK_CommandGroup=?';
	$resCG=$dbADO->Execute($queryCG,$cgID);
	$rowCG=$resCG->FetchRow();
	$description=$rowCG['Description'];
	$hint=$rowCG['Hint'];

	$selectCameraCG_C='
		SELECT CommandGroup.PK_CommandGroup, FK_Array,FK_Installation, CommandGroup.Description, IK_CommandParameter, FK_CommandGroup_Command
		FROM CommandGroup 
			INNER JOIN CommandGroup_Command ON FK_CommandGroup=PK_CommandGroup 
			INNER JOIN CommandGroup_Command_CommandParameter ON FK_CommandGroup_Command=PK_CommandGroup_Command 
		WHERE FK_CommandParameter=? AND PK_CommandGroup=?';

	$resCameraCG_C=$dbADO->Execute($selectCameraCG_C,array($GLOBALS['commandParameterVariableNumber'],$cgID));
	$CG_CArray=array();
	while($rowCameraCG_C=$resCameraCG_C->FetchRow()){
		$CG_CArray[$rowCameraCG_C['IK_CommandParameter']]=$rowCameraCG_C['FK_CommandGroup_Command'];
	}
	if($resCameraCG_C->RecordCount()>0){
		$selectCameraID='
			SELECT CommandGroup.PK_CommandGroup, FK_Array,FK_Installation, CommandGroup.Description, IK_CommandParameter, FK_CommandGroup_Command
			FROM CommandGroup 
				INNER JOIN CommandGroup_Command ON FK_CommandGroup=PK_CommandGroup 
				INNER JOIN CommandGroup_Command_CommandParameter ON FK_CommandGroup_Command=PK_CommandGroup_Command 
			WHERE PK_CommandGroup=? AND FK_CommandGroup_Command IN ('.join(',',$CG_CArray).') AND FK_CommandParameter=?';
		$resCameraID=$dbADO->Execute($selectCameraID,array($cgID,$GLOBALS['commandParameterValueToAsign']));
		$oldCameraIDs=array();
		while($rowCameraID=$resCameraID->FetchRow()){
			$oldCameraIDs[$rowCameraID['FK_CommandGroup_Command']]=$rowCameraID['IK_CommandParameter'];
		}
		$oldIDs=array();
		foreach($CG_CArray AS $cameraFlag=>$cameraCG_C){
			$oldIDs[$cameraFlag]=$oldCameraIDs[$cameraCG_C];
		}
	}else
		$oldIDs=array();

	$out.='	<form action="index.php" method="POST" name="securityScenarios">
		<input type="hidden" name="section" value="securityScenarios">
		<input type="hidden" name="action" value="update">	
		<input type="hidden" name="cgID" value="'.$cgID.'">	
		<table cellpadding="4" cellspacing="0" border="0">
			<tr>
				<td colspan="2"><h3>Edit quad camera security scenario</h3></td>
			</tr>
			<tr>
				<td><B>Description: </B></td>
				<td><input type="text" name="description" value="'.$description.'"></td>
			</tr>
			<tr>
				<td><B>Hint: </B></td>
				<td><input type="text" name="hint" value="'.$hint.'"></td>
			</tr>
			<tr>
				<td><B>Icon: </B></td>
				<td><select name="icon">
					<option value="0"></option>';
				$queryIcons='SELECT * FROM Icon ORDER BY Description ASC';
				$resIcons=$dbADO->Execute($queryIcons);
				while($rowIcons=$resIcons->FetchRow()){
					$out.='<option value="'.$rowIcons['PK_Icon'].'" '.(($rowIcons['PK_Icon']==$rowCG['FK_Icon'])?'selected':'').'>'.$rowIcons['Description'].'</option>';
				}
				$out.='	
				</select>
				</td>
			</tr>
			<tr>
				<td colspan="2"><b>Select cameras for quad view:</b></td>
			</tr>';
			foreach($GLOBALS['camerasVariableNumbersArray'] as $pos=>$cameraNO){
				$out.='
				<input type="hidden" name="oldCamera_'.$cameraNO.'" value="'.@$oldIDs[$cameraNO].'">
				<input type="hidden" name="oldCameraCG_C_'.$cameraNO.'" value="'.@$CG_CArray[$cameraNO].'">
				<tr>
					<td><B>Camera '.($pos+1).': </B></td>
					<td><select name="camera_'.$cameraNO.'">
						<option value="0"></option>';
					foreach($cameraDevices as $cameraID=>$cameraDescription){
						$out.='<option value="'.$cameraID.'" '.(($cameraID==@$oldIDs[$cameraNO])?'selected':'').'>'.$cameraDescription.'</option>';
					}
				
				$out.='						
					</select></td>
				</tr>';
			}
		$out.='	
			<tr>
				<td align="center" colspan="2"><input type="submit" name="editScenario" value="Update quad camera security scenario"> <input type="button" name="cancel" value="Cancel" onClick="self.location=\'index.php?section=securityScenarios\'"></td>
			</tr>
		</table>
			<script>
		 		var frmvalidator = new formValidator("securityScenarios");
 				frmvalidator.addValidation("description","req","Please enter a description");			
			</script>		
	</form>
';
}else{
	// check if the user has the right to modify installation
	$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
	if (!$canModifyInstallation){
		header("Location: index.php?section=securityScenarios&error=You are not authorised to change the installation.");
		exit(0);
	}
	if(isset($_POST['addScenario'])){
		$roomID=(int)$_REQUEST['roomID'];
		$description=cleanString($_POST['description']);
		$hint=cleanString($_POST['hint']);
		$icon=((int)$_POST['icon']!=0)?(int)$_POST['icon']:NULL;
		
		$insertCommandGroup='
			INSERT INTO CommandGroup 
				(FK_Array,FK_Installation,Description,Hint,CanTurnOff,AlwaysShow,CanBeHidden,FK_Template,FK_Icon)
			VALUES
				(?,?,?,?,?,?,?,?,?)';
		$dbADO->Execute($insertCommandGroup,array($arrayID,$installationID,$description,$hint,0,0,0,$GLOBALS['securityScenariosTemplate'],$icon));
		$cgID=$dbADO->Insert_ID();
		
		$insertCG_Room='
			INSERT INTO CommandGroup_Room 
				(FK_CommandGroup, FK_Room,Sort)
			VALUES
				(?,?,?)';
		$dbADO->Execute($insertCG_Room,array($cgID,$roomID,$cgID));
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
				
			$dbADO->Execute($insertCG_C_CP,array($cg_cID,$GLOBALS['commandParameterVariableNumber'],$GLOBALS['camerasVariableNumbersArray'][$i-1]));
			$dbADO->Execute($insertCG_C_CP,array($cg_cID,$GLOBALS['commandParameterValueToAsign'],$camera));
		}
		
		$dbADO->Execute($insertCG_C,array($cgID,$GLOBALS['commandGotoScreen'],0,4));
		$cg_cID=$dbADO->Insert_ID();
		$dbADO->Execute($insertCG_C_CP,array($cg_cID,$GLOBALS['commandParameterObjectScreen'],$GLOBALS['mnuSecurityCamerasDesignObj']));
	}

	
	
	if(isset($_POST['editScenario'])){
		$description=cleanString($_POST['description']);
		$hint=cleanString($_POST['hint']);
		$icon=((int)$_POST['icon']!=0)?(int)$_POST['icon']:NULL;
		$cgID=$_REQUEST['cgID'];

		$updateCommandGroup='
			UPDATE CommandGroup SET 
				Description=?,Hint=?,FK_Icon=?
			WHERE
				PK_CommandGroup=?';
		$dbADO->Execute($updateCommandGroup,array($description,$hint,$icon,$cgID));
		
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

		foreach($GLOBALS['camerasVariableNumbersArray'] as $cameraNO){			
			$oldCamera=(int)$_POST['oldCamera_'.$cameraNO];
			$oldCameraCG_C=(int)$_POST['oldCameraCG_C_'.$cameraNO];
			$camera=(int)$_POST['camera_'.$cameraNO];

			if($oldCamera==0){
				if($camera!=0){
					$dbADO->Execute($insertCG_C,array($cgID,$GLOBALS['commandSetVar'],0,0));
					$cg_cID=$dbADO->Insert_ID();
				
					$dbADO->Execute($insertCG_C_CP,array($cg_cID,$GLOBALS['commandParameterVariableNumber'],$cameraNO));
					$dbADO->Execute($insertCG_C_CP,array($cg_cID,$GLOBALS['commandParameterValueToAsign'],$camera));
				}
			}elseif($camera==0 || $camera!=$oldCamera){
				$deleteCG_C='DELETE FROM CommandGroup_Command WHERE PK_CommandGroup_Command=?';
				$dbADO->Execute($deleteCG_C,$oldCameraCG_C);
				
				$deleteParameters='
					DELETE CommandGroup_Command_CommandParameter
					FROM CommandGroup_Command_CommandParameter
					JOIN CommandGroup_Command on FK_CommandGroup_Command=PK_CommandGroup_Command
					WHERE FK_CommandGroup=? AND FK_CommandParameter=? AND IK_CommandParameter=?';
				$dbADO->Execute($deleteParameters,array($oldCameraCG_C,$GLOBALS['commandParameterVariableNumber'],$cameraNO));
				$dbADO->Execute($deleteParameters,array($oldCameraCG_C,$GLOBALS['commandParameterValueToAsign'],$oldCamera));
				if($camera!=$oldCamera && $camera!=0){
					$dbADO->Execute($insertCG_C,array($cgID,$GLOBALS['commandSetVar'],0,0));
					$cg_cID=$dbADO->Insert_ID();
				
					$dbADO->Execute($insertCG_C_CP,array($cg_cID,$GLOBALS['commandParameterVariableNumber'],$cameraNO));
					$dbADO->Execute($insertCG_C_CP,array($cg_cID,$GLOBALS['commandParameterValueToAsign'],$camera));
				}
			}
		}
	}
	
	
	if(isset($_POST['updateScenario'])){
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

		$displayedRoomsArray=explode(',',$_POST['displayedRooms']);
		
		$selectArmDisarm='SELECT * FROM CommandGroup WHERE FK_Template=?';
		$resArmDisarm=$dbADO->Execute($selectArmDisarm,$GLOBALS['SecurityArmDisarmTemplate']);
		if($resArmDisarm->RecordCount()){
			$rowArmDisarm=$resArmDisarm->FetchRow();
			$armDisarmCG=$rowArmDisarm['PK_CommandGroup'];
		}else{
			$insertCommandGroup='
				INSERT INTO CommandGroup 
					(FK_Array,FK_Installation,Description,CanTurnOff,AlwaysShow,CanBeHidden,FK_Template)
				VALUES
					(?,?,?,?,?,?,?)';
			$dbADO->Execute($insertCommandGroup,array($GLOBALS['ArrayIDForSecurity'],$installationID,'Arm Disarm',0,0,0,$GLOBALS['SecurityArmDisarmTemplate']));
			$armDisarmCG=$dbADO->Insert_ID();
			
			$dbADO->Execute($insertCG_C,array($armDisarmCG,$GLOBALS['commandGotoScreen'],0,0));
			$cg_cID=$dbADO->Insert_ID();
			$dbADO->Execute($insertCG_C_CP,array($cg_cID,$GLOBALS['commandParameterObjectScreen'],$GLOBALS['mnuSecurityCamerasDesignObj']));
		}

		$selectViewAll='SELECT * FROM CommandGroup WHERE FK_Template=?';
		$resViewAll=$dbADO->Execute($selectViewAll,$GLOBALS['SecurityViewCamerasTemplate']);
		if($resViewAll->RecordCount()){
			$rowViewAll=$resViewAll->FetchRow();
			$viewAllCG=$rowViewAll['PK_CommandGroup'];
		}else{
			$insertCommandGroup='
				INSERT INTO CommandGroup 
					(FK_Array,FK_Installation,Description,CanTurnOff,AlwaysShow,CanBeHidden,FK_Template)
				VALUES
					(?,?,?,?,?,?,?)';
			$dbADO->Execute($insertCommandGroup,array($GLOBALS['ArrayIDForSecurity'],$installationID,'View All Cameras',0,0,0,$GLOBALS['SecurityViewCamerasTemplate']));
			$viewAllCG=$dbADO->Insert_ID();
			
			$dbADO->Execute($insertCG_C,array($viewAllCG,$GLOBALS['commandGotoScreen'],0,0));
			$cg_cID=$dbADO->Insert_ID();
			$dbADO->Execute($insertCG_C_CP,array($cg_cID,$GLOBALS['commandParameterObjectScreen'],$GLOBALS['mnuSecurityCamerasDesignObj']));
		}

		$selectSOS='SELECT * FROM CommandGroup WHERE FK_Template=?';
		$resSOS=$dbADO->Execute($selectSOS,$GLOBALS['SecuritySOSTemplate']);
		if($resSOS->RecordCount()){
			$rowSOS=$resSOS->FetchRow();
			$sosCG=$rowSOS['PK_CommandGroup'];
		}else{
			$insertCommandGroup='
				INSERT INTO CommandGroup 
					(FK_Array,FK_Installation,Description,CanTurnOff,AlwaysShow,CanBeHidden,FK_Template)
				VALUES
					(?,?,?,?,?,?,?)';
			$dbADO->Execute($insertCommandGroup,array($GLOBALS['ArrayIDForSecurity'],$installationID,'*SOS*',0,0,0,$GLOBALS['SecuritySOSTemplate']));
			$sosCG=$dbADO->Insert_ID();

			$dbADO->Execute($insertCG_C,array($sosCG,$GLOBALS['commandGotoScreen'],0,0));
			$cg_cID=$dbADO->Insert_ID();
			$dbADO->Execute($insertCG_C_CP,array($cg_cID,$GLOBALS['commandParameterObjectScreen'],$GLOBALS['mnuSecurityCamerasDesignObj']));
		}

		$insertCG_Room='
			INSERT INTO CommandGroup_Room 
				(FK_CommandGroup, FK_Room,Sort)
			VALUES
				(?,?,?)';
		$deleteCG_Room='DELETE FROM CommandGroup_Room  WHERE FK_CommandGroup=? AND FK_Room=?';
		$uncheckedArmDisarm=0;
		$uncheckedViewAll=0;
		$uncheckedSOS=0;
		foreach($displayedRoomsArray as $room){
			$armDisarm=(int)@$_POST['armDisarm_'.$room];
			$oldArmDisarm=(int)@$_POST['oldArmDisarm_'.$room];
			if($oldArmDisarm==0){
				if($armDisarm==1){
					$dbADO->Execute($insertCG_Room,array($armDisarmCG,$room,$armDisarmCG));
				}else 
					$uncheckedArmDisarm++;
			}elseif($armDisarm==0){
				$uncheckedArmDisarm++;
				$dbADO->Execute($deleteCG_Room,array($armDisarmCG,$room));
			}

			
			$viewAll=(int)@$_POST['viewAll_'.$room];
			$oldViewAll=(int)@$_POST['oldViewAll_'.$room];
			if($oldViewAll==0){
				if($viewAll==1){
					$dbADO->Execute($insertCG_Room,array($viewAllCG,$room,$viewAllCG));
				}else 
					$uncheckedViewAll++;
			}elseif($viewAll==0){
				$uncheckedViewAll++;
				$dbADO->Execute($deleteCG_Room,array($viewAllCG,$room));
			}
			
			$sos=(int)@$_POST['sos_'.$room];
			$oldSos=(int)@$_POST['oldSOS_'.$room];
			if($oldSos==0){
				if($sos==1){
					$dbADO->Execute($insertCG_Room,array($sosCG,$room,$sosCG));
				}else
					$uncheckedSOS++;
			}elseif($sos==0){
				$uncheckedSOS++;
				$dbADO->Execute($deleteCG_Room,array($sosCG,$room));
			}
		}
		if($uncheckedArmDisarm==count($displayedRoomsArray))
			deleteCommandGroup($armDisarmCG,$dbADO);
		if($uncheckedViewAll==count($displayedRoomsArray))
			deleteCommandGroup($viewAllCG,$dbADO);
		if($uncheckedSOS==count($displayedRoomsArray))
			deleteCommandGroup($sosCG,$dbADO);

			
		// Update CommandGroups description
		$displayedCommandGroupsArray=explode(',',$_POST['displayedCommandGroups']);
		foreach($displayedCommandGroupsArray as $commandGroup){
			$description=@$_POST['commandGroup_'.$commandGroup];
			$updateCommandGroup='UPDATE CommandGroup SET Description=? WHERE PK_CommandGroup=?';
			$dbADO->Execute($updateCommandGroup,array($description,$commandGroup));
		}

		$selectCameraCG='
			SELECT CommandGroup.*,IK_CommandParameter 
			FROM CommandGroup 
				INNER JOIN CommandGroup_Command ON FK_CommandGroup=PK_CommandGroup
				INNER JOIN CommandGroup_Command_CommandParameter ON FK_CommandGroup_Command=PK_CommandGroup_Command
			WHERE FK_Template=? AND FK_Installation=? AND FK_CommandParameter=?';
		$resCameraCG=$dbADO->Execute($selectCameraCG,array($GLOBALS['SecurityViewOneCameraTemplate'],$installationID,$GLOBALS['commandParameterValueToAsign']));
		$camerasCGArray=array();
		while($rowCameraCG=$resCameraCG->FetchRow()){
			$camerasCGArray[$rowCameraCG['IK_CommandParameter']]=$rowCameraCG['PK_CommandGroup'];
		}
		// insert CommandGroups for each camera if doesn't exist
		foreach($cameraDevices AS $cameraID=>$cameraDescription){
			$uncheckedRooms=0;
			foreach($displayedRoomsArray as $room){		
				$addToRoom=(isset($_POST['camera_'.$cameraID.'_room_'.$room]))?(int)$_POST['camera_'.$cameraID.'_room_'.$room]:0;
				$oldCamera=(int)$_POST['oldCamera_'.$cameraID.'_room_'.$room];
				if($addToRoom==1){
					if(!isset($camerasCGArray[$cameraID])){
						// insert command group
						$insertCommandGroup='
							INSERT INTO CommandGroup 
								(FK_Array,FK_Installation,Description,CanTurnOff,AlwaysShow,CanBeHidden,FK_Template)
							VALUES
								(?,?,?,?,?,?,?)';
						$dbADO->Execute($insertCommandGroup,array($GLOBALS['ArrayIDForSecurity'],$installationID,'View '.$cameraDescription,0,0,0,$GLOBALS['SecurityViewOneCameraTemplate']));
						$cgID=$dbADO->Insert_ID();
						$camerasCGArray[$cameraID]=$cgID;
						
						$dbADO->Execute($insertCG_C,array($cgID,$GLOBALS['commandSetVar'],0,0));					
						$cg_cID=$dbADO->Insert_ID();
						
						$dbADO->Execute($insertCG_C_CP,array($cg_cID,$GLOBALS['commandParameterVariableNumber'],$GLOBALS['cameraVariable']));
						$dbADO->Execute($insertCG_C_CP,array($cg_cID,$GLOBALS['commandParameterValueToAsign'],$cameraID));
						
						$dbADO->Execute($insertCG_C,array($cgID,$GLOBALS['commandGotoScreen'],0,1));
						$cg_cID=$dbADO->Insert_ID();
						$dbADO->Execute($insertCG_C_CP,array($cg_cID,$GLOBALS['commandParameterObjectScreen'],$GLOBALS['mnuSingleCameraViewOnlyDesignObj']));
						
						$dbADO->Execute($insertCG_Room,array($camerasCGArray[$cameraID],$room,$camerasCGArray[$cameraID]));	
					}else{
						if($oldCamera==0)
							$dbADO->Execute($insertCG_Room,array($camerasCGArray[$cameraID],$room,$camerasCGArray[$cameraID]));	
					}
				}elseif($oldCamera==1){
					$dbADO->Execute($deleteCG_Room,array($camerasCGArray[$cameraID],$room));
					$uncheckedRooms++;
				}else
					$uncheckedRooms++;
			}
			if($uncheckedRooms==count($displayedRoomsArray)){
				// all camera checkboxes are unchecked, delete the command group
				if(isset($camerasCGArray[$cameraID]))
					deleteCommandGroup($camerasCGArray[$cameraID],$dbADO);
			}
		}
	}
	
	header("Location: index.php?section=securityScenarios");
}


	$output->setNavigationMenu(array("My Scenarios"=>'index.php?section=myScenarios',"Security Scenarios"=>'index.php?section=securityScenarios'));
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Security Scenarios');
	$output->output();

}
?>