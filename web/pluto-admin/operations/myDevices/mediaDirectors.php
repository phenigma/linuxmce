<?
function mediaDirectors($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/mediaDirectors.lang.php');
	
	global $dbPlutoMainDatabase;
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
//	$dbADO->debug=true;
	$userID = (int)@$_SESSION['userID'];
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];

	$pvrArray=getAssocArray('DeviceTemplate','PK_DeviceTemplate','Description',$dbADO,' WHERE FK_DeviceCategory IN ('.$GLOBALS['PVRCaptureCards'].','.$GLOBALS['DigitalTVCards'].')','ORDER BY Description ASC');
	$soundArray=getAssocArray('DeviceTemplate','PK_DeviceTemplate','Description',$dbADO,' WHERE FK_DeviceCategory='.$GLOBALS['SoundCards'],'ORDER BY Description ASC');
	$videoArray=getAssocArray('DeviceTemplate','PK_DeviceTemplate','Description',$dbADO,' WHERE FK_DeviceCategory='.$GLOBALS['VideoCards'],'ORDER BY Description ASC');
	$audioSettingsArray=array('M'=>'Manual settings', 'C'=>'SPDIF Coax', 'O'=>'SPDIF TosLink', 'S'=>'Stereo', 'L'=>'Multi-channel analog');
	
	$deviceCategory=$GLOBALS['rootMediaDirectors'];
	$specificFloorplanType=$GLOBALS['EntertainmentZone'];
	$output->setHelpSrc('/support/index.php?section=document&docID=129');

	// get selected category Device Templates
	$mdArray=getDeviceTemplatesFromCategory($deviceCategory,$dbADO,1);
	$DTArray=array_values($mdArray);
	$DTIDArray=array_keys($mdArray);

	// get AV Device Templates
	$avDTIDArray=getDeviceTemplatesFromCategory($GLOBALS['rootAVEquipment'],$dbADO);

	$queryRooms='
		SELECT Room.Description, PK_Room,Device.Description AS md,PK_Device 
		FROM Room 
		LEFT JOIN Device ON FK_Room=PK_Room AND FK_DeviceTemplate IN ('.join(',',$DTIDArray).')
		WHERE Room.FK_Installation=? 
		ORDER BY Description ASC';
	$resRooms=$dbADO->Execute($queryRooms,$installationID);
	$roomIDArray=array();
	$roomArray=array();
	while($rowRoom=$resRooms->FetchRow()){
		if((!is_null($rowRoom['PK_Device']))){
			$suffix=' [ '.$rowRoom['md'].' ]';
		}else{
			$suffix='';	
		}
		$roomArray[]=$rowRoom['Description'].$suffix;
		$roomIDArray[]=$rowRoom['PK_Room'];
	}

	if(isset($_REQUEST['newMD']) && (int)$_REQUEST['newMD']==1){
		$out.='
		<script>
			alert("'.$TEXT_WAIT_FOR_PREPARATION_NOTIFICATION_CONST.'")
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
	<div class="err" align="center">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
	<div class="confirm" align="center"><B>'.strip_tags(@$_GET['msg']).'</B></div>
	<form action="index.php" method="POST" name="mediaDirectors">
	<input type="hidden" name="section" value="mediaDirectors">
	<input type="hidden" name="action" value="add">
	<input type="hidden" name="cmd" value="0">			
		
	<div align="center"><h3>'.$TEXT_MEDIA_DIRECTORS_CONST.'</h3></div>';
		
		$infraredAndSpecialisedDevices=getDevicesFromCategories(array($GLOBALS['specialized'],$GLOBALS['InfraredInterface']),$dbADO);
		$specialisedAndComputerDevices=getDevicesFromCategories(array($GLOBALS['rootComputerID'],$GLOBALS['specialized']),$dbADO);

		$out.='
		<table align="center" border="0" cellpadding="2" cellspacing="0">
				<tr bgcolor="lightblue">
					<td align="center" rowspan="2"><B>'.$TEXT_DEVICE_CONST.'</B></td>
					<td align="center" rowspan="2"><B>'.$TEXT_ROOM_CONST.'</B></td>
					<td align="center" colspan="4"><B>'.$TEXT_PIPES_CONST.'</B></td>
					<td align="center" rowspan="2"><B>'.$TEXT_DEVICE_DATA_CONST.'</B></td>
					<td align="center" rowspan="2"><B>'.$TEXT_ACTION_CONST.'</B></td>
				</tr>
				<tr bgcolor="lightblue">
					<td align="center"><B>'.$TEXT_OUTPUT_CONST.'</B></td>
					<td align="center"><B>'.$TEXT_CONNECTED_TO_CONST.'</B></td>
					<td align="center"><B>'.$TEXT_INPUT_CONST.'</B></td>
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
				$oldAudioDD=null;
				$oldVideoDD=null;
				while($rowD=$resDevice->FetchRow()){
					$displayedDevices[$rowD['PK_Device']]=$rowD['Description'];
					
					if($rowD['PK_Device']!=$firstDevice){
						$firstDevice=$rowD['PK_Device'];
						$deviceDataArray[$firstDevice]=array();
					}else{
						$deviceDataArray[$firstDevice][]=$rowD;
					}
					
					$oldAudioDD=($rowD['FK_DeviceData']==$GLOBALS['AudioSettings'])?$rowD['IK_DeviceData']:$oldAudioDD;
					if(!is_null($oldAudioDD)){
						$oldAC3[$rowD['PK_Device']]=strstr($oldAudioDD,'3')?'checked':'';
						$oldForce48k[$rowD['PK_Device']]=strstr($oldAudioDD,'K')?'checked':'';
						$oldAudioSettings[$rowD['PK_Device']]=str_replace(array('3','K'), '', $oldAudioDD);
					}
					$oldVideoDD=($rowD['FK_DeviceData']==$GLOBALS['VideoSettings'])?$rowD['IK_DeviceData']:$oldVideoDD;
					if(!is_null($oldVideoDD)){
						$oldVideoDDArray=explode('/',$oldVideoDD);
						$oldResolution=@$oldVideoDDArray[0];
						$oldRefresh=@$oldVideoDDArray[1];
					}
					
				}

				$joinArray=array_keys($displayedDevices);	// used only for query when there are no Devices in selected category

			$resDevice->MoveFirst();
			$pos=0;
			$embededRows=array();
			$deviceDisplayed=0;
			while($rowD=$resDevice->FetchRow()){
				if($rowD['PK_Device']!=$deviceDisplayed){
					$deviceDisplayed=$rowD['PK_Device'];
					$pos++;
					
					$deviceName='<input type="text" name="description_'.$rowD['PK_Device'].'" value="'.$rowD['Description'].'">';
					$deviceName.=' # '.$rowD['PK_Device'];
					$roomPulldown='<select name="room_'.$rowD['PK_Device'].'">
							<option value="0">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;- Select room -&nbsp;&nbsp;&nbsp;&nbsp;</option>';
					foreach($roomIDArray as $key => $value){
						$roomPulldown.='<option value="'.$value.'" '.(($rowD['FK_Room']==$value)?'selected':'').'>'.$roomArray[$key].'</option>';
					}
					$roomPulldown.='</select>';
					$devicePipes=getPipes($rowD['PK_Device'],$dbADO);
	
					$buttons='
					<input value="'.$TEXT_HELP_CONST.'" type="button" class="button" name="help" onClick="self.location=\'index.php?section=help&deviceID='.$rowD['PK_Device'].'\'"><br>
					<input type="button" class="button" name="edit_'.$rowD['PK_Device'].'" value="'.$TEXT_ADVANCED_CONST.'"  onClick="self.location=\'index.php?section=editDeviceParams&deviceID='.$rowD['PK_Device'].'\';"><br>
					<input type="submit" class="button" name="delete_'.$rowD['PK_Device'].'" value="'.$TEXT_DELETE_CONST.'"  onclick="if(confirm(\''.$TEXT_CONFIRM_DELETE_MEDIA_DIRECTOR_CONST.'\'))return true;else return false;">';
					$buttons.=(isDiskless($rowD['PK_Device'],$deviceDataArray[$rowD['PK_Device']]))?'<br><input type="submit" class="button" name="rebuild_diskless_'.$rowD['PK_Device'].'" value="'.$TEXT_REBUILD_DISKLESS_IMAGE_CONST.'">':'';
						
					
					$controlledByPulldown='&nbsp;';
						
					$out.='
					<tr>
						<td align="center" bgcolor="#F0F3F8"><a name="deviceLink_'.$rowD['PK_Device'].'"></a>'.$deviceName.'</td>
						<td  align="right">'.$roomPulldown.'</td>
						<td bgcolor="#F0F3F8">A: '.@$devicePipes['1']['output'].'</td>
						<td bgcolor="#F0F3F8">'.@$devicePipes['1']['to'].'</td>
						<td bgcolor="#F0F3F8">'.@$devicePipes['1']['input'].'</td>
						<td bgcolor="#F0F3F8" rowspan="2"><a href="javascript:windowOpen(\'index.php?section=editPipes&deviceID='.$rowD['PK_Device'].'&from=mediaDirectors\',\'width=600,height=300,toolbars=true,scrollbars=1,resizable=1\');">'.$TEXT_EDIT_CONST.'</a></td>
						<td rowspan="2" valign="top" align="right">'.formatDeviceData($rowD['PK_Device'],$deviceDataArray[$rowD['PK_Device']],$dbADO,$rowD['IsIPBased']).'</td>
						<td align="center" rowspan="2" valign="top" bgcolor="#F0F3F8">'.$buttons.'</td>
					</tr>
					<tr>			
						<td align="center" title="'.$TEXT_DEVICE_CATEGORY_CONST.': '.$rowD['CategoryName'].', '.strtolower($TEXT_MANUFACTURER_CONST).': '.$rowD['ManufacturerName'].'">'.$TEXT_DEVICE_TEMPLATE_CONST.': '.$rowD['TemplateName'].'</td>
						<td align="right">'.$controlledByPulldown.'</td>
						<td bgcolor="#F0F3F8">V: '.@$devicePipes['2']['output'].'</td>
						<td bgcolor="#F0F3F8">'.@$devicePipes['2']['to'].'</td>
						<td bgcolor="#F0F3F8">'.@$devicePipes['2']['input'].'</td>
					</tr>';

					$orbiterMDChild=getMediaDirectorOrbiterChild($rowD['PK_Device'],$dbADO);
					if($orbiterMDChild){
						$soundDevice=getSubDT($rowD['PK_Device'],$GLOBALS['SoundCards'],$dbADO);
						$videoDevice=getSubDT($rowD['PK_Device'],$GLOBALS['VideoCards'],$dbADO);
						$out.='
							<input type="hidden" name="oldSound_'.$rowD['PK_Device'].'" value="'.$soundDevice.'">
							<input type="hidden" name="oldVideo_'.$rowD['PK_Device'].'" value="'.$videoDevice.'">
							<tr class="normaltext">
								<td colspan="7">
									<table border="0">
										<tr>
											<td valign="top" rowspan="2">'.$TEXT_PVR_CAPTURE_CARD_CONST.htmlPulldown($pvrArray,'PVRCaptureCard_'.$rowD['PK_Device'],0,'None').'<br>
											'.getPVRCards(array_keys($pvrArray),$_SESSION['installationID'],$rowD['PK_Device'],(int)$rowD['FK_Device_ControlledVia'],$dbADO).'
											</td>
											<td align="right">'.$TEXT_SOUND_CARD_CONST.' '.htmlPulldown($soundArray,'SoundCard_'.$rowD['PK_Device'],$soundDevice,'Standard Sound Card').'<br>
											'.$TEXT_AUDIO_SETTINGS_CONST.' '.pulldownFromArray($audioSettingsArray,'audioSettings_'.$rowD['PK_Device'],@$oldAudioSettings[$rowD['PK_Device']]).'<br>
											'.$TEXT_AC3_PASSTHROUGH_CONST.' <input type="checkbox" name="ac3_'.$rowD['PK_Device'].'" value="3" '.@$oldAC3[$rowD['PK_Device']].'><br>
											'.$TEXT_FORCE_48K_CONST.' <input type="checkbox" name="force48k_'.$rowD['PK_Device'].'" value="K" '.@$oldForce48k[$rowD['PK_Device']].'>
											</td>
											<td align="right" valign="top">'.$TEXT_VIDEO_CARD_CONST.' '.htmlPulldown(@$videoArray,'VideoCard_'.$rowD['PK_Device'],$videoDevice,'Standard Video Card').'<br>
											<input type="button" class="button" name="setResolution" value="'.$TEXT_SET_RESOLUTION_REFRESH_CONST.'" onclick="self.location=\'index.php?section=setResolution&mdID='.$rowD['PK_Device'].'\'";>
											</td>
										</tr>
									</table>
								</td>
							</tr>';
	
						$out.='
							<tr>
								<td colspan="8">'.getInstallWizardDeviceTemplates(6,$dbADO,$rowD['PK_Device'],$GLOBALS['mdDistro'],1).'<br>'.displayRemotes($rowD['PK_Device'],$dbADO,'mediaDirectors').'<br>'.displayReceivers($rowD['PK_Device'],$dbADO).'<br><input type="button" class="button" name="editCA_" value="Edit Computing Applications" onclick="windowOpen(\'index.php?section=editComputingApplications&mdID='.$rowD['PK_Device'].'\',\'width=800,height=600,toolbars=true,scrollbars=1,resizable=1\')"><hr></td>
							</tr>';
						$setupDisklessMD=' <input type="button" class="button" name="setupDisklessMD" value="'.$TEXT_SETUP_DISKLESS_MD_CONST.' *" onClick="self.location=\'index.php?section=setupDisklessMD\';">';
						$setupDisklessMDInfo='* '.$TEXT_DISKLESS_MD_NOTICE_CONST;
					}
				}
			}
			$out.='
				<input type="hidden" name="DeviceDataToDisplay" value="'.join(',',$GLOBALS['DeviceDataToDisplay']).'">
				<input type="hidden" name="displayedDevices" value="'.join(',',array_keys($displayedDevices)).'">';

			if($resDevice->RecordCount()!=0){
				$out.='
				<tr>
					<td colspan="8" align="center"><input type="submit" class="button" name="update" value="'.$TEXT_UPDATE_CONST.'">'.@$setupDisklessMD.'</td>
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
					<td colspan="8" align="center"><input type="button" class="button" name="button" value="'.$TEXT_ADD_MD_CONST.'" onClick="document.mediaDirectors.action.value=\'externalSubmit\';document.mediaDirectors.submit();windowOpen(\'index.php?section=deviceTemplatePicker&allowAdd=1&from=mediaDirectors&categoryID='.$deviceCategory.'\',\'width=800,height=600,toolbars=true,scrollbars=1,resizable=1\');"></td>
				</tr>
			</table>
	</form>
	';
	} else {
		$cmd=cleanInteger(@$_POST['cmd']);
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=mediaDirectors&type=$type&error=$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST");
			exit(0);
		}
		
		if($action=='delCard'){
			deleteDevice((int)$_REQUEST['delCard'],$dbADO);
			header("Location: index.php?section=mediaDirectors&msg=$TEXT_PVR_CARD_DELETED_CONST");		
			exit();
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
				$cmd='sudo -u root /usr/pluto/bin/DHCP_config.sh';
				exec($cmd);
			}
			
			if(isset($_POST['rebuild_diskless_'.$value])){
				$mdData=getFieldsAsArray('Device','IPaddress,MACaddress',$dbADO,'WHERE PK_Device='.$value);
				$cmd='sudo -u root /usr/pluto/bin/DeleteMD.sh "'.$mdData['IPaddress'][0].'" "'.$mdData['MACaddress'][0].'"';
				exec($cmd);
				deleteDevice($value,$dbADO);
				header('Location: index.php?section=setupDisklessMD');
				exit();
			}
			
		}
		
		if(isset($_POST['update']) || $cmd==1 || $action=='externalSubmit'){
			setDCERouterNeedConfigure($_SESSION['installationID'],$dbADO);
			$DeviceDataToDisplayArray=explode(',',@$_POST['DeviceDataToDisplay']);
			if(@$_POST['displayedDevices']!=''){
				$usedRooms=array();
				foreach($displayedDevicesArray as $key => $value){
					if($_POST['room_'.$value]!=0){
						$usedRooms[$_POST['room_'.$value]][]=$value;
					}
				}
				foreach ($usedRooms AS $usedID=>$usedArray){
					if(count($usedArray)>1){
						header('Location: index.php?section=mediaDirectors&error='.$TEXT_ERROR_DUPLICATE_ROOMS_CONST);
						exit();
					}
				}
				
				foreach($displayedDevicesArray as $key => $value){
					$description=stripslashes(@$_POST['description_'.$value]);
					if(isset($_POST['ip_'.$value])){
						$ip=$_POST['ip_'.$value];
						$mac=$_POST['mac_'.$value];
						$updateMacIp=",IPaddress='$ip', MACaddress='$mac'";
					}
										
							
					$room=(@$_POST['room_'.$value]!=0)?(int)@$_POST['room_'.$value]:NULL;
					$controlledBy=(@$_POST['controlledBy_'.$value]!=0)?(int)@$_POST['controlledBy_'.$value]:NULL;
					$dbADO->Execute('UPDATE Device Dparent JOIN Device Dchild ON Dchild.FK_Device_ControlledVia=Dparent.PK_Device SET Dparent.FK_Room=? WHERE Dchild.PK_Device=?',array($room,$value));
					
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
					$installOptionsArray=explode(',',@$_POST['displayedTemplatesRequired_'.$value]);
					foreach($installOptionsArray AS $elem){
						$oldDevice=@$_POST['oldDevice_'.$value.'_requiredTemplate_'.$elem];
						$optionalDevice=(isset($_POST['device_'.$value.'_requiredTemplate_'.$elem]))?$_POST['device_'.$value.'_requiredTemplate_'.$elem]:0;
						if($optionalDevice!=0){
							$OptionalDeviceName=cleanString(@$_POST['templateName_'.$elem]);
							if($oldDevice==''){
								// hard-coded: for BlueTooth dongle I create the device as child of media director instead of osd
								$parent=($elem==$GLOBALS['BluetoothDongle'])?$value:$orbiterMDChild;
								$cmd='sudo -u root /usr/pluto/bin/CreateDevice -h localhost -D '.$dbPlutoMainDatabase.' -d '.$elem.' -i '.$installationID.' -C '.$parent;

								$insertID=exec($cmd);
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
					if($pvrDT!=0){
						createDevice($pvrDT,$_SESSION['installationID'],$value,$room,$dbADO,1);
						//recreateDevice($value,$GLOBALS['PVRCaptureCards'],$pvrDT,$_SESSION['installationID'],$room,$dbADO,$GLOBALS['DigitalTVCards']);
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
					$err=processAudioSettings($value,$dbADO);
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
			header("Location: index.php?section=mediaDirectors&newMD=1&lastAdded=$deviceTemplate#deviceLink_".@$insertID);
			exit();
		}
		
		
		header("Location: index.php?section=mediaDirectors&msg=$TEXT_MD_UPDATED_CONST&error=".@$err.@$anchor);		
	}

	$output->setScriptCalendar('null');
	$output->setNavigationMenu(array($TEXT_MEDIA_DIRECTORS_CONST=>'index.php?section=mediaDirectors'));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_MEDIA_DIRECTORS_CONST);
	$output->output();
}

function getSubDT($mdID,$categoryID,$dbADO,$alternateCategory=0)
{
	$res=$dbADO->Execute('
		SELECT FK_DeviceTemplate
		FROM Device 
		INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
		WHERE FK_DeviceCategory=? AND FK_Device_ControlledVia=?',array($categoryID,$mdID));
	if($res->RecordCount()>0){
		$row=$res->FetchRow();
		return $row['FK_DeviceTemplate'];
	}elseif($alternateCategory!=0){
		return getSubDT($mdID,$alternateCategory,$dbADO);
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

function recreateDevice($mdID,$categoryID,$dtID,$installationID,$roomID,$dbADO,$alternateCategory=0)
{
	$deviceID=getSubDevice($mdID,$categoryID,$dbADO);
	if($deviceID!=0){
		deleteDevice($deviceID,$dbADO);
	}elseif($alternateCategory!=0){
		$altDevice=getSubDevice($mdID,$alternateCategory,$dbADO);
		if($altDevice!=0){
			deleteDevice($altDevice,$dbADO);
		}
	}
	if($dtID!=0){
		createDevice($dtID,$installationID,$mdID,$roomID,$dbADO,1);
	}
}

function getPVRCards($pvrArray,$instalationID,$mdID,$parentID,$dbADO){
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/mediaDirectors.lang.php');
	
	$filterParent=($parentID!=0)?'OR FK_Device_ControlledVia='.$parentID:'';

	$res=$dbADO->Execute('SELECT PK_Device,Description FROM Device WHERE FK_Installation=? AND (FK_Device_ControlledVia=? '.$filterParent.') AND FK_DeviceTemplate IN ('.join(',',$pvrArray).')',array($instalationID,$mdID));
	$pvrCards=array();
	if($res->RecordCount()==0){
		return '<div align="right">'.$TEXT_NO_PVR_CARDS_CONST.'</div>';
	}
	while($row=$res->fetchRow()){
		$pvrCards[$row['PK_Device']]=$row['Description'];
	}

	$out='<table cellpadding="2" cellspacing="0" align="right" width="100%">';
	$pos=0;
	foreach ($pvrCards AS $pid=>$description){
		$pos++;
		$color=($pos%2!=0)?'#F0F3F8':'#FFFFFF';
		$out.='
			<tr bgcolor="'.$color.'">
				<td>'.$description.'</td>
				<td><a href="javascript:if(confirm(\''.$TEXT_CONFIRM_DELETE_PVR_CARD_CONST.'\'))self.location=\'index.php?section=mediaDirectors&action=delCard&delCard='.$pid.'\';">'.$TEXT_DELETE_CONST.'</a></td>
			</tr>';
	}
	$out.='</table>';
	
	return $out;
}

function isDiskless($deviceID,$deviceData){
	foreach ($deviceData AS $pos=>$ddArray){
		if($ddArray['PK_Device']==$deviceID && $ddArray['FK_DeviceData']==$GLOBALS['DisklessBoot'] && $ddArray['IK_DeviceData']==1){
			return true;
		}
	}
	
	return false;
}
?>
