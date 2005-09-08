<?
function irCodes($output)
{
	$time_start = getmicrotime();
	
	global $dbPlutoAdminServer,$dbPlutoAdminUser,$dbPlutoAdminPass,$dbPlutoAdminDatabase;
	global $inputCommandsArray, $dspmodeCommandsArray,$powerCommands;
	include_once('include/plutoAdminUtils.inc.php');

	$publicADO = &ADONewConnection('mysql');
	$publicADO->NConnect($dbPlutoAdminServer,urlencode($dbPlutoAdminUser),urlencode($dbPlutoAdminPass),urlencode($dbPlutoAdminDatabase));
	
	/* @var $publicADO ADOConnection */
	/* @var $rs ADORecordSet */
	/* @var $resDTData ADORecordSet */
	
	$out='';
//	$publicADO->debug=true;;
	$installationID = 0;
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	$label=(isset($_REQUEST['label']))?$_REQUEST['label']:'infrared';
	$GLOBALS['label']=$label;
	
	$dtID = (int)$_REQUEST['dtID'];
	$deviceID = (isset($_REQUEST['deviceID']) && (int)@$_REQUEST['deviceID']!=0)?(int)$_REQUEST['deviceID']:NULL;
	$userID = (int)@$_SESSION['userID'];
	if ($dtID==0) {
		header("Location: index.php?section=login");
	}

	if(!isset($_REQUEST['infraredGroupID'])){
		$resDefaultIG=$publicADO->Execute('SELECT FK_InfraredGroup FROM DeviceTemplate WHERE PK_DeviceTemplate=?',$dtID);
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
	$GLOBALS['displayedCommands']=array();

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
			SELECT 
				DeviceTemplate.Description AS Template, 
				DeviceCategory.Description AS Category,
				Manufacturer.Description AS Manufacturer, 
				FK_Manufacturer,
				FK_DeviceCategory,
				DeviceTemplate.psc_user AS User,
				FK_InfraredGroup,
				DeviceTemplate_AV.*,
				DeviceTemplate_AV.FK_DeviceTemplate AS AVTemplate
			FROM DeviceTemplate
			INNER JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory
			INNER JOIN Manufacturer ON FK_Manufacturer=PK_Manufacturer
			LEFT JOIN DeviceTemplate_AV ON DeviceTemplate_AV.FK_DeviceTemplate=PK_DeviceTemplate
			WHERE PK_DeviceTemplate=?';
		$resDTData=$publicADO->Execute($selectDTData,$dtID);
		if($resDTData->RecordCount()==0){
			header("Location: index.php?section=userHome");
			exit();
		}
		$rowDTData=$resDTData->FetchRow();
		$togglePower=(@$rowDTData['TogglePower']==1)?1:0;
		$toggleInput=(@$rowDTData['ToggleInput']==1)?1:0;
		$toggleDSP=(@$rowDTData['ToggleDSP']==1)?1:0;
		
		// create the record in DeviceTemplate_AV for templates who doesn't have it
		if(is_null($rowDTData['AVTemplate'][0])){
			$publicADO->Execute('INSERT INTO DeviceTemplate_AV (FK_DeviceTemplate) VALUES (?)',$dtID);
		}		
		
		$manufacturerID=$rowDTData['FK_Manufacturer'];
		$deviceCategoryID=$rowDTData['FK_DeviceCategory'];
		$owner=$rowDTData['User'];

		$GLOBALS['btnEnabled']=(!isset($_SESSION['userID']) || $owner!=@$_SESSION['userID'] )?'disabled':'';

		$selectedIRGrops=array();
		$selectedIRGrops[]=$rowDTData['FK_InfraredGroup'];

		$inputCommandsArray=getAssocArray('Command','PK_Command','Command.Description',$publicADO,'INNER JOIN DeviceTemplate_Input ON FK_Command=PK_Command WHERE FK_DeviceTemplate='.$dtID,'ORDER BY Command.Description ASC');
		$dspmodeCommandsArray=getAssocArray('Command','PK_Command','Command.Description',$publicADO,'INNER JOIN DeviceTemplate_DSPMode ON FK_Command=PK_Command WHERE FK_DeviceTemplate='.$dtID,'ORDER BY Command.Description ASC');
		$powerCommands=array($GLOBALS['genericONCommand']=>'ON',$GLOBALS['genericOFFCommand']=>'OFF');
		
		$out.='
		<input type="hidden" name="oldIRGroup" value="'.@$infraredGroupID.'">
		<h3>Edit '.$label.' codes</h3>
		<table border="0" width="100%" class="normaltext">
			<tr>
				<td class="err" colspan="2">WARNING: the changes will afect all devices created from <B>'.$rowDTData['Template'].'</B> template.</td>
			</tr>
			<tr>
				<td valign="top" colspan="2">Device template <B>'.$rowDTData['Template'].'</B>, category <B>'.$rowDTData['Category'].'</B> and manufacturer <B>'.$rowDTData['Manufacturer'].'</B>.<td>
			</tr>
			<tr>
				<td valign="top" colspan="2">Delays: Power: <B>'.$rowDTData['IR_PowerDelay'].'</B> seconds, Mode: <B>'.$rowDTData['IR_ModeDelay'].'</B> seconds, Other: <B>'.round(($rowDTData['DigitDelay']/1000),3).'</B> seconds  <a href="index.php?section=addModel&step=2&dtID='.$dtID.'&return=1">[change/explain]</a><td>
			</tr>
			<tr>
				<td valign="top" colspan="2">Tuning: <B>'.((str_replace('E','',$rowDTData['NumericEntry'])=='')?'No fixed digits':'Fixed Digits: '.str_replace('E','',$rowDTData['NumericEntry'])).'</B>  ['.((strpos($rowDTData['NumericEntry'],'E')!==false)?'x':'').'] terminate with enter  <a href="index.php?section=addModel&step=3&dtID='.$dtID.'&return=1">[change/explain]</a><td>
			</tr>		
			<tr>
				<td valign="top" colspan="2">Power: <B>'.(($rowDTData['TogglePower']==0)?'Discrete':'Toggle').'</B> <a href="index.php?section=addModel&step=4&dtID='.$dtID.'&return=1">[change/explain]</a><td>
			</tr>		
			<tr>
				<td valign="top" colspan="2">Inputs: <B>'.join(', ',$inputCommandsArray).'</B> <a href="index.php?section=addModel&step=5&dtID='.$dtID.'&return=1">[change/explain]</a><td>
			</tr>		
			<tr>
				<td valign="top" colspan="2">DSP Modes: <B>'.join(', ',$dspmodeCommandsArray).'</B> <a href="index.php?section=addModel&step=6&dtID='.$dtID.'&return=1">[change/explain]</a><td>
			</tr>		
		';
			$out.='
			<tr>
				<td colspan="2">Uses Group/Codeset <select name="irGroup" onChange="document.irCodes.submit();">
					<option value="0" '.(($infraredGroupID==0)?'selected':'').'>I don\'t know the group</option>';
			$queryIG='
				SELECT PK_InfraredGroup,InfraredGroup.Description 
				FROM InfraredGroup 
				WHERE FK_Manufacturer=? AND FK_DeviceCategory=?
				ORDER BY Description ASC';
			$resIG=$publicADO->Execute($queryIG,array($manufacturerID,$deviceCategoryID));
			while($rowIG=$resIG->FetchRow()){
				$out.='<option value="'.$rowIG['PK_InfraredGroup'].'" '.(($rowIG['PK_InfraredGroup']==$infraredGroupID)?'selected':'').'>'.$rowIG['Description'].'</option>';
			}
			$out.='
				</select> <input type="button" class="button" name="step7" value="Help me choose" onclick="self.location=\'index.php?section=addModel&step=7&dtID='.$dtID.'&deviceID='.$deviceID.'\'"></td>
		</tr>';
			
		$out.='
			<tr>
				<td valign="top" width="250">Implement Command Groups </td>
				<td>'.DeviceCommandGroupTable($dtID,$deviceCategoryID,$publicADO).'</td>
		</tr>
		';
		
		$out.='		
			<tr>
				<td colspan="3" align="center"><input type="button" class="button" name="button" value="Add/Remove commands" '.$GLOBALS['btnEnabled'].' onClick="windowOpen(\'index.php?section=infraredCommands&infraredGroup='.$infraredGroupID.'&deviceID='.$deviceID.'&dtID='.$dtID.(($GLOBALS['label']!='infrared')?'&rootNode=1':'').'\',\'width=800,height=600,toolbars=true,scrollbars=1,resizable=1\');"> <input type="submit" class="button" name="update" value="Update" '.((!isset($_SESSION['userID']))?'disabled':'').'></td>
			</tr>';

			// extract data from InfraredGroup_Command an put it in multi-dimmensional array
			$codesArray=extractCodesTree($infraredGroupID,$dtID,$publicADO);
			
			// display the html rows 
			$out.=getCodesTableRows('irCodes',$infraredGroupID,$dtID,$deviceID,$codesArray,$togglePower,$toggleInput,$toggleDSP);

			$out.='
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
				<td colspan="3" align="center"><input type="submit" class="button" name="update" value="Update" '.((!isset($_SESSION['userID']))?'disabled':'').'></td>
			</tr>
	';
		$out.='
		</table>
			<input type="hidden" name="displayedCommands" value="'.join(',',$GLOBALS['displayedCommands']).'">
			<input type="hidden" name="displayedIRGC" value="'.join(',',$GLOBALS['displayedIRGC']).'">
		</form>
	<span class="normaltext">If you have question or comments, please contact us by <a href="mailto:support@plutohome.com?subject=IR Group '.$infraredGroupID.' x Device Template '.$dtID.' x UserID '.$userID.'">email</a>.</span><br><br>		
	';	
		$out.=(($GLOBALS['btnEnabled']=='disabled')?'<span class="normaltext"><em>* Add/Edit options are available only for your own device templates.</em></span>':'');		
	} else {
		$time_start = getmicrotime();
		//$publicADO->debug=true;
		
		$newIRGroup=((int)@$_POST['irGroup']>0)?(int)$_POST['irGroup']:NULL;
		$oldIRGroup=(int)$_POST['oldIRGroup'];
		if($newIRGroup!=$oldIRGroup){

			$publicADO->Execute('UPDATE DeviceTemplate SET FK_InfraredGroup=? WHERE PK_DeviceTemplate=?',array($newIRGroup,$dtID));
			$publicADO->Execute('UPDATE InfraredGroup_Command SET FK_InfraredGroup=? WHERE FK_DeviceTemplate=? AND FK_InfraredGroup IS NOT NULL AND psc_user=?',array($newIRGroup,$dtID,$userID));

			header("Location: index.php?section=irCodes&from=$from&dtID=$dtID&deviceID=$deviceID&infraredGroupID=$newIRGroup&msg=IR Group changed for selected device template.&label=".$GLOBALS['label']);
			exit();
		}
		
		if(isset($_POST['irgroup_command']) && (int)$_POST['irgroup_command']>0){
			$irg_c=(int)$_POST['irgroup_command'];
			if($action!='delete'){
				$publicADO->Execute('INSERT INTO InfraredGroup_Command (FK_InfraredGroup,FK_Command,FK_Device,FK_DeviceTemplate,IRData,FK_Users,psc_user) SELECT FK_InfraredGroup,FK_Command,'.$deviceID.','.$dtID.',IRData,'.$_SESSION['userID'].','.$_SESSION['userID'].' FROM InfraredGroup_Command WHERE PK_InfraredGroup_Command=?',$irg_c);

				header("Location: index.php?section=irCodes&from=$from&dtID=$dtID&deviceID=$deviceID&infraredGroupID=$infraredGroupID&msg=Custom code added.&label=".$GLOBALS['label']);
				exit();
			}else{
				$publicADO->Execute('DELETE FROM InfraredGroup_Command WHERE PK_InfraredGroup_Command=?',$irg_c);
				
				header("Location: index.php?section=irCodes&from=$from&dtID=$dtID&deviceID=$deviceID&infraredGroupID=$infraredGroupID&msg=Custom code deleted.&label=".$GLOBALS['label']);
				exit();
			}
		}
		
		if(isset($_POST['update']) && isset($_SESSION['userID'])){
			
			$deviceCGArray=explode(',',$_POST['deviceCG']);
			$oldCheckedDCG=explode(',',$_POST['oldCheckedDCG']);
			$displayedCommands=explode(',',$_POST['displayedCommands']);
			$infraredGroupID=($infraredGroupID==0)?NULL:$infraredGroupID;
			
			foreach ($deviceCGArray AS $deviceCG){
				if(isset($_POST['dcg_'.$deviceCG])){

					$commands=explode(',',$_POST['commands_'.$deviceCG]);
					
					foreach ($commands AS $commandID){
						if(!in_array($commandID,$displayedCommands)){			
							$publicADO->Execute('INSERT INTO InfraredGroup_Command (FK_InfraredGroup,FK_Command,FK_Device,FK_DeviceTemplate,IRData,FK_Users,psc_user) VALUES (?,?,?,?,?,?,?)',array($infraredGroupID,$commandID,$deviceID,$dtID,'',$_SESSION['userID'],$_SESSION['userID']));
						}
					}
					if(!in_array($deviceCG,$oldCheckedDCG))
						$publicADO->Execute('INSERT INTO DeviceTemplate_DeviceCommandGroup (FK_DeviceTemplate, FK_DeviceCommandGroup) VALUES (?,?)',array($dtID,$deviceCG));

				}elseif(!isset($_POST['dcg_'.$deviceCG]) && in_array($deviceCG,$oldCheckedDCG)){
					// delete from IRG_C
					$commands=explode(',',$_POST['commands_'.$deviceCG]);
					if(count($commands)>0){
						$publicADO->Execute('DELETE FROM InfraredGroup_Command WHERE FK_Command in ('.join(',',$commands).') AND FK_Users=? AND psc_user=?',array($_SESSION['userID'],$_SESSION['userID']));

					}
					$publicADO->Execute('DELETE FROM DeviceTemplate_DeviceCommandGroup WHERE FK_DeviceTemplate=? AND FK_DeviceCommandGroup=?',array($dtID,$deviceCG));
				}
			}
			
			
			$customCodesNoArray=explode(',',@$_POST['displayedIRGC']);
			foreach ($customCodesNoArray as $ig_c){
				if(isset($_POST['irData_'.$ig_c])){
					$irData=stripslashes($_POST['irData_'.$ig_c]);
					$publicADO->Execute('UPDATE InfraredGroup_Command SET IRData=? WHERE PK_InfraredGroup_Command=? AND psc_user=?',array($irData,$ig_c,(int)$_SESSION['userID']));
				}
			}
			$time_end= getmicrotime();
			//print '<p class="normaltext">Page generated in '.round(($time_end-$time_start),3).' s.';
			
			header("Location: index.php?section=irCodes&from=$from&deviceID=$deviceID&dtID=$dtID&infraredGroupID=$infraredGroupID&msg=IR codes updated.&label=".$GLOBALS['label']);
			exit();
		}

		header("Location: index.php?section=irCodes&from=$from&dtID=$dtID&deviceID=$deviceID&infraredGroupID=$infraredGroupID&lastAction=".@$lastAction);
	}
	$time_end = getmicrotime();
	$out.='<br><p class="normaltext">Page generated in '.round(($time_end-$time_start),3).' s.';
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: IR Codes');
	$output->output();
}

function formatCode($section,$dataArray,$pos,$infraredGroupID,$dtID,$deviceID){
	if($dataArray['DefaultOrder'][$pos]==1){
		$RowColor='lightblue';
	}
	else{
		$RowColor=(isset($_SESSION['userID']) && $dataArray['psc_user'][$pos]==@$_SESSION['userID'])?'yellow':'lightyellow';
	}
	$deleteButton=(isset($_SESSION['userID']) && $dataArray['psc_user'][$pos]==@$_SESSION['userID'])?'<input type="button" class="button" name="delCustomCode" value="Delete code" onClick="if(confirm(\'Are you sure you want to delete this code?\')){document.'.$section.'.action.value=\'delete\';document.'.$section.'.irgroup_command.value='.$pos.';document.'.$section.'.submit();}">':'';

	$out='
		<table width="100%">
			<tr bgcolor="'.$RowColor.'">
				<td align="center" width="100"><B>'.$dataArray['Description'][$pos].'</B> <br><input type="button" class="button" name="learnCode" value="New code" onClick="windowOpen(\'index.php?section='.(($section=='rubyCodes')?'newRubyCode':'newIRCode').'&deviceID='.$deviceID.'&dtID='.$dtID.'&infraredGroupID='.$infraredGroupID.'&commandID='.$dataArray['FK_Command'][$pos].'&action=sendCommand\',\'width=750,height=310,toolbars=true,scrollbars=1,resizable=1\');" '.((!isset($_SESSION['userID']))?'disabled':'').'></td>
				<td><textarea name="irData_'.$pos.'" rows="2" style="width:100%">'.$dataArray['IRData'][$pos].'</textarea></td>
				<td align="center" width="100">'.$deleteButton.'  <a name="test_'.$pos.'"></td>
			</tr>
		</table>';

	return $out;

}
?>
