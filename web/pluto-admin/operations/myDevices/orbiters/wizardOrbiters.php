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
	
	$queryRooms='SELECT * FROM Room WHERE FK_Installation=? ORDER BY Description ASC';
	$resRooms=$dbADO->Execute($queryRooms,$installationID);
	$roomIDArray=array();
	$roomArray=array();
	while($rowRoom=$resRooms->FetchRow()){
		$roomArray[]=$rowRoom['Description'];
		$roomIDArray[]=$rowRoom['PK_Room'];
	}
	
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
		<table border="0">
			<tr>
				<td colspan="2" align="center"><input type="submit" class="button" name="QuickRegenAll" value="Quick Regen All"> <input type="submit" class="button" name="FullRegenAll" value="Full Regen All"> <input type="checkbox" name="reset_all" value="1"> Reset Router when done regenerating</td>
			</tr>';
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
				$joinArray[]=56;		// Timeout
				$joinArray[]=84;		// Leave Monitor on for OSD
				$joinArray[]=91;		// Main Menu
			
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
					<td align="left"><input type="text" name="description_'.$rowD['PK_Device'].'" value="'.stripslashes($rowD['Description']).'"> # '.$rowD['PK_Device'].'</td>
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
				$selectedMenu=0;
				$defaultMenuValue=-1;
				$isOSD=0;
				$ddHTML='';
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
					if($DeviceDataToDisplay[$key]==91){
						$selectedMenu=@$ddValue;
					}
					if($DeviceDataToDisplay[$key]==84 && @$ddValue==1){
						$isOSD=1;
					}
					
					if((@$rowDDforDevice['ShowInWizard']==1 || @$rowDDforDevice['ShowInWizard']=='') && $DeviceDataToDisplay[$key]!=91){
						if($DeviceDataToDisplay[$key]!=56){
							$ddHTML.='
							<tr>
								<td align="right"><b>'.((@$rowDDforDevice['ShortDescription']!='')?$rowDDforDevice['ShortDescription']:str_replace('PK_','',$DeviceDataDescriptionToDisplay[$key])).'</b> '.((@$rowDDforDevice['Tooltip']!='')?'<img src="include/images/tooltip.gif" title="'.@$rowDDforDevice['Tooltip'].'" border="0" align="middle">':'').'</td>
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
										$ddHTML.='<select name="deviceData_'.$rowD['PK_Device'].'_'.$value.'" '.((isset($rowDDforDevice['AllowedToModify']) && $rowDDforDevice['AllowedToModify']==0)?'disabled':'').' '.(($DeviceDataToDisplay[$key]==24)?'onChange="document.wizardOrbiters.action.value=\'form\';document.wizardOrbiters.submit();"':'').'>
												<option value=""></option>';
										while($rowTable=$resTable->FetchRow()){
											$ddHTML.='<option value="'.$rowTable[$DeviceDataDescriptionToDisplay[$key]].'" '.(($rowTable[$DeviceDataDescriptionToDisplay[$key]]==@$ddValue)?'selected':'').'>'.$rowTable['Description'].'</option>';
										}
										$ddHTML.='</select>';
									}
									else 
										$ddHTML.='<input type="text" name="deviceData_'.$rowD['PK_Device'].'_'.$value.'" value="'.@$ddValue.'" '.((isset($rowDDforDevice['AllowedToModify']) && $rowDDforDevice['AllowedToModify']==0)?'disabled':'').'>';
								break;
								case 'bool':
									$ddHTML.='<input type="checkbox" name="deviceData_'.$rowD['PK_Device'].'_'.$value.'" value="1" '.((@$ddValue!=0)?'checked':'').' '.((isset($rowDDforDevice['AllowedToModify']) && $rowDDforDevice['AllowedToModify']==0)?'disabled':'').'>';
								break;
								default:
									$ddHTML.='<input type="text" name="deviceData_'.$rowD['PK_Device'].'_'.$value.'" value="'.@$ddValue.'" '.((isset($rowDDforDevice['AllowedToModify']) && $rowDDforDevice['AllowedToModify']==0)?'disabled':'').'>';
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
								<input type="hidden" name="oldDeviceData_'.$rowD['PK_Device'].'_'.$value.'" value="'.(($resDDforDevice->RecordCount()>0)?$ddValue:'NULL').'">';					

						if($DeviceDataToDisplay[$key]==24){
							$selSkin=(isset($_POST['deviceData_'.$rowD['PK_Device'].'_'.$value]) && (int)$_POST['deviceData_'.$rowD['PK_Device'].'_'.$value]!=0)?(int)$_POST['deviceData_'.$rowD['PK_Device'].'_'.$value]:@$ddValue;
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
				}
			
			$out.=$ddHTML;
			
			if($isOSD==0){
				$out.='<tr>
					<td><B>This device uses a Wi-Fi connection</B></td>
					<td><input type="checkbox" name="PingTest_'.$rowD['PK_Device'].'" value="1" '.(($rowD['PingTest']==1)?'checked':'').'></td>
				</tr>';			
			}
			
			$out.='
				<tr>
					<td align="right"><input type="checkbox" name="reset_'.$rowD['PK_Device'].'" value="1"></td>
					<td>Reset Router when done regenerating</td>
				</tr>	
				<tr>
					<td align="center" colspan="2"><input type="submit" class="button" name="quickRegen_'.$rowD['PK_Device'].'" value="Quick regen"  >&nbsp;&nbsp;<input type="submit" class="button" name="fullRegen_'.$rowD['PK_Device'].'" value="Full regen"  >&nbsp;&nbsp;<input type="submit" class="button" name="update" value="Update"  > &nbsp;&nbsp;<input type="button" class="button" name="edit_'.$rowD['PK_Device'].'" value="Adv"  onClick="self.location=\'index.php?section=editDeviceParams&deviceID='.$rowD['PK_Device'].'\';"> &nbsp;&nbsp; <input type="submit" class="button" name="delete_'.$rowD['PK_Device'].'" value="Delete"  onclick="if(!confirm(\'Are you sure you want to delete this orbiter?\'))return false;">
					
					
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
							$deviceData=(isset($_POST['deviceData_'.$value.'_'.$ddValue]))?$_POST['deviceData_'.$value.'_'.$ddValue]:'';
						}else{
							$deviceData=$_POST['timeoutSS_'.$value].','.$_POST['timeoutPO_'.$value];
						}
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
				
				// main menu
				$mainMenu=((int)$_POST['mainMenu_'.$value]>0)?(int)$_POST['mainMenu_'.$value]:'';
				$dbADO->Execute('UPDATE Device_DeviceData SET IK_DeviceData=? WHERE FK_Device=? AND FK_DeviceData=?',array($mainMenu,$value,91));
			}
		}
		
		if(isset($_POST['add'])){
			$deviceTemplate=(int)$_POST['deviceTemplate'];
			if($deviceTemplate!=0){
				$insertID=exec('/usr/pluto/bin/CreateDevice -h localhost -D '.$dbPlutoMainDatabase.' -d '.$deviceTemplate.' -i '.$installationID);				
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
