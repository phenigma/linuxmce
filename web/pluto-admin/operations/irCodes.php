<?
function irCodes($output,$dbADO) {
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$dbADO->debug=false;

	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	
	$dtID = (int)$_REQUEST['dtID'];
	$deviceID = (int)$_REQUEST['deviceID'];
	$userID = $_SESSION['userID'];
	if ($dtID==0) {
		header("Location: index.php?section=login");
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
			<input type="hidden" name="dtID" value="'.$dtID.'">
			<input type="hidden" name="deviceID" value="'.$deviceID.'">
			<input type="hidden" name="irgroup_command" value="">';
	$selectDTData='
		SELECT DeviceTemplate.Description AS Template, DeviceCategory.Description AS Category,Manufacturer.Description AS Manufacturer, FK_Manufacturer,FK_DeviceCategory
		FROM DeviceTemplate
		INNER JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory
		INNER JOIN Manufacturer ON FK_Manufacturer=PK_Manufacturer
		WHERE PK_DeviceTemplate=?';
	$resDTData=$dbADO->Execute($selectDTData,$dtID);
	if($resDTData->RecordCount()==0){
		header("Location: index.php?section=userHome");
		exit();
	}
	$rowDTData=$resDTData->FetchRow();
	$manufacturerID=$rowDTData['FK_Manufacturer'];
	$deviceCategoryID=$rowDTData['FK_DeviceCategory'];
	
	$selectedIRGrops=array();
	$resDT_IG=$dbADO->Execute('SELECT * FROM DeviceTemplate_InfraredGroup WHERE FK_DeviceTemplate=?',$dtID);
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
		SELECT Command.Description, FK_Command,IRData,PK_InfraredGroup_Command,FK_DeviceTemplate, FK_InfraredGroup  
		FROM InfraredGroup_Command
		INNER JOIN Command ON FK_Command=PK_Command
		WHERE FK_InfraredGroup IN ('.join(',',$selectedIRGrops).') OR (FK_InfraredGroup IS NULL AND FK_Device=?)';
	$resCommand=$dbADO->Execute($queryCommands,$deviceID);
	$pos=0;
	$customCodesNo=array();
	while($rowCommands=$resCommand->FetchRow()){
		if($rowCommands['FK_DeviceTemplate'])
			$customCodesNo[]=$rowCommands['PK_InfraredGroup_Command'];
		$pos++;
		$RowColor=(($pos%2==1)?'#EEEEEE':'');
		if($rowCommands['FK_InfraredGroup']=='')
			$RowColor='yellow';
		$out.='
		<tr bgcolor="'.$RowColor.'">
			<td align="center">'.$rowCommands['Description'].(($rowCommands['FK_DeviceTemplate']=='')?'':': Custom code for device').'</td>
			<td><input type="text" name="irData_'.$rowCommands['PK_InfraredGroup_Command'].'" value="'.$rowCommands['IRData'].'" '.(($rowCommands['FK_DeviceTemplate']=='')?'disabled':'').'></td>
			<td align="center">'.(($rowCommands['FK_DeviceTemplate']=='')?'<input type="button" class="button" name="addCustomCode" value="Add custom code" onClick="document.irCodes.irgroup_command.value='.$rowCommands['PK_InfraredGroup_Command'].';document.irCodes.submit();">':'<input type="button" class="button" name="delCustomCode" value="Delete code" onClick="document.irCodes.action.value=\'delete\';document.irCodes.irgroup_command.value='.$rowCommands['PK_InfraredGroup_Command'].';document.irCodes.submit();">').' 
				<input type="button" class="button" name="testCode" value="Test code" onClick="self.location=\'index.php?section=irCodes&from=avWizard&dtID='.$dtID.'&deviceID='.$deviceID.'&from=avWizard&action=testCode&irCode='.$rowCommands['IRData'].'\'">
				<input type="button" class="button" name="learnCode" value="Learn code" onClick="windowOpen(\'index.php?section=learnCode&deviceID='.$deviceID.'&commandID='.$rowCommands['FK_Command'].'&action=sendCommand\',\'width=400,height=250,toolbars=true,scrollbars=1,resizable=1\');">
			</td>
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
	// process
	$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
	if (!$canModifyInstallation){
		header("Location: index.php?section=irCodes&error=You are not authorised to change the installation.");
		exit();
	}		
	
	if($action=='testCode'){
		$irCode=$_REQUEST['irCode'];
		$commandToTest='/usr/pluto/bin/MessageSend localhost 0 '.$deviceID.' 1 191 9 '.$irCode;
		system($commandToTest);
		header("Location: index.php?section=irCodes&from=$from&dtID=$dtID&deviceID=$deviceID&infraredGroupID=$infraredGroupID&msg=The command was sent.");
		exit();
	}
	
	$newIRGroup=(int)$_POST['irGroup'];
	$oldIRGroup=(int)$_POST['oldIRGroup'];
	if($newIRGroup!=$oldIRGroup){
		$dbADO->Execute('DELETE FROM DeviceTemplate_InfraredGroup WHERE FK_DeviceTemplate=?',$dtID);
		$dbADO->Execute('INSERT INTO DeviceTemplate_InfraredGroup (FK_DeviceTemplate, FK_InfraredGroup) VALUES (?,?)',array($dtID,$newIRGroup));
		$dbADO->Execute('UPDATE InfraredGroup_Command SET FK_InfraredGroup=? WHERE FK_DeviceTemplate=?',array($newIRGroup,$dtID));
		header("Location: index.php?section=irCodes&from=$from&dtID=$dtID&infraredGroupID=$infraredGroupID&msg=IR Group changed for selected device template.");
		exit();
	}
	
	if(isset($_POST['irgroup_command']) && (int)$_POST['irgroup_command']>0){
		$irg_c=(int)$_POST['irgroup_command'];
		if($action!='delete'){
			$dbADO->Execute('INSERT INTO InfraredGroup_Command (FK_InfraredGroup,FK_Command,FK_DeviceTemplate,IRData) SELECT FK_InfraredGroup,FK_Command,'.$dtID.',IRData FROM InfraredGroup_Command WHERE PK_InfraredGroup_Command=?',$irg_c);
			
			header("Location: index.php?section=irCodes&from=$from&dtID=$dtID&infraredGroupID=$infraredGroupID&msg=Custom code added.");
			exit();
		}else{
			$dbADO->Execute('DELETE FROM InfraredGroup_Command WHERE PK_InfraredGroup_Command=?',$irg_c);
			header("Location: index.php?section=irCodes&from=$from&dtID=$dtID&infraredGroupID=$infraredGroupID&msg=Custom code deleted.");
			exit();
		}
	}

	if(isset($_POST['update'])){
		$customCodesNoArray=explode(',',$_POST['customCodesNo']);
		foreach ($customCodesNoArray as $ig_c){
			$irData=$_POST['irData_'.$ig_c];
			$dbADO->Execute('UPDATE InfraredGroup_Command SET IRData=? WHERE PK_InfraredGroup_Command=?',array($irData,$ig_c));
		}
		header("Location: index.php?section=irCodes&from=$from&deviceID=$deviceID&dtID=$dtID&infraredGroupID=$infraredGroupID&msg=Custom code(s) updated.");
		exit();
	}
		
	header("Location: index.php?section=irCodes&from=$from&dtID=$dtID&infraredGroupID=$infraredGroupID&lastAction=".@$lastAction);
}

$output->setBody($out);
$output->setTitle(APPLICATION_NAME.' :: IR Codes');			
$output->output();  

}
?>
