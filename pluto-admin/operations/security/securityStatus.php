<?
function securityStatus($output,$dbADO) {
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
//	$dbADO->debug=true;
	$userID = (int)@$_SESSION['userID'];
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$type = isset($_REQUEST['type'])?cleanString($_REQUEST['type']):'lights';
	$installationID = (int)@$_SESSION['installationID'];

	$deviceCategory=$GLOBALS['rootSecurity'];

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
	<div align="center"><h3>Security status</h3></div>
	<table align="center" cellpading="3" cellspacing="3">
			<tr bgcolor="lightblue">
				<td align="center"><B>Device</B></td>
				<td align="center"><B>Device Template</B></td>
				<td align="center"><B>State</B></td>
				<td align="center"><B>Status</B></td>
				<td align="center"><B>FloorplanObjectType</B></td>
			</tr>';
		foreach($roomArray as $roomID=>$roomName){
			$out.='
				<tr>
					<td colspan="6" align="center" bgcolor="#EEEEEE"><B>'.$roomName.'</B></td>
				</tr>';
			$queryDevice='
				SELECT 
					Device.Description, State,Status, DeviceTemplate.Description AS Template, IK_DeviceData, FloorplanObjectType.Description AS FloorplanType
				FROM Device 
					INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
					LEFT JOIN Device_DeviceData ON FK_Device=PK_Device AND FK_DeviceData=?
					INNER JOIN FloorplanObjectType ON PK_FloorplanObjectType=IK_DeviceData
				WHERE
					FK_DeviceTemplate IN ('.join(',',$DTArray).') AND FK_Installation=? AND FK_Room=?';	
			$resDevice=$dbADO->Execute($queryDevice,array($GLOBALS['FloorplanObjectType'],$installationID,$roomID));
			if($resDevice->RecordCount()==0){
				$out.='
						<tr>
							<td colspan="6" align="center">No devices</td>
						</tr>
					';
			}
			while($rowD=$resDevice->FetchRow()){
				$out.='
				<tr>
					<td align="center"><B>'.$rowD['Description'].'</B></td>
					<td align="center">'.$rowD['Template'].'</td>
					<td align="center">'.$rowD['State'].'</td>
					<td align="center">'.$rowD['Status'].'</td>
					<td align="center">'.$rowD['FloorplanType'].'</td>
				</tr>';
			}
		}
		$out.='
				<tr>
					<td colspan="5" align="center" bgcolor="#EEEEEE"><B>Not asigned</B></td>
				</tr>
			';
			$queryDevice='
				SELECT 
					Device.Description, State, Status, DeviceTemplate.Description AS Template, IK_DeviceData, FloorplanObjectType.Description AS FloorplanType
				FROM Device 
					INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
					LEFT JOIN Device_DeviceData ON FK_Device=PK_Device AND FK_DeviceData=?
					INNER JOIN FloorplanObjectType ON PK_FloorplanObjectType=IK_DeviceData
				WHERE
					FK_DeviceTemplate IN ('.join(',',$DTArray).') AND FK_Installation=? AND FK_Room IS NULL';	
			$resDevice=$dbADO->Execute($queryDevice,array($GLOBALS['FloorplanObjectType'],$installationID));
		if($resDevice->RecordCount()==0){
			$out.='
					<tr>
						<td colspan="5" align="center">No devices</td>
					</tr>
				';
		}
	
		while($rowD=$resDevice->FetchRow()){
			$out.='
				<tr>
					<td align="center"><B>'.$rowD['Description'].'</B></td>
					<td align="center">'.$rowD['Template'].'</td>
					<td align="center">'.$rowD['State'].'</td>
					<td align="center">'.$rowD['Status'].'</td>
					<td align="center">'.$rowD['FloorplanType'].'</td>
				</tr>';
		}

	$out.='
	</table>';

	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Security status');
	$output->output();
}
?>