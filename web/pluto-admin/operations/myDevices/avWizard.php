<?
function avWizard($output,$dbADO) {
	global $dbPlutoMainDatabase;
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
//	$dbADO->debug=true;
	$userID = (int)@$_SESSION['userID'];
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$type = isset($_REQUEST['type'])?cleanString($_REQUEST['type']):'avEquipment';
	$installationID = (int)@$_SESSION['installationID'];

	switch($type){
		case 'avEquipment':
			$deviceCategory=$GLOBALS['rootAVEquipment'];
			$specificFloorplanType=$GLOBALS['AVEquipmentFlorplanType'];
			$title='A/V Equipment';
			$output->setHelpSrc('/support/index.php?section=document&docID=131');
		break;
		case 'media_directors':
			$deviceCategory=$GLOBALS['rootMediaDirectors'];
			$specificFloorplanType=$GLOBALS['EntertainmentZone'];
			$output->setHelpSrc('/support/index.php?section=document&docID=129');
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

	if(isset($_REQUEST['lastAdded'])){
		$rs=$dbADO->Execute('SELECT Comments FROM DeviceTemplate WHERE PK_DeviceTemplate=?',(int)$_REQUEST['lastAdded']);
		$row=$rs->FetchRow();
		if($row['Comments']!=''){
			$out.='<script>
				alert(\''.addslashes($row['Comments']).'\')
			</script>';
		}
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
	<div align="center"><h3>'.((isset($title))?$title:strtoupper(str_replace('_',' ',$type))).'</h3></div>';
		if($type=='avEquipment'){
			$queryDevice='
				SELECT Device.* 
				FROM Device
				WHERE FK_DeviceTemplate=? AND Device.FK_Installation=?';
			$resDevice=$dbADO->Execute($queryDevice,array($GLOBALS['rootCoreID'],$installationID));
			if($resDevice->RecordCount()!=0){
				$rowDevice=$resDevice->FetchRow();
				$coreID=$rowDevice['PK_Device'];
			}
			if(isset($coreID)){
				$resDevice_StartupScript=$dbADO->Execute('SELECT * FROM Device_StartupScript WHERE FK_Device=? AND FK_StartupScript=?',array($coreID,$GLOBALS['ShareIRCodes']));
				$rowShare=$resDevice_StartupScript->FetchRow();
				$sharedWithOthers=($rowShare['Enabled']==1)?1:0;
			}
			$out.='<div align="center"><input type="checkbox" name="shareIRCodes" value="1" '.((@$sharedWithOthers>0)?'checked':'').' onClick="document.avWizard.submit();"> Share my I/R codes with other Pluto users.</div>';
			$out.='	<input type="hidden" name="coreID" value="'.$coreID.'">
					<input type="hidden" name="oldShareIRCodes" value="'.((@$sharedWithOthers>0)?'1':'0').'">';
		}
		$out.='
		<table align="center" border="0" cellpadding="2" cellspacing="0">
				<tr>
					<td align="center"><B>Device</B></td>
					<td align="center"><B>Room '.(($type!='media_directors')?'/ Controlled by':'').'</B></td>';
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
			
			
				$displayedDevices=array();
				$DeviceDataToDisplay=array();
				$DDTypesToDisplay=array();	
				$joinArray=$DTIDArray;	// used only for query when there are no DT in selected category
				$orderFilter=($type!='media_directors')?'ORDER BY FK_Device_ControlledVia DESC, Device.Description ASC':'';
				$queryDevice='
					SELECT 
						Device.*, DeviceTemplate.Description AS TemplateName, DeviceCategory.Description AS CategoryName,Manufacturer.Description AS ManufacturerName,IsIPBased
					FROM Device 
						INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
						INNER JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory
						INNER JOIN Manufacturer ON FK_Manufacturer=PK_Manufacturer
					WHERE FK_DeviceTemplate IN ('.join(',',$joinArray).') AND FK_Installation=? '.$orderFilter;	
				$resDevice=$dbADO->Execute($queryDevice,$installationID);
				$childOf=array();
				while($rowD=$resDevice->FetchRow()){
					$childOf[$rowD['PK_Device']]=$rowD['FK_Device_ControlledVia'];
					$displayedDevices[$rowD['PK_Device']]=$rowD['Description'];
				}
				$joinArray=array_keys($displayedDevices);	// used only for query when there are no Devices in selected category
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
			$embededRows=array();
			while($rowD=$resDevice->FetchRow()){
				$pos++;
				
				$deviceName=(@$childOf[$rowD['PK_Device']]=='')?'<input type="text" name="description_'.$rowD['PK_Device'].'" value="'.$rowD['Description'].'">':'<input type="hidden" name="description_'.$rowD['PK_Device'].'" value="'.$rowD['Description'].'"><B>'.$rowD['Description'].'</B>';
				$deviceName.=' # '.$rowD['PK_Device'];
				$roomPulldown='<select name="room_'.$rowD['PK_Device'].'">
						<option value="0">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;- Select room -&nbsp;&nbsp;&nbsp;&nbsp;</option>';
				foreach($roomIDArray as $key => $value){
					$roomPulldown.='<option value="'.$value.'" '.(($rowD['FK_Room']==$value)?'selected':'').'>'.$roomArray[$key].'</option>';
				}
				$roomPulldown.='</select>';
				
				$audioOutputPulldown='
					<select name="audioOutput_'.$rowD['PK_Device'].'">
						<option value="0"></option>';
					$queryDDP='
						SELECT Device.*,FK_Command_Output,FK_Device_To,FK_Command_Input
						FROM Device_Device_Pipe
						INNER JOIN Device ON FK_Device_To=PK_Device
						WHERE FK_Device_From=? AND FK_Pipe=?';
					$resDDP=$dbADO->Execute($queryDDP,array($rowD['PK_Device'],$GLOBALS['AudioPipe']));
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
				
					$audioInputPulldown='<select name="audioInput_'.$rowD['PK_Device'].'">
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
					
					$audioConnectToPulldown='<input type="hidden" name="oldAudioPipe_'.$rowD['PK_Device'].'" value="'.@$toDevice.','.$audioInput.','.$audioOutput.'"><a name="AudioPipe_'.$rowD['PK_Device'].'"></a>
					<select name="audioConnectTo_'.$rowD['PK_Device'].'" onChange="document.avWizard.cmd.value=1;document.forms[0].submit();">
						<option value="0"></option>';
					foreach($conD AS $key=>$device){
						if($key!=$rowD['PK_Device'])
							$audioConnectToPulldown.='<option value="'.$key.'" '.(($key==@$toDevice)?'selected':'').'>'.$device.'</option>';					
					}
					$audioConnectToPulldown.='</select>';
					
					$deviceDataBox='';
					foreach($DeviceDataToDisplay as $key => $value){
						$queryDDforDevice='
							SELECT DeviceData.Description, ParameterType.Description AS typeParam, Device_DeviceData.IK_DeviceData,ShowInWizard,ShortDescription,AllowedToModify,DeviceTemplate_DeviceData.Description AS Tooltip
							FROM DeviceData 
							INNER JOIN ParameterType ON FK_ParameterType = PK_ParameterType 
							INNER JOIN Device_DeviceData ON Device_DeviceData.FK_DeviceData=PK_DeviceData 
							INNER JOIN Device ON FK_Device=PK_Device
							LEFT JOIN DeviceTemplate_DeviceData ON DeviceTemplate_DeviceData.FK_DeviceData=Device_DeviceData.FK_DeviceData AND DeviceTemplate_DeviceData.FK_DeviceTemplate=Device.FK_DeviceTemplate
							WHERE FK_Device = ? AND Device_DeviceData.FK_DeviceData=?';
	
						$resDDforDevice=$dbADO->Execute($queryDDforDevice,array($rowD['PK_Device'],$value));
	
						$rowDDforDevice=$resDDforDevice->FetchRow();
						$ddValue=$rowDDforDevice['IK_DeviceData'];
						
						if($rowDDforDevice['ShowInWizard']==1 || $rowDDforDevice['ShowInWizard']==''){
							$deviceDataBox.='<b>'.((@$rowDDforDevice['ShortDescription']!='')?$rowDDforDevice['ShortDescription']:$DeviceDataDescriptionToDisplay[$key]).'</b> '.((@$rowDDforDevice['Tooltip']!='')?'<img src="include/images/tooltip.gif" title="'.@$rowDDforDevice['Tooltip'].'" border="0" align="middle"> ':'');
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
										$deviceDataBox.='<select name="deviceData_'.$rowD['PK_Device'].'_'.$value.'" '.((isset($rowDDforDevice['AllowedToModify']) && $rowDDforDevice['AllowedToModify']==0)?'disabled':'').'>
												<option value="0"></option>';
										while($rowTable=$resTable->FetchRow()){
											$deviceDataBox.='<option value="'.$rowTable[$DeviceDataDescriptionToDisplay[$key]].'" '.(($rowTable[$DeviceDataDescriptionToDisplay[$key]]==@$ddValue)?'selected':'').'>'.$rowTable['Description'].'</option>';
										}
										$deviceDataBox.='</select>';
									}
									else 
										$deviceDataBox.='<input type="text" name="deviceData_'.$rowD['PK_Device'].'_'.$value.'" value="'.@$ddValue.'" '.((isset($rowDDforDevice['AllowedToModify']) && $rowDDforDevice['AllowedToModify']==0)?'disabled':'').'>';
								break;
								case 'bool':
									$deviceDataBox.='<input type="checkbox" name="deviceData_'.$rowD['PK_Device'].'_'.$value.'" value="1" '.((@$ddValue!=0)?'checked':'').' '.((isset($rowDDforDevice['AllowedToModify']) && $rowDDforDevice['AllowedToModify']==0)?'disabled':'').'>';
								break;
								default:
									$deviceDataBox.='<input type="text" name="deviceData_'.$rowD['PK_Device'].'_'.$value.'" value="'.@$ddValue.'" '.((isset($rowDDforDevice['AllowedToModify']) && $rowDDforDevice['AllowedToModify']==0)?'disabled':'').'>';
							}
							$mdDistro=($value==$GLOBALS['rootPK_Distro'])?@$ddValue:0;
							
							$deviceDataBox.='	
								<input type="hidden" name="oldDeviceData_'.$rowD['PK_Device'].'_'.$value.'" value="'.(($resDDforDevice->RecordCount()>0)?$ddValue:'NULL').'">';					
							unset($ddValue);
							$deviceDataBox.='<br>';
						}
					}
					if($rowD['IsIPBased']==1){
						$deviceDataBox.='IP <input type="text" name="ip_'.$rowD['PK_Device'].'" value="'.$rowD['IPaddress'].'"><br>
								MAC <input type="text" name="mac_'.$rowD['PK_Device'].'" value="'.$rowD['MACaddress'].'">';
					}
					
					$buttons='';
					if($type=='avEquipment'){
						$buttons.='	<input type="button" class="button" name="btn" value="A/V Properties" onClick="windowOpen(\'index.php?section=editAVDevice&deviceID='.$rowD['FK_DeviceTemplate'].'&from='.urlencode('avWizard&type='.$type).'\',\'width=1024,height=768,toolbars=true,scrollbars=1,resizable=1\');"><br>
							<input type="button" class="button" name="btn" value="IR Codes" onClick="windowOpen(\'index.php?section=irCodes&from=avWizard&deviceID='.$rowD['PK_Device'].'&dtID='.$rowD['FK_DeviceTemplate'].'&from='.urlencode('avWizard&type='.$type).'\',\'width=1024,height=768,toolbars=true,scrollbars=1,resizable=1\');"><br>';
					}
					$buttons.='<input type="submit" class="button" name="delete_'.$rowD['PK_Device'].'" value="Delete"  ></td>';
					
					
					if($type!='media_directors'){
					$controlledByPulldown='
						<select name="controlledBy_'.$rowD['PK_Device'].'">
							<option value="0">- Select device -</option>';
	
					GetDeviceControlledVia($rowD['PK_Device'], $dbADO);
					if(count(@$GLOBALS['DeviceIDControlledVia'])>0){
						foreach($GLOBALS['DeviceIDControlledVia'] as $key => $value){
							$controlledByPulldown.='<option value="'.$value.'" '.(($rowD['FK_Device_ControlledVia']==$value)?'selected':'').'>'.$GLOBALS['DeviceControlledVia'][$key].'</option>';
						}
					}
					$controlledByPulldown.='</select>';
					unset($GLOBALS['DeviceIDControlledVia']);
					unset($GLOBALS['DeviceControlledVia']);
				}else 
					$controlledByPulldown='&nbsp;';

				$videoOutputPulldown='<select name="videoOutput_'.$rowD['PK_Device'].'" '.(($childOf[$rowD['PK_Device']]=='')?'':'disabled').'>
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
						$videoOutputPulldown.='<option value="'.$rowOutput['PK_Command'].'" '.(($rowOutput['PK_Command']==@$videoOutput)?'selected':'').'>'.$rowOutput['Description'].'</option>';
					}
					$resOutput->Close();
					$videoOutputPulldown.='</select>';
					
					
					$videoConnectToPulldown='<input type="hidden" name="oldVideoPipe_'.$rowD['PK_Device'].'" value="'.@$toDevice.','.$videoInput.','.$videoOutput.'">
						<a name="VideoPipe_'.$rowD['PK_Device'].'"></a>
					<select name="videoConnectTo_'.$rowD['PK_Device'].'" onChange="document.avWizard.cmd.value=1;document.forms[0].submit();" '.(($childOf[$rowD['PK_Device']]=='')?'':'disabled').'>
						<option value="0"></option>';
					foreach($conD AS $key=>$device){
						if($key!=$rowD['PK_Device'])
							$videoConnectToPulldown.='<option value="'.$key.'" '.(($key==@$toDevice)?'selected':'').'>'.$device.'</option>';					
					}
					$videoConnectToPulldown.='</select>';
					
					$videoInputPulldown='<select name="videoInput_'.$rowD['PK_Device'].'" '.(($childOf[$rowD['PK_Device']]=='')?'':'disabled').'>
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
					
				if($childOf[$rowD['PK_Device']]==''){	
					$out.='
				<tr>
					<td align="center"><a name="deviceLink_'.$rowD['PK_Device'].'"></a>'.$deviceName.'</td>
					<td>'.$roomPulldown.'</td>
					<td>A: '.$audioOutputPulldown.'</td>
					<td>'.$audioConnectToPulldown.'</td>
					<td>'.$audioInputPulldown.'</td>
					<td rowspan="2" valign="top" align="right">'.$deviceDataBox.'</td>
					<td align="center" rowspan="2" valign="top">'.$buttons.'</td>
				</tr>
				<tr>			
					<td align="center" title="Category: '.$rowD['CategoryName'].', manufacturer: '.$rowD['ManufacturerName'].'">DT: '.$rowD['TemplateName'].'</td>
					<td>'.$controlledByPulldown.'</td>
					<td>V: '.$videoOutputPulldown.'</td>
					<td>'.$videoConnectToPulldown.'</td>
					<td>'.$videoInputPulldown.'</td>
				</tr>';
					if(isset($embededRows[$rowD['PK_Device']])){
						foreach($embededRows[$rowD['PK_Device']] as $tuner){
							$out.=$tuner;
						}
					}
					$out.='
					<tr>
						<td colspan="7"><hr></td>
					</tr>';
				}else{
					$embededRows[$rowD['FK_Device_ControlledVia']][]='
				<tr>
					<td align="center"><a name="deviceLink_'.$rowD['PK_Device'].'"></a>'.$deviceName.'</td>
					<td>- Embeded device -</td>
					<td>A: '.$audioOutputPulldown.'</td>
					<td>'.$audioConnectToPulldown.'</td>
					<td>'.$audioInputPulldown.'</td>
					<td valign="top" align="right">- embedded device -</td>
					<td align="center" rowspan="2" valign="top">&nbsp;</td>
				</tr>
				<tr>			
					<td align="center" title="Category: '.$rowD['CategoryName'].', manufacturer: '.$rowD['ManufacturerName'].'">DT: '.$rowD['TemplateName'].'</td>
					<td>&nbsp;</td>
					<td>V: '.$videoOutputPulldown.'</td>
					<td>'.$videoConnectToPulldown.'</td>
					<td>'.$videoInputPulldown.'</td>
				</tr>';
				}
				if($type=='media_directors'){
					$orbiterMDChild=getMediaDirectorOrbiterChild($rowD['PK_Device'],$dbADO);
					if($orbiterMDChild){
						$out.='
						<tr>
							<td colspan="8">'.getInstallWizardDeviceTemplates(6,$dbADO,$orbiterMDChild,$mdDistro,1).'</td>
						</tr>';
					}
				}
			}
			$out.='
				<input type="hidden" name="DeviceDataToDisplay" value="'.join(',',$DeviceDataToDisplay).'">
				<input type="hidden" name="displayedDevices" value="'.join(',',array_keys($displayedDevices)).'">';
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
					<td colspan="8" align="center"><input type="button" class="button" name="button" value="Add device" onClick="document.avWizard.action.value=\'externalSubmit\';document.avWizard.submit();windowOpen(\'index.php?section=deviceTemplatePicker&allowAdd=1&from='.urlencode('avWizard&type='.$type).'&categoryID='.$deviceCategory.'\',\'width=800,height=600,toolbars=true,scrollbars=1,resizable=1\');"></td>
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
		
		
		$oldShareIRCodes=@(int)$_POST['oldShareIRCodes'];
		$coreID=@(int)$_POST['coreID'];
		$resShare=$dbADO->Execute('SELECT * FROM Device_StartupScript WHERE FK_Device=? AND FK_StartupScript=?',array($coreID,$GLOBALS['ShareIRCodes']));
		if(isset($_POST['shareIRCodes'])){
			if($oldShareIRCodes==0 && $coreID!=0){
				if($resShare->RecordCount()==0)
					$dbADO->Execute('INSERT INTO Device_StartupScript (FK_Device, FK_StartupScript, Enabled) VALUES (?,?,1)',array($coreID,$GLOBALS['ShareIRCodes']));
				else
					$dbADO->Execute('UPDATE Device_StartupScript SET Enabled=1 WHERE FK_Device=? AND FK_StartupScript=?',array($coreID,$GLOBALS['ShareIRCodes']));
			}
		}else{
			if($oldShareIRCodes!=0){
				$dbADO->Execute('UPDATE Device_StartupScript SET Enabled=0 WHERE FK_Device=? AND FK_StartupScript=?',array($coreID,$GLOBALS['ShareIRCodes']));
			}
		}

		$displayedDevicesArray=explode(',',@$_POST['displayedDevices']);
		foreach($displayedDevicesArray as $value){
			if(isset($_POST['delete_'.$value])){
				deleteDevice($value,$dbADO);
			}
		}
		
		if(isset($_POST['update']) || $cmd==1 || $action=='externalSubmit'){
			setDCERouterNeedConfigure($_SESSION['installationID'],$dbADO);
			$DeviceDataToDisplayArray=explode(',',@$_POST['DeviceDataToDisplay']);
			if(@$_POST['displayedDevices']!=''){
				foreach($displayedDevicesArray as $key => $value){
					$description=@$_POST['description_'.$value];
					if(isset($_POST['ip_'.$value])){
						$ip=$_POST['ip_'.$value];
						$mac=$_POST['mac_'.$value];
						$updateMacIp=",IPaddress='$ip', MACaddress='$mac'";
					}
									
					$room=(@$_POST['room_'.$value]!=0)?(int)@$_POST['room_'.$value]:NULL;
					$controlledBy=(@$_POST['controlledBy_'.$value]!=0)?(int)@$_POST['controlledBy_'.$value]:NULL;
					
					if(isset($_POST['controlledBy_'.$value])){
						$updateDevice='UPDATE Device SET Description=?, FK_Room=?,FK_Device_ControlledVia =? '.@$updateMacIp.' WHERE PK_Device=?';
						$dbADO->Execute($updateDevice,array($description,$room,$controlledBy,$value));
					}else{
						$updateDevice='UPDATE Device SET Description=? '.@$updateMacIp.' WHERE PK_Device=?';
						$dbADO->Execute($updateDevice,array($description,$value));
					}
	
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
					$oldAudioPipeArray=explode(',',@$_POST['oldAudioPipe_'.$value]);
					$oldTo=@$oldAudioPipeArray[0];
					$oldInput=@$oldAudioPipeArray[1];
					$oldOutput=@$oldAudioPipeArray[2];
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
	

					if(isset($_POST['videoOutput_'.$value])){
						
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
					}

					if($type=='media_directors'){
						$orbiterMDChild=getMediaDirectorOrbiterChild($value,$dbADO);
						$installOptionsArray=explode(',',@$_POST['displayedTemplatesRequired_'.$orbiterMDChild]);
						foreach($installOptionsArray AS $elem){
							$oldDevice=@$_POST['oldDevice_'.$orbiterMDChild.'_requiredTemplate_'.$elem];
							$optionalDevice=(isset($_POST['device_'.$orbiterMDChild.'_requiredTemplate_'.$elem]))?$_POST['device_'.$orbiterMDChild.'_requiredTemplate_'.$elem]:0;
							if($optionalDevice!=0){
								$OptionalDeviceName=cleanString(@$_POST['templateName_'.$elem]);
								
								if($oldDevice==''){
									$insertID=exec('/usr/pluto/bin/CreateDevice -h localhost -D '.$dbPlutoMainDatabase.' -d '.$elem.' -i '.$installationID.' -C '.$orbiterMDChild);
									$dbADO->Execute('UPDATE Device SET Description=?,FK_Room=? WHERE PK_Device=?',array($OptionalDeviceName,$room,$insertID));
								}
							}else{
								if($oldDevice!=''){
									deleteDevice($oldDevice,$dbADO);
								}
							}
						}
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
			header("Location: index.php?section=avWizard&type=$type&lastAdded=$deviceTemplate#deviceLink_".$insertID);
			exit();
		}
		
		
		header("Location: index.php?section=avWizard&type=$type".@$anchor);		
	}

	$output->setScriptCalendar('null');

	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.((isset($title))?' :: '.$title:' :: '.strtoupper(str_replace('_',' ',$type))));
	$output->output();
}
?>
