<?php
function deleteDeviceCategory($output,$dbADO) {
	//$dbADO->debug=true;
	$out='';
	
	$from = cleanString($_REQUEST['from']);	
	$manufacturerID = (int)$_REQUEST['manufacturers'];
	$deviceCategSelected = (int)$_REQUEST['deviceCategSelected'];
	$deviceSelected = (int)$_REQUEST['deviceSelected'];
	$modelSelected = (int)$_REQUEST['model'];
	
	// check if is owner of category

	getDeviceCategoryChildsArray($deviceSelected,$dbADO);

	$GLOBALS['childsDeviceCategoryArray']=cleanArray($GLOBALS['childsDeviceCategoryArray']);
	$GLOBALS['childsDeviceCategoryArray'][]=$deviceSelected;

	if ((int)$deviceSelected!=0) {
		$deleteObjFromDevice = 'delete from DeviceCategory where PK_DeviceCategory in ('.join(",",$GLOBALS['childsDeviceCategoryArray']).')';
		$query = $dbADO->_Execute($deleteObjFromDevice);
		$out.="
				<script>
					alert('Device category deleted!');
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