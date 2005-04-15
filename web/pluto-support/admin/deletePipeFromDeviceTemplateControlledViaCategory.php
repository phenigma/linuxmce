<?php
	//$publicADO->debug=true;
	$out='';
	
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';		
	$objID = (int)$_REQUEST['objID'];
	$pipe= (int)$_REQUEST['pipe'];
	
	if ((int)$objID!=0) {
			$deleteObjFromDevice = 'DELETE FROM DeviceTemplate_DeviceCategory_ControlledVia_Pipe WHERE FK_DeviceTemplate_DeviceCategory_ControlledVia = ? AND FK_Pipe=?';
			$query = $publicADO->Execute($deleteObjFromDevice,array($objID,$pipe));
			$out.="
			<script>
				alert('Pipe - controlled via device category - ".($publicADO->Affected_Rows()==1?"":"not")." deleted from master device!');
				opener.location.reload();				
				self.close();
				window.close();
			</script>
			";			
			$out .= '<a href="javascript:window.close();">Close</a>';
		} else {
			$out = 'Nothing to delete.&nbsp;<a href="javascript:window.close();">Close</a>';
		}		
	
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();
?>