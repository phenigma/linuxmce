<?php
	$out='';
	
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';	
	$deviceID = (int)$_REQUEST['deviceID'];
	$objID = (int)$_REQUEST['objID'];
		
	if ((int)$objID!=0 && (int)$deviceID!=0) {
			$deleteObjFromDevice = 'delete from DeviceTemplate_DesignObj where FK_DeviceTemplate=? and FK_DesignObj=?';
			$query = $publicADO->Execute($deleteObjFromDevice,array($deviceID,$objID));
			$out.="
			<script>
				alert('Object deleted from device!');
				opener.location.reload();
				self.close();
			</script>
			";			
		} else {
			$out = 'Nothing to delete.&nbsp;<a href="javascript:window.close();">Close</a>';
		}		
	
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();
?>