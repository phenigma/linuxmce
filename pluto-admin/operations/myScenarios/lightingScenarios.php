<?php
function lightingScenarios($output,$dbADO) {
/* @var $dbADO ADOConnection */
/* @var $rs ADORecordSet */

$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
$out='';
$installationID = (int)@$_SESSION['installationID'];
$arrayID = $GLOBALS['ArrayIDForLight'];	

if($action=='form') {
	
	$queryArray = 'SELECT * FROM Array WHERE PK_Array=?';	
	$resArray = $dbADO->Execute($queryArray,$arrayID);
	$rowArray=$resArray->FetchRow();

	if(isset($_GET['cgDelID']) && (int)$_GET['cgDelID']!=0){
		$cgToDelete=(int)$_GET['cgDelID'];
		deleteCommandGroup($cgToDelete,$dbADO);
		header("Location: index.php?section=lightingScenarios");
	}

	
	$out.='<h2>'.$rowArray['Description'].'</h2>';

	$out.='
	<form action="index.php" method="POST" name="lightingScenarios">
		<input type="hidden" name="section" value="lightingScenarios">
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
			<td><B>'.$rowRooms['RoomName'].'</B></td>
			<td>&nbsp;</td>
			<td align="right"><input type="button" name="add" value="Add scenario" onClick="javascript:document.lightingScenarios.action.value=\'add\';document.lightingScenarios.roomID.value='.$rowRooms['PK_Room'].';document.lightingScenarios.submit();"></td>
		</tr>';
		$selectCommandGroups='
			SELECT * 
			FROM CommandGroup
			INNER JOIN CommandGroup_Room ON FK_CommandGroup=PK_CommandGroup
			WHERE FK_Room=? AND FK_Installation=? AND FK_Template=?';
		$resCommandGroups=$dbADO->Execute($selectCommandGroups,array($rowRooms['PK_Room'],$installationID,$GLOBALS['LightingScenariosTemplate']));
		if($resCommandGroups->RecordCount()==0){
		$out.='
			<tr>
				<td colspan="3">No lighting scenarios for this room.</td>
			</tr>';
		}
		while($rowCG=$resCommandGroups->FetchRow()){
			$displayedCommandGroups[]=$rowCG['PK_CommandGroup'];
			$out.='
			<tr>
				<td><input type="text" name="commandGroup_'.$rowCG['PK_CommandGroup'].'" value="'.$rowCG['Description'].'"></td>
				<td><a href="index.php?section=lightingScenarios&cgID='.$rowCG['PK_CommandGroup'].'&action=edit&roomID='.$rowRooms['PK_Room'].'">Edit</a> <a href="#" onClick="javascript:if(confirm(\'Are you sure you want to delete this scenario?\'))self.location=\'index.php?section=lightingScenarios&cgDelID='.$rowCG['PK_CommandGroup'].'\';">Delete</a></td>
				<td>&nbsp;</td>
			</tr>
			';
		}
	}
	if(count($displayedCommandGroups)>0){
		$out.='
		<tr>
			<td colspan="2" align="center"><input type="submit" name="updateCG" value="Update"></td>
			<td>&nbsp;</td>
		</tr>';
	}
	$out.='
		</table>
	<input type="hidden" name="displayedCommandGroups" value="'.join(',',$displayedCommandGroups).'">	
	</form>
	';
							
}elseif($action=='edit'){
	$roomID=$_REQUEST['roomID'];
	$cgID=$_REQUEST['cgID'];
	$queryRoom='SELECT Description FROM Room WHERE PK_Room=?';
	$resRoom=$dbADO->Execute($queryRoom,$roomID);
	$rowRoom=$resRoom->FetchRow();

	$queryCG='SELECT Description FROM CommandGroup WHERE PK_CommandGroup=?';
	$resCG=$dbADO->Execute($queryCG,$cgID);
	$rowCG=$resCG->FetchRow();

	$out.='
		<form action="index.php" method="POST" name="lightingScenarios">
			<input type="hidden" name="section" value="lightingScenarios">
			<input type="hidden" name="action" value="add">	
			<input type="hidden" name="roomID" value="'.$roomID.'">
			<input type="hidden" name="cgID" value="'.$cgID.'">
			<table>
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
				<td align="center"><input type="radio" name="command_'.$rowGetRoomsDevice['PK_Device'].'" value="4" '.(($selectedCommand==4)?'checked':'').'> <input type="text" name="dimValue_'.$rowGetRoomsDevice['PK_Device'].'" value="'.@$dimValue.'" size="3" onClick="eval(\'document.lightingScenarios.command_'.$rowGetRoomsDevice['PK_Device'].'[3].checked=true\');"></td>
			</tr>
			<input type="hidden" name="oldCommand_'.$rowGetRoomsDevice['PK_Device'].'" value="'.$selectedCommand.'">
			<input type="hidden" name="oldDimValue_'.$rowGetRoomsDevice['PK_Device'].'" value="'.@$dimValue.'">		
		';
	}
	if(count($displayedDevices)>0){
		$out.='
			<tr>
				<td colspan="6" align="center"><input type="submit" name="updateDevices" value="Update"></td>
			</tr>
		';
	}
	$out.='</table>
		<input type="hidden" name="displayedDevices" value="'.join(',',$displayedDevices).'">
	</form>';
}else{	
	// action='add'
	// insert command group in specified room
	if(isset($_POST['roomID']) && (int)$_POST['roomID']!=0 && !isset($_POST['updateDevices'])){
		$roomID=(int)$_POST['roomID'];
		$insertCommandGroup='INSERT INTO CommandGroup (Description,FK_Array,FK_Installation,FK_Template) VALUES (?,?,?,?)';
		$dbADO->Execute($insertCommandGroup,array('New Lighting Scenario',$arrayID,$installationID,$GLOBALS['LightingScenariosTemplate']));
		$insertID=$dbADO->Insert_ID();
		$insertCG_Room='INSERT INTO CommandGroup_Room (FK_CommandGroup,FK_Room,Sort) VALUES (?,?,?)';
		$dbADO->Execute($insertCG_Room,array($insertID,$roomID,$insertID));
	}
	
	if(isset($_POST['updateCG'])){
		$displayedCommandGroupsArray=explode(',',$_POST['displayedCommandGroups']);
		foreach($displayedCommandGroupsArray as $elem){
			$cgDescription=cleanString($_POST['commandGroup_'.$elem]);
			$updateCG='UPDATE CommandGroup SET Description=? WHERE PK_CommandGroup=?';
			$dbADO->Execute($updateCG,array($cgDescription,$elem));
		}
	}
	
	if(isset($_POST['updateDevices'])){
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
		header("Location: index.php?section=lightingScenarios&roomID=$roomID&cgID=$cgID&action=edit");
		exit();
	}
	
	header("Location: index.php?section=lightingScenarios");
}


	$output->setNavigationMenu(array("My Scenarios"=>'index.php?section=myScenarios',"Lighting Scenarios"=>'index.php?section=lightingScenarios'));
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();

}
?>