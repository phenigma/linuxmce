<?
function irCodes($output,$dbADO) {
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$dbADO->debug=false;

	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	
	$deviceID = (int)$_REQUEST['deviceID'];
	$userID = $_SESSION['userID'];
	
	if ($deviceID==0) {
		header("Location: index.php?section=userHome");
	}
	
	$infraredGroupID=@$_REQUEST['infraredGroupID'];	

	if ($action=='form') {
	$out.='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		</script>	
	<div class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
	<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>	
		<form action="index.php" method="POST" name="irCodes">
			<input type="hidden" name="section" value="irCodes">
			<input type="hidden" name="action" value="update">
			<input type="hidden" name="deviceID" value="'.$deviceID.'">
			<input type="hidden" name="irgroup_command" value="">';
	$selectDTData='
		SELECT DeviceTemplate.Description AS Template, DeviceCategory.Description AS Category,Manufacturer.Description AS Manufacturer, FK_Manufacturer,FK_DeviceCategory
		FROM DeviceTemplate
		INNER JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory
		INNER JOIN Manufacturer ON FK_Manufacturer=PK_Manufacturer
		WHERE PK_DeviceTemplate=?';
	$resDTData=$dbADO->Execute($selectDTData,$deviceID);
	if($resDTData->RecordCount()==0){
		header("Location: index.php?section=userHome");
		exit();
	}
	$rowDTData=$resDTData->FetchRow();
	$manufacturerID=$rowDTData['FK_Manufacturer'];
	$deviceCategoryID=$rowDTData['FK_DeviceCategory'];
	
	$selectedIRGrops=array();
	$resDT_IG=$dbADO->Execute('SELECT * FROM DeviceTemplate_InfraredGroup WHERE FK_DeviceTemplate=?',$deviceID);
	if($resDT_IG->RecordCount()>0){
		while($rowDT_IG=$resDT_IG->FetchRow()){
			$selectedIRGrops[]=$rowDT_IG['FK_InfraredGroup'];
		}
	}
	
	$out.='
		<input type="hidden" name="oldIRGroup" value="'.@$selectedIRGrops[0].'">
		<h3>Edit infrared codes</h3>
		<table>
			<tr>
				<td class="err">WARNING: the changes will afect all devices created from <B>'.$rowDTData['Template'].'</B> template.</td>
			</tr>
			<tr>
				<td valign="top">Device template <B>'.$rowDTData['Template'].'</B>, category <B>'.$rowDTData['Category'].'</B> and manufacturer <B>'.$rowDTData['Manufacturer'].'</B>.<td>
			</tr>
			<tr>
				<td>Uses Infrared Group <select name="irGroup" onChange="document.irCodes.submit();">
					<option value="0">- No IR</option>';
		$queryIG='
			SELECT PK_InfraredGroup,InfraredGroup.Description 
			FROM InfraredGroup 
			WHERE FK_Manufacturer=? AND FK_DeviceCategory=?
			ORDER BY Description ASC';
		$resIG=$dbADO->Execute($queryIG,array($manufacturerID,$deviceCategoryID));
		while($rowIG=$resIG->FetchRow()){
			$out.='<option value="'.$rowIG['PK_InfraredGroup'].'" '.(($rowIG['PK_InfraredGroup']==@$selectedIRGrops[0])?'selected':'').'>'.$rowIG['Description'].'</option>';
		}
		$out.='
				</select></td>
		</tr>
		<tr>
			<td><table>
		<tr>
			<td align="center"><B>Command</B></td>
			<td align="center"><B>IR Data</B></td>
			<td align="center"><B>Action</B></td>
		</tr>';
		
	if(count($selectedIRGrops)==0)
		$selectedIRGrops[]=0;
	$queryCommands='
		SELECT Command.Description, FK_Command,IRData,PK_InfraredGroup_Command,FK_DeviceTemplate  
		FROM InfraredGroup_Command
		INNER JOIN Command ON FK_Command=PK_Command
		WHERE FK_InfraredGroup IN ('.join(',',$selectedIRGrops).')';
	$resCommand=$dbADO->Execute($queryCommands);
	$pos=0;
	$customCodesNo=array();
	while($rowCommands=$resCommand->FetchRow()){
		if($rowCommands['FK_DeviceTemplate'])
			$customCodesNo[]=$rowCommands['PK_InfraredGroup_Command'];
		$pos++;
		$out.='
		<tr bgcolor="'.(($pos%2==1)?'#EEEEEE':'').'">
			<td align="center">'.$rowCommands['Description'].(($rowCommands['FK_DeviceTemplate']=='')?'':': Custom code for device').'</td>
			<td><input type="text" name="irData_'.$rowCommands['PK_InfraredGroup_Command'].'" value="'.$rowCommands['IRData'].'" '.(($rowCommands['FK_DeviceTemplate']=='')?'disabled':'').'></td>
			<td align="center">'.(($rowCommands['FK_DeviceTemplate']=='')?'<input type="button" class="button" name="addCustomCode" value="Add custom code" onClick="document.irCodes.irgroup_command.value='.$rowCommands['PK_InfraredGroup_Command'].';document.irCodes.submit();">':'<input type="button" class="button" name="delCustomCode" value="Delete code" onClick="document.irCodes.action.value=\'delete\';document.irCodes.irgroup_command.value='.$rowCommands['PK_InfraredGroup_Command'].';document.irCodes.submit();">').'</td>
		</tr>';
	}
	if(count($customCodesNo)!=0){
		$out.='
		<tr>
			<td colspan="3" align="center"><input type="submit" class="button" name="update" value="Update custom codes"></td>
		</tr>';
	}
	$out.='		</table>
				</td>
			</tr>
		</table>
		<input type="hidden" name="customCodesNo" value="'.join(',',$customCodesNo).'">
		</form>
	';	
	} else {
	// process DT	
	$newIRGroup=(int)$_POST['irGroup'];
	$oldIRGroup=(int)$_POST['oldIRGroup'];
	if($newIRGroup!=$oldIRGroup){
		$dbADO->Execute('DELETE FROM DeviceTemplate_InfraredGroup WHERE FK_DeviceTemplate=?',$deviceID);
		$dbADO->Execute('INSERT INTO DeviceTemplate_InfraredGroup (FK_DeviceTemplate, FK_InfraredGroup) VALUES (?,?)',array($deviceID,$newIRGroup));
		$dbADO->Execute('UPDATE InfraredGroup_Command SET FK_InfraredGroup=? WHERE FK_DeviceTemplate=?',array($newIRGroup,$deviceID));
		header("Location: index.php?section=irCodes&from=$from&deviceID=$deviceID&infraredGroupID=$infraredGroupID&msg=IR Group changed for selected device template.");
		exit();
	}
	
	if(isset($_POST['irgroup_command']) && (int)$_POST['irgroup_command']>0){
		$irg_c=(int)$_POST['irgroup_command'];
		if($action!='delete'){
			$dbADO->Execute('INSERT INTO InfraredGroup_Command (FK_InfraredGroup,FK_Command,FK_DeviceTemplate,IRData) SELECT FK_InfraredGroup,FK_Command,'.$deviceID.',IRData FROM InfraredGroup_Command WHERE PK_InfraredGroup_Command=?',$irg_c);
			
			header("Location: index.php?section=irCodes&from=$from&deviceID=$deviceID&infraredGroupID=$infraredGroupID&msg=Custom code added.");
			exit();
		}else{
			$dbADO->Execute('DELETE FROM InfraredGroup_Command WHERE PK_InfraredGroup_Command=?',$irg_c);
			header("Location: index.php?section=irCodes&from=$from&deviceID=$deviceID&infraredGroupID=$infraredGroupID&msg=Custom code deleted.");
			exit();
		}
	}

	if(isset($_POST['update'])){
		$customCodesNoArray=explode(',',$_POST['customCodesNo']);
		foreach ($customCodesNoArray as $ig_c){
			$irData=$_POST['irData_'.$ig_c];
			$dbADO->Execute('UPDATE InfraredGroup_Command SET IRData=? WHERE PK_InfraredGroup_Command=?',array($irData,$ig_c));
		}
		header("Location: index.php?section=irCodes&from=$from&deviceID=$deviceID&infraredGroupID=$infraredGroupID&msg=Custom code(s) updated.");
		exit();
	}
		
	header("Location: index.php?section=irCodes&from=$from&deviceID=$deviceID&infraredGroupID=$infraredGroupID&lastAction=".@$lastAction);
}

$output->setBody($out);
$output->setTitle(APPLICATION_NAME.' :: IR Codes');			
$output->output();  

}
?>
