<?
function wizardOrbiters($output,$dbADO) {
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
//	$dbADO->debug=true;
	$userID = (int)@$_SESSION['userID'];
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$type = isset($_REQUEST['type'])?cleanString($_REQUEST['type']):'lights';
	$installationID = (int)@$_SESSION['installationID'];
	
	$deviceCategory=$GLOBALS['rootOrbiterID'];

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
	
	$queryRooms='SELECT * FROM Room ORDER BY Description ASC';
	$resRooms=$dbADO->Execute($queryRooms);
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
	<input type="hidden" name="section" value="wizardOrbiters">
	<input type="hidden" name="action" value="add">	
	<div align="center"><h3>Orbiters</h3></div>
		<table align="center" border="0">
				<tr>
					<td align="center"><B>DeviceTemplate</B></td>
					<td align="center"><B>Description</B></td>
					<td align="center"><B>Room</B></td>';
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
				$ddNo=0;
				while($rowDDD=$resDDD->FetchRow()){
					$ddNo++;
					$DeviceDataToDisplay[]=$rowDDD['PK_DeviceData'];
					$DDTypesToDisplay[]=$rowDDD['paramName'];
					$DeviceDataDescriptionToDisplay[]=$rowDDD['Description'];;
					$out.='<td align="center"><b>'.$rowDDD['Description'].'</b></td>';
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
								$queryTable="SELECT * FROM $tableName ORDER BY Description ASC";
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
					<td align="center"><input type="submit" name="delete_'.$rowD['PK_Device'].'" value="Delete"></td>
				</tr>';
			}
			$out.='
				<input type="hidden" name="DeviceDataToDisplay" value="'.join(',',$DeviceDataToDisplay).'">
				<input type="hidden" name="displayedDevices" value="'.join(',',$displayedDevices).'">';
			if($resDevice->RecordCount()!=0){
				$out.='
				<tr>
					<td colspan="'.(4+$ddNo).'" align="center"><input type="submit" name="update" value="Update"></td>
				</tr>';
			}
			$out.='
				<tr>
					<td colspan="'.(4+$ddNo).'">&nbsp;</td>
				</tr>
				<tr>
					<td colspan="'.(4+$ddNo).'" align="center"><select name="deviceTemplate">
						<option value="0">Select device template</option>';
			foreach($DTIDArray as $key => $value){
				$out.='<option value="'.$value.'">'.$DTArray[$key].'</option>';
			}
			$out.='</select>
					<input type="submit" name="add" value="Add device"></td>
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
		$displayedDevicesArray=explode(',',$_POST['displayedDevices']);
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
				
				$updateDevice='UPDATE Device SET FK_DeviceTemplate=?, Description=?, FK_Room=? WHERE PK_Device=?';
				$dbADO->Execute($updateDevice,array($deviceTemplate,$description,$room,$value));

				foreach($DeviceDataToDisplayArray as $ddValue){
					$deviceData=(isset($_POST['deviceData_'.$value.'_'.$ddValue]))?$_POST['deviceData_'.$value.'_'.$ddValue]:NULL;
					$oldDeviceData=$_POST['oldDeviceData_'.$value.'_'.$ddValue];
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
		
		if(isset($_POST['add'])){
			$deviceTemplate=(int)$_POST['deviceTemplate'];
			if($deviceTemplate!=0){
				$insertDevice='
					INSERT INTO Device
						(Description,FK_DeviceTemplate,FK_Installation)
					VALUES
						(?,?,?)';
				$dbADO->Execute($insertDevice,array('New',$deviceTemplate,$installationID));
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