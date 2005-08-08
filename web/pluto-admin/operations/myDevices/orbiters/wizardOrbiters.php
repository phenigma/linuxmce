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
	
	$orbitersDTArray=getDeviceTemplatesFromCategory($deviceCategory,$dbADO,1);
	$roomsArray=getAssocArray('Room','PK_Room','Description',$dbADO,'WHERE FK_Installation='.$installationID, 'ORDER BY Description ASC');

	if ($action == 'form') {
		$out.=setLeftMenu($dbADO).'
	<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
	</script>
	<div class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
	<div class="confirm"><B>'.@stripslashes($_GET['msg']).'</B></div>
	<form action="index.php" method="POST" name="devices">
	<input type="hidden" name="section" value="wizardOrbiters">
	<input type="hidden" name="action" value="add">	
	<h3  align="left">Orbiters</h3>
	Download <a href="index.php?section=orbitersWin">Orbiter Win Installer</a>
		<div id="preloader" style="display:;">
			<table width="100%">
				<tr>
					<td align="center">Loading, please wait ...</td>
				</tr>
			</table>
		</div>
		
		<div id="content" style="display:none;">
		<table border="0" align="center">
			<tr>
				<td colspan="2" align="center">
					<input type="submit" class="button" name="QuickRegenAll" value="Quick Regen All"> 
					<input type="submit" class="button" name="FullRegenAll" value="Full Regen All"> 
					<input type="checkbox" name="reset_all" value="1"> Reset Router when done regenerating</td>
			</tr>';
		
		
		
			$displayedDevices=array();
			$DeviceDataToDisplay=array();
			$DeviceDataDescriptionToDisplay=array();

			$orbiterDD=array();
			// WARNING: hard-coded values
			$orbiterDD[]=3;			// default user
			$orbiterDD[]=20;		// No effects
			$orbiterDD[]=21;		// Main menu
			$orbiterDD[]=22;		// Sleeping menu
			$orbiterDD[]=23;		// Screen saver menu
			$orbiterDD[]=24;		// Skin
			$orbiterDD[]=25;		// Size
			$orbiterDD[]=26;		// Language
			$orbiterDD[]=56;		// Timeout
			$orbiterDD[]=84;		// Leave Monitor on for OSD
			$orbiterDD[]=91;		// Main Menu
			$orbiterDD[]=104;		// UI

			$excludedData['standard_roaming_orbiters']=array(25);
			$excludedData['mobile_orbiters']=array(84,20,'room','wifi',56);
			$excludedData['on_screen_orbiters']=array('dt','ip_mac','room','wifi');
			
			$queryData='
					SELECT 
						Device.*, 
						DeviceTemplate.Description AS TemplateName, 
						DeviceCategory.Description AS CategoryName, 
						FK_DeviceCategory,
						Manufacturer.Description AS ManufacturerName, 
						IsIPBased, 
						RegenInProgress, 
						DeviceData.Description AS ddDescription, 
						ParameterType.Description AS typeParam, 
						Device_DeviceData.IK_DeviceData,
						Device_DeviceData.FK_DeviceData,
						ShowInWizard,
						ShortDescription,
						AllowedToModify,
						DeviceTemplate_DeviceData.Description AS Tooltip 
					FROM Device 
					INNER JOIN DeviceTemplate ON Device.FK_DeviceTemplate=PK_DeviceTemplate 
					INNER JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory 
					INNER JOIN Manufacturer ON FK_Manufacturer=PK_Manufacturer 
					LEFT JOIN Orbiter ON PK_Orbiter=PK_Device 
					INNER JOIN Device_DeviceData ON Device_DeviceData.FK_Device=PK_Device
					INNER JOIN DeviceData ON Device_DeviceData.FK_DeviceData=PK_DeviceData
					INNER JOIN ParameterType ON FK_ParameterType = PK_ParameterType 
					LEFT JOIN DeviceTemplate_DeviceData ON DeviceTemplate_DeviceData.FK_DeviceData=Device_DeviceData.FK_DeviceData AND DeviceTemplate_DeviceData.FK_DeviceTemplate=Device.FK_DeviceTemplate
					WHERE Device.FK_DeviceTemplate IN ('.join(',',array_keys($orbitersDTArray)).') AND Device_DeviceData.FK_DeviceData IN ('.join(',',$orbiterDD).') AND FK_Installation=?
					ORDER BY Device.Description ASC';
			$resDevice=$dbADO->Execute($queryData,$installationID);

			$orbiterCount=array('standard_roaming_orbiters'=>0,'mobile_orbiters'=>0,'on_screen_orbiters'=>0);
			$orbiterDisplayed=0;
			$PingTest=0;
			$isOSD=0;
			$RegenInProgress=0;
			$orbiterGroupDisplayed='';
			$content=array('standard_roaming_orbiters'=>'','mobile_orbiters'=>'','on_screen_orbiters'=>'');
			while($rowD=$resDevice->FetchRow()){
				// set orbiter group to be displayed
				$orbiterGroup='standard_roaming_orbiters';
				if($rowD['FK_DeviceTemplate']==62){		// hard coded
					$orbiterGroup='on_screen_orbiters';
				}
				if($rowD['FK_DeviceCategory']==2){		// hard coded
					$orbiterGroup='mobile_orbiters';
				}
				
				if(!in_array($rowD['FK_DeviceData'],$DeviceDataToDisplay)){
					$DeviceDataToDisplay[]=$rowD['FK_DeviceData'];
				}
				
				$PingTest=$rowD['PingTest'];
				$RegenInProgress=$rowD['RegenInProgress'];
				if($rowD['FK_DeviceData']==84 && @$ddValue==1){
					$isOSD=1;
				}
				
				// set orbiter devices array and the counter for colors
				if($rowD['PK_Device']!=$orbiterDisplayed){
					$orbiterCount[$orbiterGroup]++;
					$displayedDevices[]=$rowD['PK_Device'];
					
					if($orbiterDisplayed!=0 && $orbiterGroupDisplayed!=''){
						if(!in_array('wifi',$excludedData[$orbiterGroupDisplayed])){
							$content[$orbiterGroupDisplayed].=displayWiFiRow($orbiterDisplayed,$PingTest,$isOSD);
						}
						$content[$orbiterGroupDisplayed].=displayButtons($orbiterDisplayed,$RegenInProgress).'
							</table></td>';
						$content[$orbiterGroupDisplayed].=($orbiterCount[$orbiterGroupDisplayed]%2==1)?'</tr>':'';
					}	
					
					$orbiterDisplayed=$rowD['PK_Device'];
					$orbiterGroupDisplayed=$orbiterGroup;
					
					
					// build content table for each orbiter
				
					$content[$orbiterGroup].=($orbiterCount[$orbiterGroup]%2==1)?'<tr>':'';
					$content[$orbiterGroup].='<td bgcolor="'.(($orbiterCount[$orbiterGroup]%4==1 || $orbiterCount[$orbiterGroup]%4==2)?'#F0F3F8':'').'">';
					$content[$orbiterGroup].='
						<table align="center" border="0">';
					if(!in_array('dt',$excludedData[$orbiterGroup])){
						$content[$orbiterGroup].='
							<tr>
								<td align="right"><B>DeviceTemplate</B></td>
								<td align="left" title="Category: '.$rowD['CategoryName'].', manufacturer: '.$rowD['ManufacturerName'].'">'.$rowD['TemplateName'].'</td>
							</tr>
							';
					}
					if(!in_array('description',$excludedData[$orbiterGroup])){
						$content[$orbiterGroup].='
							<tr>
								<td align="right"><B>Description</B></td>
								<td align="left"><input type="text" name="description_'.$rowD['PK_Device'].'" value="'.stripslashes($rowD['Description']).'"> # '.$rowD['PK_Device'].'</td>
							</tr>';
					}
	
					if($rowD['IsIPBased']==1 && !in_array('ip_mac',$excludedData[$orbiterGroup])){
						$content[$orbiterGroup].='
								<tr>
									<td align="right"><B>IP</B></td>
									<td><input type="text" name="ip_'.$rowD['PK_Device'].'" value="'.$rowD['IPaddress'].'"></td>
								</tr>
								<tr>
									<td align="right"><B>MAC</B></td>
									<td><input type="text" name="mac_'.$rowD['PK_Device'].'" value="'.$rowD['MACaddress'].'"></td>
								</tr>';
					}
					if(!in_array('room',$excludedData[$orbiterGroup])){
						$content[$orbiterGroup].='
							<tr>
								<td align="right"><B>Room</B></td>
								<td>'.pulldownFromArray($roomsArray,'room_'.$rowD['PK_Device'],$rowD['FK_Room']).'</td>
							</tr>
							';
					}
					
					if(!in_array($rowD['FK_DeviceData'],$excludedData[$orbiterGroup])){
						$content[$orbiterGroup].=formatDDRows($rowD,$dbADO);
					}
				}else{
					// display device data
					if(!in_array($rowD['FK_DeviceData'],$excludedData[$orbiterGroup])){
						$content[$orbiterGroup].=formatDDRows($rowD,$dbADO);
					}
				}
												
			}
			
			$content[$orbiterGroupDisplayed].='	
					<tr>
						<td align="center" colspan="2">'.displayButtons($orbiterDisplayed,$RegenInProgress).'</td>
					</tr>
					</table></td>';
			$content[$orbiterGroupDisplayed].=($orbiterCount[$orbiterGroupDisplayed]%2==1)?'</tr>':'';			
			
			$content['mobile_orbiters']=(!isset($content['mobile_orbiters']))?'<tr><td colspan="2" align="center">No orbiters in this category.</td></tr>':$content['mobile_orbiters'];
			$content['standard_roaming_orbiters']=(!isset($content['standard_roaming_orbiters']))?'<tr><td colspan="2" align="center">No orbiters in this category.</td></tr>':$content['standard_roaming_orbiters'];
			$content['on_screen_orbiters']=(!isset($content['on_screen_orbiters']))?'<tr><td colspan="2" align="center">No orbiters in this category.</td></tr>':$content['on_screen_orbiters'];
			
			$out.='
				<tr>
					<td bgcolor="lightblue" colspan="2" align="center"><B>Mobile phone orbiters</B></td>
				</tr>
				'.$content['mobile_orbiters'].'
				<tr>
					<td bgcolor="lightblue" colspan="2" align="center"><B>Standard roaming orbiters</B></td>
				</tr>
				'.$content['standard_roaming_orbiters'].'
				<tr>
					<td bgcolor="lightblue" colspan="2" align="center"><B>On-screen displays for media directors</B></td>
				</tr>
				'.$content['on_screen_orbiters'].'
			';
			
			
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
					<td colspan="2" align="center">'.pulldownFromArray($orbitersDTArray,'deviceTemplate',0).'
					<input type="submit" class="button" name="add" value="Add orbiter"  ></td>
				</tr>
			</table>
			</div>
		</form>
		<script>
		 	var frmvalidator = new formValidator("devices");
 //			frmvalidator.addValidation("Description","req","Please enter a device description");			
//	 		frmvalidator.addValidation("masterDevice","dontselect=0","Please select a Device Template!");			
		</script>
	
	</form>
	';
		$output->setScriptInBody('onLoad="document.getElementById(\'preloader\').style.display=\'none\';document.getElementById(\'content\').style.display=\'\';";');			
	} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=devices&error=You are not authorised to change the installation.");
			exit(0);
		}
		
		if(isset($_POST['QuickRegenAll'])){
			$ResetRouter=((int)@$_POST['reset_all']==1)?' 24 1':'';
			setOrbitersNeedConfigure($installationID,$dbADO);
				
			$commandToSend='/usr/pluto/bin/MessageSend localhost -targetType template 0 '.$GLOBALS['OrbiterPlugIn'].' 1 266 2 0 21 "-r"'.$ResetRouter;
			exec($commandToSend);
			
		}		
		if(isset($_POST['FullRegenAll'])){
			$ResetRouter=((int)@$_POST['reset_all']==1)?' 24 1':'';
			setOrbitersNeedConfigure($installationID,$dbADO);
				
			$commandToSend='/usr/pluto/bin/MessageSend localhost -targetType template 0 '.$GLOBALS['OrbiterPlugIn'].' 1 266 2 0 21 "-r"'.$ResetRouter;
			exec($commandToSend);
		}		

		$displayedDevicesArray=explode(',',$_POST['displayedDevices']);
		foreach($displayedDevicesArray as $value){
			if(isset($_POST['delete_'.$value])){
				deleteDevice($value,$dbADO); 
			}
			if(isset($_POST['quickRegen_'.$value])){
				$updateOrbiter='UPDATE Orbiter SET Regen=1 WHERE PK_Orbiter=?';
				$dbADO->Execute($updateOrbiter,$value); 
				$dbADO->Execute('UPDATE Device SET NeedConfigure=1 WHERE PK_Device=?',$value);
				$updateOrbiters=true;
				$ResetRouter=((int)@$_POST['reset_'.$value]==1)?' 24 1':'';
				
				$commandToSend='/usr/pluto/bin/MessageSend localhost -targetType template '.$value.' '.$GLOBALS['OrbiterPlugIn'].' 1 266 2 '.$value.' 21 "-r"'.$ResetRouter;
				exec($commandToSend,$tmp);
			}
			if(isset($_POST['fullRegen_'.$value])){
				$updateOrbiter='UPDATE Orbiter SET Modification_LastGen=0 WHERE PK_Orbiter=?';
				$dbADO->Execute($updateOrbiter,$value); 
				$dbADO->Execute('UPDATE Device SET NeedConfigure=1 WHERE PK_Device=?',$value);
				$updateOrbiters=true;
				$ResetRouter=((int)@$_POST['reset_'.$value]==1)?' 24 1':'';
				
				$commandToSend='/usr/pluto/bin/MessageSend localhost -targetType template '.$value.' '.$GLOBALS['OrbiterPlugIn'].' 1 266 2 '.$value.' 21 "-r"'.$ResetRouter;
				exec($commandToSend);
			}
		}
		
		if(isset($_POST['update']) || isset($updateOrbiters) || $action=='externalSubmit'){
			$DeviceDataToDisplayArray=explode(',',$_POST['DeviceDataToDisplay']);
			foreach($displayedDevicesArray as $key => $value){
				$deviceTemplate=(int)@$_POST['deviceTemplate_'.$value];
				$description=@$_POST['description_'.$value];
				if(isset($_POST['ip_'.$value])){
					$ip=$_POST['ip_'.$value];
					$mac=$_POST['mac_'.$value];
					$updateMacIp=",IPaddress='$ip', MACaddress='$mac'";
				}else{
					$updateMacIp='';
				}
				$room=(@$_POST['room_'.$value]!=0)?(int)@$_POST['room_'.$value]:NULL;
				$pingTest=(int)@$_POST['PingTest_'.$value];
				
				$updateDevice='UPDATE Device SET Description=?, FK_Room=?, PingTest=? '.@$updateMacIp.' WHERE PK_Device=?';
				$dbADO->Execute($updateDevice,array($description,$room,$pingTest,$value));

				foreach($DeviceDataToDisplayArray as $ddValue){
					if($ddValue!=91){
						if($ddValue!=56){
							$deviceData=(isset($_POST['deviceData_'.$value.'_'.$ddValue]))?$_POST['deviceData_'.$value.'_'.$ddValue]:NULL;
						}else{
							$deviceData=(isset($_POST['timeoutSS_'.$value]))?$_POST['timeoutSS_'.$value].','.$_POST['timeoutPO_'.$value]:NULL;
						}
						if(!is_null($deviceData)){
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
				
				// main menu
				$mainMenu=((int)@$_POST['mainMenu_'.$value]>0)?(int)$_POST['mainMenu_'.$value]:'';
				$dbADO->Execute('UPDATE Device_DeviceData SET IK_DeviceData=? WHERE FK_Device=? AND FK_DeviceData=?',array($mainMenu,$value,91));
			}
		}
		
		if(isset($_POST['add'])){
			$deviceTemplate=(int)$_POST['deviceTemplate'];
			if($deviceTemplate!=0){
				$insertID=exec('sudo -u root /usr/pluto/bin/CreateDevice -h localhost -D '.$dbPlutoMainDatabase.' -d '.$deviceTemplate.' -i '.$installationID);				
			}
		}
		$commandMessage=(isset($commandToSend))?'<br>Command sent: '.$commandToSend:'';
		header("Location: index.php?section=wizardOrbiters&msg=Orbiters updated.".$commandMessage);		
	}

	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Orbiters');
	$output->output();
}


// format and return device data rows
function formatDDRows($rowD,$dbADO)
{
	$ddHTML='';
	$selectedMenu=0;
	$ddValue=$rowD['IK_DeviceData'];
	if((@$rowD['ShowInWizard']==1 || @$rowD['ShowInWizard']=='') && $rowD['FK_DeviceData']!=91){
		if($rowD['FK_DeviceData']!=56){
			$ddHTML.='
				<tr>
					<td align="right"><b>'.((@$rowD['ShortDescription']!='')?$rowD['ShortDescription']:str_replace('PK_','',$rowD['ddDescription'])).'</b> '.((@$rowD['Tooltip']!='')?'<img src="include/images/tooltip.gif" title="'.@$rowD['Tooltip'].'" border="0" align="middle">':'').'</td>
					<td align="left">';
			switch($rowD['typeParam']){
				case 'int':
				if(in_array($rowD['ddDescription'],$GLOBALS['DeviceDataLinkedToTables']))
				{
					$tableName=str_replace('PK_','',$rowD['ddDescription']);
					if(!isset($GLOBALS['ddTableArray_'.$tableName])){
						if($tableName!='Users')
							$GLOBALS['ddTableArray_'.$tableName]=getAssocArray($tableName,$rowD['ddDescription'],'Description',$dbADO,'','ORDER BY Description ASC');
						else
							$GLOBALS['ddTableArray_'.$tableName]=getAssocArray('Users','PK_Users','UserName',$dbADO,'','ORDER BY UserName ASC');
					}
					
					$ddHTML.=pulldownFromArray($GLOBALS['ddTableArray_'.$tableName],'deviceData_'.$rowD['PK_Device'].'_'.$rowD['FK_DeviceData'],$ddValue,((isset($rowD['AllowedToModify']) && $rowD['AllowedToModify']==0)?'disabled':'').' '.(($rowD['FK_DeviceData']==24)?'onChange="document.wizardOrbiters.action.value=\'form\';document.wizardOrbiters.submit();"':''),'key','- Please select -');
				}
				else
					$ddHTML.='<input type="text" name="deviceData_'.$rowD['PK_Device'].'_'.$rowD['FK_DeviceData'].'" value="'.@$ddValue.'" '.((isset($rowD['AllowedToModify']) && $rowD['AllowedToModify']==0)?'disabled':'').'>';
				break;
				case 'bool':
					$ddHTML.='<input type="checkbox" name="deviceData_'.$rowD['PK_Device'].'_'.$rowD['FK_DeviceData'].'" value="1" '.((@$ddValue!=0)?'checked':'').' '.((isset($rowD['AllowedToModify']) && $rowD['AllowedToModify']==0)?'disabled':'').'>';
				break;
				default:
					$ddHTML.='<input type="text" name="deviceData_'.$rowD['PK_Device'].'_'.$rowD['FK_DeviceData'].'" value="'.@$ddValue.'" '.((isset($rowD['AllowedToModify']) && $rowD['AllowedToModify']==0)?'disabled':'').'>';
			}

			$ddHTML.='</td>
				</tr>';
		}
		else{
			$parts=explode(',',$ddValue);
			$ddHTML.='
					<tr>
						<td align="right"><B>Seconds before screen saver</B></td>
						<td><input type="text" name="timeoutSS_'.$rowD['PK_Device'].'" value="'.@$parts[0].'"></td>
					</tr>
						<tr>
						<td align="right"><B>Seconds before power off</B></td>
						<td><input type="text" name="timeoutPO_'.$rowD['PK_Device'].'" value="'.@$parts[1].'"></td>
					</tr>							';			
		}
		$ddHTML.='
					<input type="hidden" name="oldDeviceData_'.$rowD['PK_Device'].'_'.$rowD['FK_DeviceData'].'" value="'.((!is_null($rowD['IK_DeviceData']))?$ddValue:'NULL').'">';					

		if($rowD['FK_DeviceData']==24){
			$selSkin=(isset($_POST['deviceData_'.$rowD['PK_Device'].'_'.$rowD['FK_DeviceData']]) && (int)$_POST['deviceData_'.$rowD['PK_Device'].'_'.$rowD['FK_DeviceData']]!=0)?(int)$_POST['deviceData_'.$rowD['PK_Device'].'_'.$rowD['FK_DeviceData']]:@$ddValue;
			if($selSkin!=0){
				$defMenuArray=getFieldsAsArray('Skin','FK_DesignObj_MainMenu',$dbADO,'WHERE PK_Skin='.$selSkin);
				$defaultMenuValue=$defMenuArray['FK_DesignObj_MainMenu'][0];

				$objArray=getArrayFromTable('DeviceTemplate_DesignObj','FK_DesignObj','Description',$dbADO,'INNER JOIN DesignObj ON FK_DesignObj=PK_DesignObj WHERE FK_DeviceTemplate=8','ORDER BY Description ASC');
				$ddHTML.='
										<tr>
											<td align="right"><B>Main menu</B></td>
											<td>'.pulldownFromArray($objArray,'mainMenu_'.$rowD['PK_Device'],$selectedMenu,'','key','- Please select -',$defaultMenuValue).'</td>
										</tr>';
			}
		}
		unset($ddValue);
	}

	return $ddHTML;
}

function displayButtons($orbiter,$RegenInProgress){
	$out='';
			
	$out.='
		<tr>
			<td align="right"><input type="checkbox" name="reset_'.$orbiter.'" value="1"></td>
			<td>Reset Router when done regenerating</td>
		</tr>';
	if(@$RegenInProgress==1){
		$out.='
			<tr>
				<td colspan="2" align="center"><B><font color="red">Orbiter Generation in process</font></B></td>
			</tr>';
	}

	$out.='
			<tr>
				<td align="center" colspan="2">
					<input type="submit" class="button" name="quickRegen_'.$orbiter.'" value="Quick regen"  >&nbsp;&nbsp;
					<input type="submit" class="button" name="fullRegen_'.$orbiter.'" value="Full regen"  >&nbsp;&nbsp;
					<input type="button" class="button" name="customBg" value="Custom Bg"  onclick="windowOpen(\'index.php?section=customBackground&oID='.$orbiter.'\',\'width=600,height=400,scrollbars=1,resizable=1\');">&nbsp;&nbsp;
					<input type="submit" class="button" name="update" value="Update"  >&nbsp;&nbsp;
					<input type="button" class="button" name="edit_'.$orbiter.'" value="Adv"  onClick="self.location=\'index.php?section=editDeviceParams&deviceID='.$orbiter.'\';">&nbsp;&nbsp;
					<input type="submit" class="button" name="delete_'.$orbiter.'" value="Delete"  onclick="if(!confirm(\'Are you sure you want to delete this orbiter?\'))return false;">
				</td>
			</tr>	
	';
	return $out;
}

function displayWiFiRow($orbiter,$isOSD,$PingTest){
	$out='';
	
	if($isOSD==0){
		$out.='
		<tr>
			<td><B>This device uses a Wi-Fi connection</B></td>
			<td><input type="checkbox" name="PingTest_'.$orbiter.'" value="1" '.(($PingTest==1)?'checked':'').'></td>
		</tr>';			
	}
	
	return $out;
}
?>