<?php
function mobileScenarios($output,$dbADO) {
/* @var $dbADO ADOConnection */
/* @var $rs ADORecordSet */

$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
$out='
<script>
function windowOpen(locationA,attributes) 
{
	window.open(locationA,\'\',attributes);
}
</script>
';

$installationID = (int)@$_SESSION['installationID'];
$arrayID = $GLOBALS['ArrayIDForMobileOrbiterScenarios'];	
$templateArray=array($GLOBALS['LightingScenariosTemplate']=>'Lighting Scenarios', $GLOBALS['ClimateScenariosTemplate']=>'Climate Scenarios');

$allTemplateArray=array();
$resAll=$dbADO->Execute('SELECT Description, PK_Template FROM Template ORDER BY Description ASC');
while($rowAll=$resAll->FetchRow()){
	$allTemplateArray[$rowAll['PK_Template']]=$rowAll['Description'];
}

if($action=='form') {
	
	$queryArray = 'SELECT * FROM Array WHERE PK_Array=?';	
	$resArray = $dbADO->Execute($queryArray,$arrayID);
	$rowArray=$resArray->FetchRow();

	$out.='
	<div align="center" class="confirm"><B>'.(isset($_GET['msg'])?strip_tags($_GET['msg'].'<br>'):'').'</B></div>
	<h2 align="center">'.$rowArray['Description'].'</h2>';

	$out.='
	<form action="index.php" method="POST" name="mobileScenarios">
		<input type="hidden" name="section" value="mobileScenarios">
		<input type="hidden" name="action" value="add">	
		<input type="hidden" name="roomID" value="">
		<input type="hidden" name="editedCgID" value="">
		<input type="hidden" name="editedTemplate" value="">	
		<input type="hidden" name="roomName" value="">
	
	<table width="100%" cellpadding="4" cellspacing="0" border="0">';
	$queryRooms='
		SELECT Room.*, Room.Description AS RoomName FROM Room
		WHERE FK_Installation=?
			ORDER BY Room.Description ASC';
	$resRooms=$dbADO->Execute($queryRooms,$installationID);
	if($resRooms->RecordCount()==0){
		$out.='There are no rooms in this installation. Go to <a href="index.php?section=rooms">rooms</a> section to add rooms.';
	}
	$displayedCommandGroups=array();
	$firstRoomArray=array();
	while($rowRooms=$resRooms->FetchRow()){
		$out.='
		<tr bgcolor="#D1D9EA">
			<td><B>'.$rowRooms['RoomName'].'</B></td>
			<td colspan="4" align="right"><select name="newScenarioType_'.$rowRooms['PK_Room'].'">';
			foreach($templateArray AS $scenarioType=>$scenarioDescription){
				$out.='<option value="'.$scenarioType.'">'.$scenarioDescription.'</option>';
			}
			$out.='
				<option value="0">Advanced mode</option>
			</select> <input type="button" class="button" name="add" value="Add scenario" onClick="javascript:document.mobileScenarios.action.value=\'addToRoom\';document.mobileScenarios.roomID.value='.$rowRooms['PK_Room'].';document.mobileScenarios.roomName.value=\''.$rowRooms['RoomName'].'\';document.mobileScenarios.submit();"></td>
		</tr>';
		$selectCommandGroups='
			SELECT * 
			FROM CommandGroup
			INNER JOIN CommandGroup_Room ON FK_CommandGroup=PK_CommandGroup
			WHERE FK_Array=? AND FK_Room=? AND FK_Installation=?
			ORDER BY Sort ASC';
		$resCommandGroups=$dbADO->Execute($selectCommandGroups,array($arrayID,$rowRooms['PK_Room'],$installationID));
		if($resCommandGroups->RecordCount()==0){
		$out.='
			<tr>
				<td colspan="5">No mobile orbiter scenarios for this room.</td>
			</tr>';
		}
		$pos=0;
		$posInRoom=array();
		while($rowCG=$resCommandGroups->FetchRow()){
			$posInRoom[$rowCG['PK_CommandGroup']]=$rowCG['Sort'];
		}
		$resCommandGroups->MoveFirst();
		while($rowCG=$resCommandGroups->FetchRow()){
			if(!in_array($rowCG['PK_CommandGroup'],$displayedCommandGroups))
				$firstRoomArray[$rowCG['PK_CommandGroup']]=$rowRooms['RoomName'];
			$pos++;
			$out.='
			<tr bgcolor="'.(($rowCG['PK_CommandGroup']==@$_REQUEST['lastAdded'])?'lightgreen':'').'">
				<td><input type="button" class="button" name="posDown" value="-" onClick="self.location=\'index.php?section=mobileScenarios&cgID='.$rowCG['PK_CommandGroup'].'&action=process&roomID='.$rowRooms['PK_Room'].'&operation=down&posInRoom='.urlencode(serialize($posInRoom)).'\'"> 
					<input type="button" class="button" name="posUp" value="+" onClick="self.location=\'index.php?section=mobileScenarios&cgID='.$rowCG['PK_CommandGroup'].'&action=process&roomID='.$rowRooms['PK_Room'].'&operation=up&posInRoom='.urlencode(serialize($posInRoom)).'\'">
				 Description: '.((!in_array($rowCG['PK_CommandGroup'],$displayedCommandGroups))?'<input type="text" name="commandGroup_'.$rowCG['PK_CommandGroup'].'" value="'.$rowCG['Description'].'"> Hint: <input type="text" name="hintCommandGroup_'.$rowCG['PK_CommandGroup'].'" value="'.$rowCG['Hint'].'">':'<b>'.$rowCG['Description'].': </b>Hint: <b>'.$rowCG['Hint'].'</b> (See '.$firstRoomArray[$rowCG['PK_CommandGroup']].')').'</td>
				<td>Category: <b>'.$allTemplateArray[$rowCG['FK_Template']].'</b></td>
				<td>&nbsp;</td>';
			if(in_array($rowCG['FK_Template'],array_keys($templateArray)))
				$editLink='<a href="#" onClick="document.mobileScenarios.editedTemplate.value='.$rowCG['FK_Template'].';document.mobileScenarios.editedCgID.value='.$rowCG['PK_CommandGroup'].';document.mobileScenarios.roomID.value='.$rowRooms['PK_Room'].';document.mobileScenarios.submit();">Edit</a>';
			else
				$editLink='<a href="index.php?section=editCommandGroup&cgID='.$rowCG['PK_CommandGroup'].'">Edit</a>';
			$out.='
				<td>'.$editLink.' <a href="#" onClick="javascript:if(confirm(\'Are you sure you want to delete this scenario?\'))self.location=\'index.php?section=mobileScenarios&action=delete&cgDelID='.$rowCG['PK_CommandGroup'].'\';">Delete</a></td>
				<td>&nbsp;</td>
			</tr>
			';
			$displayedCommandGroups[]=$rowCG['PK_CommandGroup'];
		}
	}
	if(count($displayedCommandGroups)>0){
		$out.='
		<tr>
			<td colspan="3" align="center"><input type="submit" class="button" name="updateCG" value="Update"  ></td>
			<td>&nbsp;</td>
		</tr>';
	}
	$out.='
		</table>
	<input type="hidden" name="displayedCommandGroups" value="'.join(',',$displayedCommandGroups).'">	
	</form>
	';
							
}elseif($action=='editLighting'){
	$roomID=$_REQUEST['roomID'];
	$cgID=$_REQUEST['cgID'];
	$queryRoom='SELECT Description FROM Room WHERE PK_Room=?';
	$resRoom=$dbADO->Execute($queryRoom,$roomID);
	$rowRoom=$resRoom->FetchRow();

	$queryCG='SELECT Description FROM CommandGroup WHERE PK_CommandGroup=?';
	$resCG=$dbADO->Execute($queryCG,$cgID);
	$rowCG=$resCG->FetchRow();

	$out.='
		<div align="center" class="confirm"><B>'.(isset($_GET['msg'])?strip_tags($_GET['msg'].'<br>'):'').'</B></div>	
		<form action="index.php" method="POST" name="mobileScenarios">
			<input type="hidden" name="section" value="mobileScenarios">
			<input type="hidden" name="action" value="add">	
			<input type="hidden" name="roomID" value="'.$roomID.'">
			<input type="hidden" name="cgID" value="'.$cgID.'">
			<table align="center">
			<tr bgcolor="#D1D9EA">
				<td colspan="3">Room: <B>'.$rowRoom['Description'].'</B></td>
				<td colspan="3">Lighting scenario: <B>'.$rowCG['Description'].'</B></td>
			</tr>
			<tr bgcolor="#DDDDDD">
				<td align="center"><B>Device / Room</B></td>
				<td align="center"><B>Type</B></td>
				<td align="center"><B>Unchanged</B></td>
				<td align="center"><B>On</B></td>
				<td align="center"><B>Off</B></td>
				<td align="center"><B>Set level</B></td>
			</tr>
	';
	
	$lightingDevicesArray=getValidLightingObjectsArray($installationID,$dbADO);
	if(count($lightingDevicesArray)==0)
		$lightingDevicesArray[]=0;
	$queryGetRoomsDevice = '
			SELECT Device.*, DeviceTemplate.Description AS Type, Room.Description AS RoomName 
				FROM Device 
			INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
			LEFT JOIN Room ON FK_Room=PK_Room
			WHERE PK_Device IN ('.join(',',$lightingDevicesArray).')
			ORDER BY Description ASC';
	$lineCount=0;
	$resGetRoomsDevice = $dbADO->Execute($queryGetRoomsDevice);
	if($resGetRoomsDevice->RecordCount()==0){
		$out.='
			<tr>
				<td colspan="6">No lighting devices available in this room.</td>
			</tr>';
	}
	$displayedDevices=array();
	while ($rowGetRoomsDevice = $resGetRoomsDevice->FetchRow()) {
		$displayedDevices[]=$rowGetRoomsDevice['PK_Device'];
		$lineCount++;
		$lightingCommands=$GLOBALS['genericONCommand'].','.$GLOBALS['genericOFFCommand'].','.$GLOBALS['genericSetLevelCommand'];
		$queryCGCommands='
			SELECT Device.PK_Device,Device.Description, CommandGroup_Command.FK_Command,FK_CommandParameter,IK_CommandParameter 
			FROM Command 
			INNER JOIN CommandGroup_Command ON CommandGroup_Command.FK_Command=PK_Command
			INNER JOIN Device ON CommandGroup_Command.FK_Device=PK_Device
			LEFT JOIN CommandGroup_Command_CommandParameter ON CommandGroup_Command_CommandParameter.FK_CommandGroup_Command=PK_CommandGroup_Command  
			WHERE FK_Device =? AND PK_Command IN ('.$lightingCommands.') AND FK_CommandGroup=?
			ORDER BY Description ASC';

		$resCGCommands=$dbADO->Execute($queryCGCommands,array($rowGetRoomsDevice['PK_Device'],$cgID));
		$dimValue='';
		if($resCGCommands->RecordCount()==0)
			$selectedCommand=1;
		else{
			$rowCGCommands=$resCGCommands->FetchRow();
			switch($rowCGCommands['FK_Command']){
				case $GLOBALS['genericONCommand']:
					$selectedCommand=2;
				break;
				case $GLOBALS['genericOFFCommand']:
					$selectedCommand=3;
				break;
				case $GLOBALS['genericSetLevelCommand']:
					$selectedCommand=4;
					$dimValue=($rowCGCommands['FK_CommandParameter']==$GLOBALS['commandParamAbsoluteLevel'])?$rowCGCommands['IK_CommandParameter']:'';
				break;
			}
		}
		$out.='
			<tr bgcolor="'.(($lineCount%2==0)?'#DDDDDD':'').'">
				<td>&nbsp;&nbsp;&nbsp;&nbsp;<B>'.$rowGetRoomsDevice['Description'].' / '.$rowGetRoomsDevice['RoomName'].'</B></td>
				<td>&nbsp;&nbsp;&nbsp;&nbsp;<B>'.$rowGetRoomsDevice['Type'].'</B></td>
				<td align="center"><input type="radio" name="command_'.$rowGetRoomsDevice['PK_Device'].'" value="1" '.(($selectedCommand==1)?'checked':'').'></td>
				<td align="center"><input type="radio" name="command_'.$rowGetRoomsDevice['PK_Device'].'" value="2" '.(($selectedCommand==2)?'checked':'').'></td>
				<td align="center"><input type="radio" name="command_'.$rowGetRoomsDevice['PK_Device'].'" value="3" '.(($selectedCommand==3)?'checked':'').'></td>
				<td align="center"><input type="radio" name="command_'.$rowGetRoomsDevice['PK_Device'].'" value="4" '.(($selectedCommand==4)?'checked':'').'> <input type="text" name="dimValue_'.$rowGetRoomsDevice['PK_Device'].'" value="'.@$dimValue.'" size="3" onClick="eval(\'document.mobileScenarios.command_'.$rowGetRoomsDevice['PK_Device'].'[3].checked=true\');"></td>
			</tr>
			<input type="hidden" name="oldCommand_'.$rowGetRoomsDevice['PK_Device'].'" value="'.$selectedCommand.'">
			<input type="hidden" name="oldDimValue_'.$rowGetRoomsDevice['PK_Device'].'" value="'.@$dimValue.'">		
		';
	}
	if(count($displayedDevices)>0){
		$out.='
			<tr>
				<td colspan="6" align="center"><input type="submit" class="button" name="updateLightingDevices" value="Update"  ></td>
			</tr>
		';
	}
	$out.='</table>
		<input type="hidden" name="displayedDevices" value="'.join(',',$displayedDevices).'">
	</form>';
}elseif($action=='editClimate'){
	$roomID=$_REQUEST['roomID'];
	$cgID=$_REQUEST['cgID'];
	$queryRoom='SELECT Description FROM Room WHERE PK_Room=?';
	$resRoom=$dbADO->Execute($queryRoom,$roomID);
	$rowRoom=$resRoom->FetchRow();

	$queryCG='SELECT Description FROM CommandGroup WHERE PK_CommandGroup=?';
	$resCG=$dbADO->Execute($queryCG,$cgID);
	$rowCG=$resCG->FetchRow();

	$out.='
		<div align="center" class="confirm"><B>'.(isset($_GET['msg'])?strip_tags($_GET['msg'].'<br>'):'').'</B></div>	
		<form action="index.php" method="POST" name="mobileScenarios">
			<input type="hidden" name="section" value="mobileScenarios">
			<input type="hidden" name="action" value="add">	
			<input type="hidden" name="roomID" value="'.$roomID.'">
			<input type="hidden" name="cgID" value="'.$cgID.'">
			<table align="center">
			<tr bgcolor="#D1D9EA">
				<td colspan="3">Room: <B>'.$rowRoom['Description'].'</B></td>
				<td colspan="6">Climate scenario: <B>'.$rowCG['Description'].'</B></td>
			</tr>
			<tr bgcolor="#DDDDDD">
				<td align="center"><B>Device / Room</B></td>
				<td align="center"><B>Type</B></td>
				<td align="center"><B>Unchanged</B></td>
				<td align="center"><B>Auto</B></td>
				<td align="center"><B>Heat</B></td>
				<td align="center"><B>Cool</B></td>
				<td align="center"><B>Off</B></td>
				<td align="center"><B>Set temperature</B></td>
			</tr>
	';
	
	$climateDevicesArray=getValidClimateObjectsArray($installationID,$dbADO);
	if(count($climateDevicesArray)==0)
		$climateDevicesArray[]=0;
	$queryGetRoomsDevice = '
			SELECT Device.*, DeviceTemplate.Description AS Type, Room.Description AS RoomName 
				FROM Device 
			INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
			LEFT JOIN Room ON FK_Room=PK_Room
			WHERE PK_Device IN ('.join(',',$climateDevicesArray).')
			ORDER BY Description ASC';
	$lineCount=0;
	$resGetRoomsDevice = $dbADO->Execute($queryGetRoomsDevice);
	if($resGetRoomsDevice->RecordCount()==0){
		$out.='
			<tr>
				<td colspan="9">No climate devices available in this room.</td>
			</tr>';
	}
	$displayedDevices=array();
	while ($rowGetRoomsDevice = $resGetRoomsDevice->FetchRow()) {
		$displayedDevices[]=$rowGetRoomsDevice['PK_Device'];
		$lineCount++;
		$climateCommands=$GLOBALS['genericONCommand'].','.$GLOBALS['genericHeatCommand'].','.$GLOBALS['genericCoolCommand'].','.$GLOBALS['genericOFFCommand'].','.$GLOBALS['genericSetLevelCommand'];
		$queryCGCommands='
			SELECT Device.PK_Device,Device.Description, CommandGroup_Command.FK_Command,FK_CommandParameter,IK_CommandParameter 
			FROM Command 
			INNER JOIN CommandGroup_Command ON CommandGroup_Command.FK_Command=PK_Command
			INNER JOIN Device ON CommandGroup_Command.FK_Device=PK_Device
			LEFT JOIN CommandGroup_Command_CommandParameter ON CommandGroup_Command_CommandParameter.FK_CommandGroup_Command=PK_CommandGroup_Command  
			WHERE FK_Device =? AND PK_Command IN ('.$climateCommands.') AND FK_CommandGroup=?
			ORDER BY Description ASC';

		$resCGCommands=$dbADO->Execute($queryCGCommands,array($rowGetRoomsDevice['PK_Device'],$cgID));
		$dimValue='';
		if($resCGCommands->RecordCount()==0)
			$selectedCommand=1;
		else{
			$rowCGCommands=$resCGCommands->FetchRow();
			switch($rowCGCommands['FK_Command']){
				case $GLOBALS['genericONCommand']:
					$selectedCommand=2;
				break;
				case $GLOBALS['genericHeatCommand']:
					$selectedCommand=3;
				break;
				case $GLOBALS['genericCoolCommand']:
					$selectedCommand=4;
				break;
				case $GLOBALS['genericOFFCommand']:
					$selectedCommand=5;
				break;
				case $GLOBALS['genericSetLevelCommand']:
					$selectedCommand=6;
					$dimValue=($rowCGCommands['FK_CommandParameter']==$GLOBALS['commandParamAbsoluteLevel'])?$rowCGCommands['IK_CommandParameter']:'';
				break;
			}
		}
		$out.='
			<tr bgcolor="'.(($lineCount%2==0)?'#DDDDDD':'').'">
				<td>&nbsp;&nbsp;&nbsp;&nbsp;<B>'.$rowGetRoomsDevice['Description'].' / '.$rowGetRoomsDevice['RoomName'].'</B></td>
				<td>&nbsp;&nbsp;&nbsp;&nbsp;<B>'.$rowGetRoomsDevice['Type'].'</B></td>
				<td align="center"><input type="radio" name="command_'.$rowGetRoomsDevice['PK_Device'].'" value="1" '.(($selectedCommand==1)?'checked':'').'></td>
				<td align="center"><input type="radio" name="command_'.$rowGetRoomsDevice['PK_Device'].'" value="2" '.(($selectedCommand==2)?'checked':'').'></td>
				<td align="center"><input type="radio" name="command_'.$rowGetRoomsDevice['PK_Device'].'" value="3" '.(($selectedCommand==3)?'checked':'').'></td>
				<td align="center"><input type="radio" name="command_'.$rowGetRoomsDevice['PK_Device'].'" value="4" '.(($selectedCommand==4)?'checked':'').'></td>
				<td align="center"><input type="radio" name="command_'.$rowGetRoomsDevice['PK_Device'].'" value="5" '.(($selectedCommand==5)?'checked':'').'></td>
				<td align="center"><input type="radio" name="command_'.$rowGetRoomsDevice['PK_Device'].'" value="6" '.(($selectedCommand==6)?'checked':'').'> <input type="text" name="dimValue_'.$rowGetRoomsDevice['PK_Device'].'" value="'.@$dimValue.'" size="3" onClick="eval(\'document.mobileScenarios.command_'.$rowGetRoomsDevice['PK_Device'].'[5].checked=true\');"></td>
			</tr>
			<input type="hidden" name="oldCommand_'.$rowGetRoomsDevice['PK_Device'].'" value="'.$selectedCommand.'">
			<input type="hidden" name="oldDimValue_'.$rowGetRoomsDevice['PK_Device'].'" value="'.@$dimValue.'">		
		';
	}
	if(count($displayedDevices)>0){
		$out.='
			<tr>
				<td colspan="9" align="center"><input type="submit" class="button" name="updateClimateDevices" value="Update"  ></td>
			</tr>
		';
	}
	$out.='</table>
		<input type="hidden" name="displayedDevices" value="'.join(',',$displayedDevices).'">
	</form>';
}
else{	
	// action='add'
	// insert command group in specified room
	if($action=='addToRoom'){
		$roomID=(int)$_POST['roomID'];
		$roomName=$_POST['roomName'];
		$newScenarioType=$_POST['newScenarioType_'.$roomID];
		if($newScenarioType==0){
			$out.='
			<script>
				windowOpen(\'index.php?section=addCommandGroupAsScenario&from=mobileScenarios&arrayID='.$arrayID.'\',\'width=600,height=600,toolbars=true,resizable=1,scrollbars=1\');
				self.location="index.php?section=mobileScenarios";
			</script>';
		}else{
			$insertCommandGroup='INSERT INTO CommandGroup (Description,FK_Array,FK_Installation,FK_Template,Hint) VALUES (?,?,?,?,?)';
			$dbADO->Execute($insertCommandGroup,array('New '.$templateArray[$newScenarioType],$arrayID,$installationID,$newScenarioType,$roomName));
			$insertID=$dbADO->Insert_ID();
			$insertCG_Room='INSERT INTO CommandGroup_Room (FK_CommandGroup,FK_Room,Sort) VALUES (?,?,?)';
			$dbADO->Execute($insertCG_Room,array($insertID,$roomID,$insertID));
			setOrbitersNeedConfigure($installationID,$dbADO);
			$msg='New '.$templateArray[$newScenarioType].' added';

			header("Location: index.php?section=mobileScenarios&msg=".@$msg.'&lastAdded='.$insertID);
			exit();
		}
	}

	if(isset($_POST['updateCG']) || $action=='externalSubmit' || @(int)$_REQUEST['editedCgID']!=0){
		$displayedCommandGroupsArray=explode(',',$_POST['displayedCommandGroups']);
		foreach($displayedCommandGroupsArray as $elem){
			$cgDescription=cleanString($_POST['commandGroup_'.$elem]);
			$cgHint=cleanString($_POST['hintCommandGroup_'.$elem]);
			$updateCG='UPDATE CommandGroup SET Description=?, Hint=? WHERE PK_CommandGroup=?';
			$dbADO->Execute($updateCG,array($cgDescription,$cgHint,$elem));
		}
		setOrbitersNeedConfigure($installationID,$dbADO);
		
		if(@(int)$_REQUEST['editedCgID']!=0 && @(int)$_REQUEST['roomID']!=0){
			header('Location: index.php?section=mobileScenarios&cgID='.$_REQUEST['editedCgID'].'&action='.(((int)$_REQUEST['editedTemplate']==$GLOBALS['LightingScenariosTemplate'])?'editLighting':'editClimate').'&roomID='.$_REQUEST['roomID']);
			exit();
		}
		$msg="Mobile Orbiter Scenario updated.";
		header("Location: index.php?section=mobileScenarios&msg=".@$msg);
		exit();
	}
	
	if(isset($_REQUEST['operation'])){
		$fromCgID=$_REQUEST['cgID'];
		$roomID=$_REQUEST['roomID'];
		$operation=$_REQUEST['operation'];
		$posInRoom=unserialize(stripslashes($_REQUEST['posInRoom']));
		
		if($operation=='up'){
			$offset=array_keys($posInRoom);
			$fromPos=array_search($fromCgID,$offset);
			if(isset($offset[$fromPos+1])){
				$toCgID=$offset[$fromPos+1];
			
				$fromSort=$posInRoom[$fromCgID];
				$toSort=$posInRoom[$toCgID];
	
				$updateCG_R='UPDATE CommandGroup_Room SET Sort=? WHERE FK_CommandGroup=? AND FK_Room=?';
				$dbADO->Execute($updateCG_R,array($toSort,$fromCgID,$roomID));
				$dbADO->Execute($updateCG_R,array($fromSort,$toCgID,$roomID));
				$msg="Positions was updated";
			}
		}else{
			$offset=array_keys($posInRoom);
			$fromPos=array_search($fromCgID,$offset);
			if(isset($offset[$fromPos-1])){
				$toCgID=$offset[$fromPos-1];
			
				$fromSort=$posInRoom[$fromCgID];
				$toSort=$posInRoom[$toCgID];
	
				$updateCG_R='UPDATE CommandGroup_Room SET Sort=? WHERE FK_CommandGroup=? AND FK_Room=?';
				$dbADO->Execute($updateCG_R,array($toSort,$fromCgID,$roomID));
				$dbADO->Execute($updateCG_R,array($fromSort,$toCgID,$roomID));
				$msg="Positions was updated";
			}
		}
		header("Location: index.php?section=mobileScenarios&msg=".@$msg);
		exit();
	}

	if(isset($_POST['updateLightingDevices'])){
		$roomID=$_POST['roomID'];
		$cgID=$_POST['cgID'];
		$displayedDevicesArray=explode(',',$_POST['displayedDevices']);
		foreach($displayedDevicesArray AS $elem){
			$deviceCommand=(int)$_POST['command_'.$elem];
			$dimValue=$_POST['dimValue_'.$elem];
			$oldCommand=$_POST['oldCommand_'.$elem];
			$oldDimValue=$_POST['oldDimValue_'.$elem];
			$deleteCommand='DELETE FROM CommandGroup_Command WHERE FK_CommandGroup=? AND FK_Device=? AND FK_Command=?';
			$deleteParameters='
				DELETE CommandGroup_Command_CommandParameter
				FROM CommandGroup_Command_CommandParameter
				JOIN CommandGroup_Command on FK_CommandGroup_Command=PK_CommandGroup_Command
				WHERE FK_CommandGroup=? AND FK_Device=?';
			
			if($deviceCommand!=$oldCommand || ($deviceCommand==$oldCommand && $oldDimValue!=$dimValue))	{
				$dbADO->Execute($deleteCommand,array($cgID,$elem,$GLOBALS['genericONCommand']));
				$dbADO->Execute($deleteCommand,array($cgID,$elem,$GLOBALS['genericOFFCommand']));
				$dbADO->Execute($deleteCommand,array($cgID,$elem,$GLOBALS['genericSetLevelCommand']));
				$dbADO->Execute($deleteParameters,array($cgID,$elem));						

				switch($deviceCommand){
					case 1:
						// do nothing, delete is done above
					break;
					case 2:
						$insertCG_C='INSERT INTO CommandGroup_Command (FK_CommandGroup, FK_Device, FK_Command) VALUES (?,?,?)';
						$dbADO->Execute($insertCG_C,array($cgID,$elem,$GLOBALS['genericONCommand']));
					break;
					case 3:
						$insertCG_C='INSERT INTO CommandGroup_Command (FK_CommandGroup, FK_Device, FK_Command) VALUES (?,?,?)';
						$dbADO->Execute($insertCG_C,array($cgID,$elem,$GLOBALS['genericOFFCommand']));
					break;
					case 4:
						$insertCG_C='INSERT INTO CommandGroup_Command (FK_CommandGroup, FK_Device, FK_Command) VALUES (?,?,?)';
						$dbADO->Execute($insertCG_C,array($cgID,$elem,$GLOBALS['genericSetLevelCommand']));
						$cgcInsertID=$dbADO->Insert_ID();
											
						$insertCG_C_P='INSERT INTO CommandGroup_Command_CommandParameter (FK_CommandGroup_Command,FK_CommandParameter,IK_CommandParameter) VALUES (?,?,?)';
						$dbADO->Execute($insertCG_C_P,array($cgcInsertID,$GLOBALS['commandParamAbsoluteLevel'],$dimValue));
					break;
				}
			}
		}
		header("Location: index.php?section=mobileScenarios&roomID=$roomID&cgID=$cgID&action=editLighting&msg=Mobile Obiter Scenario updated.");
		exit();
	}
	
	if(isset($_POST['updateClimateDevices'])){
		$roomID=$_POST['roomID'];
		$cgID=$_POST['cgID'];
		$displayedDevicesArray=explode(',',$_POST['displayedDevices']);
		foreach($displayedDevicesArray AS $elem){
			$deviceCommand=(int)$_POST['command_'.$elem];
			$dimValue=$_POST['dimValue_'.$elem];
			$oldCommand=$_POST['oldCommand_'.$elem];
			$oldDimValue=$_POST['oldDimValue_'.$elem];
			$deleteCommand='DELETE FROM CommandGroup_Command WHERE FK_CommandGroup=? AND FK_Device=? AND FK_Command=?';
			$deleteParameters='
				DELETE CommandGroup_Command_CommandParameter
				FROM CommandGroup_Command_CommandParameter
				JOIN CommandGroup_Command on FK_CommandGroup_Command=PK_CommandGroup_Command
				WHERE FK_CommandGroup=? AND FK_Device=?';
			if($deviceCommand!=$oldCommand || ($deviceCommand==$oldCommand && $oldDimValue!=$dimValue))	{
				$dbADO->Execute($deleteCommand,array($cgID,$elem,$GLOBALS['genericONCommand']));
				$dbADO->Execute($deleteCommand,array($cgID,$elem,$GLOBALS['genericHeatCommand']));
				$dbADO->Execute($deleteCommand,array($cgID,$elem,$GLOBALS['genericCoolCommand']));
				$dbADO->Execute($deleteCommand,array($cgID,$elem,$GLOBALS['genericOFFCommand']));
				$dbADO->Execute($deleteCommand,array($cgID,$elem,$GLOBALS['genericSetLevelCommand']));
				$dbADO->Execute($deleteParameters,array($cgID,$elem));

				switch($deviceCommand){
					case 1:
						// do nothing, delete is done above
					break;
					case 2:
						$insertCG_C='INSERT INTO CommandGroup_Command (FK_CommandGroup, FK_Device, FK_Command) VALUES (?,?,?)';
						$dbADO->Execute($insertCG_C,array($cgID,$elem,$GLOBALS['genericONCommand']));
					break;
					case 3:
						$insertCG_C='INSERT INTO CommandGroup_Command (FK_CommandGroup, FK_Device, FK_Command) VALUES (?,?,?)';
						$dbADO->Execute($insertCG_C,array($cgID,$elem,$GLOBALS['genericHeatCommand']));
					break;
					case 4:
						$insertCG_C='INSERT INTO CommandGroup_Command (FK_CommandGroup, FK_Device, FK_Command) VALUES (?,?,?)';
						$dbADO->Execute($insertCG_C,array($cgID,$elem,$GLOBALS['genericCoolCommand']));
					break;
					case 5:
						$insertCG_C='INSERT INTO CommandGroup_Command (FK_CommandGroup, FK_Device, FK_Command) VALUES (?,?,?)';
						$dbADO->Execute($insertCG_C,array($cgID,$elem,$GLOBALS['genericOFFCommand']));
					break;
					case 6:
						$insertCG_C='INSERT INTO CommandGroup_Command (FK_CommandGroup, FK_Device, FK_Command) VALUES (?,?,?)';
						$dbADO->Execute($insertCG_C,array($cgID,$elem,$GLOBALS['genericSetLevelCommand']));
						$cgcInsertID=$dbADO->Insert_ID();
											
						$insertCG_C_P='INSERT INTO CommandGroup_Command_CommandParameter (FK_CommandGroup_Command,FK_CommandParameter,IK_CommandParameter) VALUES (?,?,?)';
						$dbADO->Execute($insertCG_C_P,array($cgcInsertID,$GLOBALS['commandParamAbsoluteLevel'],$dimValue));
					break;
				}
			}
			setOrbitersNeedConfigure($installationID,$dbADO);
		}
		header("Location: index.php?section=mobileScenarios&roomID=$roomID&cgID=$cgID&action=editClimate&msg=Mobile Obiter Scenario updated.");
		exit();
	}
	
	if(isset($_GET['cgDelID']) && (int)$_GET['cgDelID']!=0){
		$cgToDelete=(int)$_GET['cgDelID'];
		deleteCommandGroup($cgToDelete,$dbADO);
		header("Location: index.php?section=mobileScenarios&msg=Lighting scenario deleted.");
	}

}

	$output->setScriptInBody("onLoad=\"javascript:top.treeframe.location.reload();\"");
	$output->setNavigationMenu(array("My Scenarios"=>'index.php?section=myScenarios',"Mobile Orbiter Scenarios"=>'index.php?section=mobileScenarios'));
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Mobile Orbiter Scenarios');
	$output->output();

}
?>
