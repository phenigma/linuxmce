<?
function callRouting($output,$dbADO) {
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];


	if($action=='form'){
		
		$out.='
		<div align="center" class="err">'.@$_REQUEST['error'].'</div>
		<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
		<h3 align="center">Call routing</h3>
		
		<form action="index.php" method="POST" name="callRouting">
			<input type="hidden" name="section" value="callRouting">
			<input type="hidden" name="action" value="update">

		
		</form>
		
		';
	}else{
	// process area
		
		
		header('Location: index.php?section=callRouting');
	}
	
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}
?>