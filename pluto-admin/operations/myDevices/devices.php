<?
function devices($output,$dbADO) {
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
		case 'interfaces':
			$output->setHelpSrc('/support/index.php?section=document&docID=126');
			$deviceCategory=$GLOBALS['rootInterfaces'];
		break;
		case 'avEquipment':
			$deviceCategory=$GLOBALS['rootAVEquipment'];
			$title='A/V Equipment';
		break;
		case 'lights':
			$output->setHelpSrc('/support/index.php?section=document&docID=128');
			$deviceCategory=$GLOBALS['rootLights'];
			$specificFloorplanType=$GLOBALS['LightingFoorplanType'];
		break;
		case 'climate':
			$output->setHelpSrc('/support/index.php?section=document&docID=130');
			$deviceCategory=$GLOBALS['rootClimate'];
			$specificFloorplanType=$GLOBALS['ClimateFoorplanType'];
		break;
		case 'security':
			$output->setHelpSrc('/support/index.php?section=document&docID=127');
			$deviceCategory=$GLOBALS['rootSecurity'];
			$specificFloorplanType=$GLOBALS['SecurityFoorplanType'];
		break;
		case 'surveillance_cameras':
			$output->setHelpSrc('/support/index.php?section=document&docID=147');
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
	<form action="index.php" method="POST" name="devices">
	<input type="hidden" name="section" value="devices">
	<input type="hidden" name="type" value="'.$type.'">
	<input type="hidden" name="action" value="add">	
	<div align="center"><h3>'.((isset($title))?$title:strtoupper(str_replace('_',' ',$type))).'</h3></div>
		<table align="center" cellpadding="3" cellspacing="0">
				<tr>
					<td align="center"><B>Type of Device</B></td>
					<td align="center"><B>Description</B></td>
					<td align="center"><B>Room</B></td>
					<td align="center"><B>Controlled by</B></td>
					<td align="center"><B>Device Data</B></td>';
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
						//$out.='<td align="center"><b>'.$rowDDD['Description'].'</b></td>';
					}
				}	
				
				$out.='<td align="center">&nbsp;</td>
				</tr>';				
			$resDevice->MoveFirst();
			$pos=0;
			while($rowD=$resDevice->FetchRow()){
				$pos++;
				$out.='
				<tr bgcolor="'.(($pos%2==1)?'#EEEEEE':'').'">
					<td align="center" title="Category: '.$rowD['CategoryName'].', manufacturer: '.$rowD['ManufacturerName'].'"><B>'.$rowD['TemplateName'].'</B>';
				if($rowD['IsIPBased']==1){
					$out.='<table>
						<tr>
							<td>IP</td>
							<td><input type="text" name="ip_'.$rowD['PK_Device'].'" value="'.$rowD['IPaddress'].'"></td>
						</tr>
						<tr>
							<td>MAC</td>
							<td><input type="text" name="mac_'.$rowD['PK_Device'].'" value="'.$rowD['MACaddress'].'"></td>
						</tr>
					</table>';
				}
				$out.='
					</td>
					<td align="center" ><input type="text" name="description_'.$rowD['PK_Device'].'" value="'.$rowD['Description'].'"></td>
					<td ><select name="room_'.$rowD['PK_Device'].'">
						<option value="0"></option>';
				foreach($roomIDArray as $key => $value){
					$out.='<option value="'.$value.'" '.(($rowD['FK_Room']==$value)?'selected':'').'>'.$roomArray[$key].'</option>';
				}

			$out.='		</select></td>
					<td ><select name="controlledBy_'.$rowD['PK_Device'].'">
						<option value="0"></option>';

			GetDeviceControlledVia($rowD['PK_Device'], $dbADO);
			foreach($GLOBALS['DeviceIDControlledVia'] as $key => $value){
				$out.='<option value="'.$value.'" '.(($rowD['FK_Device_ControlledVia']==$value)?'selected':'').'>'.$GLOBALS['DeviceControlledVia'][$key].'</option>';
			}
			unset($GLOBALS['DeviceIDControlledVia']);
			unset($GLOBALS['DeviceControlledVia']);
			
			$out.='</td>
					';
				$out.='<td align="right" >';
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

					if($resDDforDevice->RecordCount()>0){
						$rowDDforDevice=$resDDforDevice->FetchRow();
						$ddValue=$rowDDforDevice['IK_DeviceData'];
					}
					if(($rowDDforDevice['ShowInWizard']==1 || $rowDDforDevice['ShowInWizard']=='') && $resDDforDevice->RecordCount()>0 && $value!=$GLOBALS['securityAlert']){
						$out.='<b>'.((@$rowDDforDevice['ShortDescription']!='')?$rowDDforDevice['ShortDescription']:$DeviceDataDescriptionToDisplay[$key]).'</b> ';
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
									$out.='<select name="deviceData_'.$rowD['PK_Device'].'_'.$value.'" '.((isset($rowDDforDevice['AllowedToModify']) && $rowDDforDevice['AllowedToModify']==0)?'disabled':'').'>
											<option value="0">- Please select -</option>';
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
						
						
						$out.=' <img src="include/images/tooltip.gif" title="'.$rowDDforDevice['Tooltip'].'" border="0" align="middle"><br>	
							<input type="hidden" name="oldDeviceData_'.$rowD['PK_Device'].'_'.$value.'" value="'.(($resDDforDevice->RecordCount()>0)?$ddValue:'NULL').'">';					
						unset($ddValue);
					}
				}
			$out.='	</td>
					<td align="center">';
					$out.='<input type="submit" class="button" name="delete_'.$rowD['PK_Device'].'" value="Delete"  ></td>
				</tr>';
			}
			$out.='
				<input type="hidden" name="DeviceDataToDisplay" value="'.join(',',$DeviceDataToDisplay).'">
				<input type="hidden" name="displayedDevices" value="'.join(',',$displayedDevices).'">';
			if($resDevice->RecordCount()!=0){
				$out.='
				<tr>
					<td colspan="5" align="center"><input type="submit" class="button" name="update" value="Update"  ></td>
				</tr>';
			}
			$out.='
				<tr>
					<td colspan="5">&nbsp;</td>
				</tr>
				<tr>
					<td colspan="5" align="center"><input type="button" class="button" name="button" value="Add device" onClick="windowOpen(\'index.php?section=deviceTemplatePicker&from='.urlencode('devices&type='.$type).'&categoryID='.$deviceCategory.'\',\'width=800,height=600,toolbars=true,scrollbars=1,resizable=1\');"></td>
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
			setDCERouterNeedConfigure($installationID,$dbADO);
			$DeviceDataToDisplayArray=explode(',',$_POST['DeviceDataToDisplay']);
			foreach($displayedDevicesArray as $key => $value){
				$description=@$_POST['description_'.$value];
				if(isset($_POST['ip_'.$value])){
					$ip=$_POST['ip_'.$value];
					$mac=$_POST['mac_'.$value];
					$updateMacIp=",IPaddress='$ip', MACaddress='$mac'";
				}
				$room=(@$_POST['room_'.$value]!=0)?(int)@$_POST['room_'.$value]:NULL;
				$controlledBy=(@$_POST['controlledBy_'.$value]!=0)?(int)@$_POST['controlledBy_'.$value]:NULL;
				
				$updateDevice='UPDATE Device SET Description=?, FK_Room=?,FK_Device_ControlledVia =? '.@$updateMacIp.' WHERE PK_Device=?';
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
			}
		}
		
		if(isset($_REQUEST['add'])){
			unset($_SESSION['from']);
			$deviceTemplate=(int)$_REQUEST['deviceTemplate'];
			if($deviceTemplate!=0){
				$insertID=exec('/usr/pluto/bin/CreateDevice -h localhost -D '.$dbPlutoMainDatabase.' -d '.$deviceTemplate.' -i '.$installationID);				
				setDCERouterNeedConfigure($installationID,$dbADO);
			}
			header("Location: index.php?section=devices&type=$type&lastAdded=$deviceTemplate");
			exit();
		}
		header("Location: index.php?section=devices&type=$type");		
	}

	$output->setScriptCalendar('null');

	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}
?>
