<?
function deviceTemplatePicker($output,$dbADO) {
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$userID= (int)@$_SESSION['userID'];
	$out='';
	$dbADO->debug=false;
	$_SESSION['from']=(isset($_REQUEST['from']))?cleanString(@$_REQUEST['from']):@$_SESSION['from'];
	$_SESSION['categoryID']=(isset($_REQUEST['categoryID']))?cleanInteger(@$_REQUEST['categoryID']):@$_SESSION['categoryID'];
	
	$out.=pickDeviceTemplate($_SESSION['categoryID'],0,0,0,1,1,'deviceTemplatePicker','',$dbADO);
	
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();  		
}
?>