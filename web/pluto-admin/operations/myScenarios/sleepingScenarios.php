<?php
function sleepingScenarios($output,$dbADO) {
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
$arrayID = $GLOBALS['ArrayIDSleepingScenarios'];	
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

	$out.=setLeftMenu($dbADO).'
	<div align="center" class="confirm"><B>'.(isset($_GET['msg'])?strip_tags($_GET['msg'].'<br>'):'').'</B></div>
	<h2 align="center">'.$rowArray['Description'].'</h2>';

	$out.='
	<form action="index.php" method="POST" name="sleepingScenarios">
		<input type="hidden" name="section" value="sleepingScenarios">
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
			<td colspan="4" align="right"><input type="button" class="button" name="add" value="Add scenario" onClick="javascript:document.sleepingScenarios.action.value=\'addToRoom\';document.sleepingScenarios.roomID.value='.$rowRooms['PK_Room'].';document.sleepingScenarios.roomName.value=\''.urlencode($rowRooms['RoomName']).'\';document.sleepingScenarios.submit();"></td>
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
				<td colspan="5">No sleeping scenarios for this room.</td>
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
				<td><input type="button" class="button" name="posDown" value="-" onClick="self.location=\'index.php?section=sleepingScenarios&cgID='.$rowCG['PK_CommandGroup'].'&action=process&roomID='.$rowRooms['PK_Room'].'&operation=down&posInRoom='.urlencode(serialize($posInRoom)).'\'"> 
					<input type="button" class="button" name="posUp" value="+" onClick="self.location=\'index.php?section=sleepingScenarios&cgID='.$rowCG['PK_CommandGroup'].'&action=process&roomID='.$rowRooms['PK_Room'].'&operation=up&posInRoom='.urlencode(serialize($posInRoom)).'\'">
				 Description: '.((!in_array($rowCG['PK_CommandGroup'],$displayedCommandGroups))?'<input type="text" name="commandGroup_'.$rowCG['PK_CommandGroup'].'" value="'.$rowCG['Description'].'"> Hint: <input type="text" name="hintCommandGroup_'.$rowCG['PK_CommandGroup'].'" value="'.$rowCG['Hint'].'">':'<b>'.$rowCG['Description'].': </b>Hint: <b>'.$rowCG['Hint'].'</b> (See '.$firstRoomArray[$rowCG['PK_CommandGroup']].')').'</td>
				<td>&nbsp;</td>
				<td>&nbsp;</td>
				<td><a href="#" onclick="document.sleepingScenarios.editedCgID.value='.$rowCG['PK_CommandGroup'].';document.sleepingScenarios.roomID.value='.$rowRooms['PK_Room'].';document.sleepingScenarios.submit();">Edit</a> <a href="#" onClick="javascript:if(confirm(\'Are you sure you want to delete this scenario?\'))self.location=\'index.php?section=sleepingScenarios&action=delete&cgDelID='.$rowCG['PK_CommandGroup'].'\';">Delete</a></td>
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
							
}else{	
	// action='add'
	// insert command group in specified room
	if($action=='addToRoom'){
		$roomID=(int)$_POST['roomID'];
		$roomName=urldecode($_POST['roomName']);

		$insertCommandGroup='INSERT INTO CommandGroup (Description,FK_Array,FK_Installation,FK_Template,Hint) VALUES (?,?,?,?,?)';
		$dbADO->Execute($insertCommandGroup,array('New sleeping scenario',$arrayID,$installationID,NULL,$roomName));
		$insertID=$dbADO->Insert_ID();
		$insertCG_Room='INSERT INTO CommandGroup_Room (FK_CommandGroup,FK_Room,Sort) VALUES (?,?,?)';
		$dbADO->Execute($insertCG_Room,array($insertID,$roomID,$insertID));
		setOrbitersNeedConfigure($installationID,$dbADO);
		$msg='New sleeping scenario added.&lastAdded='.$insertID;
	}

	if(isset($_POST['updateCG']) || $action=='externalSubmit' || @(int)$_REQUEST['editedCgID']!=0 || $action=='addToRoom'){
		$displayedCommandGroupsArray=explode(',',$_POST['displayedCommandGroups']);
		foreach($displayedCommandGroupsArray as $elem){
			$cgDescription=cleanString(@$_POST['commandGroup_'.$elem]);
			$cgHint=cleanString(@$_POST['hintCommandGroup_'.$elem]);
			$updateCG='UPDATE CommandGroup SET Description=?, Hint=? WHERE PK_CommandGroup=?';
			$dbADO->Execute($updateCG,array($cgDescription,$cgHint,$elem));
		}
		setOrbitersNeedConfigure($installationID,$dbADO);
		
		if(@(int)$_REQUEST['editedCgID']!=0 && @(int)$_REQUEST['roomID']!=0){
			header('Location: index.php?section=scenarioWizard&from=sleepingScenarios&cgID='.$_REQUEST['editedCgID'].'&roomID='.$_REQUEST['roomID'].'&wizard=2');
			exit();
		}
		$msg=(isset($msg))?$msg:"Sleeping Scenario updated.";
		header("Location: index.php?section=sleepingScenarios&msg=".@$msg);
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
				$msg="Positions were updated";
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
				$msg="Positions were updated";
			}
		}
		header("Location: index.php?section=sleepingScenarios&msg=".@$msg);
		exit();
	}

	if(isset($_GET['cgDelID']) && (int)$_GET['cgDelID']!=0){
		$cgToDelete=(int)$_GET['cgDelID'];
		deleteCommandGroup($cgToDelete,$dbADO);
		header("Location: index.php?section=sleepingScenarios&msg=Lighting scenario deleted.");
	}
	setOrbitersNeedConfigure($installationID,$dbADO);
	header("Location: index.php?section=sleepingScenarios&roomID=$roomID&cgID=$cgID&msg=Sleeping Obiter Scenario updated.");
	exit();

}

	$output->setNavigationMenu(array("My Scenarios"=>'index.php?section=myScenarios',"Sleeping Scenarios"=>'index.php?section=sleepingScenarios'));
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Sleeping Scenarios');
	$output->output();

}
?>
