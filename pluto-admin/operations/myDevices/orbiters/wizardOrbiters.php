<?
function wizardOrbiters($output,$dbADO) {
	global $dbPlutoMainDatabase;
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
//	$dbADO->debug=true;
	$userID = (int)@$_SESSION['userID'];
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
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
	<h3  align="left">Orbiters</h3>
		<table border="0">';
				$displayedDevices=array();
				$DeviceDataToDisplay=array();
				$DeviceDataDescriptionToDisplay=array();
				$DDTypesToDisplay=array();	
				$joinArray=$DTIDArray;	// used only for query when there are no DT in selected category
				$joinArray[]=0;
				$queryDevice='
					SELECT 
						Device.*, DeviceTemplate.Description AS TemplateName, DeviceCategory.Description AS CategoryName,Manufacturer.Description AS ManufacturerName,IsIPBased
					FROM Device 
						INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
						INNER JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory
						INNER JOIN Manufacturer ON FK_Manufacturer=PK_Manufacturer
					WHERE
						FK_DeviceTemplate IN ('.join(',',$joinArray).') AND FK_Installation=?';	
				$resDevice=$dbADO->Execute($queryDevice,$installationID);
				while($rowD=$resDevice->FetchRow()){
					$displayedDevices[]=$rowD['PK_Device'];
				}
				$joinArray=$displayedDevices;	// temp array used only for query when there are no Devices in selected category
				// WARNING: hard-coded values
				$joinArray[]=3;			// default user
				$joinArray[]=20;		// No effects
				$joinArray[]=21;		// Main menu
				$joinArray[]=22;		// Sleeping menu
				$joinArray[]=23;		// Screen saver menu
				$joinArray[]=24;		// Skin
				$joinArray[]=25;		// Size
				$joinArray[]=26;		// Language
			
				$queryDeviceDeviceData='
					SELECT 
						DeviceData.PK_DeviceData,DeviceData.Description, ParameterType.Description AS paramName
					FROM DeviceData
						INNER JOIN ParameterType ON 
							FK_ParameterType = PK_ParameterType 
					WHERE
						PK_DeviceData IN ('.join(',',$joinArray).')
					ORDER BY Description ASC';
				$resDDD=$dbADO->Execute($queryDeviceDeviceData);
				while($rowDDD=$resDDD->FetchRow()){
					$DeviceDataToDisplay[]=$rowDDD['PK_DeviceData'];
					$DDTypesToDisplay[]=$rowDDD['paramName'];
					$DeviceDataDescriptionToDisplay[]=$rowDDD['Description'];;
				}	
				

			$resDevice->MoveFirst();
			$orbiterCount=0;
			while($rowD=$resDevice->FetchRow()){
				$orbiterCount++;
				$out.=($orbiterCount%2==1)?'<tr>':'';
				
				$out.='<td bgcolor="'.(($orbiterCount%4==1 || $orbiterCount%4==2)?'#F0F3F8':'').'">
				<table align="center" border="0">
				<tr>
					<td align="right"><B>DeviceTemplate</B></td>
					<td align="left" title="Category: '.$rowD['CategoryName'].', manufacturer: '.$rowD['ManufacturerName'].'">'.$rowD['TemplateName'].'</td>
				</tr>
				<tr>
					<td align="right"><B>Description</B></td>
					<td align="left"><input type="text" name="description_'.$rowD['PK_Device'].'" value="'.stripslashes($rowD['Description']).'"></td>
				</tr>';
				if($rowD['IsIPBased']==1){
					$out.='
						<tr>
							<td>IP</td>
							<td><input type="text" name="ip_'.$rowD['PK_Device'].'" value="'.$rowD['IPaddress'].'"></td>
						</tr>
						<tr>
							<td>MAC</td>
							<td><input type="text" name="mac_'.$rowD['PK_Device'].'" value="'.$rowD['MACaddress'].'"></td>
						</tr>';
				}				
			$out.='
				<tr>
					<td align="right"><B>Room</B></td>
					<td><select name="room_'.$rowD['PK_Device'].'">
										<option value="0"></option>';
				foreach($roomIDArray as $key => $value){
					$out.='<option value="'.$value.'" '.(($rowD['FK_Room']==$value)?'selected':'').'>'.$roomArray[$key].'</option>';
				}

			$out.='		</select></td>
				</tr>
					';
				foreach($DeviceDataToDisplay as $key => $value){
					$queryDDforDevice='
						SELECT DeviceData.Description, ParameterType.Description AS typeParam, Device_DeviceData.IK_DeviceData,ShowInWizard,ShortDescription,AllowedToModify,DeviceTemplate_DeviceData.Description AS Tooltip 
						FROM DeviceData 
						INNER JOIN ParameterType ON FK_ParameterType = PK_ParameterType 
						INNER JOIN Device_DeviceData ON Device_DeviceData.FK_DeviceData=PK_DeviceData 
						INNER JOIN Device ON FK_Device=PK_Device
						LEFT JOIN DeviceTemplate_DeviceData ON DeviceTemplate_DeviceData.FK_DeviceData=Device_DeviceData.FK_DeviceData AND DeviceTemplate_DeviceData.FK_DeviceTemplate=Device.FK_DeviceTemplate
						WHERE FK_Device = ? AND Device_DeviceData.FK_DeviceData=? ';

					$resDDforDevice=$dbADO->Execute($queryDDforDevice,array($rowD['PK_Device'],$value));

					if($resDDforDevice->RecordCount()>0){
						$rowDDforDevice=$resDDforDevice->FetchRow();
						$ddValue=$rowDDforDevice['IK_DeviceData'];
					}
					if($rowDDforDevice['ShowInWizard']==1 || $rowDDforDevice['ShowInWizard']==''){
						$out.='
						<tr>
							<td align="right"><b>'.((@$rowDDforDevice['ShortDescription']!='')?$rowDDforDevice['ShortDescription']:$DeviceDataDescriptionToDisplay[$key]).'</b></td>
							<td align="left">';
						switch($DDTypesToDisplay[$key]){
							case 'int':
								if(in_array($DeviceDataDescriptionToDisplay[$key],$GLOBALS['DeviceDataLinkedToTables']))
								{
									$tableName=str_replace('PK_','',$DeviceDataDescriptionToDisplay[$key]);
									if($tableName!='Users')
										$queryTable="SELECT * FROM $tableName ORDER BY Description ASC";
									else
										$queryTable="SELECT Users.*, Users.Username AS Description FROM Users ORDER BY Description ASC";
									$resTable=$dbADO->Execute($queryTable);
									$out.='<select name="deviceData_'.$rowD['PK_Device'].'_'.$value.'" '.((isset($rowDDforDevice['AllowedToModify']) && $rowDDforDevice['AllowedToModify']==0)?'disabled':'').'>
											<option value="0"></option>';
									while($rowTable=$resTable->FetchRow()){
										$out.='<option value="'.$rowTable[$DeviceDataDescriptionToDisplay[$key]].'" '.(($rowTable[$DeviceDataDescriptionToDisplay[$key]]==@$ddValue)?'selected':'').'>'.$rowTable['Description'].'</option>';
									}
									$out.='</select>';
								}
								else 
									$out.='<input type="text" name="deviceData_'.$rowD['PK_Device'].'_'.$value.'" value="'.@$ddValue.'" '.((isset($rowDDforDevice['AllowedToModify']) && $rowDDforDevice['AllowedToModify']==0)?'disabled':'').'>';
							break;
							case 'bool':
								$out.='<input type="checkbox" name="deviceData_'.$rowD['PK_Device'].'_'.$value.'" value="1" '.((@$ddValue!=0)?'checked':'').' '.((isset($rowDDforDevice['AllowedToModify']) && $rowDDforDevice['AllowedToModify']==0)?'disabled':'').'>';
							break;
							default:
								$out.='<input type="text" name="deviceData_'.$rowD['PK_Device'].'_'.$value.'" value="'.@$ddValue.'" '.((isset($rowDDforDevice['AllowedToModify']) && $rowDDforDevice['AllowedToModify']==0)?'disabled':'').'>';
						}
						
						
						$out.='	<img src="include/images/tooltip.gif" title="'.$rowDDforDevice['Tooltip'].'" border="0" align="middle"></td>
							</tr>
							<input type="hidden" name="oldDeviceData_'.$rowD['PK_Device'].'_'.$value.'" value="'.(($resDDforDevice->RecordCount()>0)?$ddValue:'NULL').'">';					
						unset($ddValue);
					}
				}
			$out.='	
				<tr>
					<td align="center" colspan="2"><input type="submit" class="button" name="quickRegen_'.$rowD['PK_Device'].'" value="Quick regen"  >&nbsp;&nbsp;<input type="submit" class="button" name="fullRegen_'.$rowD['PK_Device'].'" value="Full regen"  >&nbsp;&nbsp;<input type="submit" class="button" name="update" value="Update"  > &nbsp;&nbsp;<input type="submit" class="button" name="delete_'.$rowD['PK_Device'].'" value="Delete"  >
					
					
					</td>
					</tr>
				</table></td>';
				$out.=($orbiterCount%2==0)?'</tr>
					<tr>
						<td colspan="2">&nbsp;</td>
					</tr>':'';
			}
			$out.='
				<input type="hidden" name="DeviceDataToDisplay" value="'.join(',',$DeviceDataToDisplay).'">
				<input type="hidden" name="displayedDevices" value="'.join(',',$displayedDevices).'">';
			$out.='
			</table>
			<table align="center" border="0">
				<tr>
					<td colspan="2">&nbsp;</td>
				</tr>
				<tr>
					<td colspan="2" align="center"><select name="deviceTemplate">
						<option value="0">Select device template</option>';
			foreach($DTIDArray as $key => $value){
				$out.='<option value="'.$value.'">'.$DTArray[$key].'</option>';
			}
			$out.='</select>
					<input type="submit" class="button" name="add" value="Add orbiter"  ></td>
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
			header("Location: index.php?section=devices&error=You are not authorised to change the installation.");
			exit(0);
		}
		$displayedDevicesArray=explode(',',$_POST['displayedDevices']);
		foreach($displayedDevicesArray as $value){
			if(isset($_POST['delete_'.$value])){
				$deleteDevice='DELETE FROM Device WHERE PK_Device=?';
				$dbADO->Execute($deleteDevice,$value); 
			}
			if(isset($_POST['quickRegen_'.$value])){
				$updateOrbiter='UPDATE Orbiter SET Regen=1 WHERE PK_Orbiter=?';
				$dbADO->Execute($updateOrbiter,$value); 
				$updateOrbiters=true;
			}
			if(isset($_POST['fullRegen_'.$value])){
				$updateOrbiter='UPDATE Orbiter SET Modification_LastGen=0 WHERE PK_Orbiter=?';
				$dbADO->Execute($updateOrbiter,$value); 
				$updateOrbiters=true;
			}
		}
		
		if(isset($_POST['update']) || isset($updateOrbiters)){
			$DeviceDataToDisplayArray=explode(',',$_POST['DeviceDataToDisplay']);
			foreach($displayedDevicesArray as $key => $value){
				$deviceTemplate=(int)@$_POST['deviceTemplate_'.$value];
				$description=@$_POST['description_'.$value];
				if(isset($_POST['ip_'.$value])){
					$ip=$_POST['ip_'.$value];
					$mac=$_POST['mac_'.$value];
					$updateMacIp=",IPaddress='$ip', MACaddress='$mac'";
				}
				$room=(@$_POST['room_'.$value]!=0)?(int)@$_POST['room_'.$value]:NULL;
				
				$updateDevice='UPDATE Device SET Description=?, FK_Room=? '.@$updateMacIp.' WHERE PK_Device=?';
				$dbADO->Execute($updateDevice,array($description,$room,$value));

				foreach($DeviceDataToDisplayArray as $ddValue){
					$deviceData=(isset($_POST['deviceData_'.$value.'_'.$ddValue]))?$_POST['deviceData_'.$value.'_'.$ddValue]:'';
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
				$insertID=exec('/usr/pluto/bin/CreateDevice -h localhost -D '.$dbPlutoMainDatabase.' -d '.$deviceTemplate.' -i '.$installationID);				
			}
		}
		header("Location: index.php?section=wizardOrbiters");		
	}

	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Orbiters');
	$output->output();
}
