<?
function genericSerialDevices($output,$dbADO) {
	global $dbPlutoMainDatabase;
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
//	$dbADO->debug=true;
	$userID = (int)@$_SESSION['userID'];
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];

	$deviceCategory=$GLOBALS['GenericSerialDevices'];

	
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

			$portsArray=array();
			$resPorts=$dbADO->Execute('SELECT * FROM Device_DeviceData WHERE FK_Device=? AND FK_DeviceData=?',array($coreID,$GLOBALS['Port']));
			if($resPorts->RecordCount()>0){
				$rowPorts=$resPorts->FetchRow();
				$partsArray=explode(';',$rowPorts['IK_DeviceData']);
				foreach($partsArray AS $part){
					$secondParts=explode('|',$part);
					if(count($secondParts)==2)
					$portsArray[$secondParts[0]]=$secondParts[1];
					else
					$portsArray[$part]=$part;
				}
			}
		}
		
		$out.=setLeftMenu($dbADO).'
	<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
	</script>
	<div class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
	<div class="confirm" align="center"><B>'.strip_tags(@$_GET['msg']).'</B></div>
	<form action="index.php" method="POST" name="genericSerialDevices">
	<input type="hidden" name="section" value="genericSerialDevices">
	<input type="hidden" name="action" value="add">
	<input type="hidden" name="cmd" value="0">			
	<div align="center"><h3>Generic Serial Devices</h3></div>';
			
		$out.='
		<table align="center" border="0" cellpadding="2" cellspacing="0">
				<tr>
					<td align="center"><B>Device</B></td>
					<td align="center"><B>Device template</B></td>
					<td align="center"><B>Room</b>
					<td><B>Controlled by</B></td>
					<td align="center"><B>Data</B></td>
					<td align="center"><B>Actions</B></td>
				</tr>
					';
				$displayedAVDevices=array();
				$displayedAVDevicesDescription=array();
				$queryDevice='
					SELECT Device.*,ImplementsDCE
					FROM Device 
					INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
					WHERE CommandLine=? AND FK_Installation=?';	
				$resDevice=$dbADO->Execute($queryDevice,array($GLOBALS['GenericSerialDeviceCommandLine'],$installationID));
				while($rowD=$resDevice->FetchRow()){
					$displayedAVDevices[]=$rowD['PK_Device'];
					$displayedAVDevicesDescription[]=$rowD['Description'];
					
					$qdev=$dbADO->Execute('
						SELECT Device.* 
						FROM DeviceTemplate_DeviceTemplate_ControlledVia 
						INNER JOIN DeviceTemplate ON DeviceTemplate_DeviceTemplate_ControlledVia.FK_DeviceTemplate=PK_DeviceTemplate
						INNER JOIN Device ON Device.FK_DeviceTemplate=DeviceTemplate_DeviceTemplate_ControlledVia.FK_DeviceTemplate
						WHERE FK_DeviceTemplate_ControlledVia = ? AND ImplementsDCE=1',$rowD['FK_DeviceTemplate']);
					if($qdev->RecordCount()>0){
						while($rowDev=$qdev->FetchRow()){
							$displayedAVDevices[]=$rowDev['PK_Device'];
							$displayedAVDevicesDescription[]=$rowDev['Description'];
							$gsdChilds=array();
							$gsdChilds=getChildDevices($gsdChilds,$rowDev['PK_Device'],$dbADO,'INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE FK_Device_ControlledVia=? AND ImplementsDCE=1');
							foreach ($gsdChilds AS $devID=>$devName){
								$displayedAVDevices[]=$devID;
								$displayedAVDevicesDescription[]=$devName;
							}
						}
					}
				}
				$resDevice->Close();

				if(count($displayedAVDevices)==0)
					$displayedAVDevices[]=0;
				$displayedDevices=array();
				$DeviceDataToDisplay=array();
				$DDTypesToDisplay=array();	
							$queryDevice='
					SELECT 
						Device.*, DeviceTemplate.Description AS TemplateName, DeviceCategory.Description AS CategoryName,Manufacturer.Description AS ManufacturerName,IsIPBased,DeviceTemplate_AV.UsesIR
					FROM Device 
						INNER JOIN DeviceTemplate ON Device.FK_DeviceTemplate=PK_DeviceTemplate
						LEFT JOIN DeviceTemplate_AV ON Device.FK_DeviceTemplate=DeviceTemplate_AV.FK_DeviceTemplate
						INNER JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory
						INNER JOIN Manufacturer ON FK_Manufacturer=PK_Manufacturer
					WHERE PK_Device IN ('.join(',',$displayedAVDevices).')';	
				$resDevice=$dbADO->Execute($queryDevice);
				$childOf=array();
				while($rowD=$resDevice->FetchRow()){
					if($rowD['FK_Device_ControlledVia']==$rowD['FK_Device_RouteTo'])
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
									if($DeviceDataDescriptionToDisplay[$key]=='Port'){
										$deviceDataBox.='<select name="deviceData_'.$rowD['PK_Device'].'_'.$value.'">
											<option value="">- Please select -</option>';
										foreach ($portsArray AS $portValue=>$portLabel){
											$deviceDataBox.='<option value="'.$portValue.'" '.((@$ddValue==$portValue)?'selected':'').'>'.$portLabel.'</option>';
										}
										'</select>';
									}else{
										$deviceDataBox.='<input type="text" name="deviceData_'.$rowD['PK_Device'].'_'.$value.'" value="'.@$ddValue.'" '.((isset($rowDDforDevice['AllowedToModify']) && $rowDDforDevice['AllowedToModify']==0)?'disabled':'').'>';
									}
							}
							$mdDistro=($value==$GLOBALS['rootPK_Distro'])?@$ddValue:0;
							
							$deviceDataBox.='	
								<input type="hidden" name="oldDeviceData_'.$rowD['PK_Device'].'_'.$value.'" value="'.(($resDDforDevice->RecordCount()>0)?$ddValue:'NULL').'">';					
							unset($ddValue);
							$deviceDataBox.='<br>';
						}
					}
					if($rowD['IsIPBased']==1){
						$deviceDataBox.='<B>IP</B> <input type="text" name="ip_'.$rowD['PK_Device'].'" value="'.$rowD['IPaddress'].'"><br>
								<B>MAC</B> <input type="text" name="mac_'.$rowD['PK_Device'].'" value="'.$rowD['MACaddress'].'">';
					}
					
					$buttons='';
					$buttons.='	<input type="button" class="button" name="btn" value="Ruby Source Code" onClick="windowOpen(\'index.php?section=irCodes&from=genericSerialDevices&deviceID='.$rowD['PK_Device'].'&dtID='.$rowD['FK_DeviceTemplate'].'&from=genericSerialDevices&label=ruby\',\'width=1024,height=768,toolbars=true,scrollbars=1,resizable=1\');"> ';
					$buttons.=' <input type="submit" class="button" name="delete_'.$rowD['PK_Device'].'" value="Delete"  onclick="return confirm(\'Are you sure you want to delete this device?\');"></td>';
					
					$controlledByPulldown='<select name="controlledBy_'.$rowD['PK_Device'].'">
						<option value="0"></option>';
					GetDeviceControlledVia($rowD['PK_Device'], $dbADO);
					if(count(@$GLOBALS['DeviceIDControlledVia'])>0){
						foreach($GLOBALS['DeviceIDControlledVia'] as $key => $value){
							$controlledByPulldown.='<option value="'.$value.'" '.(($rowD['FK_Device_ControlledVia']==$value)?'selected':'').'>'.$GLOBALS['DeviceControlledVia'][$key].'</option>';
						}
					}
					$controlledByPulldown.='</select>';
					unset($GLOBALS['DeviceIDControlledVia']);
					unset($GLOBALS['DeviceControlledVia']);

	
					
			$out.='
				<tr>
					<td align="center"><a name="deviceLink_'.$rowD['PK_Device'].'"></a>'.$deviceName.'</td>
					<td align="center" title="Category: '.$rowD['CategoryName'].', manufacturer: '.$rowD['ManufacturerName'].'">DT: '.$rowD['TemplateName'].'</td>
					<td  align="right">'.$roomPulldown.'</td>
					<td align="right">'.$controlledByPulldown.'</td>
					<td valign="top" align="right">'.$deviceDataBox.'</td>
					<td align="center" valign="top">'.$buttons.'</td>
				</tr>';
			$out.='
					<tr>
						<td colspan="8"><hr></td>
					</tr>';					
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
					<td colspan="8" align="center"><input type="button" class="button" name="button" value="Add device" onClick="document.genericSerialDevices.action.value=\'externalSubmit\';document.genericSerialDevices.submit();windowOpen(\'index.php?section=deviceTemplatePicker&allowAdd=1&from=genericSerialDevices\',\'width=800,height=600,toolbars=true,scrollbars=1,resizable=1\');"></td>
				</tr>
			</table>
		</form>
		<script>
		 	var frmvalidator = new formValidator("genericSerialDevices");
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
			header("Location: index.php?section=genericSerialDevices&error=You are not authorised to change the installation.");
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
					$description=stripslashes(@$_POST['description_'.$value]);
					if(isset($_POST['ip_'.$value])){
						$ip=$_POST['ip_'.$value];
						$mac=$_POST['mac_'.$value];
						$updateMacIp=",IPaddress='$ip', MACaddress='$mac'";
					}
									
					$room=(@$_POST['room_'.$value]!=0)?(int)@$_POST['room_'.$value]:NULL;
					$controlledBy=(@$_POST['controlledBy_'.$value]!=0)?(int)@$_POST['controlledBy_'.$value]:NULL;

					if(isset($_POST['controlledBy_'.$value])){
						$updateDevice='UPDATE Device SET Description=?, FK_Room=?, FK_Device_ControlledVia=? '.@$updateMacIp.' WHERE PK_Device=?';
						$dbADO->Execute($updateDevice,array($description,$room,$controlledBy,$value));
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


				}
			}
			
			$commandToSend='/usr/pluto/bin/UpdateEntArea -h localhost';
			exec($commandToSend);
		}
				
		if(isset($_REQUEST['add'])){
			unset($_SESSION['from']);
			$deviceTemplate=(int)$_REQUEST['deviceTemplate'];
			if($deviceTemplate!=0){
				$insertID=exec('/usr/pluto/bin/CreateDevice -h localhost -D '.$dbPlutoMainDatabase.' -d '.$deviceTemplate.' -i '.$installationID);	
				setDCERouterNeedConfigure($_SESSION['installationID'],$dbADO);
				$commandToSend='/usr/pluto/bin/UpdateEntArea -h localhost';
				exec($commandToSend);
			}
			header("Location: index.php?section=genericSerialDevices&lastAdded=$deviceTemplate#deviceLink_".$insertID);
			exit();
		}
		
		
		header("Location: index.php?section=genericSerialDevices&msg=The devices was updated".@$anchor);		
	}

	$output->setScriptCalendar('null');

	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Generic Serial Devices');
	$output->output();
}

function getChildDevices($childs,$deviceID,$dbADO,$filter){
	if($deviceID==''){
		return $childs;
	}
	$res=$dbADO->Execute('
		SELECT * 
		FROM Device '.$filter,$deviceID);
	while($row=$res->FetchRow()){
		$childs[$row['PK_Device']]=$row['Description'];
		$childs=getChildDevices($childs,$row['PK_Device'],$dbADO,$filter);
	}
	
	return $childs;
}
?>
