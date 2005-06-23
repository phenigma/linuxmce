<?php
function editPipes($output,$dbADO) {
	//$dbADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	$installationID = (int)@$_SESSION['installationID'];
	$deviceID = (int)$_REQUEST['deviceID'];
	
	
	if ($action=='form') {
		$rowD=getFieldsAsArray('Device','FK_DeviceTemplate',$dbADO,'WHERE PK_Device='.$deviceID);
		$avDTIDArray=getDeviceTemplatesFromCategory($GLOBALS['rootAVEquipment'],$dbADO);
		
		$audioOutputPulldown='
					<select name="audioOutput_'.$deviceID.'">
						<option value="0"></option>';
		$queryDDP='
			SELECT Device.*,FK_Command_Output,FK_Device_To,FK_Command_Input
			FROM Device_Device_Pipe
			INNER JOIN Device ON FK_Device_To=PK_Device
			WHERE FK_Device_From=? AND FK_Pipe=?';
		$resDDP=$dbADO->Execute($queryDDP,array($deviceID,$GLOBALS['AudioPipe']));
		if($resDDP->RecordCount()>0){
			$rowDDP=$resDDP->FetchRow();
			$toDevice=$rowDDP['FK_Device_To'];
			$toDeviceTemplate=$rowDDP['FK_DeviceTemplate'];
			$audioOutput=$rowDDP['FK_Command_Output'];
			$audioInput=$rowDDP['FK_Command_Input'];
		}else{
			$toDevice='';
			$audioOutput='';
			$audioInput='';
		}
		$resDDP->Close();
		$queryOutput='
			SELECT Command.Description,PK_Command
			FROM DeviceTemplate_Output
			INNER JOIN Command ON FK_Command=PK_Command
			WHERE FK_DeviceTemplate=?';
		$resOutput=$dbADO->Execute($queryOutput,$rowD['FK_DeviceTemplate']);
		while($rowOutput=$resOutput->FetchRow()){
			$audioOutputPulldown.='<option value="'.$rowOutput['PK_Command'].'" '.(($rowOutput['PK_Command']==@$audioOutput)?'selected':'').'>'.$rowOutput['Description'].'</option>';
		}
		$resOutput->Close();
		$audioOutputPulldown.='</select>';

		$audioInputPulldown='<select name="audioInput_'.$deviceID.'">
						<option value="0"></option>';
		$queryInput='
			SELECT Command.Description,PK_Command
			FROM DeviceTemplate_Input
			INNER JOIN Command ON FK_Command=PK_Command
			WHERE FK_DeviceTemplate=?';

		$toDeviceTemplate=(isset($toDeviceTemplate))?$toDeviceTemplate:-1;

		$resInput=$dbADO->Execute($queryInput,$toDeviceTemplate);

		while($rowInput=$resInput->FetchRow()){
			$audioInputPulldown.='<option value="'.$rowInput['PK_Command'].'" '.(($rowInput['PK_Command']==@$audioInput)?'selected':'').'>'.$rowInput['Description'].'</option>';
		}
		$resInput->Close();
		$audioInputPulldown.='</select>';

		$queryConnectedToDevices='
			SELECT DISTINCT Device.*
			FROM Device 
			INNER JOIN DeviceTemplate ON Device.FK_DeviceTemplate=PK_DeviceTemplate
			INNER JOIN DeviceTemplate_Input ON DeviceTemplate_Input.FK_DeviceTemplate=Device.FK_DeviceTemplate 
			WHERE Device.FK_DeviceTemplate IN ('.join(',',$avDTIDArray).') AND FK_Installation=?';	
		$resConnectedToDevices=$dbADO->Execute($queryConnectedToDevices,$installationID);
		$conD=array();
		while($rowConD=$resConnectedToDevices->FetchRow()){
			$conD[$rowConD['PK_Device']]=$rowConD['Description'];
		}
		$resConnectedToDevices->Close();

		
		
		$audioConnectToPulldown='<input type="hidden" name="oldAudioPipe_'.$deviceID.'" value="'.@$toDevice.','.$audioInput.','.$audioOutput.'"><a name="AudioPipe_'.$deviceID.'"></a>
					<select name="audioConnectTo_'.$deviceID.'" onChange="document.editPipes.cmd.value=1;document.forms[0].submit();">
						<option value="0"></option>';
		foreach($conD AS $key=>$device){
			if($key!=$deviceID)
			$audioConnectToPulldown.='<option value="'.$key.'" '.(($key==@$toDevice)?'selected':'').'>'.$device.'</option>';
		}
		$audioConnectToPulldown.='</select>';

		
		$videoOutputPulldown='<select name="videoOutput_'.$deviceID.'" '.((@$childOf[$deviceID]=='')?'':'disabled').'>
						<option value="0"></option>';
		$queryDDP='
			SELECT Device.*,FK_Command_Output,FK_Device_To,FK_Command_Input
			FROM Device_Device_Pipe
			INNER JOIN Device ON FK_Device_To=PK_Device
			WHERE FK_Device_From=? AND FK_Pipe=?';

		$resDDP=$dbADO->Execute($queryDDP,array($deviceID,$GLOBALS['VideoPipe']));
		if($resDDP->RecordCount()>0){
			// TODO: check and process if multiple pipes
			$rowDDP=$resDDP->FetchRow();
			$toDevice=$rowDDP['FK_Device_To'];
			$toDeviceTemplate=$rowDDP['FK_DeviceTemplate'];
			$videoOutput=$rowDDP['FK_Command_Output'];
			$videoInput=$rowDDP['FK_Command_Input'];
		}else{
			$toDevice='';
			$videoOutput='';
			$videoInput='';
		}
		$queryOutput='
			SELECT Command.Description,PK_Command
			FROM DeviceTemplate_Output
			INNER JOIN Command ON FK_Command=PK_Command
			WHERE FK_DeviceTemplate=?';
		$resOutput=$dbADO->Execute($queryOutput,$rowD['FK_DeviceTemplate']);
		while($rowOutput=$resOutput->FetchRow()){
			$videoOutputPulldown.='<option value="'.$rowOutput['PK_Command'].'" '.(($rowOutput['PK_Command']==@$videoOutput)?'selected':'').'>'.$rowOutput['Description'].'</option>';
		}
		$resOutput->Close();
		$videoOutputPulldown.='</select>';


		$videoConnectToPulldown='<input type="hidden" name="oldVideoPipe_'.$deviceID.'" value="'.@$toDevice.','.$videoInput.','.$videoOutput.'">
						<a name="VideoPipe_'.$deviceID.'"></a>
					<select name="videoConnectTo_'.$deviceID.'" onChange="document.editPipes.cmd.value=1;document.forms[0].submit();" '.((@$childOf[$deviceID]=='')?'':'disabled').'>
						<option value="0"></option>';
		foreach($conD AS $key=>$device){
			if($key!=$deviceID)
			$videoConnectToPulldown.='<option value="'.$key.'" '.(($key==@$toDevice)?'selected':'').'>'.$device.'</option>';
		}
		$videoConnectToPulldown.='</select>';

		$videoInputPulldown='<select name="videoInput_'.$deviceID.'" '.((@$childOf[$deviceID]=='')?'':'disabled').'>
						<option value="0"></option>';
		$queryInput='
			SELECT Command.Description,PK_Command
			FROM DeviceTemplate_Input
			INNER JOIN Command ON FK_Command=PK_Command
			WHERE FK_DeviceTemplate=?';

		$toDeviceTemplate=(isset($toDeviceTemplate))?$toDeviceTemplate:-1;

		$resInput=$dbADO->Execute($queryInput,$toDeviceTemplate);

		while($rowInput=$resInput->FetchRow()){
			$videoInputPulldown.='<option value="'.$rowInput['PK_Command'].'" '.(($rowInput['PK_Command']==@$videoInput)?'selected':'').'>'.$rowInput['Description'].'</option>';
		}
		$resInput->Close();
		$videoInputPulldown.='</select>';

		
		$out.='
		<div class="err">'.@$_GET['error'].'</div>
		<div class="confirm" align="center"><B>'.strip_tags(@$_GET['msg']).'</B></div>
		
		<form action="index.php" method="post" name="editPipes">
		<input type="hidden" name="section" value="editPipes">
		<input type="hidden" name="action" value="add">
		<input type="hidden" name="deviceID" value="'.$deviceID.'">
		<input type="hidden" name="from" value="'.$from.'">
		<input type="hidden" name="cmd" value="0">
		
		<h2>Pipes Used by device #'.$deviceID.'</h2>
		<table>
			<tr>
				<td align="center"><B>Pipe</B></td>
				<td align="center"><B>Output</B></td>
				<td align="center"><B>Connected to</B></td>
				<td align="center"><B>Input</B></td>
			</tr>
			<tr>
				<td><B>Audio</B></td>
				<td>'.$audioOutputPulldown.'</td>
				<td>'.$audioConnectToPulldown.'</td>
				<td>'.$audioInputPulldown.'</td>
			</tr>
			<tr>			
				<td><B>Video</B></td>
				<td>'.$videoOutputPulldown.'</td>
				<td>'.$videoConnectToPulldown.'</td>
				<td>'.$videoInputPulldown.'</td>
			</tr>
			<tr>			
				<td colspan="4" align="center"><input type="submit" class="button" name="update" value="Update"> <input type="button" class="button" name="close" value="Close" onClick="self.close();"></td>
			</tr>		
		</table>										
		</form>
		';
		
	} else {
		$cmd=cleanInteger(@$_POST['cmd']);	

		$oldAudioPipeArray=array();
		$oldAudioPipeArray=explode(',',$_POST['oldAudioPipe_'.$deviceID]);
		$oldTo=@$oldAudioPipeArray[0];
		$oldInput=@$oldAudioPipeArray[1];
		$oldOutput=@$oldAudioPipeArray[2];
		$audioOutput=(isset($_POST['audioOutput_'.$deviceID]) && $_POST['audioOutput_'.$deviceID]!='0')?cleanInteger($_POST['audioOutput_'.$deviceID]):NULL;
		$audioInput=(isset($_POST['audioInput_'.$deviceID]) && $_POST['audioInput_'.$deviceID]!='0')?cleanInteger($_POST['audioInput_'.$deviceID]):NULL;
		$audioConnectTo=(isset($_POST['audioConnectTo_'.$deviceID]) && $_POST['audioConnectTo_'.$deviceID]!='0')?cleanInteger($_POST['audioConnectTo_'.$deviceID]):NULL;
		if($oldTo!=$audioConnectTo || $oldInput!=$audioInput || $oldOutput!=$audioOutput){
			if($oldTo=='' || is_null($oldTo)){
				$insertDDP='
					INSERT IGNORE INTO Device_Device_Pipe 
						(FK_Device_From, FK_Device_To, FK_Command_Input, FK_Command_Output, FK_Pipe)
					VALUES
						(?,?,?,?,?)';
				if(!is_null($audioConnectTo))
				$dbADO->Execute($insertDDP,array($deviceID,$audioConnectTo,$audioInput,$audioOutput,$GLOBALS['AudioPipe']));
			}else{
				if(is_null($audioConnectTo)){
					$deleteDDP='DELETE FROM Device_Device_Pipe WHERE FK_Device_From=? AND FK_Device_To=? AND FK_Pipe=?';
					$dbADO->Execute($deleteDDP,array($deviceID,$oldTo,$GLOBALS['AudioPipe']));
				}else{
					$updateDDP='
						UPDATE Device_Device_Pipe 
						SET FK_Device_To=?, FK_Command_Input=?, FK_Command_Output=? 
						WHERE FK_Device_From=? AND FK_Device_To=? AND FK_Pipe=?';
					$dbADO->Execute($updateDDP,array($audioConnectTo,$audioInput,$audioOutput,$deviceID,$oldTo,$GLOBALS['AudioPipe']));
				}
			}
		}

		$oldVideoPipeArray=array();
		$oldVideoPipeArray=explode(',',$_POST['oldVideoPipe_'.$deviceID]);
		$oldTo=$oldVideoPipeArray[0];
		$oldInput=$oldVideoPipeArray[1];
		$oldOutput=$oldVideoPipeArray[2];

		$videoOutput=(isset($_POST['videoOutput_'.$deviceID]) && $_POST['videoOutput_'.$deviceID]!='0')?cleanInteger($_POST['videoOutput_'.$deviceID]):NULL;
		$videoInput=(isset($_POST['videoInput_'.$deviceID]) && $_POST['videoInput_'.$deviceID]!='0')?cleanInteger($_POST['videoInput_'.$deviceID]):NULL;
		$videoConnectTo=(isset($_POST['videoConnectTo_'.$deviceID]) && $_POST['videoConnectTo_'.$deviceID]!='0')?cleanInteger($_POST['videoConnectTo_'.$deviceID]):NULL;
		if($oldTo!=$videoConnectTo || $oldInput!=$videoInput || $oldOutput!=$videoOutput){
			if($oldTo=='' || is_null($oldTo)){
				$insertDDP='
					INSERT IGNORE INTO Device_Device_Pipe 
						(FK_Device_From, FK_Device_To, FK_Command_Input, FK_Command_Output, FK_Pipe)
					VALUES
						(?,?,?,?,?)';
				if(!is_null($videoConnectTo))
				$dbADO->Execute($insertDDP,array($deviceID,$videoConnectTo,$videoInput,$videoOutput,$GLOBALS['VideoPipe']));
			}else{
				if(is_null($videoConnectTo)){
					$deleteDDP='DELETE FROM Device_Device_Pipe WHERE FK_Device_From=? AND FK_Device_To=? AND FK_Pipe=?';
					$dbADO->Execute($deleteDDP,array($deviceID,$oldTo,$GLOBALS['VideoPipe']));
				}else{
					$updateDDP='
						UPDATE Device_Device_Pipe 
						SET FK_Device_To=?, FK_Command_Input=?, FK_Command_Output=? 
						WHERE FK_Device_From=? AND FK_Device_To=? AND FK_Pipe=?';
					$dbADO->Execute($updateDDP,array($videoConnectTo,$videoInput,$videoOutput,$deviceID,$oldTo,$GLOBALS['VideoPipe']));
				}
			}
		}

		
		$out.='
		<script>
			opener.document.avWizard.action.value="form";
			opener.document.avWizard.submit();
			self.location="index.php?section=editPipes&from=$from&deviceID='.$deviceID.'&msg=The pipes was updated.";
		</script>';

	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Edit pipes');			
	$output->output();
}
?>
