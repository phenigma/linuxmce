<?php
	//$publicADO->debug=true;
	$out='';
	
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';		
	$objID = (int)$_REQUEST['objID'];
		
	if ((int)$objID!=0) {
			$deleteObjFromDevice = 'delete from DeviceTemplate_DeviceTemplate_ControlledVia_Pipe  where FK_DeviceTemplate_DeviceTemplate_ControlledVia = ?';
			$query = $publicADO->Execute($deleteObjFromDevice,array($objID));
			$out.="
			<script>
				alert('Pipe - controlled via device template - ".($publicADO->Affected_Rows()==1?"":"not")." deleted from master device!');
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