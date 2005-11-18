<?php
function deleteDeviceRelatedFromDeviceParams($output,$dbADO) {
	//$dbADO->debug=true;
	$out='';
	
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';		
	$deviceID = (int)$_REQUEST['deviceID'];
	$relatedDeviceID = (int)$_REQUEST['relatedID'];
	
	$installationID = cleanInteger($_SESSION['installationID']);
	
	//check if current user canModifyInstallation
	$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$installationID,$dbADO);
	
	if ($canModifyInstallation) {
		
			if ((int)$deviceID!=0 && $relatedDeviceID!=0) {
				$deleteObjFromDevice = 'delete from Device_Device_Related where FK_Device = ? and FK_Device_Related = ?';
				$query = $dbADO->Execute($deleteObjFromDevice,array($deviceID,$relatedDeviceID));
				
				$out.="
				<script>
					alert('Device related removed from device configuration!');
				    opener.document.forms.{$from}.action.value='form';
					opener.document.forms.{$from}.submit();
					self.close();
				</script>
				";			
			} else {
				$out = 'Nothing to delete.&nbsp;<a href="javascript:window.close();">Close</a>';
			}		
			
		
	} else {
			$out = 'You are not allowed to do that!<a href="javascript:window.close();">Close</a>';
	}
		
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();
}
?>