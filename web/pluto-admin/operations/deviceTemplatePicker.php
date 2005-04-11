<?
function deviceTemplatePicker($output,$dbADO) {
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$userID= (int)@$_SESSION['userID'];
	$out='';
	$dbADO->debug=false;
	//unset($_SESSION['categoryID']);
	$_SESSION['from']=(isset($_REQUEST['from']))?cleanString(@$_REQUEST['from']):@$_SESSION['from'];
	$_SESSION['categoryID']=(isset($_SESSION['categoryID']))?$_SESSION['categoryID']:NULL;
	$_SESSION['categoryID']=(isset($_REQUEST['categoryID']))?cleanInteger($_REQUEST['categoryID']):$_SESSION['categoryID'];

	$allowAddDT=((int)@$_REQUEST['allowAdd']==1)?1:0;
	
	$genericSerialDevices=(isset($_REQUEST['from']) && $_REQUEST['from']=='genericSerialDevices')?1:0;
	
	$out.=pickDeviceTemplate($_SESSION['categoryID'],0,0,$allowAddDT,1,1,'deviceTemplatePicker','',$dbADO,0,$genericSerialDevices);
	
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Pick Device Template');			
	$output->output();  		
}
?>