<?
function deviceStatus($output,$dbADO) {
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
	
	if($action=='form'){
		
		$out.='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		</script>		
			<div align="center" class="err">'.@$_REQUEST['error'].'</div>
			<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
			<h3 align="center">Device status</h3>
			<form action="index.php" method="POST" name="deviceStatus">
			<input type="hidden" name="section" value="deviceStatus">
			<input type="hidden" name="action" value="update">
		<table cellpadding="4" cellspacing="0" border="0" align="center">
			<tr bgcolor="lightblue">
				<td align="center"><B>Device</B></td>
				<td align="center"><B>State</B></td>
				<td align="center"><B>Status</B></td>
				<td align="center"><B>IP Address</B></td>
				<td align="center"><B>MAC Address</B></td>
			</tr>';
		$queryDevices='
			SELECT PK_Device, Device.Description AS DeviceName, State,Status, IPaddress,MACaddress,Room.Description AS RoomName
			FROM Device
			LEFT JOIN Room ON FK_Room=PK_Room
			WHERE Device.FK_Installation=?
			ORDER BY RoomName ASC, DeviceName ASC';
		$resDevices=$dbADO->Execute($queryDevices,$installationID);
		$initRoom='undefined';
		$pos=0;
		while($rowDevices=$resDevices->FetchRow()){
			$pos++;
			if($rowDevices['RoomName']!=$initRoom){
				$out.='
				<tr bgcolor="#D1D9EA">
					<td colspan="5"><B>'.(($rowDevices['RoomName']=='')?'Unassigned to a room':$rowDevices['RoomName']).'</B></td>
				</tr>';
				$initRoom=$rowDevices['RoomName'];
			}
			$out.='
				<tr bgcolor="'.(($pos%2==0)?'#EBEFF9':'').'">
					<td><a href="index.php?section=editDeviceParams&deviceID='.$rowDevices['PK_Device'].'">'.$rowDevices['DeviceName'].'</a></td>
					<td>'.$rowDevices['State'].'</td>
					<td>'.$rowDevices['Status'].'</td>
					<td>'.$rowDevices['IPaddress'].'</td>
					<td>'.$rowDevices['MACaddress'].'</td>
				</tr>
			';
		}
		$out.='
		</table>
		</form>';
	}else{
	// process area
			
		header('Location: index.php?section=deviceStatus');
	}
	
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}
?>