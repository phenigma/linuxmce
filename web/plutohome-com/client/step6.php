<?
if($_SESSION['sollutionType']==3){
		$out='
			<table align="center" border="0" width="100%">
				 <tr>
					<td align="left" class="normaltext"><img src="images/install_wizard.gif"></td>
					<td align="right" class="normaltext"><a href="index.php?section=wizard&step=5">&lt;&lt; Previous</a> <a href="index.php?section=wizard&step=7">Next &gt;&gt;</a></td>
				</tr>				
				<tr>
					<td colspan="2" align="center" bgcolor="#DADDE4"><B>Step 6 of 8: Media Directors</B></td>
				</tr>
				<tr>
					<td colspan="2" align="center" class="insidetable" height="150">You have selected the single PC solution, so you can skip this step by clicking NEXT.  </td>
				</tr>
				<tr>
					<td colspan="2" class="insidetable" align="center"><input type="button" name="next" value="Next" onClick="self.location=\'index.php?section=wizard&step=7\'"></td>
				</tr>		
		</table>
	';
}else{
	$queryCoreDeviceTemplates='SELECT * FROM DeviceTemplate WHERE FK_DeviceCategory=?';
	$resCoreDeviceTemplates=$dbADO->Execute($queryCoreDeviceTemplates,$GLOBALS['rootMediaDirectorID']);
	$deviceTemplateIdArray=array();
	$deviceTemplateDescriptionArray=array();
	while($rowCoreDeviceTemplates=$resCoreDeviceTemplates->FetchRow()){
		$deviceTemplateDescriptionArray[]=$rowCoreDeviceTemplates['Description'];
		$deviceTemplateIdArray[]=$rowCoreDeviceTemplates['PK_DeviceTemplate'];
	}
	
	if(isset($_POST['newType']) && $_POST['newType']!='0'){
		$getRestrictions='SELECT * FROM DeviceTemplate WHERE PK_DeviceTemplate=?';
		$resRestrictions=$dbADO->Execute($getRestrictions,$_POST['newType']);
		$rowRestrictions=$resRestrictions->FetchRow();
		$filterRestrictions=($rowRestrictions['FK_OperatingSystem']!='')?" AND FK_Operatingsystem='".$rowRestrictions['FK_OperatingSystem']."'":'';
		$filterRestrictions=($rowRestrictions['FK_Distro']!='')?" AND PK_Distro='".$rowRestrictions['FK_Distro']."'":$filterRestrictions;
	}
	
	$queryDistroOS='
						SELECT Distro.*, OperatingSystem.Description AS OS FROM Distro
							INNER JOIN OperatingSystem 
								ON FK_OperatingSystem=PK_OperatingSystem
						WHERE Distro.MediaDirector=1 '.@$filterRestrictions;
	$resDistroOS=$dbADO->Execute($queryDistroOS);
	$distroIdArray=array();
	$distroDescriptionArray=array();
	while($row=$resDistroOS->FetchRow()){
		$distroDescriptionArray[$row['PK_Distro']]=$row['Description']." / ".$row['OS'];
		$distroIdArray[]=$row['PK_Distro'];
	}
	$displayedDevicesArray=explode(',',@$_POST['displayedDevices']);
	
	if($action=='form'){
		// display form
	
	

		$action=((isset($_POST['newPlatform']) && isset($_POST['newPlatform'])!='0')|| @$_SESSION['EnableDHCP']==1)?'add':$action;
		$out = '
					<script>
					function showOptions()
					{
						if(document.wizard.newType.selectedIndex!=0 && document.wizard.newPlatform.selectedIndex!=0)
						{
							document.wizard.action.value="form";
							document.wizard.submit();
						}
					}
					function showPlatforms()
					{
						document.wizard.action.value="form";
						document.wizard.submit();
					}';
		$out.='				
					</script>
						<br>
					   <form action="index.php" method="POST" name="wizard">
						<input type="hidden" name="section" value="wizard">
						<input type="hidden" name="step" value="'.$step.'">
						<input type="hidden" name="action" value="'.$action.'">	
				      		<table align="center" width="100%">
								 <tr>
									<td align="left" class="normaltext"><img src="images/install_wizard.gif"></td>
									<td align="right" class="normaltext"><a href="index.php?section=wizard&step=5">&lt;&lt; Previous</a> <a href="#" onClick="javascript:document.wizard.action.value=\'update\';document.wizard.submit();">Next &gt;&gt;</a></td>
								</tr>				
								<tr>
									<td colspan="2" align="center" bgcolor="#DADDE4"><B>Step 6 of 8: Media Directors</B></td>
								</tr>
								<tr>
									<td colspan="2" align="left" class="insidetable">Here you will add the media directors that will connect to your '.$_SESSION['coreName'].'.  You can add more by filling in the \'Add a Media Director\' section.  Be sure to give them descriptive names like \'Master Bedroom\'.
									If you bought a Pluto Media Director from a dealer you don\'t need to add it--it\'s plug and play so just plug it in.<br><br>
									The simplest way to have a Media Director is with Network Boot.  If you use a Network Boot then you won\'t need to install any software on the Media Director.  <a href="support/index.php?section=document&docID=144" target="_blank">Explain this</a><br><br>
									<b>If you want to use your Mobile Phone as a remote control or use follow-me features</b> be sure to check the "Bluetooth Dongle" option!</td>
								</tr>
								<tr>
									<td colspan="2" class="insidetable">
									<table align="center">
										<tr bgcolor="#DADDE4" class="normaltext">
											<td align="center"><B>Name</B></td>
											<td align="center"><B>Room</B></td>
											<td align="center"><B>Type</B></td>
											<td align="center"><B>Platform</B></td>
											<td align="center"><B>Network boot</B></td>
											<td>&nbsp;</td>
										</tr>';
		$queryMediaDirectors='
							SELECT Device.*, Room.Description AS Room
							FROM Device
							LEFT JOIN Room ON FK_Room=PK_Room
							WHERE FK_DeviceTemplate=? AND Device.FK_Installation=?';
		$resMediaDirectors=$dbADO->Execute($queryMediaDirectors,array($GLOBALS['rootGenericMediaDirector'],$installationID));
		$displayedDevices=array();
		if($resMediaDirectors->RecordCount()==0){
			$out.='		<tr class="normaltext">
											<td colspan="7" align="center">No Media Directors</td>
										</tr>';
		}
		$counter=0;
		while($rowMediaDirectors=$resMediaDirectors->FetchRow()){
			$displayedDevices[]=$rowMediaDirectors['PK_Device'];
			$counter++;
			if( $counter % 2==1 )
				$rowcolor = 'bgcolor="#F0F3F8"';
			else
				$rowcolor = '';
	
			$out.='
					<input type="hidden" name="oldRoomID_'.$rowMediaDirectors['PK_Device'].'" value="'.$rowMediaDirectors['FK_Room'].'">
					<input type="hidden" name="oldRoom_'.$rowMediaDirectors['PK_Device'].'" value="'.$rowMediaDirectors['Room'].'">
					<tr '.$rowcolor.'>';
	
			$queryDeviceDeviceData='SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device=? AND FK_DeviceData=?';
			$resDeviceDeviceData=$dbADO->Execute($queryDeviceDeviceData,array($rowMediaDirectors['PK_Device'],$GLOBALS['rootPK_Distro']));
			if($resDeviceDeviceData->RecordCount()>0){
				$row=$resDeviceDeviceData->FetchRow();
				$selectedDistro=$row['IK_DeviceData'];
			}
	
			if( $rowMediaDirectors['FK_Device_ControlledVia']=='' ) {
				$out.='
							<td><input type="text" name="description_'.$rowMediaDirectors['PK_Device'].'" value="'.$rowMediaDirectors['Description'].'"></td>
							<td><input type="text" name="room_'.$rowMediaDirectors['PK_Device'].'" value="'.$rowMediaDirectors['Room'].'"></td>
							<td><select name="deviceTemplate_'.$rowMediaDirectors['PK_Device'].'">';
				foreach ($deviceTemplateIdArray as $key => $value){
					$out.='<option value="'.$value.'" '.(($rowMediaDirectors['FK_DeviceTemplate']==$value)?'selected':'').'>'.$deviceTemplateDescriptionArray[$key].'</option>';
				}
				$out.='	</select>
											</td>
											<td class="normaltext">'.$distroDescriptionArray[$selectedDistro];

				$queryDiskless='SELECT * FROM Device_DeviceData WHERE FK_Device=? AND FK_DeviceData=?';
				$resDiskless=$dbADO->Execute($queryDiskless,array($rowMediaDirectors['PK_Device'],$GLOBALS['rootDisklessBoot']));
	
				$out .='<input type="hidden" name="oldDisklessBoot_'.$rowMediaDirectors['PK_Device'].'" value="'.(($resDiskless->RecordCount()>0)?'1':'0').'">
						</td>
							<td  align="center">'.(($selectedDistro==1)?'<input type="checkbox" name="disklessBoot_'.$rowMediaDirectors['PK_Device'].'" '.(($resDiskless->RecordCount()>0)?'checked':'').' value="1" onClick="showMAC(\''.$rowMediaDirectors['PK_Device'].'\');"><span id="MACBox_'.$rowMediaDirectors['PK_Device'].'" name="MACBox_'.$rowMediaDirectors['PK_Device'].'" style="display:'.(($resDiskless->RecordCount()>0)?'':'none').'" class="normaltext">MAC Address: <input type="text" name="mdMAC_'.$rowMediaDirectors['PK_Device'].'" value="'.$rowMediaDirectors['MACaddress'].'"></span>':'&nbsp;').'</td>
							<td class="normaltext">'.'<input type="button" name="delete_'.$rowMediaDirectors['PK_Device'].'" value="Delete" onClick="self.location=\'index.php?section=wizard&step=6&did='.$rowMediaDirectors['PK_Device'].'&action=remove\'"></td>
						</tr>';
			}
			else {
				$out .='
					<td class="normaltext"><B>'.$rowMediaDirectors['Description'].'</B></td>
					<input type="hidden" name="description_'.$rowMediaDirectors['PK_Device'].'" value="'.$rowMediaDirectors['Description'].'">
					<input type="hidden" name="platform_'.$rowMediaDirectors['PK_Device'].'" value="'.@$selectedDistro.'">
					<input type="hidden" name="deviceTemplate_'.$rowMediaDirectors['PK_Device'].'" value="'.$GLOBALS['rootGenericMediaDirector'].'">
					<td colspan="6" class="normaltext">This is your hybrid--the media director software will be running on the same computer as the core.  Choose the media director-related software modules you want on this hybrid.  To change the core-related software modules or other options, return to the <a href="index.php?section=wizard&step=5">Core page</a>.  You can add more stand-alone media directors below that will connect to this hybrid.</td>
					</tr>';
			}
			$orbiterMDChild=getMediaDirectorOrbiterChild($rowMediaDirectors['PK_Device'],$dbADO);
			if($orbiterMDChild){
				$out.='
						<tr '.$rowcolor.'>	
							<td colspan="7">'.getInstallWizardDeviceTemplates($step,$dbADO,$orbiterMDChild,$selectedDistro).'</td>
						</tr>';
			}
		}
		$out.='<input type="hidden" name="displayedDevices" value="'.join(',',$displayedDevices).'">';
		$out.='		</table>
									</td>
								</tr>';
		$showPlatform=(@$_SESSION['EnableDHCP']==1)?1:0;
		if(isset($_POST['newType'])){
			$showPlatform=(@$_POST['disklessBoot_']==1)?1:0;
		}

		$out.='
					<tr class="normaltext">
						<td colspan="2" align="center"><input type="button" name="continue" value="Next" onClick="javascript:document.wizard.action.value=\'update\';document.wizard.submit();"></td>
					</tr>';
		$out.='
								<tr class="normaltext">
									<td colspan="2"><B>Add a Media Director</B><a name="addForm"></a></td>
								</tr>
								<tr class="normaltext">
									<td>Name</td>
									<td><input type="text" name="Description" value="'.@stripslashes($_POST['Description']).'"> "Joe\'s PC", "Aluminum HTPC", etc.</td>
								</tr>
								<tr class="normaltext">
									<td>Room</td>
									<td><input type="text" name="mdRoom" value="'.@stripslashes($_POST['mdRoom']).'"> "Master bedroom", "Family room", etc.</td>
								</tr>					
								<tr class="normaltext">
									<td>Type</td>
									<td><select name="newType" '.(($showPlatform!=1)?'onChange="showPlatforms();"':'').'>
										<option value="0">-Please select-</option>';
		foreach ($deviceTemplateIdArray as $key => $value){
			$out.='<option value="'.$value.'" '.(($value==@$_POST['newType'])?'selected':'').'>'.$deviceTemplateDescriptionArray[$key].'</option>';
		}
		
		$out.='	</select> "Generic PC" if your model isn\'t listed</td>
								</tr>
								<tr class="normaltext">
									<td>Network boot '.((@$_SESSION['EnableDHCP']==1)?'(Recommended)':'').'</td>
									<td><input type="checkbox" name="disklessBoot_" value="1" '.(($showPlatform==1)?'checked':'').' onClick="document.wizard.action.value=\'form\';document.wizard.submit();"> <span id="MACBox_" name="MACBox_" style="display:'.(($showPlatform==1)?'':'none').'">MAC Address: <input type="text" name="mdMAC" value=""></span> <a href="support/index.php?section=document&docID=144" target="_blank">what\'s the MAC address?</a></td>
								</tr>';
		if($showPlatform==1){
			$selectedPlatform=1;	// Debian-sarge
		}
		else{
			$selectedPlatform=(int)@$_POST['newPlatform'];
			$out.='		
								<tr class="normaltext">
									<td>Platform</td>
									<td><select name="newPlatform" onChange="showOptions();">
										<option value="0">-'.((isset($_POST['newType']))?'Please select':'Select type first').'-</option>';
			if(isset($_POST['newType'])){
				foreach ($distroDescriptionArray as $key => $value){
					$out.='<option value="'.$key.'" '.(($key==@$_POST['newPlatform'])?'selected':'').'>'.$value.'</option>';
				}
			}
			$out.='</select> Normally Debian Sarge / Linux</td>
								</tr>';
		}
		if($selectedPlatform!=0)
			$out.='				<tr class="normaltext">
									<td colspan="2">'.getInstallWizardDeviceTemplates($step,$dbADO,'',$selectedPlatform).'</td>
								</tr>';
		$out.='	<tr>
									<td colspan="2" align="center">'.(($selectedPlatform!=0)?'<input type="submit" name="continue" value="Add Media Director">':'').'</td>
								</tr>
				      		</table>
						</form>
						<script>
						function showMAC(deviceID)
						{
							isDiskless=eval("document.wizard.disklessBoot_"+deviceID+".checked");
							if(isDiskless)
								eval("document.getElementById(\'MACBox_"+deviceID+"\').style.display=\'\';")
							else
								eval("document.getElementById(\'MACBox_"+deviceID+"\').style.display=\'none\';")
						}
						</script>
						';
		if($selectedPlatform!=0){
			$out.='
					 	<script>
			 				var frmvalidator = new formValidator("wizard");
	 						frmvalidator.addValidation("Description","req","Please enter a description.");
							frmvalidator.addValidation("mdRoom","req","Please enter a name for the room.");
							frmvalidator.addValidation("newType","dontselect=0","Please select the type for the media director.");
						</script>';
		}
		if(isset($_POST['Description']) && $_POST['Description']!=''){
			$scriptInBody='onLoad="self.location=\'#addForm\';"';
			$output->setScriptInBody($scriptInBody);
		}
	
	}elseif($action=='add'){
		// process form step 5
		updateMediaDirectors($displayedDevicesArray,$dbADO);
		
		if(isset($_POST['continue'])){
			$description=stripslashes($_POST['Description']);
			$FK_DeviceTemplate=$_POST['newType'];
			$FK_Distro=((isset($_SESSION['EnableDHCP']) && $_SESSION['EnableDHCP']==1) || isset($_POST['disklessBoot_']))?1:$_POST['newPlatform'];
			$mdRoom=stripslashes($_POST['mdRoom']);

			$queryRoom='SELECT * FROM Room WHERE Description=? AND FK_Installation=?';
			$resRoom=$dbADO->Execute($queryRoom,array($mdRoom,$installationID));
			if($resRoom->RecordCount()==0){
				$insertRoom='INSERT INTO Room (Description,FK_RoomType,FK_Installation) VALUES (?,?,?)';
				$dbADO->Execute($insertRoom,array($mdRoom,$GLOBALS['MiscelaneousRoomType'],$installationID));
				$roomID=$dbADO->Insert_ID();
				addScenariosToRoom($roomID, $installationID, $dbADO);
	
			}else{
				$rowRoom=$resRoom->FetchRow();
				$roomID=$rowRoom['PK_Room'];
			}
	
	
			$diskless=(isset($_POST['disklessBoot_']))?$_POST['disklessBoot_']:0;
			$macAddress=($diskless!=0)?$_POST['mdMAC']:NULL;
	
			$insertDevice='INSERT INTO Device (Description, FK_DeviceTemplate, FK_Installation,MACaddress,FK_Room) VALUES (?,?,?,?,?)';
			$dbADO->Execute($insertDevice,array($description,$FK_DeviceTemplate,$_SESSION['installationID'],$macAddress,$roomID));
			$insertID=$dbADO->Insert_ID();
	
			InheritDeviceData($FK_DeviceTemplate,$insertID,$dbADO);
			createChildsForControledViaDeviceTemplate($FK_DeviceTemplate,$_SESSION['installationID'],$insertID,$dbADO,$roomID);
			createChildsForControledViaDeviceCategory($FK_DeviceTemplate,$_SESSION['installationID'],$insertID,$dbADO,$roomID);
	
	
			// install options
			$orbiterMDChild=getMediaDirectorOrbiterChild($insertID,$dbADO);
			$installOptionsArray=explode(',',$_POST['displayedTemplatesRequired_']);
			foreach($installOptionsArray AS $elem){
				$optionalDevice=(isset($_POST['device__requiredTemplate_'.$elem]))?$_POST['device__requiredTemplate_'.$elem]:0;
				if($optionalDevice!=0){
					$OptionalDeviceName=cleanString(@$_POST['templateName_'.$elem]);
					$insertDevice='
										INSERT INTO Device 
											(Description, FK_DeviceTemplate, FK_Installation, FK_Device_ControlledVia,FK_Room) 
										VALUES (?,?,?,?,?)';
					$dbADO->Execute($insertDevice,array($OptionalDeviceName,$elem,$installationID,$orbiterMDChild,$roomID));
					$optionInsertId=$dbADO->Insert_ID();
				}
			}
	
			$insertDeviceDeviceData='INSERT INTO Device_DeviceData (FK_Device,FK_DeviceData,IK_DeviceData) VALUES (?,?,?)';
			$dbADO->Execute($insertDeviceDeviceData,array($insertID,$GLOBALS['rootPK_Distro'],$FK_Distro));
	

			if($diskless!=0){
				$insertDeviceDeviceData='INSERT INTO Device_DeviceData (FK_Device,FK_DeviceData,IK_DeviceData) VALUES (?,?,?)';
				$dbADO->Execute($insertDeviceDeviceData,array($insertID,$GLOBALS['rootDisklessBoot'],1));
			}
		}
		
		header("Location: index.php?section=wizard&step=6");
	}else{
		// action =update and jump to next step

		if(isset($_REQUEST['did']) && (int)$_REQUEST['did']>0){
			$did=(int)$_REQUEST['did'];
			$isInInstallation=$dbADO->Execute('SELECT * FROM Device WHERE PK_Device=? AND FK_Installation=?',array($did,$installationID));
			if($isInInstallation->RecordCount()==0){
				header("Location: index.php?section=wizard&step=6");				
				exit();
			}else{
				deleteDevice($did,$dbADO);
				header("Location: index.php?section=wizard&step=6");				
				exit();
			}
		}
		updateMediaDirectors($displayedDevicesArray,$dbADO);
		
		header("Location: index.php?section=wizard&step=".(($action=='updateOnly')?'6':'7'));
	}
}

function updateMediaDirectors($displayedDevicesArray,$dbADO)
{
	foreach($displayedDevicesArray as $value){
		$description=stripslashes(@$_POST['description_'.$value]);
		$FK_DeviceTemplate=@$_POST['deviceTemplate_'.$value];
		$oldInstallSourceCode=@$_POST['oldInstallSourceCode_'.$value];
		$oldDisklessBoot=@$_POST['oldDisklessBoot_'.$value];
		$diskless=(isset($_POST['disklessBoot_'.$value]))?$_POST['disklessBoot_'.$value]:0;
		$macAddress=($diskless!=0)?@$_POST['mdMAC_'.$value]:NULL;
		$oldRoomID=@$_POST['oldRoomID_'.$value];
		$oldRoom=@$_POST['oldRoom_'.$value];
		$mdRoom=stripslashes(@$_POST['room_'.$value]);

		$updateDevice='UPDATE Device SET Description=?, FK_DeviceTemplate=?,MACaddress=? WHERE PK_Device=?';
		$dbADO->Execute($updateDevice,array($description,$FK_DeviceTemplate,$macAddress,$value));

		if($oldRoom!=$mdRoom && $mdRoom!=''){
			$updateRoom='UPDATE Room SET Description=? WHERE PK_Room=?';
			$dbADO->Execute($updateRoom,array($mdRoom,$oldRoomID));
		}

		if($oldDisklessBoot!=$diskless){
			if($diskless==1 && $value!=''){
				$insertDeviceDeviceData='INSERT INTO Device_DeviceData (FK_Device,FK_DeviceData,IK_DeviceData) VALUES (?,?,?)';
				$dbADO->Execute($insertDeviceDeviceData,array($value,$GLOBALS['rootDisklessBoot'],1));
			}else{
				$deleteDeviceDeviceData='DELETE FROM Device_DeviceData WHERE FK_Device=? AND FK_DeviceData=?';
				$dbADO->Execute($deleteDeviceDeviceData,array($value,$GLOBALS['rootDisklessBoot']));
			}
		}

		// install options - delete or insert devices
		//$dbADO->debug=true;
		$orbiterMDChild=getMediaDirectorOrbiterChild($value,$dbADO);
		$installOptionsArray=explode(',',@$_POST['displayedTemplatesRequired_'.$orbiterMDChild]);
		foreach($installOptionsArray AS $elem){
			$oldDevice=@$_POST['oldDevice_'.$orbiterMDChild.'_requiredTemplate_'.$elem];
			$optionalDevice=(isset($_POST['device_'.$orbiterMDChild.'_requiredTemplate_'.$elem]))?$_POST['device_'.$orbiterMDChild.'_requiredTemplate_'.$elem]:0;
			if($optionalDevice!=0){
				$OptionalDeviceName=cleanString(@$_POST['templateName_'.$elem]);
				$insertDevice='
						INSERT INTO Device 
							(Description, FK_DeviceTemplate, FK_Installation, FK_Device_ControlledVia,FK_Room) 
						VALUES (?,?,?,?,?)';
				if($oldDevice==''){
					$dbADO->Execute($insertDevice,array($OptionalDeviceName,$elem,$installationID,$orbiterMDChild,$oldRoomID));
					$optionInsertId=$dbADO->Insert_ID();
				}
			}else{
				deleteDevice($oldDevice,$dbADO);
			}
		}
	}
}
?>
