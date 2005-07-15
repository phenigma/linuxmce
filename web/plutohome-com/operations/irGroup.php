<?
function irGroup($output)
{
	global $dbPlutoAdminServer,$dbPlutoAdminUser,$dbPlutoAdminPass,$dbPlutoAdminDatabase;
	include_once('include/plutoAdminUtils.inc.php');

	$publicADO = &ADONewConnection('mysql');
	$publicADO->NConnect($dbPlutoAdminServer,urlencode($dbPlutoAdminUser),urlencode($dbPlutoAdminPass),urlencode($dbPlutoAdminDatabase));
	
	/* @var $publicADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$publicADO->debug=false;
	$installationID = 0;
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	$label=(isset($_REQUEST['label']))?$_REQUEST['label']:'infrared';
	$GLOBALS['label']=$label;
	
	$irgID = (int)$_REQUEST['irgID'];

	$userID = (int)@$_SESSION['userID'];
	if ($irgID==0) {
		header("Location: index.php?section=login");
	}

	$GLOBALS['displayedIRGC']=array();
	$GLOBALS['preferredIGC']=array();
	$GLOBALS['igcPrefered']=array();	

	if ($action=='form') {
		$out.='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		
		</script>	
	<div class="err"><br>'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
	<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>	
		<form action="index.php" method="POST" name="irGroup">
			<input type="hidden" name="section" value="irGroup">
			<input type="hidden" name="action" value="update">
			<input type="hidden" name="irgID" value="'.$irgID.'">
			<input type="hidden" name="irgroup_command" value="">
			<input type="hidden" name="label" value="'.$label.'">';

	
		$out.='
		<h3>Edit IR Group</h3>
		<table border="0" width="100%" class="normaltext">
		';

		$commandsDisplayed=array();
		$queryCommands='
			SELECT DISTINCT FK_Command
			FROM InfraredGroup_Command
			WHERE FK_InfraredGroup =?';
		$resCommand=$publicADO->Execute($queryCommands,array($irgID));
		$commandsToShow=array();
		while($rowCommands=$resCommand->FetchRow()){
			$commandsToShow[]=$rowCommands['FK_Command'];
			$commandsDisplayed[]=$rowCommands['FK_Command'];
		}
		if(count($commandsToShow)>0)
			$otherCommands='
			<tr>
				<td colspan="3" align="center"><fieldset style="width:98%"><legend><B>IR Commands</B></legend>'.showCodes($commandsToShow,$irgID,NULL,NULL,$publicADO).'</fieldset></td>
			</tr>	
			';
		$out.='
			'.@$otherCommands.'
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
			</tr>';
		if(count($commandsDisplayed)>0)
			$out.='
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
		if($action=='testCode'){
			$irCode=stripslashes($_REQUEST['irCode']);
			$ig_c=(int)$_REQUEST['ig_c'];
			$owner=(int)$_REQUEST['owner'];
			if($owner==$userID){
				$publicADO->Execute('UPDATE InfraredGroup_Command SET IRData=? WHERE PK_InfraredGroup_Command=?',array($irCode,$ig_c));
			}
			$commandToTest='/usr/pluto/bin/MessageSend localhost 0 '.$deviceID.' 1 191 70 "'.$irCode.'"';
			exec($commandToTest);
			header("Location: index.php?section=irGroup&from=$from&dtID=$dtID&deviceID=$deviceID&infraredGroupID=$irgID&msg=The command was sent.&label=".$GLOBALS['label']);
			exit();
		}

		if(!is_null($deviceID)){
			$newIRGroup=((int)@$_POST['irGroup']>0)?(int)$_POST['irGroup']:NULL;
			$oldIRGroup=(int)$_POST['oldIRGroup'];
			if($newIRGroup!=$oldIRGroup){
				$publicADO->Execute('DELETE FROM DeviceTemplate_InfraredGroup WHERE FK_DeviceTemplate=?',$dtID);
				if(!is_null($newIRGroup))
					$publicADO->Execute('INSERT INTO DeviceTemplate_InfraredGroup (FK_DeviceTemplate, FK_InfraredGroup) VALUES (?,?)',array($dtID,$newIRGroup));
				$publicADO->Execute('UPDATE InfraredGroup_Command SET FK_InfraredGroup=? WHERE FK_DeviceTemplate=? AND FK_InfraredGroup IS NOT NULL',array($newIRGroup,$dtID));
				header("Location: index.php?section=irGroup&from=$from&dtID=$dtID&deviceID=$deviceID&infraredGroupID=$newIRGroup&msg=IR Group changed for selected device template.&label=".$GLOBALS['label']);
				exit();
			}
		}
		
		if(isset($_POST['irgroup_command']) && (int)$_POST['irgroup_command']>0){
			$irg_c=(int)$_POST['irgroup_command'];
			if($action!='delete'){
				$publicADO->Execute('INSERT INTO InfraredGroup_Command (FK_InfraredGroup,FK_Command,FK_Device,FK_DeviceTemplate,IRData,FK_Users) SELECT FK_InfraredGroup,FK_Command,'.$deviceID.','.$dtID.',IRData,'.$_SESSION['userID'].' FROM InfraredGroup_Command WHERE PK_InfraredGroup_Command=?',$irg_c);

				header("Location: index.php?section=irGroup&from=$from&dtID=$dtID&deviceID=$deviceID&infraredGroupID=$irgID&msg=Custom code added.&label=".$GLOBALS['label']);
				exit();
			}else{
				$publicADO->Execute('
					DELETE InfraredGroup_Command_Preferred 
					FROM InfraredGroup_Command_Preferred
					INNER JOIN InfraredGroup_Command ON FK_InfraredGroup_Command=PK_InfraredGroup_Command
					WHERE PK_InfraredGroup_Command=?',$irg_c);
				
				$publicADO->Execute('DELETE FROM InfraredGroup_Command WHERE PK_InfraredGroup_Command=?',$irg_c);
				$publicADO->Execute('DELETE FROM InfraredGroup_Command_Preferred WHERE FK_InfraredGroup_Command=?',$irg_c);
				
				header("Location: index.php?section=irGroup&from=$from&dtID=$dtID&deviceID=$deviceID&infraredGroupID=$irgID&msg=Custom code deleted.&label=".$GLOBALS['label']);
				exit();
			}
		}
		
		$commandsDisplayed=explode(',',$_POST['commandsDisplayed']);
		foreach ($commandsDisplayed AS $commandID){
			$sufix=($irgID>0)?$irgID:'';
			$preferredCommand=(int)@$_POST['prefered_'.$commandID.'_'.$sufix];
			if($preferredCommand>0){
				if(isset($GLOBALS['igcPrefered'][$commandID]) && $GLOBALS['igcPrefered'][$commandID]!=$preferredCommand){
					$publicADO->Execute('DELETE FROM InfraredGroup_Command_Preferred WHERE FK_InfraredGroup_Command=? AND FK_Installation=?',array($preferredCommand,$installationID));
					$publicADO->Execute('UPDATE InfraredGroup_Command_Preferred SET FK_InfraredGroup_Command=? WHERE FK_InfraredGroup_Command=? AND FK_Installation=?',array($preferredCommand,$GLOBALS['igcPrefered'][$commandID],$installationID));
				}elseif(!isset($GLOBALS['igcPrefered'][$commandID])){
					$publicADO->Execute('INSERT IGNORE INTO InfraredGroup_Command_Preferred (FK_InfraredGroup_Command,FK_Installation) VALUES (?,?)',array($preferredCommand,$installationID));
				}
			}
		}
		
		if(isset($_POST['update'])){
			$deviceCGArray=explode(',',$_POST['deviceCG']);
			$oldCheckedDCG=explode(',',$_POST['oldCheckedDCG']);
			foreach ($deviceCGArray AS $deviceCG){
				if(isset($_POST['dcg_'.$deviceCG])){
					// insert commands in IRG_C
					$res=$publicADO->Execute('
						SELECT DeviceCommandGroup_Command.FK_Command,FK_InfraredGroup,PK_InfraredGroup_Command
						FROM DeviceCommandGroup_Command 
						LEFT JOIN InfraredGroup_Command ON InfraredGroup_Command.FK_Command=DeviceCommandGroup_Command.FK_Command AND (FK_InfraredGroup=? OR FK_InfraredGroup IS NULL)
						WHERE FK_DeviceCommandGroup=?',array($irgID,$deviceCG));

					while($rowC=$res->FetchRow()){
						if($rowC['PK_InfraredGroup_Command']==''){
							$irgID=($irgID==0)?NULL:$irgID;
							$publicADO->Execute('INSERT INTO InfraredGroup_Command (FK_InfraredGroup,FK_Command,FK_Device,FK_DeviceTemplate,IRData,FK_Users) VALUES (?,?,?,?,?,?)',array(NULL,$rowC['FK_Command'],$deviceID,$dtID,'',$_SESSION['userID']));
						}
					}
				if(!in_array($deviceCG,$oldCheckedDCG))
					$publicADO->Execute('INSERT INTO DeviceTemplate_DeviceCommandGroup (FK_DeviceTemplate, FK_DeviceCommandGroup) VALUES (?,?)',array($dtID,$deviceCG));

				}elseif(!isset($_POST['dcg_'.$deviceCG]) && in_array($deviceCG,$oldCheckedDCG)){
					// delete from IRG_C
					$res=$publicADO->Execute('
						SELECT DeviceCommandGroup_Command.FK_Command,FK_InfraredGroup,PK_InfraredGroup_Command
						FROM DeviceCommandGroup_Command 
						LEFT JOIN InfraredGroup_Command ON InfraredGroup_Command.FK_Command=DeviceCommandGroup_Command.FK_Command AND (FK_InfraredGroup=? OR FK_InfraredGroup IS NULL)
						WHERE FK_DeviceCommandGroup=?',array($irgID,$deviceCG));

					while($rowC=$res->FetchRow()){
						if($rowC['PK_InfraredGroup_Command']!=''){
							$publicADO->Execute('DELETE FROM InfraredGroup_Command WHERE PK_InfraredGroup_Command=? AND FK_Users=?',array($rowC['PK_InfraredGroup_Command'],$_SESSION['userID']));
							$publicADO->Execute('DELETE FROM InfraredGroup_Command_Preferred WHERE FK_InfraredGroup_Command=?',array($rowC['PK_InfraredGroup_Command']));
						}
					}
					$publicADO->Execute('DELETE FROM DeviceTemplate_DeviceCommandGroup WHERE FK_DeviceTemplate=? AND FK_DeviceCommandGroup=?',array($dtID,$deviceCG));
				}
			}
			
			$customCodesNoArray=explode(',',@$_POST['displayedIRGC']);
			foreach ($customCodesNoArray as $ig_c){
				$irData=(isset($_POST['isHidden_'.$ig_c]))?stripslashes(urldecode(@$_POST['irData_'.$ig_c])):stripslashes(@$_POST['irData_'.$ig_c]);
				$publicADO->Execute('UPDATE InfraredGroup_Command SET IRData=? WHERE PK_InfraredGroup_Command=?',array($irData,$ig_c));
			}

			header("Location: index.php?section=irGroup&from=$from&deviceID=$deviceID&dtID=$dtID&infraredGroupID=$irgID&msg=IR codes updated.&label=".$GLOBALS['label']);
			exit();
		}

		header("Location: index.php?section=irGroup&from=$from&dtID=$dtID&deviceID=$deviceID&infraredGroupID=$irgID&lastAction=".@$lastAction);
	}

	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: IR Codes');
	$output->output();
}

function showCodes($commandsToShow,$irgID,$deviceID,$dtID,$publicADO)
{
//	$publicADO->debug=true;
	$installationID=0;
	$out='';
	if(count($commandsToShow)==0)
		return $out;

	foreach ($commandsToShow AS $commandID){
		if($irgID==0){
			$queryStandardCode='
				SELECT Command.Description, FK_Command, IRData, PK_InfraredGroup_Command, FK_DeviceTemplate, FK_InfraredGroup , FK_Device, PK_InfraredGroup_Command, FK_Users, PK_Command 
				FROM Command 
				LEFT JOIN InfraredGroup_Command ON FK_Command=PK_Command AND FK_DeviceTemplate IS NULL AND FK_InfraredGroup IS NULL
				WHERE PK_Command =?
				ORDER BY Command.Description ASC';
			$resStandardCode=$publicADO->Execute($queryStandardCode,$commandID);
			
			$queryUserCode='
				SELECT Command.Description, FK_Command, IRData, PK_InfraredGroup_Command, FK_DeviceTemplate, FK_InfraredGroup , FK_Device, PK_InfraredGroup_Command, FK_Users, PK_Command 
				FROM Command 
				LEFT JOIN InfraredGroup_Command ON FK_Command=PK_Command AND FK_DeviceTemplate = ? AND FK_InfraredGroup IS NULL
				WHERE PK_Command =? AND PK_InfraredGroup_Command IS NOT NULL
				ORDER BY Command.Description ASC';
			$resUserCode=$publicADO->Execute($queryUserCode,array($dtID,$commandID));
		}else{
			$queryStandardCode='
				SELECT Command.Description, FK_Command, IRData, PK_InfraredGroup_Command, FK_DeviceTemplate, FK_InfraredGroup , FK_Device, PK_InfraredGroup_Command, FK_Users, PK_Command 
				FROM Command 
				LEFT JOIN InfraredGroup_Command ON FK_Command=PK_Command AND FK_DeviceTemplate IS NULL AND FK_InfraredGroup=?
				WHERE PK_Command =?
				ORDER BY Command.Description ASC';
			$resStandardCode=$publicADO->Execute($queryStandardCode,array($irgID,$commandID));
			
			$queryUserCode='
				SELECT Command.Description, FK_Command, IRData, PK_InfraredGroup_Command, FK_DeviceTemplate, FK_InfraredGroup , FK_Device, PK_InfraredGroup_Command, FK_Users, PK_Command 
				FROM Command 
				LEFT JOIN InfraredGroup_Command ON FK_Command=PK_Command AND (FK_InfraredGroup=? OR FK_InfraredGroup IS NULL)
				WHERE PK_Command =? AND PK_InfraredGroup_Command IS NOT NULL
				ORDER BY Command.Description ASC';
			$resUserCode=$publicADO->Execute($queryUserCode,array($irgID,$commandID));

		}
		$out.='<table width="100%" cellpadding="3" cellspacing="2">
				<tr bgcolor="lightblue">
					<td align="center" width="100"><B>Command</B></td>
					<td width="25"><b>DT</b></td>
					<td><B>IR Code</B></td>
					<td align="center" width="100"></td>
				</tr>';
		$existingCodes=$resStandardCode->RecordCount()+$resUserCode->RecordCount();
		while($rowStandardCode=$resStandardCode->FetchRow()){
			if(is_null($deviceID)){
				$GLOBALS['displayedIRGC'][]=$rowStandardCode['PK_InfraredGroup_Command'];
			}
			if(is_null($deviceID)){
				$textAreaElem='<textarea name="irData_'.$rowStandardCode['PK_InfraredGroup_Command'].'" rows="2" style="width:100%">'.$rowStandardCode['IRData'].'</textarea>';
			}else{
				$textAreaElem='
					<textarea name="ir" rows="2" style="width:100%" READONLY>'.$rowStandardCode['IRData'].'</textarea>
					<input type="hidden" name="irData_'.$rowStandardCode['PK_InfraredGroup_Command'].'" value="'.urlencode(addslashes($rowStandardCode['IRData'])).'">
					<input type="hidden" name="isHidden_'.$rowStandardCode['PK_InfraredGroup_Command'].'" value="1">';
			}
			$out.='
				<tr bgcolor="lightblue">
					<td align="center" width="100"><B>'.$rowStandardCode['Description'].'</B></td>
					<td width="25">&nbsp;</td>
					<td>'.((is_null($rowStandardCode['PK_InfraredGroup_Command']))?'<B> Not in database</B>':$textAreaElem).'</td>
					<td align="center" width="100"></td>
				</tr>';
		}

		$customCodesNo=array();
		while($rowUserCode=$resUserCode->FetchRow()){
			$GLOBALS['displayedIRGC'][]=$rowUserCode['PK_InfraredGroup_Command'];
			$customCodesNo[]=$rowUserCode['PK_InfraredGroup_Command'];

			if($rowUserCode['FK_Users']==@$_SESSION['userID']){
				$textareaUserCode='<textarea name="irData_'.$rowUserCode['PK_InfraredGroup_Command'].'" rows="2" style="width:100%" onClick="setPreferred(\'prefered_'.$rowUserCode['FK_Command'].'_'.(($irgID==0)?'':$irgID).'\','.$rowUserCode['PK_InfraredGroup_Command'].')">'.$rowUserCode['IRData'].'</textarea>';
			}else{
				$textareaUserCode='
					<textarea name="ir" rows="2" style="width:100%" READONLY  onClick="setPreferred(\'prefered_'.$rowUserCode['FK_Command'].'_'.(($irgID==0)?'':$irgID).'\','.$rowUserCode['PK_InfraredGroup_Command'].')">'.$rowUserCode['IRData'].'</textarea>
					<input type="hidden" name="irData_'.$rowUserCode['PK_InfraredGroup_Command'].'" value="'.urlencode(addslashes($rowUserCode['IRData'])).'">
					<input type="hidden" name="isHidden_'.$rowUserCode['PK_InfraredGroup_Command'].'" value="1">';
			}

			$RowColor=(isset($_SESSION['userID']) && $rowUserCode['FK_Users']==@$_SESSION['userID'])?'yellow':'lightyellow';
			$out.='
				<tr bgcolor="'.$RowColor.'">
					<td align="center" width="100"><B>'.$rowUserCode['Description'].'</B><br> User: '.$rowUserCode['FK_Users'].'</td>
					<td width="25"><a href="index.php?section=editAVDevice&deviceID='.$rowUserCode['FK_DeviceTemplate'].'">'.$rowUserCode['FK_DeviceTemplate'].'</a></td>
					<td>'.$textareaUserCode.'</td>
					<td align="center" width="100">';
					if(isset($_SESSION['userID']) && $rowUserCode['FK_Users']==@$_SESSION['userID']){
						$out.='<input type="button" class="button" name="delCustomCode" value="Delete code" onClick="if(confirm(\'Are you sure you want to delete this code?\')){document.irGroup.action.value=\'delete\';document.irGroup.irgroup_command.value='.$rowUserCode['PK_InfraredGroup_Command'].';document.irGroup.submit();}"> <br>';
					}
					$out.='
					</td>
				</tr>';
		}
		
		$out.='</table>';	
	}

	return $out;
}
?>
