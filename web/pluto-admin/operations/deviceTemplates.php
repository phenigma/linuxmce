<?
function deviceTemplates($output,$dbADO) {
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$userID= (int)@$_SESSION['userID'];
	$out='';
	$dbADO->debug=false;
	unset($_SESSION['parentID']);
	
	$firstColLinks='';

	$out.=pickDeviceTemplate(NULL,1,1,1,0,0,'deviceTemplates',$firstColLinks,$dbADO,1);
	
	$output->setNavigationMenu(array("Device Templates"=>'index.php?section=deviceTemplates'));
	

	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Device Templates');			
	$output->output();  		
}
?>