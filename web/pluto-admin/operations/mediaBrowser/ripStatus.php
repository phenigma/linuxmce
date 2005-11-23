<?
function ripStatus($output,$mediadbADO,$dbADO){
	$powerFileID=getPowerFile($dbADO,(int)@$_SESSION['installationID']);
	$cmd='/usr/pluto/bin/MessageSend localhost -targetType device -o 0 '.$powerFileID.' 1 739';
	exec($cmd,$retArray);
	
	// TODO: parse response and display ripping status for each slot
	$out= join('<br>',$retArray);
	
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}
?>