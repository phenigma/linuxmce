<?php
function deleteCommandFromCommandGroup_Command($output,$dbADO) {
	//$dbADO->debug=true;
	$out='';
	
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';		
	$objID = (int)$_REQUEST['cgID'];
	
	$commandsDeleted='';
	$parametersDeleted='';
	if ($objID!=0) {
			$deleteObjFromDevice = 'delete from CommandGroup_Command where 
				PK_CommandGroup_Command = ?
			';
			$query = $dbADO->Execute($deleteObjFromDevice,array($objID));
			//delete saved values
			if ($dbADO->Affected_Rows()>0) {
				$commandsDeleted = "Command removed from this command group!";
				$deleteParamValues = 'delete from CommandGroup_Command_CommandParameter where 
					FK_CommandGroup_Command = ?
				';
				$query = $dbADO->Execute($deleteParamValues,array($objID));
				if ($dbADO->Affected_Rows()>0) {
					$parametersDeleted = 'Parameter values also deleted!';
				} else {
					$parametersDeleted = 'No parameter values deleted!';
				}
			
			} else {
				$commandsDeleted = "Command not removed from this command group!";
			}
			
			
			
			
			
			$out.="
			<script>
				alert('$commandsDeleted $parametersDeleted');
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