<?php
function deleteRoomFromInstallation($output,$dbADO) {
	//$dbADO->debug=true;
	$out='';
	
	$from = cleanString($_REQUEST['from']);	
	$roomID = (int)$_REQUEST['roomID'];
	$installationID = cleanInteger($_SESSION['installationID']);
	
	
	if ($installationID!=0 && $roomID!=0) {
			
			$deleteRoomFromInstallation = 'delete from Room where PK_Room = ?';
			$query = $dbADO->Execute($deleteRoomFromInstallation,array($roomID));
			
			$out.="
			<script>
				alert('Room deleted!');
			    	opener.document.forms.{$from}.action.value='form';
					opener.document.forms.{$from}.submit();
				self.close();
			</script>
			";			
		} else {
			$out = 'Nothing to delete(or your are not an owner).&nbsp;<a href="javascript:window.close();">Close</a>';
		}		

	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();
	
}


?>