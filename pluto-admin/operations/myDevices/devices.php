<?
function devices($output,$dbADO) {
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
//	$dbADO->debug=true;
	$userID = (int)@$_SESSION['userID'];
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$type = isset($_REQUEST['type'])?cleanString($_REQUEST['type']):'lights';
	$installationID = (int)@$_SESSION['installationID'];

	switch($type){
		case 'interfaces':
			$deviceCategory=$GLOBALS['rootInterfaces'];
		break;
		case 'avEquipment':
			$deviceCategory=$GLOBALS['rootAVEquipment'];
			$title='A/V Equipment';
		break;
		case 'lights':
			$deviceCategory=$GLOBALS['rootLights'];
			$specificFloorplanType=$GLOBALS['LightingFoorplanType'];
		break;
		case 'climate':
			$deviceCategory=$GLOBALS['rootClimate'];
			$specificFloorplanType=$GLOBALS['ClimateFoorplanType'];
		break;
		case 'security':
			$deviceCategory=$GLOBALS['rootSecurity'];
			$specificFloorplanType=$GLOBALS['SecurityFoorplanType'];
		break;
		case 'surveillance_cameras':
			$deviceCategory=$GLOBALS['rootCameras'];
			$specificFloorplanType=$GLOBALS['CameraFoorplanType'];
		break;
		default:
			$deviceCategory=$GLOBALS['rootLightsInterfaces'];
	}

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
	
	$queryRooms='SELECT * FROM Room WHERE FK_Installation=? ORDER BY Description ASC';
	$resRooms=$dbADO->Execute($queryRooms,$installationID);
	$roomIDArray=array();
	$roomArray=array();
	while($rowRoom=$resRooms->FetchRow()){
		$roomArray[]=$rowRoom['Description'];
		$roomIDArray[]=$rowRoom['PK_Room'];
	}
	
	if ($action == 'form') {
		$out.='
	<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
	</script>
	<div class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
	<form action="index.php" method="POST" name="devices">
	<input type="hidden" name="section" value="devices">
	<input type="hidden" name="type" value="'.$type.'">
	<input type="hidden" name="action" value="add">	
	<div align="center"><h3>'.((isset($title))?$title:strtoupper(str_replace('_',' ',$type))).'</h3></div>
		<table align="center">
				<tr>
					<td align="center"><B>DeviceTemplate</B></td>
					<td align="center"><B>Description</B></td>
					<td align="center"><B>Room</B></td>
					<td align="center"><B>Controlled by</B></td>';
				$displayedDevices=array();
				$DeviceDataToDisplay=array();
				$DeviceDataDescriptionToDisplay=array();
				$DDTypesToDisplay=array();	
				$joinArray=$DTIDArray;	// used only for query when there are no DT in selected category
				$joinArray[]=0;
				$queryDevice='
					SELECT 
						Device.* FROM Device 
					WHERE
						FK_DeviceTemplate IN ('.join(',',$joinArray).') AND FK_Installation=?';	
				$resDevice=$dbADO->Execute($queryDevice,$installationID);
				while($rowD=$resDevice->FetchRow()){
					$displayedDevices[]=$rowD['PK_Device'];
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
						$out.='<td align="center"><b>'.$rowDDD['Description'].'</b></td>';
					}
				}	
				
				$out.='<td align="center">&nbsp;</td>
				</tr>';				
			$resDevice->MoveFirst();
			while($rowD=$resDevice->FetchRow()){
				
				$out.='
				<tr>
					<td align="center"><select name="deviceTemplate_'.$rowD['PK_Device'].'">';
			foreach($DTIDArray as $key => $value){
				$out.='<option value="'.$value.'" '.(($rowD['FK_DeviceTemplate']==$value)?'selected':'').'>'.$DTArray[$key].'</option>';
			}
			$out.='</select></td>
					<td align="center"><input type="text" name="description_'.$rowD['PK_Device'].'" value="'.$rowD['Description'].'"></td>
					<td><select name="room_'.$rowD['PK_Device'].'">
						<option value="0"></option>';
				foreach($roomIDArray as $key => $value){
					$out.='<option value="'.$value.'" '.(($rowD['FK_Room']==$value)?'selected':'').'>'.$roomArray[$key].'</option>';
				}

			$out.='		</select></td>
					<td><select name="controlledBy_'.$rowD['PK_Device'].'">
						<option value="0"></option>';

			GetDeviceControlledVia($rowD['PK_Device'], $dbADO);
			foreach($GLOBALS['DeviceIDControlledVia'] as $key => $value){
				$out.='<option value="'.$value.'" '.(($rowD['FK_Device_ControlledVia']==$value)?'selected':'').'>'.$GLOBALS['DeviceControlledVia'][$key].'</option>';
			}
			unset($GLOBALS['DeviceIDControlledVia']);
			unset($GLOBALS['DeviceControlledVia']);
			
			$out.='</td>
					';
				foreach($DeviceDataToDisplay as $key => $value){
					$queryDDforDevice='
						SELECT 
							DeviceData.Description, 
							ParameterType.Description AS typeParam,
							Device_DeviceData.IK_DeviceData
						FROM DeviceData
							INNER JOIN ParameterType ON 
								FK_ParameterType = PK_ParameterType 
							INNER JOIN Device_DeviceData ON 
								FK_DeviceData=PK_DeviceData
						WHERE
							FK_Device = ? AND FK_DeviceData=?';

					$resDDforDevice=$dbADO->Execute($queryDDforDevice,array($rowD['PK_Device'],$value));

					if($resDDforDevice->RecordCount()>0){
						$rowDDforDevice=$resDDforDevice->FetchRow();
						$ddValue=$rowDDforDevice['IK_DeviceData'];
					}
					$out.='<td align="center">';
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
										$filterQuery=" WHERE FK_FloorplanType='".$specificFloorplanType."'";
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
					
					
					$out.='	</td>
						<input type="hidden" name="oldDeviceData_'.$rowD['PK_Device'].'_'.$value.'" value="'.(($resDDforDevice->RecordCount()>0)?$ddValue:'NULL').'">';					
					unset($ddValue);
				}
			$out.='	
					<td align="center">';
			if($type=='avEquipment')
				$out.='<input type="button" name="btn" value="IR Codes" onClick="windowOpen(\'index.php?section=irCodes&from=devices&deviceID='.$rowD['FK_DeviceTemplate'].'&from='.urlencode('devices&type='.$type).'\',\'width=800,height=600,toolbars=true,scrollbars=1,resizable=1\');"> <input type="button" name="btn" value="A/V Properties" onClick="windowOpen(\'index.php?section=editAVDevice&deviceID='.$rowD['FK_DeviceTemplate'].'&from='.urlencode('devices&type='.$type).'\',\'width=800,height=600,toolbars=true,scrollbars=1,resizable=1\');"> ';
					$out.='<input type="submit" name="delete_'.$rowD['PK_Device'].'" value="Delete"></td>
				</tr>';
			}
			$out.='
				<input type="hidden" name="DeviceDataToDisplay" value="'.join(',',$DeviceDataToDisplay).'">
				<input type="hidden" name="displayedDevices" value="'.join(',',$displayedDevices).'">';
			if($resDevice->RecordCount()!=0){
				$out.='
				<tr>
					<td colspan="5" align="center"><input type="submit" name="update" value="Update"></td>
				</tr>';
			}
			$out.='
				<tr>
					<td colspan="5">&nbsp;</td>
				</tr>
				<tr>
					<td colspan="5" align="center"><input type="button" name="button" value="Pick Device Template" onClick="windowOpen(\'index.php?section=deviceTemplatePicker&from='.urlencode('devices&type='.$type).'&categoryID='.$deviceCategory.'\',\'width=800,height=600,toolbars=true,scrollbars=1,resizable=1\');"></td>
				</tr>
			</table>
		</form>
		<script>
		 	var frmvalidator = new formValidator("devices");
 //			frmvalidator.addValidation("Description","req","Please enter a device description");			
//	 		frmvalidator.addValidation("masterDevice","dontselect=0","Please select a Device Template!");			
		</script>
	
	</form>
	';
	} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=devices&type=$type&error=You are not authorised to change the installation.");
			exit(0);
		}
		$displayedDevicesArray=explode(',',@$_POST['displayedDevices']);
		foreach($displayedDevicesArray as $value){
			if(isset($_POST['delete_'.$value])){
				$deleteDevice='DELETE FROM Device WHERE PK_Device=?';
				$dbADO->Execute($deleteDevice,$value); 
			}
		}
		
		if(isset($_POST['update'])){
			$DeviceDataToDisplayArray=explode(',',$_POST['DeviceDataToDisplay']);
			foreach($displayedDevicesArray as $key => $value){
				$deviceTemplate=(int)@$_POST['deviceTemplate_'.$value];
				$description=@$_POST['description_'.$value];
				$room=(@$_POST['room_'.$value]!=0)?(int)@$_POST['room_'.$value]:NULL;
				$controlledBy=(@$_POST['controlledBy_'.$value]!=0)?(int)@$_POST['controlledBy_'.$value]:NULL;
				
				$updateDevice='UPDATE Device SET FK_DeviceTemplate=?, Description=?, FK_Room=?,FK_Device_ControlledVia =? WHERE PK_Device=?';
				$dbADO->Execute($updateDevice,array($deviceTemplate,$description,$room,$controlledBy,$value));

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
			}
		}
		
		if(isset($_REQUEST['add'])){
			unset($_SESSION['from']);
			$deviceTemplate=(int)$_REQUEST['deviceTemplate'];
			if($deviceTemplate!=0){
				$insertDevice='
					INSERT INTO Device
						(Description,FK_DeviceTemplate,FK_Installation)
					SELECT Description, '.$deviceTemplate.','.$installationID.' FROM DeviceTemplate WHERE PK_DeviceTemplate=?';
				$dbADO->Execute($insertDevice,$deviceTemplate);
				$insertID=$dbADO->Insert_ID();
				InheritDeviceData($deviceTemplate,$insertID,$dbADO);
			}
		}
		header("Location: index.php?section=devices&type=$type");		
	}

	$output->setScriptCalendar('null');

	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}
?>