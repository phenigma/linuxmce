<?
function securityStatus($output,$dbADO) {
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
//	$dbADO->debug=true;
	$userID = (int)@$_SESSION['userID'];
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];

	$alarmModesArray=array("UNARMED","ARMED", "AT HOME SECURE", "SLEEPING", "ENTERTAINING", "EXTENDED AWAY");
	$bypassArray=array("NORMAL","WAIT", "BYPASS", "PERMBYPASS");
	$deviceCategory=$GLOBALS['rootSecurity'];

	$displayedDevices=array();
	if($action=='form'){
		getDeviceCategoryChildsArray($deviceCategory,$dbADO);
		$GLOBALS['childsDeviceCategoryArray']=cleanArray($GLOBALS['childsDeviceCategoryArray']);
		$GLOBALS['childsDeviceCategoryArray'][]=$deviceCategory;
		
		$queryDeviceTemplate='
			SELECT * FROM DeviceTemplate 
				WHERE FK_DeviceCategory IN ('.join(',',$GLOBALS['childsDeviceCategoryArray']).')
			ORDER BY Description ASC';
		$resDeviceTemplate=$dbADO->Execute($queryDeviceTemplate);
		$DTArray=array();
		while($rowDeviceCategory=$resDeviceTemplate->FetchRow()){
			$DTArray[$rowDeviceCategory['Description']]=$rowDeviceCategory['PK_DeviceTemplate'];
		}
		if(count($DTArray)==0)
			$DTArray[]=0;
		
		$queryRooms='SELECT * FROM Room WHERE FK_Installation=? ORDER BY Description ASC';
		$resRooms=$dbADO->Execute($queryRooms,$installationID);
		$roomArray=array();
		while($rowRoom=$resRooms->FetchRow()){
			$roomArray[$rowRoom['PK_Room']]=$rowRoom['Description'];
		}
	
		$out.='
	<div class="err" align="center">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
	<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
	<div align="center"><h3>Security status</h3></div>
	<form action="index.php" method="POST" name="securityStatus">
	<input type="hidden" name="section" value="securityStatus">
	<input type="hidden" name="action" value="update">		
	
	<table align="center" cellpading="3" cellspacing="3">
			<tr>
				<td align="right"> <B>Alarm:</B></td>
				<td align="right"><select name="globalAlarmMode">
					<option value="0">- Select Mode -</option>';
	foreach($alarmModesArray AS $alarmMode){
		$out.='<option value="'.$alarmMode.'">'.$alarmMode.'</option>';
	}
	$out.='
				</select></td>
				<td colspan="4"><input type="submit" class="button" name="setAlarm" value="Set mode"></td>
			</tr>
			<tr>
				<td align="center" colspan="7">&nbsp;</td>
			</tr>
			<tr bgcolor="lightblue">
				<td align="center"><B>Device</B></td>
				<td align="center"><B>Device Template</B></td>
				<td align="center"><B>Alarm Mode</B></td>
				<td align="center"><B>Bypass</B></td>
				<td align="center"><B>Delay</B></td>
				<td align="center"><B>Tripped</B></td>
				<td align="center"><B>FloorplanObjectType</B></td>
			</tr>';
		foreach($roomArray as $roomID=>$roomName){
			$out.='
				<tr>
					<td colspan="7" align="center" bgcolor="#EEEEEE"><B>'.$roomName.'</B></td>
				</tr>';
			$queryDevice='
				SELECT 
					Device.Description, State,Status, DeviceTemplate.Description AS Template, IK_DeviceData, FloorplanObjectType.Description AS FloorplanType,PK_Device
				FROM Device 
					INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
					LEFT JOIN Device_DeviceData ON FK_Device=PK_Device AND FK_DeviceData=?
					LEFT JOIN FloorplanObjectType ON PK_FloorplanObjectType=IK_DeviceData
				WHERE
					FK_DeviceTemplate IN ('.join(',',$DTArray).') AND FK_Installation=? AND FK_Room=?';	
			$resDevice=$dbADO->Execute($queryDevice,array($GLOBALS['FloorplanObjectType'],$installationID,$roomID));
			if($resDevice->RecordCount()==0){
				$out.='
						<tr>
							<td colspan="7" align="center">No devices</td>
						</tr>
					';
			}
		
			while($rowD=$resDevice->FetchRow()){
				$displayedDevices[]=$rowD['PK_Device'];
				$out.='
				<tr>
					<td align="center"><B>'.$rowD['Description'].'</B></td>
					<td align="center">'.$rowD['Template'].'</td>
					<td align="center"><select name="alarmMode_'.$rowD['PK_Device'].'">';
					$stateParts=explode(',',$rowD['State']);
					foreach($alarmModesArray AS $alarmMode){
						$out.='<option value="'.$alarmMode.'" '.(($alarmMode==$stateParts[0])?'selected':'').'>'.$alarmMode.'</option>';
					}
				$out.='
				</select></td>
					<td align="center"><select name="bypass_'.$rowD['PK_Device'].'">';
					foreach ($bypassArray AS $bypass){
						$out.='<option value="'.$bypass.'" '.(($bypass==@$stateParts[1])?'selected':'').'>'.$bypass.'</option>';
					}
					$out.='</select></td>
					<td align="center"><input type="checkbox" name="delay" '.((@$stateParts[2]!='')?'checked':'').' disabled></td>
					<td align="center"><input type="checkbox" name="tripped" '.(($rowD['Status']=='TRIPPED')?'checked':'').' disabled></td>
					<td align="center">'.$rowD['FloorplanType'].'</td>
				</tr>
				<input type="hidden" name="oldDelay_'.$rowD['PK_Device'].'" value="'.@$stateParts[2].'">
					';
			}
		}
		$out.='
				<tr>
					<td colspan="7" align="center" bgcolor="#EEEEEE"><B>Not asigned</B></td>
				</tr>
			';
			$queryDevice='
				SELECT 
					Device.Description, State, Status, DeviceTemplate.Description AS Template, IK_DeviceData, FloorplanObjectType.Description AS FloorplanType, PK_Device
				FROM Device 
					INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
					LEFT JOIN Device_DeviceData ON FK_Device=PK_Device AND FK_DeviceData=?
					LEFT JOIN FloorplanObjectType ON PK_FloorplanObjectType=IK_DeviceData
				WHERE
					FK_DeviceTemplate IN ('.join(',',$DTArray).') AND FK_Installation=? AND FK_Room IS NULL';	
			$resDevice=$dbADO->Execute($queryDevice,array($GLOBALS['FloorplanObjectType'],$installationID));
		if($resDevice->RecordCount()==0){
			$out.='
					<tr>
						<td colspan="7" align="center">No devices</td>
					</tr>
				';
		}
	
		while($rowD=$resDevice->FetchRow()){
			$displayedDevices[]=$rowD['PK_Device'];
			$out.='
				<tr>
					<td align="center"><B>'.$rowD['Description'].'</B></td>
					<td align="center">'.$rowD['Template'].'</td>
					<td align="center"><select name="alarmMode_'.$rowD['PK_Device'].'">';
			$stateParts=explode(',',$rowD['State']);
			foreach($alarmModesArray AS $alarmMode){
				$out.='<option value="'.$alarmMode.'" '.(($alarmMode==$stateParts[0])?'selected':'').'>'.$alarmMode.'</option>';
			}
			$out.='
				</select></td>
					<td align="center"><select name="bypass_'.$rowD['PK_Device'].'">';
			foreach ($bypassArray AS $bypass){
				$out.='<option value="'.$bypass.'" '.(($bypass==@$stateParts[1])?'selected':'').'>'.$bypass.'</option>';
			}
			$out.='</select></td>
					<td align="center"><input type="checkbox" name="delay" '.((@$stateParts[2]!='')?'checked':'').' disabled></td>
					<td align="center"><input type="checkbox" name="tripped" '.(($rowD['Status']!='')?'TRIPPED':'').' disabled></td>
					<td align="center">'.$rowD['FloorplanType'].'</td>
				</tr>
				<input type="hidden" name="oldDelay_'.$rowD['PK_Device'].'" value="'.@$stateParts[2].'">
					';
		}

	if(count($displayedDevices)>0){
		$out.='
			<tr>
				<td align="center" colspan="7"><input type="submit" class="button" name="update" value="Update"></td>
			</tr>';
	}

	$out.='
	</table>
		<input type="hidden" name="displayedDevices" value="'.join(',',$displayedDevices).'">
	</form>';
	}else{
		// process area
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=securityStatus&error=You are not authorised to change the installation.");
			exit(0);
		}
		
		$displayedDevicesArray=explode(',',$_POST['displayedDevices']);
		$globalAlarmMode=$_POST['globalAlarmMode'];

		foreach ($displayedDevicesArray AS $deviceID){
			$alarmMode=($globalAlarmMode!='0')?$globalAlarmMode:$_POST['alarmMode_'.$deviceID];
			$bypass=(@$_POST['bypass_'.$deviceID]=='NORMAL')?'':@$_POST['bypass_'.$deviceID];
			$delay=@$_POST['oldDelay_'.$deviceID];
			
			$newState=$alarmMode.(($bypass=='' && $delay=='')?'':','.$bypass.','.$delay);
						
			$dbADO->Execute('UPDATE Device SET State=? WHERE PK_Device=?',array($newState,$deviceID));
			
			header("Location: index.php?section=securityStatus&msg=The devices was updated.");
		}
	}
	
	
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Security status');
	$output->output();
}
?>
