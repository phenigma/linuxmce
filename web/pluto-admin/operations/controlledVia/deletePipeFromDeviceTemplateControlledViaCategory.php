<?php
function deletePipeFromDeviceTemplateControlledViaCategory($output,$dbADO) {
	//$dbADO->debug=true;
	$out='';
	
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';		
	$objID = (int)$_REQUEST['objID'];
		
	if ((int)$objID!=0) {
			$deleteObjFromDevice = 'delete from DeviceTemplate_DeviceCategory_ControlledVia_Pipe  where FK_DeviceTemplate_DeviceCategory_ControlledVia = ?';
			$query = $dbADO->Execute($deleteObjFromDevice,array($objID));
			$out.="
			<script>
				alert('Pipe - controlled via device category - ".($dbADO->Affected_Rows()==1?"":"not")." deleted from master device!');
			    opener.document.forms.{$from}.action.value='form';
				opener.document.forms.{$from}.submit();				
				self.close();
				window.close();
			</script>
			";			
			$out = '<a href="javascript:window.close();">Close</a>';
		} else {
			$out = 'Nothing to delete.&nbsp;<a href="javascript:window.close();">Close</a>';
		}		
	
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();
}
?>