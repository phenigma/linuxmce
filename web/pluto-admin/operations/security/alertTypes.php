<?
function alertTypes($output,$securitydbADO,$dbADO) {
	/* @var $securitydbADO ADOConnection */
	/* @var $res ADORecordSet */
	
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';

	if (@$_SESSION['userLoggedIn']!=true) {
		header("Location: index.php?section=login");
	}
	$installationID = cleanInteger($_SESSION['installationID']);
	$deviceNames=getDeviceNames($dbADO);
	$securityPlugIn=getDeviceFromDT($installationID,$GLOBALS['SecurityPlugin'],$dbADO);
	
	$out.='<div align="center"><h3>Alert types</h3></div>';
	
	if ($action=='form') {
		$out.=setLeftMenu($dbADO).'<div class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>';
		$out.='
			<form action="index.php" method="post" name="alertTypes">
			<input type="hidden" name="section" value="alertTypes">
			<input type="hidden" name="action" value="add">
			<input type="hidden" name="deviceToRemove" value="">
			<table border="0" align="center">
				<tr bgcolor="lightblue">
					<td align="center"><B>Description</B></td>
					<td align="center"><B>Delay Before Alarm</B></td>
					<td align="center"><B>Exit Delay</B></td>
					<td align="center"><B>Alarm Duration</B></td>
					<td align="center"><B>Pool Alerts</B></td>
				</tr>
			';
		$displayedAlertTypes=array();
		$query='SELECT * FROM AlertType ORDER BY Description ASC';
		$res=$securitydbADO->Execute($query);
		while($row=$res->FetchRow()){
			$displayedAlertTypes[]=$row['PK_AlertType'];
			$out.='
				<tr>
					<td align="center"><B>'.$row['Description'].'</B></td>
					<td align="center">'.(($row['PK_AlertType']!=5)?'<input type="text" name="delay_'.$row['PK_AlertType'].'" value="'.$row['DelayBeforeAlarm'].'">':'N/A').'</td>
					<td align="center">'.((in_array($row['PK_AlertType'],array(1,4)))?'<input type="text" name="exitDelay_'.$row['PK_AlertType'].'" value="'.$row['ExitDelay'].'">':'N/A').'</td>
					<td align="center">'.(($row['PK_AlertType']!=5)?'<input type="text" name="duration_'.$row['PK_AlertType'].'" value="'.$row['AlarmDuration'].'">':'N/A').'</td>
					<td align="center"><input type="text" name="PoolAlerts_'.$row['PK_AlertType'].'" value="'.$row['PoolAlerts'].'"></td>
				</tr>
			';
		}
		$devicesList=explode(',',getDeviceData($securityPlugIn,$GLOBALS['PK_Device'],$dbADO));
		if(count($devicesList)==0 || (count($devicesList)==1 && $devicesList[0]=='')){
			$deviceNamesList='No device selected';
			$devicesList=array(0);
		}
		else{
			$tmpArray=array();
			foreach ($devicesList AS $dev){
				if((int)$dev!=0)
					$tmpArray[]='<a href="javascript:document.alertTypes.deviceToRemove.value='.$dev.';document.alertTypes.submit();">'.$deviceNames[$dev].'</a>';
			}
			$deviceNamesList=join('<br>',$tmpArray);
		}
		
		$out.='
				<tr>
					<td align="right"><B>Announcement device(s): *</B></td>
					<td colspan="2">'.$deviceNamesList.'</td>
					<td colspan="2" align="left">'.devicesTree('device','',$dbADO,' AND PK_Device NOT IN ('.join(',',$devicesList).')').'</td>
				</tr>
				<tr>
					<td align="left" colspan="5">* Choose the device from the pulldown to add it to list. Click on device name to remove it.</td>
				</tr>		
				<tr>
					<td align="center" colspan="5"><input type="submit" class="button" name="update" value="Update"></td>
				</tr>		
			</table>
			<input type="hidden" name="oldDevices" value="'.join(',',$devicesList).'">
			<input type="hidden" name="displayedAlertTypes" value="'.join(',',$displayedAlertTypes).'">
		</form>
		';
	} else {
		$displayedAlertTypesArray=explode(',',$_POST['displayedAlertTypes']);
		foreach($displayedAlertTypesArray as $PK_AlertType){
			$delay=(isset($_POST['delay_'.$PK_AlertType]) && $_POST['delay_'.$PK_AlertType]!='')?@$_POST['delay_'.$PK_AlertType]:NULL;
			$exitDelay=(isset($_POST['exitDelay_'.$PK_AlertType]) && $_POST['exitDelay_'.$PK_AlertType]!='')?$_POST['exitDelay_'.$PK_AlertType]:NULL;
			$duration=(isset($_POST['duration_'.$PK_AlertType]) && $_POST['duration_'.$PK_AlertType]!='')?$_POST['duration_'.$PK_AlertType]:NULL;
			$PoolAlerts=(isset($_POST['PoolAlerts_'.$PK_AlertType]) && $_POST['PoolAlerts_'.$PK_AlertType]!='')?$_POST['PoolAlerts_'.$PK_AlertType]:NULL;
			
			$updateAlertType='UPDATE AlertType SET DelayBeforeAlarm=?, ExitDelay=?, AlarmDuration=?,PoolAlerts=? WHERE PK_AlertType=?';
			$securitydbADO->Execute($updateAlertType,array($delay,$exitDelay,$duration,$PoolAlerts,$PK_AlertType));
		}

		if((int)$_POST['device']!=0){
			$oldDD=$_POST['oldDevices'];
			$newDevice=(int)$_POST['device'];
			$newDD=($oldDD!='')?$oldDD.','.$newDevice:$newDevice;
			$dbADO->Execute('UPDATE Device_DeviceData SET IK_DeviceData=? WHERE FK_Device=? AND FK_DeviceData=?',array($newDD,$securityPlugIn,$GLOBALS['PK_Device']));
		}
		if((int)$_POST['deviceToRemove']!=0){
			$oldDevicesArray=explode(',',$_POST['oldDevices']);
			$newDD=implode(',',array_diff($oldDevicesArray,array((int)$_POST['deviceToRemove'])));
			$dbADO->Execute('UPDATE Device_DeviceData SET IK_DeviceData=? WHERE FK_Device=? AND FK_DeviceData=?',array($newDD,$securityPlugIn,$GLOBALS['PK_Device']));
		}
		
		header("Location: index.php?section=alertTypes&msg=Alerts types updated.");
	}
	
		
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();  		
}
?>
