<?php
function deleteParameterFromCommand($output,$dbADO) {
	//$dbADO->debug=true;
	$out='';
	
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';		
	$commandParameterID = (int)$_REQUEST['commandParameterID'];
	$commandID = (int)$_REQUEST['commandID'];
		
	if ((int)$commandParameterID!=0 && $commandID!=0) {
			$deleteObjFromDevice = 'delete from Command_CommandParameter where FK_Command = ? and FK_CommandParameter = ?';
			$query = $dbADO->Execute($deleteObjFromDevice,array($commandID,$commandParameterID));
			$out.="
			<script>
				alert('Parameter deleted from this command!');
			    opener.document.forms.{$from}.action.value='update';
				opener.document.forms.{$from}.lastAction = 'addNewParameterToCommand';
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