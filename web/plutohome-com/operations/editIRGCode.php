<?php
function editIRGCode($output)
{
	global $dbPlutoAdminServer,$dbPlutoAdminUser,$dbPlutoAdminPass,$dbPlutoAdminDatabase;
	include_once('include/plutoAdminUtils.inc.php');

	$publicADO = &ADONewConnection('mysql');
	$publicADO->NConnect($dbPlutoAdminServer,urlencode($dbPlutoAdminUser),urlencode($dbPlutoAdminPass),urlencode($dbPlutoAdminDatabase));
	
	
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$deviceID = ((int)$_REQUEST['deviceID']>0)?(int)$_REQUEST['deviceID']:NULL;
	$commandID = (int)$_REQUEST['commandID'];
	$dtID=(int)$_REQUEST['dtID'];
	$infraredGroupID=(int)@$_REQUEST['infraredGroupID'];
	$installationID=0;
	$dtID1=$dtID!=''?$dtID:$infraredGroupID;
	if ($action=='form') {		

		$selectDevice='
			select PK_InfraredGroup_Command,FK_CommandCategory,PK_Command,FK_Command,FK_Users,FK_DeviceTemplate,FK_Device,InfraredGroup_Command.psc_user,IRData,Command.Description
FROM InfraredGroup_Command
JOIN Command ON FK_Command=PK_Command
JOIN CommandCategory ON FK_CommandCategory=PK_CommandCategory
where PK_InfraredGroup_Command=?';
		$resDevice=$publicADO->Execute($selectDevice,$infraredGroupID);

		$rowDevice=$resDevice->FetchRow();
			
		$out.='
		<form action="index.php" method="post" name="editIRGCode">
		<input type="hidden" name="section" value="editIRGCode">
		<input type="hidden" name="action" value="add">
		<input type="hidden" name="deviceID" value="'.$deviceID.'">
		<input type="hidden" name="dtID" value="'.$dtID.'">
		<input type="hidden" name="commandID" value="'.$commandID.'">
		<input type="hidden" name="infraredGroupID" value="'.$infraredGroupID.'">
		<table>
			<tr>';
      if($infraredGroupID!='')
        $out.='
				<td align="center" colspan="2" height="60">Command <B>'.$rowDevice['Description'].'</B>, User <B>'.$rowDevice['FK_Users'].'</B>, </td>';
            else
            $out.='
				<td align="center" colspan="2" height="60">&nbsp;</td>';
            $out.='
			</tr>
			<tr>
				<td align="center">&nbsp;</td>
				<td align="left"><B>Type IR code </B></td>
			</tr>
			<tr>
				<td align="center">&nbsp;</td>
				<td align="center"><textarea name="irData" rows="10" cols="85">'.$rowDevice['IRData'].'</textarea></td>
			</tr>
			
			<tr>
				<td align="center">&nbsp;</td>
				<td align="center"><input type="submit" class="button" name="add" value="Edit code"> <input type="button" class="button" name="cancel" value="Cancel" onclick="self.close();"></td>
			</tr>
		</table>			
		</form>';
	
	} else { 
		if(isset($_POST['add'])){
			$irData=stripslashes($_POST['irData']);
			$infraredGroupID=($infraredGroupID==0)?NULL:$infraredGroupID;
			if($infraredGroupID!=0)
				$isSingleCode=$publicADO->Execute('
					select PK_InfraredGroup_Command,FK_CommandCategory,PK_Command,FK_Command,FK_Users,FK_DeviceTemplate,FK_Device,InfraredGroup_Command.psc_user,IRData,Command.Description
FROM InfraredGroup_Command
JOIN Command ON FK_Command=PK_Command
JOIN CommandCategory ON FK_CommandCategory=PK_CommandCategory
where PK_InfraredGroup_Command=?',array($infraredGroupID));
			else
				$isSingleCode=$publicADO->Execute('
					select PK_InfraredGroup_Command,FK_CommandCategory,PK_Command,FK_Command,FK_Users,FK_DeviceTemplate,FK_Device,InfraredGroup_Command.psc_user,IRData,Command.Description
FROM InfraredGroup_Command
JOIN Command ON FK_Command=PK_Command
JOIN CommandCategory ON FK_CommandCategory=PK_CommandCategory
where PK_InfraredGroup_Command=?',array($infraredGroupID));
               $isOtherCustomCode=$publicADO->Execute('SELECT * FROM InfraredGroup_Command WHERE FK_InfraredGroup=? AND FK_Command=? AND FK_Device=? AND FK_DeviceTemplate=? AND FK_Users=?',array($infraredGroupID,$commandID, $deviceID,$dtID,$_SESSION['userID']));
			if($isOtherCustomCode->RecordCount()>0){
				$rowOther=$isOtherCustomCode->FetchRow();
				$publicADO->Execute('UPDATE InfraredGroup_Command SET IRData=? WHERE PK_InfraredGroup_Command=?',array($irData,$rowOther['PK_InfraredGroup_Command']));
			}else
        { 
				$publicADO->Execute('UPDATE InfraredGroup_Command SET FK_Command=?, FK_Device=?, FK_DeviceTemplate=?, FK_Users=?,IRData=? WHERE PK_InfraredGroup_Command=?',array($commandID, $deviceID,$dtID,$_SESSION['userID'],$irData,$infraredGroupID));
			$igcID=$publicADO->Insert_ID();
        }
			if($isSingleCode->RecordCount()==0){
				$publicADO->Execute('UPDATE InfraredGroup_Command_Preferred SET FK_Installation=? where PK_InfraredGroup_Command=? ',array($igcID,$installationID,$infraredGroupID));
			}
			$out.='<script>
					opener.location.reload();
					self.close();
				</script>
			';
		}	
		else{
			header("Location: index.php?section=editIRGCode&deviceID=$deviceID&dtID=$dtID&infraredGroupID=$infraredGroupID&commandID=".$commandID);
		}
			
	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Edit IR Code');			
	$output->output();
}
?>
