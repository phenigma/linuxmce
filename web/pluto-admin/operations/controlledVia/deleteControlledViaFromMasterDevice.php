<?php
function deleteControlledViaFromMasterDevice($output,$dbADO) {
	//$dbADO->debug=true;
	$out='';
	
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';		
	$objID = (int)$_REQUEST['objID'];
		
	if ((int)$objID!=0) {
			$deleteObjFromDevice = 'delete from DeviceTemplate_DeviceTemplate_ControlledVia where PK_DeviceTemplate_DeviceTemplate_ControlledVia = ?';
			$query = $dbADO->Execute($deleteObjFromDevice,array($objID));
			$out.="
			<script>
				alert('Controlled via device deleted from master device!');
			    opener.document.forms.{$from}.action.value='form';
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