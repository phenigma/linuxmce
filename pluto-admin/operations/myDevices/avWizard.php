<?
function avWizard($output,$dbADO) {
	global $dbPlutoMainDatabase;
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
//	$dbADO->debug=true;
	$userID = (int)@$_SESSION['userID'];
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$type = isset($_REQUEST['type'])?cleanString($_REQUEST['type']):'lights';
	$installationID = (int)@$_SESSION['installationID'];

	switch($type){
		case 'avEquipment':
			$deviceCategory=$GLOBALS['rootAVEquipment'];
			$title='A/V Equipment';
		break;
		case 'media_directors':
			$deviceCategory=$GLOBALS['rootMediaDirectors'];
		break;
	}
	// get selected category Device Templates
	getDeviceCategoryChildsArray($deviceCategory,$dbADO);
	$GLOBALS['childsDeviceCategoryArray']=cleanArray($GLOBALS['childsDeviceCategoryArray']);
	$GLOBALS['childsDeviceCategoryArray'][]=$deviceCategory;
	
	$queryDeviceTemplate='
		SELECT * FROM DeviceTemplate 
			WHERE FK_DeviceCategory IN ('.join(',',$GLOBALS['childsDeviceCategoryArray']).')
		ORDER BY Description ASC';
	$resDeviceTemplate=$dbADO->Execute($queryDeviceTemplate);
	$DTArray=array();
	$DTIDArray=array();
	while($rowDeviceCategory=$resDeviceTemplate->FetchRow()){
		$DTArray[]=$rowDeviceCategory['Description'];
		$DTIDArray[]=$rowDeviceCategory['PK_DeviceTemplate'];
	}
	
	// get AV Device Templates
	unset($GLOBALS['childsDeviceCategoryArray']);
	getDeviceCategoryChildsArray($GLOBALS['rootAVEquipment'],$dbADO);
	$GLOBALS['childsDeviceCategoryArray']=cleanArray($GLOBALS['childsDeviceCategoryArray']);
	$GLOBALS['childsDeviceCategoryArray'][]=$GLOBALS['rootAVEquipment'];
	
	$queryDeviceTemplate='
		SELECT * FROM DeviceTemplate 
			WHERE FK_DeviceCategory IN ('.join(',',$GLOBALS['childsDeviceCategoryArray']).')
		ORDER BY Description ASC';
	$resDeviceTemplate=$dbADO->Execute($queryDeviceTemplate);
	$avDTIDArray=array();
	while($rowDeviceCategory=$resDeviceTemplate->FetchRow()){
		$avDTIDArray[]=$rowDeviceCategory['PK_DeviceTemplate'];
	}
	
	
	$queryRooms='SELECT * FROM Room WHERE FK_Installation=? ORDER BY Description ASC';
	$resRooms=$dbADO->Execute($queryRooms,$installationID);
	$roomIDArray=array();
	$roomArray=array();
	while($rowRoom=$resRooms->FetchRow()){
		$roomArray[]=$rowRoom['Description'];
		$roomIDArray[]=$rowRoom['PK_Room'];
	}

	$queryEA='
		SELECT * 
			FROM EntertainArea 
			INNER JOIN Room ON FK_Room=PK_Room
		WHERE FK_Installation=?
		ORDER BY EntertainArea.Description ASC';
	$resEA=$dbADO->Execute($queryEA,$installationID);
	$eaArray=array();
	while($rowEA=$resEA->FetchRow()){
		$eaArray[$rowEA['PK_EntertainArea']]=$rowEA['Description'];
	}

	if ($action == 'form') {
		$out.='
	<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
	</script>
	<div class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
	<form action="index.php" method="POST" name="avWizard">
	<input type="hidden" name="section" value="avWizard">
	<input type="hidden" name="type" value="'.$type.'">
	<input type="hidden" name="action" value="add">
	<input type="hidden" name="cmd" value="0">			
	<div align="center"><h3>'.((isset($title))?$title:strtoupper(str_replace('_',' ',$type))).'</h3></div>
		<table align="center" border="0" cellpadding="2" cellspacing="0">
				<tr>
					<td align="center"><B>Device</B></td>
					<td align="center"><B>Room '.(($type!='media_directors')?'/ Controlled by':'').'</B></td>';
		if($type!='media_directors')
			$out.='	<td align="center"><B>Entertain area</B></td>';
		$out.='
					<td align="center"><B>Output</B></td>
					<td align="center"><B>Connected to</B></td>
					<td align="center"><B>Input</B></td>
					<td align="center"><B>Data</B></td>
					<td align="center"><B>Actions</B></td>
				</tr>
					';
				if(count($avDTIDArray)==0)
					$avDTIDArray[]=0;
				$displayedAVDevices=array();
				$displayedAVDevicesDescription=array();
				$queryDevice='
					SELECT 
						Device.* FROM Device 
					WHERE
						FK_DeviceTemplate IN ('.join(',',$avDTIDArray).') AND FK_Installation=?';	
				$resDevice=$dbADO->Execute($queryDevice,$installationID);
				while($rowD=$resDevice->FetchRow()){
					$displayedAVDevices[]=$rowD['PK_Device'];
					$displayedAVDevicesDescription[]=$rowD['Description'];
				}
				$resDevice->Close();
				
			
				$displayedDevices=array();
				$displayedDevicesDescription=array();
				$DeviceDataToDisplay=array();
				$DeviceDataDescriptionToDisplay=array();
				$DDTypesToDisplay=array();	
				$joinArray=$DTIDArray;	// used only for query when there are no DT in selected category
				$queryDevice='
					SELECT 
						Device.*, DeviceTemplate.Description AS TemplateName 
					FROM Device 
						INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
					WHERE
						FK_DeviceTemplate IN ('.join(',',$joinArray).') AND FK_Installation=?';	
				$resDevice=$dbADO->Execute($queryDevice,$installationID);
				while($rowD=$resDevice->FetchRow()){
					$displayedDevices[]=$rowD['PK_Device'];
					$displayedDevicesDescription[]=$rowD['Description'];
				}
				$joinArray=$displayedDevices;	// used only for query when there are no Devices in selected category
				if(count($joinArray)==0)
					$joinArray[]=0;
				$queryDeviceDeviceData='
					SELECT 
						DISTINCT DeviceData.PK_DeviceData,DeviceData.Description, ParameterType.Description AS paramName
					FROM DeviceData
						INNER JOIN ParameterType ON 
							FK_ParameterType = PK_ParameterType 
						INNER JOIN Device_DeviceData ON 
							FK_DeviceData=PK_DeviceData
					WHERE
						FK_Device IN ('.join(',',$joinArray).')
					ORDER BY Description ASC';
				$resDDD=$dbADO->Execute($queryDeviceDeviceData);
				while($rowDDD=$resDDD->FetchRow()){
					if($rowDDD['Description']!='Floorplan Info'){
						$DeviceDataToDisplay[]=$rowDDD['PK_DeviceData'];
						$DDTypesToDisplay[]=$rowDDD['paramName'];
						$DeviceDataDescriptionToDisplay[]=$rowDDD['Description'];
					}
				}	
			$resDevice->MoveFirst();
			$pos=0;
			while($rowD=$resDevice->FetchRow()){
				$pos++;
				$out.='
				<tr bgcolor="'.(($pos%2==0)?'#EFF2F9':'').'">
					<td align="center"><input type="text" name="description_'.$rowD['PK_Device'].'" value="'.$rowD['Description'].'"></td>
					<td><select name="room_'.$rowD['PK_Device'].'">
						<option value="0">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;- Select room -&nbsp;&nbsp;&nbsp;&nbsp;</option>';
				foreach($roomIDArray as $key => $value){
					$out.='<option value="'.$value.'" '.(($rowD['FK_Room']==$value)?'selected':'').'>'.$roomArray[$key].'</option>';
				}

			$out.='	</select>
					</td>';
			if($type!='media_directors'){
				$out.='
					<td rowspan="2"><select name="deviceEntArea_'.$rowD['PK_Device'].'[]" multiple size="3">';
					
				$queryDEA='SELECT * FROM Device_EntertainArea WHERE FK_Device=?';
				$resDEA=$dbADO->Execute($queryDEA,$rowD['PK_Device']);
				$checkedEA=array();
				while($rowDEA=$resDEA->FetchRow()){
					$checkedEA[]=$rowDEA['FK_EntertainArea'];
				}
				foreach($eaArray AS $idEA=>$descriptionEA){
					$out.='<option value="'.$idEA.'" '.((in_array($idEA,$checkedEA))?'selected':'').'>'.$descriptionEA.'</option>';
				}
					$out.='</select>
					<input type="hidden" name="oldDeviceEA_'.$rowD['PK_Device'].'" value="'.join(',',$checkedEA).'">
					</td>';
			}
			$out.='
					<td>A: <select name="audioOutput_'.$rowD['PK_Device'].'">
						<option value="0"></option>';
					$queryDDP='
						SELECT Device.*,FK_Command_Output,FK_Device_To,FK_Command_Input
						FROM Device_Device_Pipe
						INNER JOIN Device ON FK_Device_To=PK_Device
						WHERE FK_Device_From=? AND FK_Pipe=?';
					$resDDP=$dbADO->Execute($queryDDP,array($rowD['PK_Device'],$GLOBALS['AudioPipe']));
					if($resDDP->RecordCount()>0){
						// TODO: check and process if multiple pipes
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
						$out.='<option value="'.$rowOutput['PK_Command'].'" '.(($rowOutput['PK_Command']==@$audioOutput)?'selected':'').'>'.$rowOutput['Description'].'</option>';
					}
					$resOutput->Close();
					$out.='</select>
					</td>
					<td><input type="hidden" name="oldAudioPipe_'.$rowD['PK_Device'].'" value="'.@$toDevice.','.$audioInput.','.$audioOutput.'">
						<a name="AudioPipe_'.$rowD['PK_Device'].'"></a>
					<select name="audioConnectTo_'.$rowD['PK_Device'].'" onChange="document.avWizard.cmd.value=1;document.forms[0].submit();">
						<option value="0"></option>';
					foreach($displayedAVDevices AS $key=>$device){
						if($device!=$rowD['PK_Device'])
							$out.='<option value="'.$device.'" '.(($device==@$toDevice)?'selected':'').'>'.$displayedAVDevicesDescription[$key].'</option>';					
					}
					$out.='</select></td>
					<td><select name="audioInput_'.$rowD['PK_Device'].'">
						<option value="0"></option>';
					$queryInput='
						SELECT Command.Description,PK_Command
						FROM DeviceTemplate_Input
						INNER JOIN Command ON FK_Command=PK_Command
						WHERE FK_DeviceTemplate=?';
					
					$toDeviceTemplate=(isset($toDeviceTemplate))?$toDeviceTemplate:-1;
					
					$resInput=$dbADO->Execute($queryInput,$toDeviceTemplate);

					while($rowInput=$resInput->FetchRow()){
						$out.='<option value="'.$rowInput['PK_Command'].'" '.(($rowInput['PK_Command']==@$audioInput)?'selected':'').'>'.$rowInput['Description'].'</option>';
					}
					$resInput->Close();
					$out.='</select>
					</td>
					<td rowspan="2" valign="top" align="right">';
				foreach($DeviceDataToDisplay as $key => $value){
					$queryDDforDevice='
						SELECT DeviceData.Description, ParameterType.Description AS typeParam, Device_DeviceData.IK_DeviceData,ShowInWizard,ShortDescription
						FROM DeviceData 
						INNER JOIN ParameterType ON FK_ParameterType = PK_ParameterType 
						INNER JOIN Device_DeviceData ON Device_DeviceData.FK_DeviceData=PK_DeviceData 
						INNER JOIN Device ON FK_Device=PK_Device
						LEFT JOIN DeviceTemplate_DeviceData ON DeviceTemplate_DeviceData.FK_DeviceData=Device_DeviceData.FK_DeviceData AND DeviceTemplate_DeviceData.FK_DeviceTemplate=Device.FK_DeviceTemplate
						WHERE FK_Device = ? AND Device_DeviceData.FK_DeviceData=?';

					$resDDforDevice=$dbADO->Execute($queryDDforDevice,array($rowD['PK_Device'],$value));

					if($resDDforDevice->RecordCount()>0){
						$rowDDforDevice=$resDDforDevice->FetchRow();
						$ddValue=$rowDDforDevice['IK_DeviceData'];
					}
					if(@$rowDDforDevice['ShowInWizard']==1){
						$out.='<b>'.(($rowDDforDevice['ShortDescription']!='')?$rowDDforDevice['ShortDescription']:$DeviceDataDescriptionToDisplay[$key]).'</b> ';
						switch($DDTypesToDisplay[$key]){
							case 'int':
								if(in_array($DeviceDataDescriptionToDisplay[$key],$GLOBALS['DeviceDataLinkedToTables']))
								{
									$tableName=str_replace('PK_','',$DeviceDataDescriptionToDisplay[$key]);
									$filterQuery='';
									switch($tableName){
										case 'Device':
											$filterQuery=" WHERE FK_Installation='".$installationID."'";
										break;
										case 'FloorplanObjectType':
											$filterQuery=" WHERE FK_FloorplanType='".@$specificFloorplanType."'";
									}
									
									$queryTable="SELECT * FROM $tableName $filterQuery ORDER BY Description ASC";
									$resTable=$dbADO->Execute($queryTable);
									$out.='<select name="deviceData_'.$rowD['PK_Device'].'_'.$value.'">
											<option value="0"></option>';
									while($rowTable=$resTable->FetchRow()){
										$out.='<option value="'.$rowTable[$DeviceDataDescriptionToDisplay[$key]].'" '.(($rowTable[$DeviceDataDescriptionToDisplay[$key]]==@$ddValue)?'selected':'').'>'.$rowTable['Description'].'</option>';
									}
									$out.='</select>';
								}
								else 
									$out.='<input type="text" name="deviceData_'.$rowD['PK_Device'].'_'.$value.'" value="'.@$ddValue.'">';
							break;
							case 'bool':
								$out.='<input type="checkbox" name="deviceData_'.$rowD['PK_Device'].'_'.$value.'" value="1" '.((@$ddValue!=0)?'checked':'').'>';
							break;
							default:
								$out.='<input type="text" name="deviceData_'.$rowD['PK_Device'].'_'.$value.'" value="'.@$ddValue.'">';
						}
						
						
						$out.='	
							<input type="hidden" name="oldDeviceData_'.$rowD['PK_Device'].'_'.$value.'" value="'.(($resDDforDevice->RecordCount()>0)?$ddValue:'NULL').'">';					
						unset($ddValue);
						$out.='<br>';
					}
				}
				$out.='</td>';
				$out.='<td align="center" rowspan="2" valign="top">';
				if($type=='avEquipment')
					$out.='	<input type="button" class="button" name="btn" value="IR Codes" onClick="windowOpen(\'index.php?section=irCodes&from=avWizard&deviceID='.$rowD['FK_DeviceTemplate'].'&from='.urlencode('avWizard&type='.$type).'\',\'width=800,height=600,toolbars=true,scrollbars=1,resizable=1\');"><br>
							<input type="button" class="button" name="btn" value="A/V Properties" onClick="windowOpen(\'index.php?section=editAVDevice&deviceID='.$rowD['FK_DeviceTemplate'].'&from='.urlencode('avWizard&type='.$type).'\',\'width=800,height=600,toolbars=true,scrollbars=1,resizable=1\');"><br>';
						$out.='<input type="submit" class="button" name="delete_'.$rowD['PK_Device'].'" value="Delete"  ></td>
					</tr>
					<tr bgcolor="'.(($pos%2==0)?'#EFF2F9':'').'">			
						<td align="center">'.$rowD['TemplateName'].'</td>
						<td>';
				if($type!='media_directors'){
					$out.='
						<select name="controlledBy_'.$rowD['PK_Device'].'">
							<option value="0">- Select device -</option>';
	
					GetDeviceControlledVia($rowD['PK_Device'], $dbADO);
					foreach($GLOBALS['DeviceIDControlledVia'] as $key => $value){
						$out.='<option value="'.$value.'" '.(($rowD['FK_Device_ControlledVia']==$value)?'selected':'').'>'.$GLOBALS['DeviceControlledVia'][$key].'</option>';
					}
					unset($GLOBALS['DeviceIDControlledVia']);
					unset($GLOBALS['DeviceControlledVia']);
				}else 
					$out.='&nbsp;';
				$out.='</td>
						<td>V: <select name="videoOutput_'.$rowD['PK_Device'].'">
						<option value="0"></option>';
					$queryDDP='
						SELECT Device.*,FK_Command_Output,FK_Device_To,FK_Command_Input
						FROM Device_Device_Pipe
						INNER JOIN Device ON FK_Device_To=PK_Device
						WHERE FK_Device_From=? AND FK_Pipe=?';

					$resDDP=$dbADO->Execute($queryDDP,array($rowD['PK_Device'],$GLOBALS['VideoPipe']));
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
						$out.='<option value="'.$rowOutput['PK_Command'].'" '.(($rowOutput['PK_Command']==@$videoOutput)?'selected':'').'>'.$rowOutput['Description'].'</option>';
					}
					$resOutput->Close();
					$out.='</select>
					</td>
					<td><input type="hidden" name="oldVideoPipe_'.$rowD['PK_Device'].'" value="'.@$toDevice.','.$videoInput.','.$videoOutput.'">
						<a name="VideoPipe_'.$rowD['PK_Device'].'"></a>
					<select name="videoConnectTo_'.$rowD['PK_Device'].'" onChange="document.avWizard.cmd.value=1;document.forms[0].submit();">
						<option value="0"></option>';
					foreach($displayedAVDevices AS $key=>$device){
						if($device!=$rowD['PK_Device'])
							$out.='<option value="'.$device.'" '.(($device==@$toDevice)?'selected':'').'>'.$displayedAVDevicesDescription[$key].'</option>';					
					}
					$out.='</select></td>
					<td><select name="videoInput_'.$rowD['PK_Device'].'">
						<option value="0"></option>';
					$queryInput='
						SELECT Command.Description,PK_Command
						FROM DeviceTemplate_Input
						INNER JOIN Command ON FK_Command=PK_Command
						WHERE FK_DeviceTemplate=?';
					
					$toDeviceTemplate=(isset($toDeviceTemplate))?$toDeviceTemplate:-1;
					
					$resInput=$dbADO->Execute($queryInput,$toDeviceTemplate);

					while($rowInput=$resInput->FetchRow()){
						$out.='<option value="'.$rowInput['PK_Command'].'" '.(($rowInput['PK_Command']==@$videoInput)?'selected':'').'>'.$rowInput['Description'].'</option>';
					}
					$resInput->Close();
					$out.='</select>
					</tr>
					';

			}
			$out.='
				<input type="hidden" name="DeviceDataToDisplay" value="'.join(',',$DeviceDataToDisplay).'">
				<input type="hidden" name="displayedDevices" value="'.join(',',$displayedDevices).'">';
			if($resDevice->RecordCount()!=0){
				$out.='
				<tr>
					<td colspan="8" align="center"><input type="submit" class="button" name="update" value="Update"  ></td>
				</tr>';
			}
			$out.='
				<tr>
					<td colspan="8">&nbsp;</td>
				</tr>
				<tr>
					<td colspan="8" align="center"><input type="button" class="button" name="button" value="Pick Device Template" onClick="windowOpen(\'index.php?section=deviceTemplatePicker&from='.urlencode('avWizard&type='.$type).'&categoryID='.$deviceCategory.'\',\'width=800,height=600,toolbars=true,scrollbars=1,resizable=1\');"></td>
				</tr>
			</table>
		</form>
		<script>
		 	var frmvalidator = new formValidator("avWizard");
 //			frmvalidator.addValidation("Description","req","Please enter a device description");			
//	 		frmvalidator.addValidation("masterDevice","dontselect=0","Please select a Device Template!");			
		</script>
	
	</form>
	';
	} else {
		$cmd=cleanInteger(@$_POST['cmd']);
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=avWizard&type=$type&error=You are not authorised to change the installation.");
			exit(0);
		}
		$displayedDevicesArray=explode(',',@$_POST['displayedDevices']);
		foreach($displayedDevicesArray as $value){
			if(isset($_POST['delete_'.$value])){
				$deleteDevice='DELETE FROM Device WHERE PK_Device=?';
				$dbADO->Execute($deleteDevice,$value); 
			}
		}
		
		if(isset($_POST['update']) || $cmd==1){
			setDCERouterNeedConfigure($_SESSION['installationID'],$dbADO);
			$DeviceDataToDisplayArray=explode(',',$_POST['DeviceDataToDisplay']);
			foreach($displayedDevicesArray as $key => $value){
				$description=@$_POST['description_'.$value];
				$room=(@$_POST['room_'.$value]!=0)?(int)@$_POST['room_'.$value]:NULL;
				$controlledBy=(@$_POST['controlledBy_'.$value]!=0)?(int)@$_POST['controlledBy_'.$value]:NULL;
				
				$updateDevice='UPDATE Device SET Description=?, FK_Room=?,FK_Device_ControlledVia =? WHERE PK_Device=?';
				$dbADO->Execute($updateDevice,array($description,$room,$controlledBy,$value));

				foreach($DeviceDataToDisplayArray as $ddValue){
					$deviceData=(isset($_POST['deviceData_'.$value.'_'.$ddValue]))?$_POST['deviceData_'.$value.'_'.$ddValue]:0;
					$oldDeviceData=@$_POST['oldDeviceData_'.$value.'_'.$ddValue];
					if($oldDeviceData!=$deviceData){
						if($oldDeviceData=='NULL'){
							$insertDDD='
								INSERT INTO Device_DeviceData 
									(FK_Device, FK_DeviceData, IK_DeviceData)
								VALUES 
									(?,?,?)';
							$dbADO->Execute($insertDDD,array($value,$ddValue,$deviceData));
						}
						else{
							$updateDDD='
								UPDATE Device_DeviceData 
									SET IK_DeviceData=? 
								WHERE FK_Device=? AND FK_DeviceData=?';
							$dbADO->Execute($updateDDD,array($deviceData,$value,$ddValue));
						}
					}
				}
				$oldAudioPipeArray=explode(',',$_POST['oldAudioPipe_'.$value]);
				$oldTo=$oldAudioPipeArray[0];
				$oldInput=$oldAudioPipeArray[1];
				$oldOutput=$oldAudioPipeArray[2];
				$audioOutput=(isset($_POST['audioOutput_'.$value]) && $_POST['audioOutput_'.$value]!='0')?cleanInteger($_POST['audioOutput_'.$value]):NULL;
				$audioInput=(isset($_POST['audioInput_'.$value]) && $_POST['audioInput_'.$value]!='0')?cleanInteger($_POST['audioInput_'.$value]):NULL;
				$audioConnectTo=(isset($_POST['audioConnectTo_'.$value]) && $_POST['audioConnectTo_'.$value]!='0')?cleanInteger($_POST['audioConnectTo_'.$value]):NULL;
				if($oldTo!=$audioConnectTo || $oldInput!=$audioInput || $oldOutput!=$audioOutput){
					if($oldTo=='' || is_null($oldTo)){
						$insertDDP='
							INSERT INTO Device_Device_Pipe 
								(FK_Device_From, FK_Device_To, FK_Command_Input, FK_Command_Output, FK_Pipe)
							VALUES
								(?,?,?,?,?)';
						if(!is_null($audioConnectTo))
							$dbADO->Execute($insertDDP,array($value,$audioConnectTo,$audioInput,$audioOutput,$GLOBALS['AudioPipe']));
					}else{
						if(is_null($audioConnectTo)){
							$deleteDDP='DELETE FROM Device_Device_Pipe WHERE FK_Device_From=? AND FK_Device_To=? AND FK_Pipe=?';
							$dbADO->Execute($deleteDDP,array($value,$oldTo,$GLOBALS['AudioPipe']));
						}else{
							$updateDDP='
								UPDATE Device_Device_Pipe 
								SET FK_Device_To=?, FK_Command_Input=?, FK_Command_Output=? 
								WHERE FK_Device_From=? AND FK_Device_To=? AND FK_Pipe=?';
							$dbADO->Execute($updateDDP,array($audioConnectTo,$audioInput,$audioOutput,$value,$oldTo,$GLOBALS['AudioPipe']));
						}
					}
					$anchor=($cmd==1)?'#AudioPipe_'.$value.'':'';
				}

				$oldVideoPipeArray=explode(',',$_POST['oldVideoPipe_'.$value]);
				$oldTo=$oldVideoPipeArray[0];
				$oldInput=$oldVideoPipeArray[1];
				$oldOutput=$oldVideoPipeArray[2];
				
				$videoOutput=(isset($_POST['videoOutput_'.$value]) && $_POST['videoOutput_'.$value]!='0')?cleanInteger($_POST['videoOutput_'.$value]):NULL;
				$videoInput=(isset($_POST['videoInput_'.$value]) && $_POST['videoInput_'.$value]!='0')?cleanInteger($_POST['videoInput_'.$value]):NULL;
				$videoConnectTo=(isset($_POST['videoConnectTo_'.$value]) && $_POST['videoConnectTo_'.$value]!='0')?cleanInteger($_POST['videoConnectTo_'.$value]):NULL;
				if($oldTo!=$videoConnectTo || $oldInput!=$videoInput || $oldOutput!=$videoOutput){
					if($oldTo=='' || is_null($oldTo)){
						$insertDDP='
							INSERT INTO Device_Device_Pipe 
								(FK_Device_From, FK_Device_To, FK_Command_Input, FK_Command_Output, FK_Pipe)
							VALUES
								(?,?,?,?,?)';
						if(!is_null($videoConnectTo))
							$dbADO->Execute($insertDDP,array($value,$videoConnectTo,$videoInput,$videoOutput,$GLOBALS['VideoPipe']));
					}else{
						if(is_null($videoConnectTo)){
							$deleteDDP='DELETE FROM Device_Device_Pipe WHERE FK_Device_From=? AND FK_Device_To=? AND FK_Pipe=?';
							$dbADO->Execute($deleteDDP,array($value,$oldTo,$GLOBALS['VideoPipe']));
						}else{
							$updateDDP='
								UPDATE Device_Device_Pipe 
								SET FK_Device_To=?, FK_Command_Input=?, FK_Command_Output=? 
								WHERE FK_Device_From=? AND FK_Device_To=? AND FK_Pipe=?';
							$dbADO->Execute($updateDDP,array($videoConnectTo,$videoInput,$videoOutput,$value,$oldTo,$GLOBALS['VideoPipe']));
						}
					}
					$anchor='#VideoPipe_'.$value;
				}
				if($type!='media_directors'){		
					$entAreaArray=@$_POST['deviceEntArea_'.$value];
					$oldEAArray=explode(',',@$_POST['oldDeviceEA_'.$value]);
					$toInsert=@array_diff($entAreaArray,$oldEAArray);
					$toDelete=@array_diff($oldEAArray,$entAreaArray);
					if(is_array($toInsert))
						foreach($toInsert as $entArea){
							$insertDE='INSERT INTO Device_EntertainArea (FK_Device, FK_EntertainArea) VALUES (?,?)';
							$dbADO->Execute($insertDE,array($value,$entArea));
						}
					if(is_array($toDelete))
						foreach($toDelete as $entArea){
							$delDE='DELETE FROM Device_EntertainArea WHERE FK_Device=? AND FK_EntertainArea=?';
							$dbADO->Execute($delDE,array($value,$entArea));
						}
				}
			}
		}
		
		if(isset($_REQUEST['add'])){
			unset($_SESSION['from']);
			$deviceTemplate=(int)$_REQUEST['deviceTemplate'];
			if($deviceTemplate!=0){
				$insertID=exec('/usr/pluto/bin/CreateDevice -h localhost -D '.$dbPlutoMainDatabase.' -d '.$deviceTemplate.' -i '.$installationID);	
				setDCERouterNeedConfigure($_SESSION['installationID'],$dbADO);
			}
		}
		
		
		header("Location: index.php?section=avWizard&type=$type".@$anchor);		
	}

	$output->setScriptCalendar('null');

	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.((isset($title))?' :: '.$title:' :: '.strtoupper(str_replace('_',' ',$type))));
	$output->output();
}
?>
