<?
function irCodes($output,$dbADO) {
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$dbADO->debug=false;
	$installationID = (int)@$_SESSION['installationID'];
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';

	$dtID = (int)$_REQUEST['dtID'];
	$deviceID = (int)$_REQUEST['deviceID'];
	$userID = $_SESSION['userID'];
	if ($dtID==0) {
		header("Location: index.php?section=login");
	}

	$infraredGroupID=(int)@$_REQUEST['infraredGroupID'];
	$GLOBALS['displayedIRGC']=array();
	
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
			<input type="hidden" name="infraredGroupID" value="'.$infraredGroupID.'">
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

		$commandsArray=array();
		$resAllCommands=$dbADO->Execute('SELECT * FROM Command ORDER BY Description ASC');
		while($rowAllCommands=$resAllCommands->FetchRow()){
			$commandsArray[$rowAllCommands['PK_Command']]=$rowAllCommands['Description'];
		}

		$excludedCommandsArray=array($GLOBALS['powerCommand'],$GLOBALS['genericONCommand'],$GLOBALS['genericOFFCommand'],$GLOBALS['DSPModeCommand'],$GLOBALS['inputSelectCommand'],$GLOBALS['outputSelectCommand']);
		$resGrabOldValues = $dbADO->Execute('SELECT * FROM DeviceTemplate_AV WHERE FK_DeviceTemplate = ?',$dtID);
		$row=$resGrabOldValues->FetchRow();
		$togglePower = $row['TogglePower'];
		$toggleDSP = $row['ToggleDSP'];
		$toggleInput = $row['ToggleInput'];
		$toggleOutput = $row['ToggleOutput'];
		if($togglePower==1){
			$commandsToShow=array($GLOBALS['powerCommand']);
		}else{
			$commandsToShow=array($GLOBALS['genericONCommand'],$GLOBALS['genericOFFCommand']);
		}
		$toglePowerCommands=showCodes($commandsToShow,$infraredGroupID,$deviceID,$dtID,$dbADO);

		if($toggleDSP==1){
			$commandsToShow=array($GLOBALS['DSPModeCommand']);
		}else{
			$queryDSPCommands='
				SELECT DeviceTemplate_DSPMode.*
				FROM DeviceTemplate_DSPMode 	
				WHERE FK_DeviceTemplate=? ORDER BY OrderNo ASC';
			$resDSPCommands=$dbADO->Execute($queryDSPCommands,$dtID);
			$commandsToShow=array();
			while($rowDSPCommands=$resDSPCommands->FetchRow()){
				$commandsToShow[]=$rowDSPCommands['FK_Command'];
				$excludedCommandsArray[]=$rowDSPCommands['FK_Command'];
			}
		}
		if(count($commandsToShow)>0)
			$dspModesCommands='
			<tr>
				<td colspan="3" align="center"><fieldset><legend><B>DSP Modes</B></legend>'.showCodes($commandsToShow,$infraredGroupID,$deviceID,$dtID,$dbADO).'</fieldset></td>
			</tr>	
			';

		if($toggleInput==1){
			$commandsToShow=array($GLOBALS['inputSelectCommand']);
		}else{
			$queryInputCommands='
			SELECT DeviceTemplate_Input.*
			FROM DeviceTemplate_Input 	
			WHERE FK_DeviceTemplate=? ORDER BY OrderNo ASC';
			$resInputCommands=$dbADO->Execute($queryInputCommands,$dtID);
			$commandsToShow=array();
			while($rowInputCommands=$resInputCommands->FetchRow()){
				$commandsToShow[]=$rowInputCommands['FK_Command'];
				$excludedCommandsArray[]=$rowInputCommands['FK_Command'];
			}
		}
		if(count($commandsToShow)>0)
			$inputCommands='
			<tr>
				<td colspan="3" align="center"><fieldset><legend><B>Input codes</B></legend>'.showCodes($commandsToShow,$infraredGroupID,$deviceID,$dtID,$dbADO).'</fieldset></td>
			</tr>	
			';

		if($toggleOutput==1){
			$commandsToShow=array($GLOBALS['outputSelectCommand']);
		}else{
			$queryOutputCommands='
			SELECT DeviceTemplate_Output.*
			FROM DeviceTemplate_Output 	
			WHERE FK_DeviceTemplate=? ORDER BY OrderNo ASC';
			$resOutputCommands=$dbADO->Execute($queryOutputCommands,$dtID);
			$commandsToShow=array();
			while($rowOutputCommands=$resOutputCommands->FetchRow()){
				$commandsToShow[]=$rowOutputCommands['FK_Command'];
				$excludedCommandsArray[]=$rowOutputCommands['FK_Command'];
			}
		}
		if(count($commandsToShow)>0)
			$outputCommands='
			<tr>
				<td colspan="3" align="center"><fieldset><legend><B>Output codes</B></legend>'.showCodes($commandsToShow,$infraredGroupID,$deviceID,$dtID,$dbADO).'</fieldset></td>
			</tr>	
			';
		
		$out.='
		<input type="hidden" name="oldIRGroup" value="'.@$infraredGroupID.'">
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
					<option value="0" '.(($infraredGroupID==0)?'selected':'').'>I don\'t know the group</option>';
		$queryIG='
			SELECT PK_InfraredGroup,InfraredGroup.Description 
			FROM InfraredGroup 
			WHERE FK_Manufacturer=? AND FK_DeviceCategory=?
			ORDER BY Description ASC';
		$resIG=$dbADO->Execute($queryIG,array($manufacturerID,$deviceCategoryID));
		while($rowIG=$resIG->FetchRow()){
			$out.='<option value="'.$rowIG['PK_InfraredGroup'].'" '.(($rowIG['PK_InfraredGroup']==$infraredGroupID)?'selected':'').'>'.$rowIG['Description'].'</option>';
		}
		$out.='
				</select></td>
		</tr>';

		$queryCommands='
			SELECT Command.Description, FK_Command,IRData,PK_InfraredGroup_Command,FK_DeviceTemplate, FK_InfraredGroup ,FK_Device 
			FROM InfraredGroup_Command
			INNER JOIN Command ON FK_Command=PK_Command
			WHERE (FK_InfraredGroup =? OR (FK_InfraredGroup IS NULL AND FK_Device=?)) AND FK_Command NOT IN ('.join(',',$excludedCommandsArray).')
			ORDER BY Description ASC';
		$resCommand=$dbADO->Execute($queryCommands,array($infraredGroupID,$deviceID));
		$commandsToShow=array();
		while($rowCommands=$resCommand->FetchRow()){
			$commandsToShow[]=$rowCommands['FK_Command'];
		}
		if(count($commandsToShow)>0)
			$otherCommands='
			<tr>
				<td colspan="3" align="center"><fieldset><legend><B>Other codes</B></legend>'.showCodes($commandsToShow,$infraredGroupID,$deviceID,$dtID,$dbADO).'</fieldset></td>
			</tr>	
			';
		
		
		$out.='		
			<tr>
				<td colspan="3" align="center"><input type="button" class="button" name="button" value="Add/Remove commands" onClick="windowOpen(\'index.php?section=infraredCommands&infraredGroup='.$infraredGroupID.'&deviceID='.$deviceID.'&dtID='.$dtID.'\',\'width=800,height=600,toolbars=true,scrollbars=1,resizable=1\');"></td>
			</tr>';
		$out.='
			<tr>
				<td colspan="3" align="center"><fieldset><legend><B>Power codes</B></legend>'.@$toglePowerCommands.'</fieldset></td>
			</tr>
			'.@$dspModesCommands.@$inputCommands.@$outputCommands.@$otherCommands.'
			<tr>
				<td colspan="3" align="center"><input type="submit" class="button" name="update" value="Update"></td>
			</tr>
		
	';
		$out.='
		</table>
			<input type="hidden" name="displayedIRGC" value="'.join(',',$GLOBALS['displayedIRGC']).'">
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
			header("Location: index.php?section=irCodes&from=$from&dtID=$dtID&deviceID=$deviceID&infraredGroupID=$newIRGroup&msg=IR Group changed for selected device template.");
			exit();
		}

		if(isset($_POST['irgroup_command']) && (int)$_POST['irgroup_command']>0){
			$irg_c=(int)$_POST['irgroup_command'];
			if($action!='delete'){
				$dbADO->Execute('INSERT INTO InfraredGroup_Command (FK_InfraredGroup,FK_Command,FK_Device,FK_DeviceTemplate,IRData,FK_Users) SELECT FK_InfraredGroup,FK_Command,'.$deviceID.','.$dtID.',IRData,'.$_SESSION['userID'].' FROM InfraredGroup_Command WHERE PK_InfraredGroup_Command=?',$irg_c);

				header("Location: index.php?section=irCodes&from=$from&dtID=$dtID&deviceID=$deviceID&infraredGroupID=$infraredGroupID&msg=Custom code added.");
				exit();
			}else{
				$dbADO->Execute('DELETE FROM InfraredGroup_Command WHERE PK_InfraredGroup_Command=?',$irg_c);
				header("Location: index.php?section=irCodes&from=$from&dtID=$dtID&deviceID=$deviceID&infraredGroupID=$infraredGroupID&msg=Custom code deleted.");
				exit();
			}
		}

		if(isset($_POST['update'])){
			$customCodesNoArray=explode(',',@$_POST['displayedIRGC']);
			foreach ($customCodesNoArray as $ig_c){
				$irData=@$_POST['irData_'.$ig_c];
				$dbADO->Execute('UPDATE InfraredGroup_Command SET IRData=? WHERE PK_InfraredGroup_Command=?',array($irData,$ig_c));
			}

			if(isset($_POST['preferedIGC']) && (int)$_POST['preferedIGC']!=0){
				$preferedIGC=(int)$_POST['preferedIGC'];
				$dbADO->Execute('DELETE FROM InfraredGroup_Command_Preferred WHERE FK_Installation=?',$installationID);
				$dbADO->Execute('INSERT INTO InfraredGroup_Command_Preferred  (FK_InfraredGroup_Command,FK_Installation) VALUES (?,?)',array($preferedIGC,$installationID));
			}
			header("Location: index.php?section=irCodes&from=$from&deviceID=$deviceID&dtID=$dtID&infraredGroupID=$infraredGroupID&msg=IR codes updated.");
			exit();
		}

		header("Location: index.php?section=irCodes&from=$from&dtID=$dtID&deviceID=$deviceID&infraredGroupID=$infraredGroupID&lastAction=".@$lastAction);
	}

	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: IR Codes');
	$output->output();

}

function showCodes($commandsToShow,$infraredGroupID,$deviceID,$dtID,$dbADO)
{
	$dbADO->debug=true;
	$installationID=(int)$_SESSION['installationID'];
	$out='';
	if(count($commandsToShow)==0)
		return $out;
	if($infraredGroupID==0){
		$queryCommands='
			SELECT Command.Description, FK_Command, IRData, PK_InfraredGroup_Command, FK_DeviceTemplate, FK_InfraredGroup , FK_Device, PK_InfraredGroup_Command,FK_Users,PK_Command 
			FROM  Command
			LEFT JOIN InfraredGroup_Command ON FK_Command=PK_Command 
			WHERE PK_Command IN ('.join(',',$commandsToShow).') AND FK_InfraredGroup is NULL
			ORDER BY Description ASC';
		$resCommand=$dbADO->Execute($queryCommands,$infraredGroupID);
	}else{
		$queryCommands='
			SELECT Command.Description, FK_Command, IRData, PK_InfraredGroup_Command, FK_DeviceTemplate, FK_InfraredGroup , FK_Device, PK_InfraredGroup_Command,FK_Users,PK_Command 
			FROM  Command
			LEFT JOIN InfraredGroup_Command ON FK_Command=PK_Command AND (FK_InfraredGroup =? OR FK_InfraredGroup is NULL)
			WHERE PK_Command IN ('.join(',',$commandsToShow).')
			ORDER BY Description ASC';
		$resCommand=$dbADO->Execute($queryCommands,$infraredGroupID);
	}

	$resIGCPrefered=$dbADO->Execute('SELECT * FROM InfraredGroup_Command_Preferred WHERE FK_Installation=?',$installationID);
	$rowIGC=$resIGCPrefered->FetchRow();

	$pos=0;
	$customCodesNo=array();
	$out.='<table width="100%">';
	while($rowCommands=$resCommand->FetchRow()){
		if(!is_null($rowCommands['PK_InfraredGroup_Command']))
			$GLOBALS['displayedIRGC'][]=$rowCommands['PK_InfraredGroup_Command'];
			
		if($rowCommands['FK_DeviceTemplate'])
		$customCodesNo[]=$rowCommands['PK_InfraredGroup_Command'];
		$pos++;
		$RowColor=(($pos%2==1)?'#EEEEEE':'');
		if($rowCommands['FK_InfraredGroup']=='' || !is_null($rowCommands['FK_DeviceTemplate']))
			$RowColor='yellow';
		if(is_null($rowCommands['PK_InfraredGroup_Command']))
			$RowColor='lightblue';
		$out.='
			<tr bgcolor="'.$RowColor.'">
				<td align="center" width="100"><B>'.$rowCommands['Description'].'</B>'.((($rowCommands['FK_InfraredGroup']!='' && $rowCommands['FK_DeviceTemplate']=='') || $rowCommands['PK_InfraredGroup_Command']=='')?'<br><input type="button" class="button" name="learnCode" value="Learn code" onClick="windowOpen(\'index.php?section=learnCode&deviceID='.$deviceID.'&commandID='.$rowCommands['PK_Command'].'&action=sendCommand\',\'width=400,height=250,toolbars=true,scrollbars=1,resizable=1\');">':'<br> User: '.$rowCommands['FK_Users']).'</td>
				<td>'.((!is_null($rowCommands['PK_InfraredGroup_Command']))?'<input type="radio" name="preferedIGC" value="'.$rowCommands['PK_InfraredGroup_Command'].'" '.(($rowIGC['FK_InfraredGroup_Command']==$rowCommands['PK_InfraredGroup_Command'])?'checked':'').'>':'&nbsp;').'</td>
				<td>'.((is_null($rowCommands['PK_InfraredGroup_Command']))?'<B> Not in database</B>':'<textarea name="irData_'.$rowCommands['PK_InfraredGroup_Command'].'" rows="2" cols="100" '.(($rowCommands['FK_DeviceTemplate']=='')?'disabled':'').'>'.$rowCommands['IRData'].'</textarea>').'</td>
				<td align="center">';
				if(!is_null($rowCommands['PK_InfraredGroup_Command']))
					$out.=(($rowCommands['FK_DeviceTemplate']=='')?'<input type="button" class="button" name="addCustomCode" value="Add custom code" onClick="document.irCodes.irgroup_command.value='.$rowCommands['PK_InfraredGroup_Command'].';document.irCodes.submit();">':'<input type="button" class="button" name="delCustomCode" value="Delete code" onClick="if(confirm(\'Are you sure you want to delete this code?\')){document.irCodes.action.value=\'delete\';document.irCodes.irgroup_command.value='.$rowCommands['PK_InfraredGroup_Command'].';document.irCodes.submit();}">').' 
					'.((!is_null($rowCommands['PK_InfraredGroup_Command']))?'<br><input type="button" class="button" name="testCode" value="Test code" onClick="self.location=\'index.php?section=irCodes&from=avWizard&dtID='.$dtID.'&deviceID='.$deviceID.'&infraredGroupID='.$infraredGroupID.'&from=avWizard&action=testCode&irCode='.$rowCommands['IRData'].'\'">':'').'
				</td>
			</tr>';
	}
	$out.='</table>';
	return $out;
}
?>
