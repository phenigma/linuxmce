<?php
function learnCode($output,$dbADO) {
	//$dbADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$deviceID = ((int)$_REQUEST['deviceID']>0)?(int)$_REQUEST['deviceID']:NULL;
	$commandID = (int)$_REQUEST['commandID'];
	$dtID=(int)$_REQUEST['dtID'];
	$infraredGroupID=(int)@$_REQUEST['infraredGroupID'];
	$noRefresh=(isset($_REQUEST['noRefresh']))?1:0;
	
	if ($action=='form') {		
		$res=$dbADO->Execute('SELECT psc_mod AS lastTime,PK_InfraredGroup_Command FROM InfraredGroup_Command ORDER BY psc_mod DESC');
		$row=$res->FetchRow();
		if($_SESSION['LastInfraredGroup_CommandTime']!=$row['lastTime']){
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
					WHERE FK_Command=? AND FK_InfraredGroup IS NULL',$commandID);
			if($isSingleCode->RecordCount()==0){
				$dbADO->Execute('INSERT INTO InfraredGroup_Command_Preferred (FK_InfraredGroup_Command,FK_Installation) VALUES (?,?)',array($row['PK_InfraredGroup_Command'],$_SESSION['installationID']));
			}
			$out='
			<script>
				alert(\'Infrared code learned!\');
				opener.location.reload();
				self.close();
			</script>';
		}
		else{
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
		<script>
		function disableManually(val)
		{
			document.learnCode.irData.disabled=val;
			document.learnCode.add.disabled=val;
			if(val==false){
				document.learnCode.action.value="cancel";
				document.learnCode.submit();
			}
			else{
				document.learnCode.submit();
			}
				
		}
		</script>
		<form action="index.php" method="post" name="learnCode">
		<input type="hidden" name="section" value="learnCode">
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
				<td align="center"><input type="radio" name="newCodeType" value="0" '.(($noRefresh==0)?'checked':'').' onClick="disableManually(true);"></td>
				<td align="left"><B>Learn code</B> (your interface module must support learning)</td>
			</tr>
			<tr>
				<td align="center"><input type="radio" name="newCodeType" value="1" '.(($noRefresh==1)?'checked':'').' onClick="disableManually(false);"></td>
				<td align="left"><B>Type code manually</B> (in Pronto format)</td>
			</tr>
			<tr>
				<td align="center">&nbsp;</td>
				<td align="left"><textarea name="irData" rows="3" cols="100" '.(($noRefresh==0)?'disabled':'').'></textarea></td>
			</tr>
			
			<tr>
				<td align="center">&nbsp;</td>
				<td align="center"><input type="submit" class="button" name="add" value="Add code" '.(($noRefresh==0)?'disabled':'').'> <input type="submit" class="button" name="cancel" value="Cancel"></td>
			</tr>
		</table>			
		</form>';
			if($noRefresh==0){
				$out.='	
				<script>
					var newInterval=setInterval("self.location.reload()",2000);
				</script>';
			}
		}	
	} else {
		if(isset($_REQUEST['cancel']) || $action=='cancel'){
			$commandToSend='/usr/pluto/bin/MessageSend localhost 0 '.$deviceID.' 1 245 8 0';
			system($commandToSend);
			if($action=='cancel'){
				$out.="
				<script>
					self.location='index.php?section=learnCode&deviceID=$deviceID&dtID=$dtID&infraredGroupID=$infraredGroupID&commandID=$commandID&noRefresh=1';
				</script>
				";
			}
			else
				$out.='
				<script>
					self.close();
				</script>
			';
		}elseif(isset($_POST['add'])){
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
				$dbADO->Execute('INSERT INTO InfraredGroup_Command_Preferred (FK_InfraredGroup_Command,FK_Installation) VALUES (?,?)',array($igcID,$_SESSION['installationID']));
			}
			$out.='<script>
					opener.location.reload();
					self.close();
				</script>
			';
		}else{
			$resLastCommand=$dbADO->Execute('SELECT psc_mod AS lastTime FROM InfraredGroup_Command ORDER BY psc_mod DESC');
			$rowLastCommand=$resLastCommand->FetchRow();
			$_SESSION['LastInfraredGroup_CommandTime']=$rowLastCommand['lastTime'];
			$commandToSend='/usr/pluto/bin/MessageSend localhost 0 '.$deviceID.' 1 245 8 1 71 '.$commandID;
			system($commandToSend);
			
			header("Location: index.php?section=learnCode&deviceID=$deviceID&dtID=$dtID&infraredGroupID=$infraredGroupID&commandID=".$commandID);
		}
			
	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Learn IR Code');			
	$output->output();
}
?>
