<?php
function telecomScenarios($output,$dbADO) {
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */

	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$out='';
	$installationID = (int)@$_SESSION['installationID'];
	$arrayID=$GLOBALS['ArrayIDCommunicationScenarios'];
	$templateWizard=$GLOBALS['TelecomScenariosTemplate'];

	$deviceCategory=$GLOBALS['rootPhones'];
	getDeviceCategoryChildsArray($deviceCategory,$dbADO);
	$GLOBALS['childsDeviceCategoryArray']=cleanArray($GLOBALS['childsDeviceCategoryArray']);
	$GLOBALS['childsDeviceCategoryArray'][]=$deviceCategory;

	$queryDeviceTemplate='
			SELECT * FROM DeviceTemplate 
				WHERE FK_DeviceCategory IN ('.join(',',$GLOBALS['childsDeviceCategoryArray']).')
			ORDER BY Description ASC';
	$resDeviceTemplate=$dbADO->Execute($queryDeviceTemplate);
	$phonesDTArray=array();
	while($rowDeviceCategory=$resDeviceTemplate->FetchRow()){
		$phonesDTArray[$rowDeviceCategory['PK_DeviceTemplate']]=$rowDeviceCategory['Description'];
	}

	if(isset($_REQUEST['cgID']) && (int)$_REQUEST['cgID']!=0){
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$installationID,$dbADO);
	
		if (!$canModifyInstallation) {
			header("Location: index.php?section=telecomScenarios&error=You are not authorised to modifiy installation.");
			exit();
		}

		$delCG_ID=(int)$_REQUEST['cgID'];
		deleteCommandGroup($delCG_ID,$dbADO);
		
		header("Location: index.php?section=telecomScenarios&msg=The speed dial was deleted.");
		exit();
	}

	if($action=='form') {

		$out.='<h2>Telecom Scenarios</h2>';

		$out.='
	<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>	
	<form action="index.php" method="POST" name="telecomScenarios">
		<input type="hidden" name="section" value="telecomScenarios">
		<input type="hidden" name="action" value="add">	
		<input type="hidden" name="roomID" value="">	
		<input type="hidden" name="roomName" value="">	
		
	<table width="100%" cellpadding="4" cellspacing="0" border="0">';
		$queryUsers='
		SELECT * FROM Users
			INNER JOIN Installation_Users ON FK_Users=PK_Users
		WHERE FK_Installation=? ORDER BY Users.UserName ASC';
		$resUsers=$dbADO->Execute($queryUsers,$installationID);
		$usersArray=array();
		while($rowUsers=$resUsers->FetchRow()){
			$usersArray[$rowUsers['PK_Users']]=$rowUsers['UserName'];
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
		$displayedRooms=array();
		$displayedRoomNames=array();
		$oldTelecomScenarios=array();
		$displayedSpeedDial=array();
		while($rowRooms=$resRooms->FetchRow()){
			$displayedRooms[]=$rowRooms['PK_Room'];
			$displayedRoomNames[]=$rowRooms['RoomName'];
			$queryCG='
				SELECT PK_CommandGroup FROM CommandGroup
					INNER JOIN CommandGroup_Room on CommandGroup_Room.FK_CommandGroup=PK_CommandGroup
				WHERE FK_Room=? AND FK_Template=?';
			$resCG=$dbADO->Execute($queryCG,array($rowRooms['PK_Room'],$GLOBALS['TelecomScenariosTemplate']));
			$rowCG=$resCG->FetchRow();
			$roomCG=$rowCG['PK_CommandGroup'];
			$oldTelecomScenarios[]=($resCG->RecordCount()>0)?1:0;
			$out.='
		<tr bgcolor="#D1D9EA">
			<td align="right" width="20">&nbsp;</td>
			<td><B>'.$rowRooms['RoomName'].'</B></td>
			<td align="center"><input type="button" name="addSpeedDial" value="Add speed dial" onClick="self.location=\'index.php?section=telecomScenarios&action=addSpeedDial&roomID='.$rowRooms['PK_Room'].'\'&roomName='.$rowRooms['RoomName'].'\'"></td>
		</tr>
		<tr>
			<td align="right" width="20">&nbsp;</td>
			<td><input type="checkbox" name="phoneSystem_'.$rowRooms['PK_Room'].'" value="1" '.(($resCG->RecordCount()>0)?'checked':'').' onClick="document.telecomScenarios.roomName.value=\''.$rowRooms['RoomName'].'\';document.telecomScenarios.submit();"> Click to acces phone system from this room.</td>
			<td>&nbsp;</td>
		</tr>
		<tr>
			<td align="right" width="20">&nbsp;</td>
			<td><input type="hidden" name="oldCG_R_'.$rowRooms['PK_Room'].'" value="'.$rowCG['PK_CommandGroup'].'">
			<table>
				<tr>
					<td colspan="'.(2*count($usersArray)).'"><B>Status/voicemail indicators</B></td>
				</tr>
				<tr>';
			foreach($usersArray AS $userID=>$username){
				$queryCGP='
					SELECT * FROM CommandGroup_Command_CommandParameter 
						INNER JOIN CommandGroup_Command ON FK_CommandGroup_Command=PK_CommandGroup_Command
						INNER JOIN CommandGroup ON FK_CommandGroup=PK_CommandGroup
					WHERE FK_CommandParameter=? AND IK_CommandParameter=? AND PK_CommandGroup=?';
				$resCGP=$dbADO->Execute($queryCGP,array($GLOBALS['commandParamPKUsers'],$userID,$rowCG['PK_CommandGroup']));

				$out.='
					<td>'.$username.'</td>
					<td align="center">
						<input type="hidden" name="oldStatusVM_'.$rowRooms['PK_Room'].'_'.$userID.'" value="'.(($resCGP->RecordCount()>0)?'1':'0').'">
						<input type="checkbox" name="statusVM_'.$rowRooms['PK_Room'].'_'.$userID.'" value="1" '.(($resCG->RecordCount()>0)?'':'disabled').' onClick="document.telecomScenarios.submit();" '.(($resCGP->RecordCount()>0)?'checked':'').'>
					</td>
				';
			}
			$out.='
				</tr>
			</table>
			<table>
				<tr>
					<td colspan="3"><B>Speed dial</B></td>
				</tr>
				<tr bgcolor="lightblue">
					<td align="center"><B>Description</B></td>
					<td align="center"><B>Number</B></td>
					<td align="center"><B>Phone</B></td>
					<td align="center">&nbsp;</td>
				</tr>';
				$queryOtherCG='
				SELECT PK_CommandGroup,Description FROM CommandGroup
					INNER JOIN CommandGroup_Room on CommandGroup_Room.FK_CommandGroup=PK_CommandGroup
				WHERE FK_Room=? AND FK_Template=? AND PK_CommandGroup!=?';

			$resCG=$dbADO->Execute($queryOtherCG,array($rowRooms['PK_Room'],$GLOBALS['TelecomScenariosTemplate'],$roomCG));
			if($resCG->RecordCount()==0){
				$out.='
				<tr>
					<td align="center" colspan="4">No speed dial for this room.</td>
				</tr>';
			}
			while($rowSpeedDial=$resCG->FetchRow()){
				$displayedSpeedDial[]=$rowSpeedDial['PK_CommandGroup'];
				$queryNumber='
					SELECT * FROM CommandGroup_Command_CommandParameter
						INNER JOIN CommandGroup_Command ON FK_CommandGroup_Command=PK_CommandGroup_Command
						INNER JOIN CommandGroup ON CommandGroup_Command.FK_CommandGroup=PK_CommandGroup
					WHERE PK_CommandGroup=? AND FK_CommandParameter=?';
				$resNumber=$dbADO->Execute($queryNumber,array($rowSpeedDial['PK_CommandGroup'],$GLOBALS['commandPhoneExtension']));
				$rowNumber=$resNumber->FetchRow();
				
				$querySelDevice='
					SELECT * FROM CommandGroup_Command_CommandParameter
						INNER JOIN CommandGroup_Command ON FK_CommandGroup_Command=PK_CommandGroup_Command
						INNER JOIN CommandGroup ON CommandGroup_Command.FK_CommandGroup=PK_CommandGroup
					WHERE PK_CommandGroup=? AND FK_CommandParameter=?';
				$resSelDevice=$dbADO->Execute($querySelDevice,array($rowSpeedDial['PK_CommandGroup'],$GLOBALS['commandParamPK_Device']));
				$rowSelDevice=$resSelDevice->FetchRow();
				$out.='
				<tr bgColor="#F0F3F8">
					<td align="center"><input type="text" name="description_'.$rowSpeedDial['PK_CommandGroup'].'" value="'.$rowSpeedDial['Description'].'"></td>
					<td align="center"><input type="text" name="number_'.$rowSpeedDial['PK_CommandGroup'].'" value="'.$rowNumber['IK_CommandParameter'].'"></td>
					<td align="center">';
				if(count($phonesDTArray)==0)
					$phonesDTArray[]=0;
				$queryDevice='
					SELECT 
						Device.* FROM Device 
					WHERE
						FK_DeviceTemplate IN ('.join(',',array_keys($phonesDTArray)).') AND FK_Installation=?';	
				$resDevice=$dbADO->Execute($queryDevice,$installationID);
				$out.='<select name="phone_'.$rowSpeedDial['PK_CommandGroup'].'">
						<option value="0"></option>';
				while($rowDevice=$resDevice->FetchRow()){
					$out.='<option value="'.$rowDevice['PK_Device'].'" '.(($rowDevice['PK_Device']==$rowSelDevice['IK_CommandParameter'])?'selected':'').'>'.$rowDevice['Description'].'</option>';
				}
				$out.='
				</select>';
				$out.='
					</td>
					<td><a href="#" onClick="if(confirm(\'Are you sure you want t delete this speed dial?\'))self.location=\'index.php?section=telecomScenarios&cgID='.$rowSpeedDial['PK_CommandGroup'].'\'">Delete</a></td>
				</tr>';
			}
			if($resCG->RecordCount()>0){
				$out.='
				<tr>
					<td align="center" colspan="4"><input type="submit" name="update" value="Update Speed Dial"></td>
				</tr>';
			}
			$out.='
			</table>
			</td>
			<td>&nbsp;</td>
		</tr>';
		}
		$out.='
		</table>
		
		<input type="hidden" name="displayedSpeedDial" value="'.join(',',$displayedSpeedDial).'">
		<input type="hidden" name="displayedUsers" value="'.join(',',array_keys($usersArray)).'">
		<input type="hidden" name="displayedRooms" value="'.join(',',$displayedRooms).'">
		<input type="hidden" name="displayedRoomNames" value="'.join(',',$displayedRoomNames).'">
		<input type="hidden" name="oldTelecomScenarios" value="'.join(',',$oldTelecomScenarios).'">
	</form>
	';

	}elseif($action=='addSpeedDial'){
		$out.='
		<form action="index.php" method="POST" name="telecomScenarios">
			<input type="hidden" name="section" value="telecomScenarios">
			<input type="hidden" name="action" value="add">	
			<input type="hidden" name="roomID" value="'.$_REQUEST['roomID'].'">	
			<table align="center">
				<tr bgcolor="#D1D9EA">
					<td colspan="2" align="center"><B>Add speed dial</B></td>
				</tr>
				<tr>
					<td>Description: </td>
					<td><input type="text" name="description" value=""></td>
				</tr>
				<tr>
					<td>Number: </td>
					<td><input type="text" name="number" value=""></td>
				</tr>
				<tr>
					<td>Phone: </td>
					<td>';
				if(count($phonesDTArray)==0)
					$phonesDTArray[]=0;
				$queryDevice='
					SELECT 
						Device.* FROM Device 
					WHERE
						FK_DeviceTemplate IN ('.join(',',array_keys($phonesDTArray)).') AND FK_Installation=?';	
				$resDevice=$dbADO->Execute($queryDevice,$installationID);
				$out.='<select name="phone">
						<option value="0"></option>';
				while($rowDevice=$resDevice->FetchRow()){
					$out.='<option value="'.$rowDevice['PK_Device'].'">'.$rowDevice['Description'].'</option>';
				}
				$out.='
				</select>';
				$out.='		
					</td>
				</tr>
				<tr>
					<td align="center" colspan="2"><input type="submit" name="addSpeedDial" value="Add speed dial"></td>
				</tr>
			</table>
			<script>
		 		var frmvalidator = new formValidator("telecomScenarios");
 				frmvalidator.addValidation("description","req","Please enter a description");			
				frmvalidator.addValidation("number","req","Please type the number");			
				frmvalidator.addValidation("phone","dontselect=0","Please select a phone. If there is no phone available, add one first.");			
			</script>
				
		</form>
		';
	}else{
		// process area
		//check if current user canModifyInstallation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$installationID,$dbADO);
	
		if (!$canModifyInstallation) {
			header("Location: index.php?section=telecomScenarios&error=You are not authorised to modifiy installation.");
			exit();
		}
		
		if(isset($_POST['addSpeedDial'])){
			$description=$_POST['description'];
			$number=$_POST['number'];
			$phone=(isset($_POST['phone']) && $_POST['phone']!='0')?$_POST['phone']:NULL;
			$roomID=$_REQUEST['roomID'];
			$roomName=$_REQUEST['roomName'];
			
			$insertTelecomScenario='INSERT INTO CommandGroup (FK_Array, FK_Installation, Description,FK_Template,Hint) VALUES (?,?,?,?,?)';
			$dbADO->Execute($insertTelecomScenario,array($arrayID,$installationID,$description,$templateWizard,$roomName));
			$cgID=$dbADO->Insert_ID();
					
			$insertCG_R='INSERT INTO CommandGroup_Room (FK_Room, FK_CommandGroup,Sort) VALUES (?,?,?)';
			$dbADO->Execute($insertCG_R,array($roomID,$cgID,$cgID));
			
			$queryInsertCommandGroup_Command = "INSERT INTO CommandGroup_Command (FK_CommandGroup,FK_Command,FK_Device) VALUES(?,?,?)";								
			$dbADO->Execute($queryInsertCommandGroup_Command,array($cgID,$GLOBALS['commandPL_Originate'],$GLOBALS['localOrbiter']));			
			$CG_C_insertID=$dbADO->Insert_ID();
								
			$insertCommandParam='INSERT INTO CommandGroup_Command_CommandParameter (FK_CommandGroup_Command,FK_CommandParameter,IK_CommandParameter) VALUES (?,?,?)';
			$dbADO->Execute($insertCommandParam,array($CG_C_insertID,$GLOBALS['commandPhoneExtension'],$number));
			
			$insertCommandParam='INSERT INTO CommandGroup_Command_CommandParameter (FK_CommandGroup_Command,FK_CommandParameter,IK_CommandParameter) VALUES (?,?,?)';
			$dbADO->Execute($insertCommandParam,array($CG_C_insertID,$GLOBALS['commandParamPK_Device'],$phone));
			
			header("Location: index.php?section=telecomScenarios&msg=Speed dial added.");
			exit();
		}
		
		if(isset($_POST['update'])){
			$displayedSpeedDialArray=explode(',',$_POST['displayedSpeedDial']);
			foreach($displayedSpeedDialArray AS $speedDial){
				$description=$_POST['description_'.$speedDial];
				$number=$_POST['number_'.$speedDial];
				$phone=(isset($_POST['phone_'.$speedDial]) && $_POST['phone_'.$speedDial]!='0')?$_POST['phone_'.$speedDial]:NULL;
				
				$updateCG='UPDATE CommandGroup SET Description=? WHERE PK_CommandGroup=?';
				$dbADO->Execute($updateCG,array($description,$speedDial));
				
				$queryNumberCGP='
					SELECT * FROM CommandGroup_Command_CommandParameter
						INNER JOIN CommandGroup_Command ON FK_CommandGroup_Command=PK_CommandGroup_Command
						INNER JOIN CommandGroup ON CommandGroup_Command.FK_CommandGroup=PK_CommandGroup
						INNER JOIN CommandGroup_Room ON CommandGroup_Room.FK_CommandGroup=PK_CommandGroup
					WHERE PK_CommandGroup=? AND FK_CommandParameter=?';
				$resNumber=$dbADO->Execute($queryNumberCGP,array($speedDial,$GLOBALS['commandPhoneExtension']));
				$rowNumber=$resNumber->FetchRow();
				$updateCGP='UPDATE CommandGroup_Command_CommandParameter SET IK_CommandParameter=? WHERE FK_CommandGroup_Command=? AND FK_CommandParameter=?';
				$dbADO->Execute($updateCGP,array($number,$rowNumber['FK_CommandGroup_Command'],$GLOBALS['commandPhoneExtension']));
				
				$querySelDevice='
					SELECT * FROM CommandGroup_Command_CommandParameter
						INNER JOIN CommandGroup_Command ON FK_CommandGroup_Command=PK_CommandGroup_Command
						INNER JOIN CommandGroup ON CommandGroup_Command.FK_CommandGroup=PK_CommandGroup
						INNER JOIN CommandGroup_Room ON CommandGroup_Room.FK_CommandGroup=PK_CommandGroup
					WHERE PK_CommandGroup=? AND FK_CommandParameter=?';
				$resSelDevice=$dbADO->Execute($querySelDevice,array($speedDial,$GLOBALS['commandParamPK_Device']));
				$rowSelDevice=$resSelDevice->FetchRow();
				$dbADO->Execute($updateCGP,array($phone,$rowSelDevice['FK_CommandGroup_Command'],$GLOBALS['commandParamPK_Device']));
			}
		}
		
		$displayedRoomsArray=explode(',',$_POST['displayedRooms']);
		$displayedRoomNamesArray=explode(',',$_POST['displayedRoomNames']);
		$oldTelecomScenariosArray=explode(',',$_POST['oldTelecomScenarios']);
		foreach($displayedRoomsArray as $key=>$roomID){
			$newScenario=(isset($_POST['phoneSystem_'.$roomID]) && $_POST['phoneSystem_'.$roomID]==1)?1:0;
			if($oldTelecomScenariosArray[$key]==0){
				if($newScenario==1){
					// insert new CG
					$insertTelecomScenario='INSERT INTO CommandGroup (FK_Array, FK_Installation, Description,FK_Template,Hint) VALUES (?,?,?,?,?)';
					$dbADO->Execute($insertTelecomScenario,array($arrayID,$installationID,'Telecom Scenario',$templateWizard,$displayedRoomNamesArray[$key]));
					$cgID=$dbADO->Insert_ID();
					
					$insertCG_R='INSERT INTO CommandGroup_Room (FK_Room, FK_CommandGroup,Sort) VALUES (?,?,?)';
					$dbADO->Execute($insertCG_R,array($roomID,$cgID,$cgID));
			
				}
			}elseif($newScenario==0){
				// delete CG
				$queryCG='
					SELECT PK_CommandGroup FROM CommandGroup
						INNER JOIN CommandGroup_Room on CommandGroup_Room.FK_CommandGroup=PK_CommandGroup
					WHERE FK_Room=? AND FK_Template=?';
				$resCG=$dbADO->Execute($queryCG,array($roomID,$GLOBALS['TelecomScenariosTemplate']));
				$rowCG=$resCG->FetchRow();
				deleteCommandGroup($rowCG['PK_CommandGroup'],$dbADO);
			}else{
				if(isset($_POST['oldCG_R_'.$roomID])){
					$cgID=$_POST['oldCG_R_'.$roomID];
					$displayedUsersArray=explode(',',$_POST['displayedUsers']);
					foreach($displayedUsersArray AS $userID){
						$oldCommandParam=(isset($_POST['oldStatusVM_'.$roomID.'_'.$userID]))?$_POST['oldStatusVM_'.$roomID.'_'.$userID]:0;
						$newCommandParam=isset($_POST['statusVM_'.$roomID.'_'.$userID])?$_POST['statusVM_'.$roomID.'_'.$userID]:0;
						if($oldCommandParam==0){
							if($newCommandParam==1){
								$queryInsertCommandGroup_Command = "INSERT INTO CommandGroup_Command (FK_CommandGroup,FK_Command,FK_Device) VALUES(?,?,?)";								
								$dbADO->Execute($queryInsertCommandGroup_Command,array($cgID,$GLOBALS['commandSetUserMode'],$GLOBALS['localOrbiter']));			
								$CG_C_insertID=$dbADO->Insert_ID();
								
								$insertCommandParam='INSERT INTO CommandGroup_Command_CommandParameter (FK_CommandGroup_Command,FK_CommandParameter,IK_CommandParameter) VALUES (?,?,?)';
								$dbADO->Execute($insertCommandParam,array($CG_C_insertID,$GLOBALS['commandParamPKUsers'],$userID));
						
							}
						}elseif($newCommandParam==0){
							// delete command
							$queryCGP='
								SELECT * FROM CommandGroup_Command_CommandParameter 
									INNER JOIN CommandGroup_Command ON FK_CommandGroup_Command=PK_CommandGroup_Command
									INNER JOIN CommandGroup ON FK_CommandGroup=PK_CommandGroup
								WHERE FK_CommandParameter=? AND IK_CommandParameter=? AND PK_CommandGroup=?';
							$resCGP=$dbADO->Execute($queryCGP,array($GLOBALS['commandParamPKUsers'],$userID,$cgID));

							$rowCGP=$resCGP->FetchRow();
							$deleteCommandParameter='DELETE FROM CommandGroup_Command_CommandParameter WHERE FK_CommandGroup_Command=? AND FK_CommandParameter=? AND IK_CommandParameter=?';
							$dbADO->Execute($deleteCommandParameter,array($rowCGP['FK_CommandGroup_Command'],$GLOBALS['commandParamPKUsers'],$userID));
							
							$deleteCommand='DELETE FROM CommandGroup_Command WHERE PK_CommandGroup_Command=?';
							$dbADO->Execute($deleteCommand,$rowCGP['PK_CommandGroup_Command']);
						}
					}
				}
			}
		}
		
		header("Location: index.php?section=telecomScenarios&msg=Telecom scenario updated.");
	}

	$output->setScriptInBody("onLoad=\"javascript:top.treeframe.location.reload();\"");
	$output->setNavigationMenu(array("My Scenarios"=>'index.php?section=myScenarios',"Telecom Scenarios"=>'index.php?section=telecomScenarios'));
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Lighting Scenarios');
	$output->output();

}
?>