<?
function mediaDirectors($output,$dbADO) {
	global $dbPlutoMainDatabase;
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
//	$dbADO->debug=true;
	$userID = (int)@$_SESSION['userID'];
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];

	$pvrArray=getAssocArray('DeviceTemplate','PK_DeviceTemplate','Description',$dbADO,' WHERE FK_DeviceCategory='.$GLOBALS['PVRCaptureCards'],'ORDER BY Description ASC');
	$soundArray=getAssocArray('DeviceTemplate','PK_DeviceTemplate','Description',$dbADO,' WHERE FK_DeviceCategory='.$GLOBALS['SoundCards'],'ORDER BY Description ASC');
	$videoArray=getAssocArray('DeviceTemplate','PK_DeviceTemplate','Description',$dbADO,' WHERE FK_DeviceCategory='.$GLOBALS['VideoCards'],'ORDER BY Description ASC');
	$audioSettingsArray=array('M'=>'Manual settings', 'C'=>'SPDIF Coax', 'O'=>'SPDIF TosLink', 'S'=>'Stereo', 'L'=>'Multi-channel analog');
	
	$deviceCategory=$GLOBALS['rootMediaDirectors'];
	$specificFloorplanType=$GLOBALS['EntertainmentZone'];
	$output->setHelpSrc('/support/index.php?section=document&docID=129');

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

	if(isset($_REQUEST['lastAdded']) && (int)$_REQUEST['lastAdded']!=0){
		$out.='
		<script>
			alert("This device requires some advance preparation, which can take several minutes. Your Core is doing this now and you will see a message on all orbiters and media directors notifying you when it\'s done. Please wait to use the device until then.")
		</script>';

		$rs=$dbADO->Execute('SELECT Comments FROM DeviceTemplate WHERE PK_DeviceTemplate=?',(int)$_REQUEST['lastAdded']);
		$row=$rs->FetchRow();
		if($row['Comments']!=''){
			$out.='<script>
				alert(\''.addslashes($row['Comments']).'\')
			</script>';
		}
	}

	
	if ($action == 'form') {
		$out.=setLeftMenu($dbADO).'
	<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
	</script>
	<div class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
	<div class="confirm" align="center"><B>'.strip_tags(@$_GET['msg']).'</B></div>
	<form action="index.php" method="POST" name="mediaDirectors">
	<input type="hidden" name="section" value="mediaDirectors">
	<input type="hidden" name="action" value="add">
	<input type="hidden" name="cmd" value="0">			
	<div align="center"><h3>Media Directors</h3></div>';
		
		$infraredAndSpecialisedDevices=getDevicesFromCategories(array($GLOBALS['specialized'],$GLOBALS['InfraredInterface']),$dbADO);
		$specialisedAndComputerDevices=getDevicesFromCategories(array($GLOBALS['rootComputerID'],$GLOBALS['specialized']),$dbADO);

		$out.='
		<table align="center" border="0" cellpadding="2" cellspacing="0">
				<tr bgcolor="lightblue">
					<td align="center" rowspan="2"><B>Device</B></td>
					<td align="center" rowspan="2"><B>Room</B></td>
					<td align="center" colspan="4"><B>Pipes</B></td>
					<td align="center" rowspan="2"><B>Device Data</B></td>
					<td align="center" rowspan="2"><B>Actions</B></td>
				</tr>
				<tr bgcolor="lightblue">
					<td align="center"><B>Output</B></td>
					<td align="center"><B>Connected to</B></td>
					<td align="center"><B>Input</B></td>
					<td align="center"><B>&nbsp;</B></td>
				</tr>		
					';
				if(count($avDTIDArray)==0)
					$avDTIDArray[]=0;
				$displayedAVDevices=array();
				$displayedAVDevicesDescription=array();
				$queryDevice='
					SELECT Device.*
					FROM Device 
					WHERE Device.FK_DeviceTemplate IN ('.join(',',$avDTIDArray).') AND FK_Installation=?';	
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
				$GLOBALS['DeviceDataToDisplay']=array();
				$DDTypesToDisplay=array();	
				$joinArray=$DTIDArray;	
				$joinArray[]=0; 	// used only for query when there are no DT in selected category
				$queryDevice='
					SELECT 
						PK_Device,
						Device.Description,
						IPaddress,
						MACaddress,
						FK_Device_ControlledVia,
						FK_Device_RouteTo,
						FK_Room,
						Device.FK_DeviceTemplate,
						DeviceTemplate.Description AS TemplateName, 
						DeviceCategory.Description AS CategoryName, 
						Manufacturer.Description AS ManufacturerName, 
						IsIPBased, 
						FK_DeviceCategory,
						DeviceData.Description AS dd_Description, 
						Device_DeviceData.FK_DeviceData,
						ParameterType.Description AS typeParam, 
						Device_DeviceData.IK_DeviceData,
						ShowInWizard,ShortDescription,
						AllowedToModify,
						DeviceTemplate_DeviceData.Description AS Tooltip 
					FROM DeviceData 
					INNER JOIN ParameterType ON FK_ParameterType = PK_ParameterType 
					INNER JOIN Device_DeviceData ON Device_DeviceData.FK_DeviceData=PK_DeviceData 
					INNER JOIN Device ON FK_Device=PK_Device
					LEFT JOIN DeviceTemplate_DeviceData ON DeviceTemplate_DeviceData.FK_DeviceData=Device_DeviceData.FK_DeviceData AND DeviceTemplate_DeviceData.FK_DeviceTemplate=Device.FK_DeviceTemplate
					INNER JOIN DeviceTemplate ON Device.FK_DeviceTemplate=PK_DeviceTemplate 
					LEFT JOIN DeviceTemplate_AV ON Device.FK_DeviceTemplate=DeviceTemplate_AV.FK_DeviceTemplate 
					INNER JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory 
					INNER JOIN Manufacturer ON FK_Manufacturer=PK_Manufacturer 			
					WHERE Device.FK_DeviceTemplate IN ('.join(',',$joinArray).') AND FK_Installation=? ';
				$resDevice=$dbADO->Execute($queryDevice,$installationID);
				$childOf=array();
				$firstDevice=0;
				$deviceDataArray=array();
				while($rowD=$resDevice->FetchRow()){
					if($rowD['FK_Device_ControlledVia']==$rowD['FK_Device_RouteTo'])
						$childOf[$rowD['PK_Device']]=$rowD['FK_Device_ControlledVia'];
					$displayedDevices[$rowD['PK_Device']]=$rowD['Description'];
					
					if($rowD['PK_Device']!=$firstDevice){
						$firstDevice=$rowD['PK_Device'];
						$deviceDataArray[$firstDevice]=array();
					}else{
						$deviceDataArray[$firstDevice][]=$rowD;
					}
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
						$GLOBALS['DeviceDataToDisplay'][]=$rowDDD['PK_DeviceData'];
						$DDTypesToDisplay[]=$rowDDD['paramName'];
						$DeviceDataDescriptionToDisplay[]=$rowDDD['Description'];
					}
				}	
			$resDevice->MoveFirst();
			$pos=0;
			$embededRows=array();
			$deviceDisplayed=0;
			while($rowD=$resDevice->FetchRow()){
				if($rowD['PK_Device']!=$deviceDisplayed){
					$deviceDisplayed=$rowD['PK_Device'];
					$pos++;
					
					$deviceName=(@$childOf[$rowD['PK_Device']]=='')?'<input type="text" name="description_'.$rowD['PK_Device'].'" value="'.$rowD['Description'].'">':'<input type="hidden" name="description_'.$rowD['PK_Device'].'" value="'.$rowD['Description'].'"><B>'.$rowD['Description'].'</B>';
					$deviceName.=' # '.$rowD['PK_Device'];
					$roomPulldown='<select name="room_'.$rowD['PK_Device'].'">
							<option value="0">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;- Select room -&nbsp;&nbsp;&nbsp;&nbsp;</option>';
					foreach($roomIDArray as $key => $value){
						$roomPulldown.='<option value="'.$value.'" '.(($rowD['FK_Room']==$value)?'selected':'').'>'.$roomArray[$key].'</option>';
					}
					$roomPulldown.='</select>';
					$devicePipes=getPipes($rowD['PK_Device'],$dbADO);
	
					$buttons='<input type="submit" class="button" name="delete_'.$rowD['PK_Device'].'" value="Delete"  onclick="if(confirm(\'Are you sure you want to delete this device?\'))return true;else return false;"></td>';
						
						
					$controlledByPulldown='&nbsp;';
						
					if(@$childOf[$rowD['PK_Device']]==''){	
						$out.='
					<tr>
						<td align="center" bgcolor="#F0F3F8"><a name="deviceLink_'.$rowD['PK_Device'].'"></a>'.$deviceName.'</td>
						<td  align="right">'.$roomPulldown.'</td>
						<td bgcolor="#F0F3F8">A: '.@$devicePipes['1']['output'].'</td>
						<td bgcolor="#F0F3F8">'.@$devicePipes['1']['to'].'</td>
						<td bgcolor="#F0F3F8">'.@$devicePipes['1']['input'].'</td>
						<td bgcolor="#F0F3F8" rowspan="2"><a href="javascript:windowOpen(\'index.php?section=editPipes&deviceID='.$rowD['PK_Device'].'&from=mediaDirectors\',\'width=600,height=300,toolbars=true,scrollbars=1,resizable=1\');">Edit</a></td>
						<td rowspan="2" valign="top" align="right">'.formatDeviceData($rowD['PK_Device'],$deviceDataArray[$rowD['PK_Device']],$dbADO,$rowD['IsIPBased']).'</td>
						<td align="center" rowspan="2" valign="top" bgcolor="#F0F3F8">'.$buttons.'</td>
					</tr>
					<tr>			
						<td align="center" bgcolor="#F0F3F8" title="Category: '.$rowD['CategoryName'].', manufacturer: '.$rowD['ManufacturerName'].'">DT: '.$rowD['TemplateName'].'<br><input type="button" class="button" name="edit_'.$rowD['PK_Device'].'" value="Advanced"  onClick="self.location=\'index.php?section=editDeviceParams&deviceID='.$rowD['PK_Device'].'\';"></td>
						<td align="right">'.$controlledByPulldown.'</td>
						<td bgcolor="#F0F3F8">V: '.@$devicePipes['2']['output'].'</td>
						<td bgcolor="#F0F3F8">'.@$devicePipes['2']['to'].'</td>
						<td bgcolor="#F0F3F8">'.@$devicePipes['2']['input'].'</td>
					</tr>';
						if(isset($embededRows[$rowD['PK_Device']])){
							foreach($embededRows[$rowD['PK_Device']] as $tuner){
								$out.=$tuner;
							}
						}
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
					$orbiterMDChild=getMediaDirectorOrbiterChild($rowD['PK_Device'],$dbADO);
					if($orbiterMDChild){
						$pvrDevice=getSubDT($rowD['PK_Device'],$GLOBALS['PVRCaptureCards'],$dbADO);
						$soundDevice=getSubDT($rowD['PK_Device'],$GLOBALS['SoundCards'],$dbADO);
						$videoDevice=getSubDT($rowD['PK_Device'],$GLOBALS['VideoCards'],$dbADO);
						$out.='
							<input type="hidden" name="oldPVR_'.$rowD['PK_Device'].'" value="'.$pvrDevice.'">
							<input type="hidden" name="oldSound_'.$rowD['PK_Device'].'" value="'.$soundDevice.'">
							<input type="hidden" name="oldVideo_'.$rowD['PK_Device'].'" value="'.$videoDevice.'">
							<tr class="normaltext">
								<td colspan="7">
									<table>
										<tr>
											<td valign="top">PVR Capture Card</td>
											<td valign="top">'.htmlPulldown($pvrArray,'PVRCaptureCard_'.$rowD['PK_Device'],$pvrDevice,'None').'</td>
											<td align="right">Sound Card '.htmlPulldown($soundArray,'SoundCard_'.$rowD['PK_Device'],$soundDevice,'Standard Sound Card').'<br>
											Audio settings '.pulldownFromArray($audioSettingsArray,'audioSettings_'.$rowD['PK_Device'],@$oldAudioSettings).'<br>
											AC3 passthrough <input type="checkbox" name="ac3_'.$rowD['PK_Device'].'" value="3" '.@$oldAC3.'></td>
											<td align="right" valign="top">Video Card '.htmlPulldown(@$videoArray,'VideoCard_'.$rowD['PK_Device'],$videoDevice,'Standard Video Card').'<br>
											<input type="button" class="button" name="setResolution" value="Set resolution & refresh" onclick="self.location=\'index.php?section=setResolution&mdID='.$rowD['PK_Device'].'\'";>
											</td>
										</tr>
									</table>
								</td>
							</tr>';
	
						$out.='
							<tr>
								<td colspan="8">'.getInstallWizardDeviceTemplates(6,$dbADO,$orbiterMDChild,$GLOBALS['mdDistro'],1).'<br>'.displayRemotes($rowD['PK_Device'],$dbADO,'mediaDirectors').'<br>'.displayReceivers($rowD['PK_Device'],$dbADO).'<br><input type="button" class="button" name="editCA_" value="Edit Computing Applications" onclick="windowOpen(\'index.php?section=editComputingApplications&mdID='.$rowD['PK_Device'].'\',\'width=800,height=600,toolbars=true,scrollbars=1,resizable=1\')"><hr></td>
							</tr>';
						$setupDisklessMD=' <input type="button" class="button" name="setupDisklessMD" value="Setup Diskless Media Directors *" onClick="self.location=\'index.php?section=setupDisklessMD\';">';
						$setupDisklessMDInfo='* When you add a new diskless M/D, you must first click this button, wait for the setup process to complete, then do a ‘quick reload router’, and then you can bootup your new diskless media director.';
					}
				}
			}
			$out.='
				<input type="hidden" name="DeviceDataToDisplay" value="'.join(',',$GLOBALS['DeviceDataToDisplay']).'">
				<input type="hidden" name="displayedDevices" value="'.join(',',array_keys($displayedDevices)).'">';

			if($resDevice->RecordCount()!=0){
				$out.='
				<tr>
					<td colspan="8" align="center"><input type="submit" class="button" name="update" value="Update">'.@$setupDisklessMD.'</td>
				</tr>
				<tr>
					<td colspan="8" align="left">'.@$setupDisklessMDInfo.'</td>
				</tr>';
			}
			$out.='
				<tr>
					<td colspan="8">&nbsp;</td>
				</tr>
				<tr>
					<td colspan="8" align="center"><input type="button" class="button" name="button" value="Add device" onClick="document.mediaDirectors.action.value=\'externalSubmit\';document.mediaDirectors.submit();windowOpen(\'index.php?section=deviceTemplatePicker&allowAdd=1&from=mediaDirectors&categoryID='.$deviceCategory.'\',\'width=800,height=600,toolbars=true,scrollbars=1,resizable=1\');"></td>
				</tr>
			</table>
		</form>
		<script>
		 	var frmvalidator = new formValidator("mediaDirectors");
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
			header("Location: index.php?section=mediaDirectors&type=$type&error=You are not authorised to change the installation.");
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
				$mdData=getFieldsAsArray('Device','IPaddress,MACaddress',$dbADO,'WHERE PK_Device='.$value);
				$cmd='sudo -u root /usr/pluto/bin/DeleteMD.sh "'.$mdData['IPaddress'][0].'" "'.$mdData['MACaddress'][0].'"';
				exec($cmd);
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
					}else{
						$updateDevice='UPDATE Device SET Description=?, FK_Room=? '.@$updateMacIp.' WHERE PK_Device=?';
						$dbADO->Execute($updateDevice,array($description,$room,$value));
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
					
					$orbiterMDChild=getMediaDirectorOrbiterChild($value,$dbADO);
					$installOptionsArray=explode(',',@$_POST['displayedTemplatesRequired_'.$orbiterMDChild]);
					foreach($installOptionsArray AS $elem){
						$oldDevice=@$_POST['oldDevice_'.$orbiterMDChild.'_requiredTemplate_'.$elem];
						$optionalDevice=(isset($_POST['device_'.$orbiterMDChild.'_requiredTemplate_'.$elem]))?$_POST['device_'.$orbiterMDChild.'_requiredTemplate_'.$elem]:0;
						if($optionalDevice!=0){
							$OptionalDeviceName=cleanString(@$_POST['templateName_'.$elem]);

							if($oldDevice==''){
								$insertID=exec('sudo -u root /usr/pluto/bin/CreateDevice -h localhost -D '.$dbPlutoMainDatabase.' -d '.$elem.' -i '.$installationID.' -C '.$orbiterMDChild);
								$dbADO->Execute('UPDATE Device SET Description=?,FK_Room=? WHERE PK_Device=?',array($OptionalDeviceName,$room,$insertID));
							}
						}else{
							if($oldDevice!=''){
								deleteDevice($oldDevice,$dbADO);
							}
						}
					}
					// add/delete PVR Capture Card, sound card and video card
					
					$pvrDT=$_POST['PVRCaptureCard_'.$value];
					if($pvrDT!=$_POST['oldPVR_'.$value]){
						recreateDevice($value,$GLOBALS['PVRCaptureCards'],$pvrDT,$_SESSION['installationID'],$room,$dbADO);
					}
					$soundDT=$_POST['SoundCard_'.$value];
					if($soundDT!=$_POST['oldSound_'.$value]){
						recreateDevice($value,$GLOBALS['SoundCards'],$soundDT,$_SESSION['installationID'],$room,$dbADO);
					}
					$videoDT=$_POST['VideoCard_'.$value];
					if($videoDT!=$_POST['oldVideo_'.$value]){
						recreateDevice($value,$GLOBALS['VideoCards'],$videoDT,$_SESSION['installationID'],$room,$dbADO);
					}

					processReceiver($value,$dbADO);
					processAudioSettings($value,$dbADO);
				}
			}
			
			$commandToSend='/usr/pluto/bin/UpdateEntArea -h localhost';
			exec($commandToSend);
		}
		processRemotes($dbADO);
		if(isset($_REQUEST['add']) && !isset($_REQUEST['mdID'])){
			unset($_SESSION['from']);
			$deviceTemplate=(int)$_REQUEST['deviceTemplate'];
			if($deviceTemplate!=0){
				$insertID=exec('sudo -u root /usr/pluto/bin/CreateDevice -h localhost -D '.$dbPlutoMainDatabase.' -d '.$deviceTemplate.' -i '.$installationID,$ret);	
				setDCERouterNeedConfigure($_SESSION['installationID'],$dbADO);
				$commandToSend='/usr/pluto/bin/UpdateEntArea -h localhost';
				exec($commandToSend);
			}
			header("Location: index.php?section=mediaDirectors&lastAdded=$deviceTemplate#deviceLink_".@$insertID);
			exit();
		}
		
		
		header("Location: index.php?section=mediaDirectors&msg=The devices was updated".@$anchor);		
	}

	$output->setScriptCalendar('null');

	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Media Directors');
	$output->output();
}

function getSubDT($mdID,$categoryID,$dbADO)
{
	$res=$dbADO->Execute('
		SELECT FK_DeviceTemplate
		FROM Device 
		INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
		WHERE FK_DeviceCategory=? AND FK_Device_ControlledVia=?',array($categoryID,$mdID));
	if($res->RecordCount()>0){
		$row=$res->FetchRow();
		return $row['FK_DeviceTemplate'];
	}else{
		return 0;
	}
}

function getSubDevice($mdID,$categoryID,$dbADO)
{
	$res=$dbADO->Execute('
		SELECT PK_Device
		FROM Device 
		INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
		WHERE FK_DeviceCategory=? AND FK_Device_ControlledVia=?',array($categoryID,$mdID));
	if($res->RecordCount()>0){
		$row=$res->FetchRow();
		return $row['PK_Device'];
	}else{
		return 0;
	}
}

function recreateDevice($mdID,$categoryID,$dtID,$installationID,$roomID,$dbADO)
{
	$deviceID=getSubDevice($mdID,$categoryID,$dbADO);
	if($deviceID!=0){
		deleteDevice($deviceID,$dbADO);
	}
	if($dtID!=0){
		createDevice($dtID,$installationID,$mdID,$roomID,$dbADO,1);
	}
}
?>
