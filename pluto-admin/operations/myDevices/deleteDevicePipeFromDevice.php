<?php
function deleteDevicePipeFromDevice($output,$dbADO) {
	//$dbADO->debug=true;
	$out='';
	
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';		
	$deviceFromID = (int)$_REQUEST['deviceFromID'];
	$deviceToID = (int)$_REQUEST['deviceToID'];
	
	$installationID = cleanInteger($_SESSION['installationID']);
	
	//check if current user canModifyInstallation
	$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$installationID,$dbADO);
	
	if ($canModifyInstallation) {
		
			if ((int)$deviceFromID!=0 && $deviceToID!=0) {
				
				$deleteObjFromDevice = 'delete from Device_Device_Pipe where FK_Device_From = ? and FK_Device_To = ? ';
				$query = $dbADO->Execute($deleteObjFromDevice,array($deviceFromID,$deviceToID));
				
				$out.="
				<script>
					alert('Device pipe removed!');
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