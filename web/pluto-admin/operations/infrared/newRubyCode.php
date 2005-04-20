<?php
function newRubyCode($output,$dbADO) {
	//$dbADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$deviceID = ((int)$_REQUEST['deviceID']>0)?(int)$_REQUEST['deviceID']:NULL;
	$commandID = (int)$_REQUEST['commandID'];
	$dtID=(int)$_REQUEST['dtID'];
	$infraredGroupID=(int)@$_REQUEST['infraredGroupID'];
	
	if ($action=='form') {		

		$selectDevice='
			SELECT DeviceTemplate.Description AS Template, DeviceCategory.Description AS Category,Manufacturer.Description AS Manufacturer,Device.Description
			FROM Device
			INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
			INNER JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory
			INNER JOIN Manufacturer ON FK_Manufacturer=PK_Manufacturer
			WHERE PK_DeviceTemplate=?';
		$resDevice=$dbADO->Execute($selectDevice,$dtID);

		$rowDevice=$resDevice->FetchRow();
			
		$out.='
		<form action="index.php" method="post" name="newRubyCode">
		<input type="hidden" name="section" value="newRubyCode">
		<input type="hidden" name="action" value="add">
		<input type="hidden" name="deviceID" value="'.$deviceID.'">
		<input type="hidden" name="dtID" value="'.$dtID.'">
		<input type="hidden" name="commandID" value="'.$commandID.'">
		<input type="hidden" name="infraredGroupID" value="'.$infraredGroupID.'">
		<table width="100%">
			<tr>
				<td align="center" colspan="2" height="60">Device <B>'.$rowDevice['Description'].'</B>, device template <B>'.$rowDevice['Template'].'</B>, category <B>'.$rowDevice['Category'].'</B>, manufacturer <B>'.$rowDevice['Manufacturer'].'</B></td>
			</tr>
			<tr>
				<td align="center">&nbsp;</td>
				<td align="left"><B>Type Ruby code </B></td>
			</tr>
			<tr>
				<td align="center">&nbsp;</td>
				<td align="left"><textarea name="irData" rows="30" cols="100"></textarea></td>
			</tr>
			
			<tr>
				<td align="center">&nbsp;</td>
				<td align="center"><input type="submit" class="button" name="add" value="Add code"> <input type="button" class="button" name="cancel" value="Cancel" onclick="self.close();"></td>
			</tr>
		</table>			
		</form>';
	
	} else {
		$dbADO->debug=true;
		if(isset($_POST['add'])){
			$irData=stripslashes($_POST['irData']);
			$infraredGroupID=($infraredGroupID==0)?NULL:$infraredGroupID;
			if($infraredGroupID!=0)
				$isSingleCode=$dbADO->Execute('
					SELECT * 
					FROM InfraredGroup_Command_Preferred
					INNER JOIN InfraredGroup_Command ON FK_InfraredGroup_Command=PK_InfraredGroup_Command
					WHERE FK_Command=? AND FK_InfraredGroup=?',array($commandID,$infraredGroupID));
			else
				$isSingleCode=$dbADO->Execute('
					SELECT * 
					FROM InfraredGroup_Command_Preferred
					INNER JOIN InfraredGroup_Command ON FK_InfraredGroup_Command=PK_InfraredGroup_Command
					WHERE FK_Command=? AND FK_DeviceTemplate=? AND FK_InfraredGroup IS NULL',array($commandID,$dtID));
			$isOtherCustomCode=$dbADO->Execute('SELECT * FROM InfraredGroup_Command WHERE FK_InfraredGroup=? AND FK_Command=? AND FK_Device=? AND FK_DeviceTemplate=? AND FK_Users=?',array($infraredGroupID,$commandID, $deviceID,$dtID,$_SESSION['userID']));
			if($isOtherCustomCode->RecordCount()>0){
				$rowOther=$isOtherCustomCode->FetchRow();
				$dbADO->Execute('UPDATE InfraredGroup_Command SET IRData=? WHERE PK_InfraredGroup_Command=?',array($irData,$rowOther['PK_InfraredGroup_Command']));
			}else
				$dbADO->Execute('INSERT INTO InfraredGroup_Command (FK_InfraredGroup,FK_Command, FK_Device, FK_DeviceTemplate, FK_Users,IRData) VALUES (?,?,?,?,?,?)',array($infraredGroupID,$commandID, $deviceID,$dtID,$_SESSION['userID'],$irData));
			$igcID=$dbADO->Insert_ID();
			
			if($isSingleCode->RecordCount()==0){
				$dbADO->Execute('INSERT IGNORE INTO InfraredGroup_Command_Preferred (FK_InfraredGroup_Command,FK_Installation) VALUES (?,?)',array($igcID,$_SESSION['installationID']));
			}
			$out.='<script>
					opener.location.reload();
					//self.close();
				</script>
			';
		}	
		else{
			header("Location: index.php?section=newRubyCode&deviceID=$deviceID&dtID=$dtID&infraredGroupID=$infraredGroupID&commandID=".$commandID);
		}
			
	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: New Ruby Code');			
	$output->output();
}
?>
