<?
function securitySettings($output,$dbADO) {
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
//	$dbADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
	
	$deviceCategory=$GLOBALS['rootSecurity'];
	$pullDownArray = array('Do Nothing','Security Breach','Fire Alarm','Announcement Only');
	$properties = array('armed','armed_home','entertaining','sleeping','unarmed');
	
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

	if ($action == 'form') {
		$out.='
	<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
	</script>
	<div class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
	<form action="index.php" method="POST" name="securitySettings">
	<input type="hidden" name="section" value="securitySettings">
	<input type="hidden" name="action" value="add">	
	<div align="center"><h3>Security settings</h3></div>
		<table align="center">
			<tr bgcolor="lightblue">
				<td></td>
				<td align="center"><B>Monitor Mode</B></td>
				<td align="center"><B>Armed</B></td>
				<td align="center"><B>Armed Home</B></td>
				<td align="center"><B>Entertaining</B></td>
				<td align="center"><B>Sleeping</B></td>
				<td align="center"><B>Unarmed</B></td>
			</tr>';
				$displayedDevices=array();
				$DeviceDataToDisplay=array();
				$joinArray=$DTIDArray;	// used only for query when there are no DT in selected category
				$joinArray[]=0;
				$queryDevice='
					SELECT 
						Device.*,DeviceTemplate.Description AS DTemplate
					FROM Device 
						INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
					WHERE
						FK_DeviceTemplate IN ('.join(',',$joinArray).') AND FK_Installation=?';	
				$resDevice=$dbADO->Execute($queryDevice,$installationID);
				while($rowD=$resDevice->FetchRow()){
					$displayedDevices[]=$rowD['PK_Device'];
					$queryDDD='SELECT * FROM Device_DeviceData WHERE FK_Device=? AND FK_DeviceData=?';
					$resDDD=$dbADO->Execute($queryDDD,array($rowD['PK_Device'],$GLOBALS['securityAlert']));
					if($resDDD->RecordCount()>0){
						$rowDDD=$resDDD->FetchRow();
						$oldProperties=$rowDDD['IK_DeviceData'];
						$oldPropertiesArray=explode(',',$oldProperties);
					}
					$out.='<input type="hidden" name="oldProperties_'.$rowD['PK_Device'].'" value="'.@$oldProperties.'">';
					$out.='
						<tr>
							<td align="center"><b>'.$rowD['Description'].'</b><br>('.$rowD['DTemplate'].')</td>';
					$out.='<td align="center"><input type="checkbox" name="monitor_mode_'.$rowD['PK_Device'].'" '.((@$oldPropertiesArray[0]==1)?'checked':'').'></td>';
					foreach($properties AS $itemNo=> $itemValue){
						$out.='<td><select name="'.$itemValue.'_'.$rowD['PK_Device'].'">';
						foreach($pullDownArray AS $key=>$value){
							$out.='<option value="'.$key.'" '.((@$oldPropertiesArray[$itemNo+1]==$key)?'selected':'').'>'.$value.'</option>';
						}
						$out.='</select></td>';
					}
					$out.='</tr>';
				}
				$out.='<input type="hidden" name="displayedDevices" value="'.join(',',$displayedDevices).'">';
		$out.='
			<tr>
				<td colspan="7" align="center"><input type="submit" name="update" value="Update"></td>
			</tr>
		</table>
		</form>
	
	</form>
	';
	} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=securitySettings&type=$type&error=You are not authorised to change the installation.");
			exit(0);
		}
		// process and redirect
		
		$displayedDevicesArray=explode(',',$_POST['displayedDevices']);
		foreach($displayedDevicesArray AS $device){
			$oldProperties=$_POST['oldProperties_'.$device];
			$newProperties=(isset($_POST['monitor_mode_'.$device]))?1:0;
			foreach($properties AS $itemNo=> $itemValue){
				$newProperties.=','.$_POST[$itemValue.'_'.$device];
			}
			if($oldProperties==''){
				$insertDDD='INSERT INTO Device_DeviceData (FK_Device, FK_DeviceData, IK_DeviceData) VALUES (?,?,?)';
				$dbADO->Execute($insertDDD,array($device,$GLOBALS['securityAlert'],$newProperties));
			}elseif($newProperties!=$oldProperties){
				$updateDDD='UPDATE Device_DeviceData SET IK_DeviceData=? WHERE FK_DeviceData=? AND FK_Device=?';
				$dbADO->Execute($updateDDD,array($newProperties,$GLOBALS['securityAlert'],$device));
			}
		}		
		
		header("Location: index.php?section=securitySettings");		
	}

	
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}
?>