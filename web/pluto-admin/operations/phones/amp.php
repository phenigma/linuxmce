<?
function amp($output,$dbADO) {
	/* @var $astADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];


	if($action=='form'){
		$out.=setLeftMenu($dbADO).'<iframe src="amp/admin/" style="width:98%;height:85%"></iframe>';
	}
	
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}
?>
