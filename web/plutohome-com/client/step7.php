<?
$validOrbiters = getValidOrbitersArray($installationID,$dbADO);
$queryOrbiters='SELECT * FROM DeviceTemplate WHERE PK_DeviceTemplate IN ('.join(',',$validOrbiters).') AND FK_DeviceCategory!=? AND PK_DeviceTemplate!=?';
$resOrbiters=$dbADO->Execute($queryOrbiters,array($GLOBALS['MobileOrbiterCategory'],$GLOBALS['onScreenOrbiter']));
$orbitersArray=array();
while($rowOrbiters=$resOrbiters->FetchRow()){
	$orbitersArray[$rowOrbiters['PK_DeviceTemplate']]=$rowOrbiters['Description'];
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
		INNER JOIN OperatingSystem ON FK_OperatingSystem=PK_OperatingSystem
		WHERE Distro.Orbiter=1 '.@$filterRestrictions;
$resDistroOS=$dbADO->Execute($queryDistroOS);
$distroIdArray=array();
$distroDescriptionArray=array();
while($row=$resDistroOS->FetchRow()){
	$distroDescriptionArray[]=$row['Description']." / ".$row['OS'];
	$distroIdArray[]=$row['PK_Distro'];
}


$displayedDevicesArray=explode(',',@$_POST['displayedDevices']);


if($action=='form'){
	// display form
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
				function addOrbiter()
				{
					if(document.wizard.Description.value==""){
						alert("Please insert the description for the new orbiter.");
						document.wizard.Description.focus();
					}else if(document.wizard.newType.selectedIndex==0){
						alert("Please select the orbiter type.")
						document.wizard.newType.select();
					}else{
						document.wizard.action.value="add";
						document.wizard.submit();
					}
				}
				</script>';
	$out.='
					<br>
				   <form action="index.php" method="POST" name="wizard">
					<input type="hidden" name="section" value="wizard">
					<input type="hidden" name="step" value="'.$step.'">
					<input type="hidden" name="action" value="update">	
			      		<table align="center" width="100%">
							 <tr>
								<td align="left" class="normaltext"><img src="images/install_wizard.gif"></td>
								<td align="right" class="normaltext"><a href="index.php?section=wizard&step=6">&lt;&lt; Previous</a> <a href="#" onClick="javascript:document.wizard.action.value=\'update\';document.wizard.submit();">Next &gt;&gt;</a></td>
							</tr>				
							<tr>
								<td colspan="2" align="center" bgcolor="#DADDE4"><B>Step 7 of 8: Orbiter</B></td>
							</tr>
							<tr>
								<td colspan="2" align="left" class="insidetable">Here are your orbiters.  The Media Directors will have an Orbiter on the screen already to control the system.  Also any compatible Bluetooth mobile phones will be detected automatically--you don\'t need to add them here.  But you can add additional Orbiters for other desktop PC\'s, Web Pads, PDAs, etc.  For the type just choose "Orbiter" unless you see another type that more closely describes your hardware.</td>
							</tr>
							<tr>
								<td colspan="2">
								<table align="center">
									<tr class="normaltext" bgcolor="#DADDE4">
										<td><B>Name</B></td>
										<td><B>Type</B></td>
										<td>&nbsp;</td>
									</tr>';
	$queryOrbiters='
						SELECT * FROM Device WHERE FK_DeviceTemplate IN ('.join(',',$validOrbiters).') AND FK_Installation=?  AND FK_Device_ControlledVia IS NULL';
	$resOrbiters=$dbADO->Execute($queryOrbiters,$installationID);
	$displayedDevices=array();
	if($resOrbiters->RecordCount()==0){
		$out.='		<tr class="normaltext">
										<td colspan="6" align="center">No Orbiters</td>
									</tr>';
	}
	while($rowOrbiters=$resOrbiters->FetchRow()){
		$displayedDevices[]=$rowOrbiters['PK_Device'];

		$out.='
									<tr>
										<td><input type="text" name="description_'.$rowOrbiters['PK_Device'].'" value="'.$rowOrbiters['Description'].'"></td>
										<td class="normaltext"><B>'.$orbitersArray[$rowOrbiters['FK_DeviceTemplate']].'</B></td>
										<td><input type="submit" name="delete_'.$rowOrbiters['PK_Device'].'" value="Delete"></td>
									</tr>';
	}
	$out.='<input type="hidden" name="displayedDevices" value="'.join(',',$displayedDevices).'">';
	$out.='		</table>
								</td>
							</tr>
							<tr>
								<td colspan="2" class="normaltext"><B>Add an Orbiter</B></td>
							</tr>
							<tr class="normaltext">
								<td>Name</td>
								<td><input type="text" name="Description" value="'.@$_POST['Description'].'"> "Susan\'s PDA", "Silver webpad", etc.</td>
							</tr>
							<tr class="normaltext">
								<td>Type</td>
								<td><select name="newType">
									<option value="0">-Please select-</option>';
	foreach ($orbitersArray as $key => $value){
		$out.='<option value="'.$key.'" '.(($key==@$_POST['newType'])?'selected':'').'>'.$value.'</option>';
	}
	$out.='	</select> "Orbiter" for web pad\'s and PDA\'s</td>
							</tr>';
	$out.='		<tr>
					<td colspan="2" align="center"><input type="button" name="continue" value="Add&Continue" onClick="javascript:addOrbiter();"> <input type="button" name="cancel" value="Cancel" onClick="self.location=\'index.php?section=wizard&step=7\'"></td>
				</tr>
							<tr>
								<td colspan="2" align="center" class="normaltext">If you don’t need another orbiter, just click next.<br><input type="button" name="next" value="Next" onClick="javascript:document.wizard.action.value=\'update\';document.wizard.submit();"></td>
							</tr>				
			      		</table>
					</form>';

}elseif($action=='add'){
	// process form step 6
	$description=stripslashes($_POST['Description']);
	$FK_DeviceTemplate=$_POST['newType'];

	$insertDevice='INSERT INTO Device (Description, FK_DeviceTemplate, FK_Installation) VALUES (?,?,?)';
	$dbADO->Execute($insertDevice,array($description,$FK_DeviceTemplate,$_SESSION['installationID']));
	$insertID=$dbADO->Insert_ID();
	InheritDeviceData($FK_DeviceTemplate,$insertID,$dbADO);
	InheritCategoryDeviceData($FK_DeviceTemplate,$insertID,$dbADO);
	createChildsForControledViaDeviceTemplate($FK_DeviceTemplate,$_SESSION['installationID'],$insertID,$dbADO,NULL,NULL);
	createChildsForControledViaDeviceCategory($FK_DeviceTemplate,$_SESSION['installationID'],$insertID,$dbADO,NULL,NULL);

	header("Location: index.php?section=wizard&step=7");

}else {
	foreach($displayedDevicesArray as $value){
		$description=stripslashes(@$_POST['description_'.$value]);
		$FK_DeviceTemplate=@$_POST['deviceTemplate_'.$value];
		$FK_Distro=@$_POST['platform_'.$value];
		
		$updateDevice='UPDATE Device SET Description=? WHERE PK_Device=?';
		$dbADO->Execute($updateDevice,array($description, $value));

		$updateDeviceDeviceData='UPDATE Device_DeviceData SET IK_DeviceData=? WHERE FK_Device=? AND FK_DeviceData=?';
		$dbADO->Execute($updateDeviceDeviceData,array($FK_Distro,$value,$GLOBALS['rootPK_Distro']));
	}

	$deleted=0;
	foreach($displayedDevicesArray as $value){
		if(isset($_POST['delete_'.$value])){
			$deleted++;
			deleteDevice($value,$dbADO);
		}
	}

	header("Location: index.php?section=wizard&step=".(($deleted>0)?7:8));
	
}
?>
