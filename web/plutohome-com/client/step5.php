<?
if($action=='form'){
	$out='
		<script>
					
		function check_if_enter()
		{
			if(window.Event){
				window.captureEvents(Event.KEYPRESS)
			}
			document.onkeypress=  keyPressed;
		}

		function keyPressed(e)
		{
			var n
			(window.Event) ? n=e.which : n=event.keyCode
		     if (n==13){
		         enableHybrid();
		     }
		}
		function showSourceOptions()
		{
			if(document.wizard.installSourceCode.checked){
				document.getElementById("installOptions").style.display="";
			}else
				document.getElementById("installOptions").style.display="none";
		}
		function setDHCPOptions(val)
		{
			document.wizard.enableDHCP[0].disabled=val;
			document.wizard.enableDHCP[1].disabled=val;
			document.wizard.ip_1.disabled=val;
			document.wizard.ip_2.disabled=val;		
			document.wizard.ip_3.disabled=val;
		}
		</script>
		';
	// display form
	$queryDevice='
		SELECT Device.*,Room.Description AS RoomName 
		FROM Device
		INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
		LEFT JOIN Room ON FK_Room=PK_Room
		WHERE FK_DeviceCategory=? AND Device.FK_Installation=?
		';
	$resDevice=$dbADO->Execute($queryDevice,array($GLOBALS['rootCoreID'],$installationID));
	if($resDevice->RecordCount()!=0){
		$rowDevice=$resDevice->FetchRow();
		$roomName=$rowDevice['RoomName'];
		$_SESSION['deviceID']=$rowDevice['PK_Device'];
		$_SESSION['coreRoom']=$rowDevice['FK_Room'];
	}else{
		unset($_SESSION['deviceID']);
		unset($_SESSION['isCoreFirstTime']);
		unset($_SESSION['CoreDCERouter']);
		unset($_SESSION['coreHybridID']);
		unset($_SESSION['OrbiterHybridChild']);
		unset($_SESSION['isHybridFirstTime']);
		unset($_SESSION['coreRoom']);
		$FK_DeviceTemplate=0;
		$FK_Distro=0;
	}

	if(isset($_SESSION['deviceID'])){
		$queryDevice='SELECT * FROM Device WHERE PK_Device=?';
		$resDevice=$dbADO->Execute($queryDevice,$_SESSION['deviceID']);
		$rowDevice=$resDevice->FetchRow();
		$FK_DeviceTemplate=$rowDevice['FK_DeviceTemplate'];

		$queryDeviceDeviceData='SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device=? AND FK_DeviceData=?';
		$resDeviceDeviceData=$dbADO->Execute($queryDeviceDeviceData,array($_SESSION['deviceID'],$GLOBALS['rootPK_Distro']));
		$rowDeviceDeviceData=$resDeviceDeviceData->FetchRow();
		$FK_Distro=$rowDeviceDeviceData['IK_DeviceData'];

		if($FK_Distro!=$_SESSION['distro']){
			deleteDevice($_SESSION['deviceID'],$dbADO);
			unset($_SESSION['deviceID']);
			unset($_SESSION['coreHybridID']);
			unset($_SESSION['isHybridFirstTime']);
			unset($_SESSION['OrbiterHybridChild']);
			header('Location: index.php?section=wizard&step=5');
		}
		
		$queryOS='SELECT FK_OperatingSystem FROM Distro WHERE PK_Distro=?';

		$resOS=$dbADO->Execute($queryOS,$FK_Distro);
		$rowOS=$resOS->FetchRow();
		$CoreOS=$rowOS['FK_OperatingSystem'];

		// query if get Pluto Core is checked
		$queryDeviceDeviceData='SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device=? AND FK_DeviceData=?';
		$resDeviceDeviceData=$dbADO->Execute($queryDeviceDeviceData,array($_SESSION['deviceID'],$GLOBALS['rootDevelopment']));
		$_SESSION['installSourceCode']=($resDeviceDeviceData->RecordCount()>0)?1:0;

		$queryDeviceMD='SELECT * FROM Device WHERE FK_DeviceTemplate=? AND FK_Device_ControlledVia IS NOT NULL AND FK_Installation=?';
		$resDeviceMD=$dbADO->Execute($queryDeviceMD,array($GLOBALS['rootGenericMediaDirector'],$installationID));
		if($resDeviceMD->RecordCount()>0){
			$rowDeviceMD=$resDeviceMD->FetchRow();
			$_SESSION['coreHybridID']=$rowDeviceMD['PK_Device'];
		}

		$getDCERouter='SELECT * FROM Device WHERE FK_DeviceTemplate=? AND FK_Device_ControlledVia=?';
		$resDCERouter=$dbADO->Execute($getDCERouter,array($GLOBALS['rootDCERouter'],$_SESSION['deviceID']));
		if($resDCERouter->RecordCount()!=0){
			$rowDCERouter=$resDCERouter->FetchRow();
			$_SESSION['CoreDCERouter']=$rowDCERouter['PK_Device'];
		}
		
		$queryDHCP='SELECT * FROM Device_DeviceData WHERE FK_Device=? AND FK_DeviceData=?';
		$resDHCP=$dbADO->Execute($queryDHCP,array($_SESSION['deviceID'],$GLOBALS['DHCPDeviceData']));
		if($resDHCP->RecordCount()!=0){
			$_SESSION['EnableDHCP']=1;
			$rowDHCP=$resDHCP->FetchRow();
			$ipArray=explode('-',$rowDHCP['IK_DeviceData']);
			$ipPartsArray=explode('.',$ipArray[0]);
		}else{
			$_SESSION['EnableDHCP']=0;
		}
	}

	$getInstallation='SELECT * FROM Installation WHERE PK_Installation=?';
	$resInstallation=$dbADO->Execute($getInstallation,$installationID);
	$rowInstallation=$resInstallation->FetchRow();
	$selectedRepositoryType=($rowInstallation['FK_RepositoryType_Source'])?$rowInstallation['FK_RepositoryType_Source']:4;


	if(isset($_POST['computerType']) && $_POST['computerType']!='0'){
		$getRestrictions='SELECT * FROM DeviceTemplate WHERE PK_DeviceTemplate=?';
		$resRestrictions=$dbADO->Execute($getRestrictions,$_POST['computerType']);
		$rowRestrictions=$resRestrictions->FetchRow();
		$filterRestrictions=($rowRestrictions['FK_OperatingSystem']!='')?" AND FK_Operatingsystem='".$rowRestrictions['FK_OperatingSystem']."'":'';
		$filterRestrictions=($rowRestrictions['FK_Distro']!='')?" AND PK_Distro='".$rowRestrictions['FK_Distro']."'":$filterRestrictions;
	}

	$out .= '<br>
				   <form action="index.php" method="POST" name="wizard">
					<input type="hidden" name="section" value="wizard">
					<input type="hidden" name="step" value="'.$step.'">
					<input type="hidden" name="action" value="'.((isset($_SESSION['deviceID']))?'update':'addDevice').'">	
			      		<table align="left" border="0" width="100%">
							 <tr>
								<td align="left" class="normaltext"><img src="images/install_wizard.gif"></td>
								<td align="right" class="normaltext"><a href="index.php?section=wizard&step=4">&lt;&lt; Previous</a> '.((isset($_SESSION['deviceID']))?'<a href="#" onClick="javascript:document.wizard.submit();">Next &gt;&gt;</a>':'').'</td>
							</tr>				
							<tr>
								<td colspan="2" align="center" bgcolor="#DADDE4"><B>Step 5 of 8: Pluto Core</B></td>
							</tr>
							<tr class="normaltext">
								<td colspan="2" align="left">Tell me about the computer you’ll use as the core.  Just choose "Generic PC" if we don’t yet have your specific model listed.<br><br>  
								</td>
							</tr>
							<tr class="normaltext">
								<td width="350">&nbsp;&nbsp;&nbsp;&nbsp;First, what type of computer is it?</td>
								<td><select name="computerType">
									<option value="0">- Please select -</option>';

	$queryCoreDeviceTemplates='SELECT * FROM DeviceTemplate WHERE FK_DeviceCategory=?';
	$resCoreDeviceTemplates=$dbADO->Execute($queryCoreDeviceTemplates,$GLOBALS['rootCoreID']);
	while($rowCoreDeviceTemplates=$resCoreDeviceTemplates->FetchRow()){
		$out.='<option value="'.$rowCoreDeviceTemplates['PK_DeviceTemplate'].'" '.(($rowCoreDeviceTemplates['PK_DeviceTemplate']==$FK_DeviceTemplate || $rowCoreDeviceTemplates['PK_DeviceTemplate']==@$_POST['computerType'])?'selected':'').'>'.$rowCoreDeviceTemplates['Description'].'</option>';
	}
	$out.='		</td>
							</tr>
							<tr class="normaltext">
								<td>&nbsp;&nbsp;&nbsp;&nbsp;In what room will you put it? (Living Room, Bedroom, etc.)</td>
								<td><input type="text" name="coreRoom" value="'.@$roomName.'" onkeypress="check_if_enter();"></td>
							</tr>									
							<tr>
								<td colspan="2">&nbsp;</td>
							</tr>
							<tr class="normaltext" bgcolor="#DADDE4">
								<td colspan="2"><input type="checkbox" name="installSourceCode" value="1" '.((@$_SESSION['installSourceCode']==1)?'checked':'').' onClick="showSourceOptions();"> <b>Install Pluto’s source code</b> too.  This will also add any development libraries that are needed to compile.</td>
							</tr>
							<tr class="normaltext">
								<td colspan="2"><div align="left" id="installOptions" style="background-color:#DADDE4; display:'.((@$_SESSION['installSourceCode']==1)?'':'none').';">How should the the install program fetch the source code?  SVN is preferred.<br>
										<input type="radio" name="method" value="6" '.(($selectedRepositoryType==6)?'checked':'').'> Download and uncompress an archive (tar, zip, etc.)&nbsp;&nbsp;&nbsp
										<input type="radio" name="method" value="1" '.(($selectedRepositoryType==1)?'checked':'').'> a package (deb, rpm, etc.)&nbsp;&nbsp;&nbsp
										<input type="radio" name="method" value="3" '.(($selectedRepositoryType==3)?'checked':'').'> cvs&nbsp;&nbsp;&nbsp
										<input type="radio" name="method" value="4" '.(($selectedRepositoryType==4)?'checked':'').'> svn&nbsp;&nbsp;&nbsp
									</div>
								</td>
							</tr>';
	if($_SESSION['dedicated']==1){
		$out.='
							<tr class="normaltext">
								<td colspan="2"><br>Do you want the Core to be your DHCP server?  This will allow you to use plug-and-play and network boot.  It is highly recommended.</td>
							</tr>
							<tr class="normaltext">
								<td colspan="2">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<input type="radio" name="enableDHCP" value="1" '.((!isset($_SESSION['EnableDHCP']) || @$_SESSION['EnableDHCP']==1)?'checked':'').'> Yes - The core will be my DHCP server</td>
							</tr>
							<tr class="normaltext" colspan=>
								<td colspan="2">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<input type="radio" name="enableDHCP" value="0" '.((isset($_SESSION['EnableDHCP']) && $_SESSION['EnableDHCP']==0)?'checked':'').'> No -- I have another DHCP server.  I understand I won\'t have plug-and-play or network boot.</td>
							</tr>
		

		';
	}
	$out.='
							<tr>	
								<td colspan="2" class="normaltext">'.((!isset($_SESSION['deviceID']))?getInstallWizardDeviceTemplates($step,$dbADO,'',$_SESSION['distro'],$_SESSION['OperatingSystem']):getInstallWizardDeviceTemplates($step,$dbADO,$_SESSION['CoreDCERouter'],$FK_Distro,$CoreOS)).'</td>
							</tr>	
							<tr>
								<td colspan="2" align="center"><input type="submit" name="continue" value="Next"></td>
							</tr>		
			      		</table>
						<input type="hidden" name="oldDHCP" value="'.@$rowDHCP['IK_DeviceData'].'">
					</form>
				 	<script>
		 				var frmvalidator = new formValidator("wizard");
						frmvalidator.addValidation("computerType","dontselect=0","Please select computer type.");
 						frmvalidator.addValidation("coreRoom","req","Please enter a name for the room.");
					</script>
		
			      	';

}else{
	// process form
	$deviceTemplate=$_POST['computerType'];
	$distro=$_SESSION['distro'];
	$coreRoom=cleanString($_POST['coreRoom']);

	$updateInstallation='UPDATE Installation SET FK_RepositoryType_Source=? WHERE PK_Installation=?';
	$dbADO->Execute($updateInstallation,array((int)$_POST['method'],$installationID));

	if($action=='addDevice'){
		$queryRoom='SELECT * FROM Room WHERE Description=? AND FK_Installation=?';
		$resRoom=$dbADO->Execute($queryRoom,array($coreRoom,$installationID));
		if($resRoom->RecordCount()==0){
			$insertRoom='INSERT INTO Room (Description,FK_RoomType,FK_Installation) VALUES (?,?,?)';
			$dbADO->Execute($insertRoom,array($coreRoom,$GLOBALS['MiscelaneousRoomType'],$installationID));
			$roomID=$dbADO->Insert_ID();
			addScenariosToRoom($roomID, $installationID, $dbADO);

		}else{
			$rowRoom=$resRoom->FetchRow();
			$roomID=$rowRoom['PK_Room'];
		}
		$_SESSION['coreRoom']=$roomID;

		$insertDevice='INSERT INTO Device (Description, FK_DeviceTemplate, FK_Installation,FK_Room) VALUES (?,?,?,?)';
		$dbADO->Execute($insertDevice,array('CORE',$deviceTemplate,$installationID,$roomID));
		$deviceID=$dbADO->Insert_ID();
		createChildsForControledViaDeviceTemplate($deviceTemplate,$installationID,$deviceID,$dbADO,$roomID);
		createChildsForControledViaDeviceCategory($deviceTemplate,$installationID,$deviceID,$dbADO,$roomID);

		$getDCERouter='SELECT * FROM Device WHERE FK_DeviceTemplate=? AND FK_Device_ControlledVia=?';
		$resDCERouter=$dbADO->Execute($getDCERouter,array($GLOBALS['rootDCERouter'],$deviceID));
		if($resDCERouter->RecordCount()!=0){
			$rowDCERouter=$resDCERouter->FetchRow();
			$_SESSION['CoreDCERouter']=$rowDCERouter['PK_Device'];
		}

		$insertDeviceDeviceData='INSERT INTO Device_DeviceData (FK_Device, FK_DeviceData,IK_DeviceData) VALUES (?,?,?)';
		$dbADO->Execute($insertDeviceDeviceData,array($deviceID,$GLOBALS['rootPK_Distro'],$distro));

		$_SESSION['deviceID']=$deviceID;
		$_SESSION['isCoreFirstTime']=1;

		// install options - delete or insert devices
		$installOptionsArray=explode(',',$_POST['displayedTemplatesRequired_']);
		foreach($installOptionsArray AS $elem){
			$optionalDevice=(isset($_POST['device__requiredTemplate_'.$elem]))?$_POST['device__requiredTemplate_'.$elem]:0;
			if($optionalDevice!=0){
				$OptionalDeviceName=cleanString(@$_POST['templateName_'.$elem]);
				$queryIsPlugin='SELECT IsPlugIn FROM DeviceTemplate WHERE PK_DeviceTemplate=? AND IsPlugIn=1';
				$resIsPlugin=$dbADO->Execute($queryIsPlugin,$elem);
				$parentDevice=($resIsPlugin->RecordCount()>0)?$_SESSION['CoreDCERouter']:$_SESSION['deviceID'];

				$insertDevice='
						INSERT INTO Device 
							(Description, FK_DeviceTemplate, FK_Installation, FK_Device_ControlledVia,FK_Room) 
						VALUES (?,?,?,?,?)';
				$dbADO->Execute($insertDevice,array($OptionalDeviceName,$elem,$installationID,$parentDevice,$_SESSION['coreRoom']));
				$childDeviceId=$dbADO->Insert_ID();
			}
		}

		$installSourceCode=(isset($_POST['installSourceCode'])?1:0);

		$_SESSION['installSourceCode']=$installSourceCode;
		if($installSourceCode!=0){
			$insertDeviceDeviceData='INSERT INTO Device_DeviceData (FK_Device, FK_DeviceData,IK_DeviceData) VALUES (?,?,?)';
			$dbADO->Execute($insertDeviceDeviceData,array($_SESSION['deviceID'],$GLOBALS['rootDevelopment'],'1'));
		}

		if((int)@$_POST['enableDHCP']==1){
			$IPtoDeviceDeviceData='192.168.80.2-192.168.80.128,192.168.80.129-192.168.80.254';

			$insertDHCP='INSERT INTO Device_DeviceData (FK_Device, FK_DeviceData,IK_DeviceData) VALUES (?,?,?)';
			$dbADO->Execute($insertDHCP,array($_SESSION['deviceID'],$GLOBALS['DHCPDeviceData'],$IPtoDeviceDeviceData));

			$updateDevice='UPDATE Device SET IPaddress=? WHERE PK_Device=?';
			$dbADO->Execute($updateDevice,array('192.168.80.1',$_SESSION['deviceID']));
			$_SESSION['EnableDHCP']=1;
		}

		if($_SESSION['sollutionType']!=1){
			$insertDeviceMD='INSERT INTO Device (Description, FK_DeviceTemplate, FK_Installation, FK_Device_ControlledVia,FK_Room) VALUES (?,?,?,?,?)';
			$hybridName=($_SESSION['sollutionType']==3)?'Standalone Core/Hybrid':'The core/hybrid';
			$dbADO->Execute($insertDeviceMD,array($hybridName,$GLOBALS['rootGenericMediaDirector'],$installationID,$_SESSION['deviceID'],$_SESSION['coreRoom']));
			$_SESSION['coreHybridID']=$dbADO->Insert_ID();

			// inherit DeviceData for hybrid MD
			InheritDeviceData($GLOBALS['rootGenericMediaDirector'],$_SESSION['coreHybridID'],$dbADO);
			createChildsForControledViaDeviceTemplate($GLOBALS['rootGenericMediaDirector'],$_SESSION['installationID'],$_SESSION['coreHybridID'],$dbADO,$_SESSION['coreRoom']);
			createChildsForControledViaDeviceCategory($GLOBALS['rootGenericMediaDirector'],$_SESSION['installationID'],$_SESSION['coreHybridID'],$dbADO,$_SESSION['coreRoom']);

			// get PK_Device for Orbiter child to Hybrid
			$OrbiterHybridChild=getMediaDirectorOrbiterChild($_SESSION['coreHybridID'],$dbADO);
			if(!is_null($OrbiterHybridChild))
				$_SESSION['OrbiterHybridChild']=$OrbiterHybridChild;

			$insertDeviceMDDeviceData='INSERT INTO Device_DeviceData (FK_Device, FK_DeviceData,IK_DeviceData) VALUES (?,?,?)';
			$dbADO->Execute($insertDeviceMDDeviceData,array($_SESSION['coreHybridID'],$GLOBALS['rootPK_Distro'],$distro));
			
			$_SESSION['isHybridFirstTime']=1;
		}

		if($_SESSION['dedicated']==1){
			$dbADO->Execute('INSERT INTO Device_DeviceData (FK_Device, FK_DeviceData, IK_DeviceData) VALUES (?,?,1)',array($_SESSION['deviceID'],$GLOBALS['kickStartCD']));
		}
		header("Location: index.php?section=wizard&step=6");
	}else{
		$installSourceCode=(isset($_POST['installSourceCode'])?$_POST['installSourceCode']:0);
		$oldInstallSourceCode=(isset($_SESSION['installSourceCode'])?$_SESSION['installSourceCode']:0);

		if($oldInstallSourceCode!=$installSourceCode){
			$_SESSION['installSourceCode']=$installSourceCode;
			if($installSourceCode!=0){
				$insertDeviceDeviceData='INSERT INTO Device_DeviceData (FK_Device, FK_DeviceData,IK_DeviceData) VALUES (?,?,?)';
				$dbADO->Execute($insertDeviceDeviceData,array($_SESSION['deviceID'],$GLOBALS['rootDevelopment'],'1'));
			}else{
				$deleteDeviceDeviceData='DELETE FROM Device_DeviceData WHERE FK_Device=? AND FK_DeviceData=?';
				$dbADO->Execute($deleteDeviceDeviceData,array($_SESSION['deviceID'],$GLOBALS['rootDevelopment']));
			}
		}

		$dbADO->Execute('UPDATE Room SET Description=? WHERE PK_Room=?',array($coreRoom,$_SESSION['coreRoom']));

		// install options - delete or insert devices
		$installOptionsArray=explode(',',$_POST['displayedTemplatesRequired_'.$_SESSION['CoreDCERouter']]);
		foreach($installOptionsArray AS $elem){
			$optionalDevice=(isset($_POST['device_'.$_SESSION['CoreDCERouter'].'_requiredTemplate_'.$elem]))?$_POST['device_'.$_SESSION['CoreDCERouter'].'_requiredTemplate_'.$elem]:0;
			$oldDevice=$_POST['oldDevice_'.$_SESSION['CoreDCERouter'].'_requiredTemplate_'.$elem];
			if($optionalDevice!=0){
				$OptionalDeviceName=cleanString(@$_POST['templateName_'.$elem]);
				$queryIsPlugin='SELECT IsPlugIn FROM DeviceTemplate WHERE PK_DeviceTemplate=? AND IsPlugIn=1';
				$resIsPlugin=$dbADO->Execute($queryIsPlugin,$elem);
				$parentDevice=($resIsPlugin->RecordCount()>0)?$_SESSION['CoreDCERouter']:$_SESSION['deviceID'];

				$insertDevice='
									INSERT INTO Device 
										(Description, FK_DeviceTemplate, FK_Installation, FK_Device_ControlledVia,FK_Room) 
									VALUES (?,?,?,?,?)';
				if($oldDevice==''){
					$dbADO->Execute($insertDevice,array($OptionalDeviceName,$elem,$installationID,$parentDevice,$_SESSION['coreRoom']));
					$childDeviceId=$dbADO->Insert_ID();
				}
			}else{
				$dbADO->Execute("DELETE FROM Device WHERE PK_Device='".$oldDevice."'");
			}
		}

		$enableDHCP=(int)@$_POST['enableDHCP'];
		$oldDHCP=(isset($_POST['oldDHCP']))?$_POST['oldDHCP']:'';
		if($enableDHCP==1){
			$IPtoDeviceDeviceData='192.168.1.129-192.168.1.254';
			if($oldDHCP==''){
				$insertDHCP='INSERT INTO Device_DeviceData (FK_Device, FK_DeviceData,IK_DeviceData) VALUES (?,?,?)';
				$dbADO->Execute($insertDHCP,array($_SESSION['deviceID'],$GLOBALS['DHCPDeviceData'],$IPtoDeviceDeviceData));
				
				$updateDevice='UPDATE Device SET IPaddress=? WHERE PK_Device=?';
				$dbADO->Execute($updateDevice,array("$ip_1.$ip_2.$ip_3.1",$_SESSION['deviceID']));
			}
		}else{
			$deleteDeviceDeviceData='DELETE FROM Device_DeviceData WHERE FK_Device=? AND FK_DeviceData=?';
			$dbADO->Execute($deleteDeviceDeviceData,array($_SESSION['deviceID'],$GLOBALS['DHCPDeviceData']));
		}
					
		header("Location: index.php?section=wizard&step=6");
	}
}
?>
