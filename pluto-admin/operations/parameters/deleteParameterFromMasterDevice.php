<?php
function deleteParameterFromMasterDevice($output,$dbADO) {
	//$dbADO->debug=true;
	$out='';
	
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';		
	$objID = (int)$_REQUEST['deviceData'];
	$deviceID = (int)$_REQUEST['deviceID'];
		
	if ((int)$objID!=0 && $deviceID!=0) {
			$deleteObjFromDevice = 'delete from DeviceTemplate_DeviceData where FK_DeviceTemplate = ? and FK_DeviceData = ?';
			$query = $dbADO->Execute($deleteObjFromDevice,array($deviceID,$objID));
			
			$resDevices=$dbADO->Execute('SELECT * FROM Device WHERE FK_DeviceTemplate=?',$deviceID);
			while($rowDevice=$resDevices->FetchRow()){
				$dbADO->Execute('DELETE FROM Device_DeviceData WHERE FK_DeviceData=? AND FK_Device=?',array($objID,$rowDevice['PK_Device']));
			}
			$out.="
			<script>
				alert('Parameter deleted from master device!');
			    opener.document.forms.{$from}.action.value='form';
				opener.document.forms.{$from}.lastAction.value='newDeviceData';
				opener.document.forms.{$from}.submit();
				self.close();
			</script>
			";			
		} else {
			$out = 'Nothing to delete.&nbsp;<a href="javascript:window.close();">Close</a>';
		}			
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();
}
?>