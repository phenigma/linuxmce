<?
function irCodes($output,$dbADO) {
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$dbADO->debug=false;
	$installationID = (int)@$_SESSION['installationID'];
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	$label=(isset($_REQUEST['label']))?$_REQUEST['label']:'infrared';
	$GLOBALS['label']=$label;
	
	$dtID = (int)$_REQUEST['dtID'];
	$deviceID = (isset($_REQUEST['deviceID']) && (int)@$_REQUEST['deviceID']!=0)?(int)$_REQUEST['deviceID']:NULL;
	$userID = $_SESSION['userID'];
	if ($dtID==0) {
		header("Location: index.php?section=login");
	}

	if(!isset($_REQUEST['infraredGroupID'])){
		$resDefaultIG=$dbADO->Execute('SELECT FK_InfraredGroup FROM DeviceTemplate_InfraredGroup WHERE FK_DeviceTemplate=?',$dtID);
		if($resDefaultIG->RecordCount()>0){
			$rowDefaultIG=$resDefaultIG->FetchRow();
			$infraredGroupID=$rowDefaultIG['FK_InfraredGroup'];
		}else{
			$infraredGroupID=0;
		}
	}else
		$infraredGroupID=(int)$_REQUEST['infraredGroupID'];
	$GLOBALS['displayedIRGC']=array();
	$GLOBALS['preferredIGC']=array();
	$GLOBALS['igcPrefered']=array();	

	$resIGCPrefered=$dbADO->Execute('
		SELECT InfraredGroup_Command_Preferred.*, InfraredGroup_Command.FK_Command
		FROM InfraredGroup_Command_Preferred 
		INNER JOIN InfraredGroup_Command ON FK_InfraredGroup_Command=PK_InfraredGroup_Command
		WHERE InfraredGroup_Command_Preferred.FK_Installation=? AND FK_DeviceTemplate=?',array($installationID,$dtID));
	while($rowIGC=$resIGCPrefered->FetchRow()){
		$GLOBALS['igcPrefered'][$rowIGC['FK_Command']]=$rowIGC['FK_InfraredGroup_Command'];
	}

	if ($action=='form') {
		$out.='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		
			function setPreferred(radioGroup,value)
			{
				eval("len=document.irCodes."+radioGroup+".length");
				for(i=0;i<len;i++){
					eval("if(document.irCodes."+radioGroup+"["+i+"].value=="+value+") document.irCodes."+radioGroup+"["+i+"].checked=true;");
				}
				
			}
		</script>	
	<div class="err"><br>'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
	<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>	
		
		<form action="index.php" method="POST" name="irCodes">
			<input type="hidden" name="section" value="irCodes">
			<input type="hidden" name="action" value="update">
			<input type="hidden" name="dtID" value="'.$dtID.'">
			<input type="hidden" name="deviceID" value="'.$deviceID.'">
			<input type="hidden" name="infraredGroupID" value="'.$infraredGroupID.'">
			<input type="hidden" name="irgroup_command" value="">
			<input type="hidden" name="label" value="'.$label.'">';
		
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

		$excludedCommandsArray=array($GLOBALS['powerCommand'],$GLOBALS['genericONCommand'],$GLOBALS['genericOFFCommand'],$GLOBALS['DSPModeCommand']);		// $GLOBALS['inputSelectCommand'],$GLOBALS['outputSelectCommand'] removed vor GSD
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
		
		// exception for ruby codes
		if($GLOBALS['label']!='infrared'){
			$resRubyCommands=$dbADO->Execute('SELECT * FROM Command WHERE FK_CommandCategory=?',$GLOBALS['GeneralInternal']);
			while($rowRubyCommands=$resRubyCommands->FetchRow()){
				$commandsToShow[]=$rowRubyCommands['PK_Command'];
				$excludedCommandsArray[]=$rowRubyCommands['PK_Command'];
			}
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
				<td colspan="3" align="center"><fieldset style="width:98%"><legend><B>DSP Modes</B></legend>'.showCodes($commandsToShow,$infraredGroupID,$deviceID,$dtID,$dbADO).'</fieldset></td>
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
				<td colspan="3" align="center"><fieldset style="width:98%"><legend><B>Input codes</B></legend>'.showCodes($commandsToShow,$infraredGroupID,$deviceID,$dtID,$dbADO).'</fieldset></td>
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
				<td colspan="3" align="center"><fieldset style="width:98%"><legend><B>Output codes</B></legend>'.showCodes($commandsToShow,$infraredGroupID,$deviceID,$dtID,$dbADO).'</fieldset></td>
			</tr>	
			';
		
		$out.='
		<input type="hidden" name="oldIRGroup" value="'.@$infraredGroupID.'">
		<h3>Edit '.$label.' codes</h3>
		<table border="0" width="100%">
			<tr>
				<td class="err" colspan="2">WARNING: the changes will afect all devices created from <B>'.$rowDTData['Template'].'</B> template.</td>
			</tr>
			<tr>
				<td valign="top" colspan="2">Device template <B>'.$rowDTData['Template'].'</B>, category <B>'.$rowDTData['Category'].'</B> and manufacturer <B>'.$rowDTData['Manufacturer'].'</B>.<td>
			</tr>';
		if(!is_null($deviceID)){
			$out.='
			<tr>
				<td colspan="2">Uses '.$label.' group <select name="irGroup" onChange="document.irCodes.submit();">
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
		}
		$out.='
			<tr>
				<td valign="top" width="250">Implement Command Groups </td>
				<td><table border="0" width="100%">';

		$cgArray=getCheckedDeviceCommandGroup($dtID,$deviceCategoryID,$dbADO);

		$oldCheckedDCG=array();
		$deviceCG=array();
		foreach ($cgArray AS $key=>$arrayValues){
			// get commands from DeviceCommandGroup
			$res=$dbADO->Execute('
				SELECT Command.Description,PK_Command 
				FROM DeviceCommandGroup_Command 
				INNER JOIN Command ON FK_Command=PK_Command
				WHERE FK_DeviceCommandGroup=?
				ORDER BY Description ASC',$key);
			$commandsInGroup=array();
			$commandIDs=array();
			while($rowC=$res->FetchRow()){
				$commandsInGroup[]=$rowC['Description'];
				$commandIDs[]=$rowC['PK_Command'];
			}
			
			
			// insert command in InfraredGroup_Command
			if($arrayValues['checked']==1){
				//$dbADO->debug=true;
					if(count($commandIDs)==0){
						$commandIDs[]=0;
					}
					
					$oldRes=$dbADO->Execute('
						SELECT PK_InfraredGroup_Command,FK_Command
						FROM InfraredGroup_Command 
						WHERE FK_Command IN ('.join(',',$commandIDs).') AND 
							((FK_DeviceTemplate=? AND FK_InfraredGroup IS NULL AND (FK_Device IS NULL OR FK_Device=?))
							OR (FK_DeviceTemplate IS NULL AND FK_InfraredGroup IS NOT NULL AND FK_Device IS NULL))',array($dtID,$deviceID));
					$existCommands=array();
					while($rowCom=$oldRes->FetchRow()){
						$existCommands[]=$rowCom['FK_Command'];
					}
					$commandsToAdd=array_diff($commandIDs,$existCommands);
					foreach ($commandsToAdd AS $cmd){
						$dbADO->Execute('
							INSERT INTO InfraredGroup_Command
								(FK_InfraredGroup,FK_Command,FK_DeviceTemplate,IRData,FK_Users) 
							VALUES
								(NULL,?,?,?,?)
						',array($cmd,$dtID,'',$_SESSION['userID']));
					}
					
				$oldCheckedDCG[]=$key;
			}

			$deviceCG[]=$key;
			$out.='
			<tr>
				<td><input type="checkbox" name="dcg_'.$key.'" value="1" '.(($arrayValues['checked']==1)?'checked':'').'></td>
				<td title="'.join(', ',$commandsInGroup).'">'.$arrayValues['Description'].'</td>
			</tr>';
		}
		
		$out.='	
			<input type="hidden" name="deviceCG" value="'.join(',',$deviceCG).'">
			<input type="hidden" name="oldCheckedDCG" value="'.join(',',$oldCheckedDCG).'">
				</table>
				</td>
		</tr>
		';

		
		$queryCommands='
			SELECT DISTINCT FK_Command
			FROM InfraredGroup_Command
			WHERE (FK_InfraredGroup =? OR (FK_InfraredGroup IS NULL AND FK_DeviceTemplate=?)) AND FK_Command NOT IN ('.join(',',$excludedCommandsArray).')';
		$resCommand=$dbADO->Execute($queryCommands,array($infraredGroupID,$dtID));
		$commandsToShow=array();
		$commandsDisplayed=$excludedCommandsArray;
		while($rowCommands=$resCommand->FetchRow()){
			$commandsToShow[]=$rowCommands['FK_Command'];
			$commandsDisplayed[]=$rowCommands['FK_Command'];
		}
		if(count($commandsToShow)>0)
			$otherCommands='
			<tr>
				<td colspan="3" align="center"><fieldset style="width:98%"><legend><B>Other codes</B></legend>'.showCodes($commandsToShow,$infraredGroupID,$deviceID,$dtID,$dbADO).'</fieldset></td>
			</tr>	
			';
		
		
		$out.='		
			<tr>
				<td colspan="3" align="center"><input type="button" class="button" name="button" value="Add/Remove commands" onClick="windowOpen(\'index.php?section=infraredCommands&infraredGroup='.$infraredGroupID.'&deviceID='.$deviceID.'&dtID='.$dtID.(($GLOBALS['label']!='infrared')?'&rootNode=1':'').'\',\'width=800,height=600,toolbars=true,scrollbars=1,resizable=1\');"> <input type="submit" class="button" name="update" value="Update"></td>
			</tr>';
		$out.='
			<tr>
				<td colspan="3" align="center"><fieldset style="width:98%"><legend><B>Power codes</B></legend>'.@$toglePowerCommands.'</fieldset></td>
			</tr>
			'.@$dspModesCommands.@$inputCommands.@$outputCommands.@$otherCommands.'
			<tr>
				<td colspan="3" align="center"><table>
					<tr>
						<td><B>Legend:</B> </td>
						<td width="20" bgcolor="lightblue">&nbsp;</td>
						<td>Standard codes</td>
						<td width="20" bgcolor="yellow">&nbsp;</td>
						<td>My custom codes</td>
						<td width="20" bgcolor="lightyellow">&nbsp;</td>
						<td>Other users custom codes</td>
					</tr>
				</table></td>
			</tr>
			<tr>
				<td colspan="3" align="center"><input type="submit" class="button" name="update" value="Update"></td>
			</tr>
		
	';
		$out.='
		</table>
			<input type="hidden" name="displayedIRGC" value="'.join(',',$GLOBALS['displayedIRGC']).'">
			<input type="hidden" name="commandsDisplayed" value="'.join(',',$commandsDisplayed).'">
		</form>
	';	
	} else {
		// process
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=irCodes&from=$from&dtID=$dtID&deviceID=$deviceID&infraredGroupID=$infraredGroupID&error=You are not authorised to change the installation.");
			exit();
		}
		if($action=='testCode'){
			$irCode=stripslashes($_REQUEST['irCode']);
			$ig_c=(int)$_REQUEST['ig_c'];
			$owner=(int)$_REQUEST['owner'];
			if($owner==$userID){
				$dbADO->Execute('UPDATE InfraredGroup_Command SET IRData=? WHERE PK_InfraredGroup_Command=?',array($irCode,$ig_c));
			}
			$commandToTest='/usr/pluto/bin/MessageSend localhost 0 '.$deviceID.' 1 191 70 "'.$irCode.'"';
			exec($commandToTest);
			header("Location: index.php?section=irCodes&from=$from&dtID=$dtID&deviceID=$deviceID&infraredGroupID=$infraredGroupID&msg=The command was sent.&label=".$GLOBALS['label'].'#test_'.$ig_c);
			exit();
		}

		if(!is_null($deviceID)){
			$newIRGroup=((int)@$_POST['irGroup']>0)?(int)$_POST['irGroup']:NULL;
			$oldIRGroup=(int)$_POST['oldIRGroup'];
			if($newIRGroup!=$oldIRGroup){
				$dbADO->Execute('DELETE FROM DeviceTemplate_InfraredGroup WHERE FK_DeviceTemplate=?',$dtID);
				if(!is_null($newIRGroup))
					$dbADO->Execute('INSERT INTO DeviceTemplate_InfraredGroup (FK_DeviceTemplate, FK_InfraredGroup) VALUES (?,?)',array($dtID,$newIRGroup));
				$dbADO->Execute('UPDATE InfraredGroup_Command SET FK_InfraredGroup=? WHERE FK_DeviceTemplate=? AND (FK_InfraredGroup=? OR  FK_InfraredGroup IS NULL)',array($newIRGroup,$dtID,$oldIRGroup));
				header("Location: index.php?section=irCodes&from=$from&dtID=$dtID&deviceID=$deviceID&infraredGroupID=$newIRGroup&msg=IR Group changed for selected device template.&label=".$GLOBALS['label']);
				exit();
			}
		}
		
		if(isset($_POST['irgroup_command']) && (int)$_POST['irgroup_command']>0){
			$irg_c=(int)$_POST['irgroup_command'];
			if($action!='delete'){
				$dbADO->Execute('INSERT INTO InfraredGroup_Command (FK_InfraredGroup,FK_Command,FK_Device,FK_DeviceTemplate,IRData,FK_Users) SELECT FK_InfraredGroup,FK_Command,'.$deviceID.','.$dtID.',IRData,'.$_SESSION['userID'].' FROM InfraredGroup_Command WHERE PK_InfraredGroup_Command=?',$irg_c);

				header("Location: index.php?section=irCodes&from=$from&dtID=$dtID&deviceID=$deviceID&infraredGroupID=$infraredGroupID&msg=Custom code added.&label=".$GLOBALS['label']);
				exit();
			}else{
				$dbADO->Execute('
					DELETE InfraredGroup_Command_Preferred 
					FROM InfraredGroup_Command_Preferred
					INNER JOIN InfraredGroup_Command ON FK_InfraredGroup_Command=PK_InfraredGroup_Command
					WHERE PK_InfraredGroup_Command=?',$irg_c);
				
				$dbADO->Execute('DELETE FROM InfraredGroup_Command WHERE PK_InfraredGroup_Command=?',$irg_c);
				$dbADO->Execute('DELETE FROM InfraredGroup_Command_Preferred WHERE FK_InfraredGroup_Command=?',$irg_c);
				
				header("Location: index.php?section=irCodes&from=$from&dtID=$dtID&deviceID=$deviceID&infraredGroupID=$infraredGroupID&msg=Custom code deleted.&label=".$GLOBALS['label']);
				exit();
			}
		}
		
		$commandsDisplayed=explode(',',$_POST['commandsDisplayed']);
		foreach ($commandsDisplayed AS $commandID){
			$sufix=($infraredGroupID>0)?$infraredGroupID:'';
			$preferredCommand=((int)@$_POST['prefered_'.$commandID.'_'.$sufix]!=0)?(int)@$_POST['prefered_'.$commandID.'_'.$sufix]:(int)@$_POST['prefered_'.$commandID.'_'];
			if($preferredCommand>0){
				if(isset($GLOBALS['igcPrefered'][$commandID]) && $GLOBALS['igcPrefered'][$commandID]!=$preferredCommand){
					$dbADO->Execute('DELETE FROM InfraredGroup_Command_Preferred WHERE FK_InfraredGroup_Command=? AND FK_Installation=?',array($preferredCommand,$installationID));
					$dbADO->Execute('UPDATE InfraredGroup_Command_Preferred SET FK_InfraredGroup_Command=? WHERE FK_InfraredGroup_Command=? AND FK_Installation=?',array($preferredCommand,$GLOBALS['igcPrefered'][$commandID],$installationID));
				}elseif(!isset($GLOBALS['igcPrefered'][$commandID])){
					$dbADO->Execute('INSERT IGNORE INTO InfraredGroup_Command_Preferred (FK_InfraredGroup_Command,FK_Installation) VALUES (?,?)',array($preferredCommand,$installationID));
				}
			}
		}
		
		if(isset($_POST['update'])){
			$deviceCGArray=explode(',',$_POST['deviceCG']);
			$oldCheckedDCG=explode(',',$_POST['oldCheckedDCG']);
			foreach ($deviceCGArray AS $deviceCG){
				if(isset($_POST['dcg_'.$deviceCG])){
					// insert commands in IRG_C
					$res=$dbADO->Execute('
						SELECT DeviceCommandGroup_Command.FK_Command,FK_InfraredGroup,PK_InfraredGroup_Command
						FROM DeviceCommandGroup_Command 
						LEFT JOIN InfraredGroup_Command ON InfraredGroup_Command.FK_Command=DeviceCommandGroup_Command.FK_Command AND (FK_InfraredGroup=? OR FK_InfraredGroup IS NULL)
						WHERE FK_DeviceCommandGroup=?',array($infraredGroupID,$deviceCG));

					while($rowC=$res->FetchRow()){
						if($rowC['PK_InfraredGroup_Command']==''){
							$infraredGroupID=($infraredGroupID==0)?NULL:$infraredGroupID;
							$dbADO->Execute('INSERT INTO InfraredGroup_Command (FK_InfraredGroup,FK_Command,FK_Device,FK_DeviceTemplate,IRData,FK_Users) VALUES (?,?,?,?,?,?)',array(NULL,$rowC['FK_Command'],$deviceID,$dtID,'',$_SESSION['userID']));
						}
					}
				if(!in_array($deviceCG,$oldCheckedDCG))
					$dbADO->Execute('INSERT INTO DeviceTemplate_DeviceCommandGroup (FK_DeviceTemplate, FK_DeviceCommandGroup) VALUES (?,?)',array($dtID,$deviceCG));

				}elseif(!isset($_POST['dcg_'.$deviceCG]) && in_array($deviceCG,$oldCheckedDCG)){
					// delete from IRG_C
					$res=$dbADO->Execute('
						SELECT DeviceCommandGroup_Command.FK_Command,FK_InfraredGroup,PK_InfraredGroup_Command
						FROM DeviceCommandGroup_Command 
						LEFT JOIN InfraredGroup_Command ON InfraredGroup_Command.FK_Command=DeviceCommandGroup_Command.FK_Command AND (FK_InfraredGroup=? OR FK_InfraredGroup IS NULL)
						WHERE FK_DeviceCommandGroup=?',array($infraredGroupID,$deviceCG));

					while($rowC=$res->FetchRow()){
						if($rowC['PK_InfraredGroup_Command']!=''){
							$dbADO->Execute('DELETE FROM InfraredGroup_Command WHERE PK_InfraredGroup_Command=? AND FK_Users=?',array($rowC['PK_InfraredGroup_Command'],$_SESSION['userID']));
							$dbADO->Execute('DELETE FROM InfraredGroup_Command_Preferred WHERE FK_InfraredGroup_Command=?',array($rowC['PK_InfraredGroup_Command']));
						}
					}
					$dbADO->Execute('DELETE FROM DeviceTemplate_DeviceCommandGroup WHERE FK_DeviceTemplate=? AND FK_DeviceCommandGroup=?',array($dtID,$deviceCG));
				}
			}
			
			$customCodesNoArray=explode(',',@$_POST['displayedIRGC']);
			foreach ($customCodesNoArray as $ig_c){
				$irData=(isset($_POST['isHidden_'.$ig_c]))?stripslashes(urldecode(@$_POST['irData_'.$ig_c])):stripslashes(@$_POST['irData_'.$ig_c]);
				$dbADO->Execute('UPDATE InfraredGroup_Command SET IRData=? WHERE PK_InfraredGroup_Command=?',array($irData,$ig_c));
			}

			header("Location: index.php?section=irCodes&from=$from&deviceID=$deviceID&dtID=$dtID&infraredGroupID=$infraredGroupID&msg=IR/GSD codes updated.&label=".$GLOBALS['label']);
			exit();
		}

		header("Location: index.php?section=irCodes&from=$from&dtID=$dtID&deviceID=$deviceID&infraredGroupID=$infraredGroupID&lastAction=".@$lastAction);
	}

	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: IR/GSD Codes');
	$output->output();

}

function showCodes($commandsToShow,$infraredGroupID,$deviceID,$dtID,$dbADO)
{
//	$dbADO->debug=true;
	$installationID=(int)$_SESSION['installationID'];
	$out='';
	if(count($commandsToShow)==0)
		return $out;

	foreach ($commandsToShow AS $commandID){
		if($infraredGroupID==0){
			$queryStandardCode='
				SELECT Command.Description, FK_Command, IRData, PK_InfraredGroup_Command, FK_DeviceTemplate, FK_InfraredGroup , FK_Device, PK_InfraredGroup_Command, FK_Users, PK_Command 
				FROM Command 
				LEFT JOIN InfraredGroup_Command ON FK_Command=PK_Command AND FK_DeviceTemplate IS NULL AND FK_InfraredGroup IS NULL
				WHERE PK_Command =?
				ORDER BY Command.Description ASC';
			$resStandardCode=$dbADO->Execute($queryStandardCode,$commandID);
			
			$queryUserCode='
				SELECT Command.Description, FK_Command, IRData, PK_InfraredGroup_Command, FK_DeviceTemplate, FK_InfraredGroup , FK_Device, PK_InfraredGroup_Command, FK_Users, PK_Command 
				FROM Command 
				LEFT JOIN InfraredGroup_Command ON FK_Command=PK_Command AND FK_DeviceTemplate = ? AND FK_InfraredGroup IS NULL
				WHERE PK_Command =? AND PK_InfraredGroup_Command IS NOT NULL
				ORDER BY Command.Description ASC';
			$resUserCode=$dbADO->Execute($queryUserCode,array($dtID,$commandID));
		}else{
			$queryStandardCode='
				SELECT Command.Description, FK_Command, IRData, PK_InfraredGroup_Command, FK_DeviceTemplate, FK_InfraredGroup , FK_Device, PK_InfraredGroup_Command, FK_Users, PK_Command 
				FROM Command 
				LEFT JOIN InfraredGroup_Command ON FK_Command=PK_Command AND FK_DeviceTemplate IS NULL AND FK_InfraredGroup=?
				WHERE PK_Command =?
				ORDER BY Command.Description ASC';
			$resStandardCode=$dbADO->Execute($queryStandardCode,array($infraredGroupID,$commandID));
			
			$queryUserCode='
				SELECT Command.Description, FK_Command, IRData, PK_InfraredGroup_Command, FK_DeviceTemplate, FK_InfraredGroup , FK_Device, PK_InfraredGroup_Command, FK_Users, PK_Command 
				FROM Command 
				LEFT JOIN InfraredGroup_Command ON FK_Command=PK_Command AND FK_DeviceTemplate=? AND (FK_InfraredGroup=? OR FK_InfraredGroup IS NULL)
				WHERE PK_Command =? AND PK_InfraredGroup_Command IS NOT NULL
				ORDER BY Command.Description ASC';
			$resUserCode=$dbADO->Execute($queryUserCode,array($dtID,$infraredGroupID,$commandID));

		}
		$out.='<table width="100%">';
		$existingCodes=$resStandardCode->RecordCount()+$resUserCode->RecordCount();
		while($rowStandardCode=$resStandardCode->FetchRow()){
			if(is_null($deviceID)){
				$GLOBALS['displayedIRGC'][]=$rowStandardCode['PK_InfraredGroup_Command'];
			}
			if(is_null($deviceID)){
				$textAreaElem='<textarea name="irData_'.$rowStandardCode['PK_InfraredGroup_Command'].'" rows="2" style="width:100%">'.$rowStandardCode['IRData'].'</textarea>';
			}else{
				$textAreaElem='
					<textarea name="ir" rows="2" style="width:100%" disabled>'.$rowStandardCode['IRData'].'</textarea>
					<input type="hidden" name="irData_'.$rowStandardCode['PK_InfraredGroup_Command'].'" value="'.urlencode(addslashes($rowStandardCode['IRData'])).'">
					<input type="hidden" name="isHidden_'.$rowStandardCode['PK_InfraredGroup_Command'].'" value="1">';
			}
			$out.='
				<tr bgcolor="lightblue">
					<td align="center" width="100"><B>'.$rowStandardCode['Description'].'</B> <br><input type="button" class="button" name="learnCode" value="New code" onClick="windowOpen(\'index.php?section='.(($GLOBALS['label']=='infrared')?'learnCode':'newRubyCode').'&deviceID='.$deviceID.'&dtID='.$dtID.'&infraredGroupID='.$infraredGroupID.'&commandID='.$rowStandardCode['PK_Command'].'&action=sendCommand\',\''.(($GLOBALS['label']=='infrared')?'width=600,height=250':'width=600,height=650').',toolbars=true,scrollbars=1,resizable=1\');"></td>
					<td width="25">'.((!is_null($rowStandardCode['PK_InfraredGroup_Command']))?'<input type="radio" name="prefered_'.$rowStandardCode['FK_Command'].'_'.$rowStandardCode['FK_InfraredGroup'].'" value="'.$rowStandardCode['PK_InfraredGroup_Command'].'" '.(($existingCodes==1 || in_array($rowStandardCode['PK_InfraredGroup_Command'],$GLOBALS['igcPrefered']))?'checked':'').'>':'&nbsp;').'</td>
					<td>'.((is_null($rowStandardCode['PK_InfraredGroup_Command']))?'<B> Not in database</B>':$textAreaElem).'</td>
					<td align="center" width="100">';
					if(!is_null($rowStandardCode['PK_InfraredGroup_Command']))
						$out.=((!is_null($rowStandardCode['PK_InfraredGroup_Command']))?'<input type="button" class="button" name="edit" value="Edit" onClick="windowOpen(\'index.php?section=editCode&ircode='.$rowStandardCode['PK_InfraredGroup_Command'].'\',\'width=800,height=600,toolbars=true,scrollbars=1,resizable=1\');"><br><input type="button" class="button" name="testCode" value="Test code" onClick="self.location=\'index.php?section=irCodes&from=avWizard&dtID='.$dtID.'&deviceID='.$deviceID.'&infraredGroupID='.$infraredGroupID.'&from=avWizard&action=testCode&owner='.$rowStandardCode['FK_Users'].'&ig_c='.$rowStandardCode['PK_InfraredGroup_Command'].'&irCode=\'+escape(document.irCodes.irData_'.$rowStandardCode['PK_InfraredGroup_Command'].'.value);"><a name="test_'.$rowStandardCode['PK_InfraredGroup_Command'].'"></a>':'').'
					</td>
				</tr>';
		}

		$customCodesNo=array();
		while($rowUserCode=$resUserCode->FetchRow()){
			$GLOBALS['displayedIRGC'][]=$rowUserCode['PK_InfraredGroup_Command'];
			$customCodesNo[]=$rowUserCode['PK_InfraredGroup_Command'];

			if($rowUserCode['FK_Users']==$_SESSION['userID']){
				$textareaUserCode='<textarea name="irData_'.$rowUserCode['PK_InfraredGroup_Command'].'" rows="2" style="width:100%" onClick="setPreferred(\'prefered_'.$rowUserCode['FK_Command'].'_'.(($infraredGroupID==0)?'':$infraredGroupID).'\','.$rowUserCode['PK_InfraredGroup_Command'].')">'.$rowUserCode['IRData'].'</textarea>';
			}else{
				$textareaUserCode='
					<textarea name="ir" rows="2" style="width:100%" disabled onClick="setPreferred(\'prefered_'.$rowUserCode['FK_Command'].'_'.(($infraredGroupID==0)?'':$infraredGroupID).'\','.$rowUserCode['PK_InfraredGroup_Command'].')">'.$rowUserCode['IRData'].'</textarea>
					<input type="hidden" name="irData_'.$rowUserCode['PK_InfraredGroup_Command'].'" value="'.urlencode(addslashes($rowUserCode['IRData'])).'">
					<input type="hidden" name="isHidden_'.$rowUserCode['PK_InfraredGroup_Command'].'" value="1">';
			}

			$RowColor=(($rowUserCode['FK_Users']==$_SESSION['userID'])?'yellow':'lightyellow');
			$out.='
				<tr bgcolor="'.$RowColor.'">
					<td align="center" width="100"><B>'.$rowUserCode['Description'].'</B><br> User: '.$rowUserCode['FK_Users'].'</td>
					<td width="25"><input type="radio" name="prefered_'.$rowUserCode['FK_Command'].'_'.$rowUserCode['FK_InfraredGroup'].'" value="'.$rowUserCode['PK_InfraredGroup_Command'].'" '.((in_array($rowUserCode['PK_InfraredGroup_Command'],$GLOBALS['igcPrefered']))?'checked':'').'></td>
					<td>'.$textareaUserCode.'</td>
					<td align="center" width="100">
						<input type="button" class="button" name="edit" value="Edit" onClick="windowOpen(\'index.php?section=editCode&ircode='.$rowUserCode['PK_InfraredGroup_Command'].'\',\'width=800,height=600,toolbars=true,scrollbars=1,resizable=1\');"><br>';
					if($rowUserCode['FK_Users']==$_SESSION['userID']){
						$out.='<input type="button" class="button" name="delCustomCode" value="Delete code" onClick="if(confirm(\'Are you sure you want to delete this code?\')){document.irCodes.action.value=\'delete\';document.irCodes.irgroup_command.value='.$rowUserCode['PK_InfraredGroup_Command'].';document.irCodes.submit();}"> <br>';
					}
					$out.='
						<input type="button" class="button" name="testCode" value="Test code" onClick="self.location=\'index.php?section=irCodes&from=avWizard&dtID='.$dtID.'&deviceID='.$deviceID.'&infraredGroupID='.$infraredGroupID.'&from=avWizard&action=testCode&owner='.$rowUserCode['FK_Users'].'&ig_c='.$rowUserCode['PK_InfraredGroup_Command'].'&irCode=\'+escape(document.irCodes.irData_'.$rowUserCode['PK_InfraredGroup_Command'].'.value);"> <a name="test_'.$rowUserCode['PK_InfraredGroup_Command'].'"></a>
					</td>
				</tr>';
		}
		
		$out.='</table>';	
	}

	return $out;
}
?>