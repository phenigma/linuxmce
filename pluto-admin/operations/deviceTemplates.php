<?
function deviceTemplates($output,$dbADO) {
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$userID= (int)@$_SESSION['userID'];
	$out='';
	$dbADO->debug=false;

	$firstColLinks='
		<br /><br /><br /><br />					
		<a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=manageCategories&action=commands&from=deviceTemplates\',\'width=600,height=800,toolbars=true\');">Manage Commands Categories</a><br />
		<a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=manageCategories&action=events&from=deviceTemplates\',\'width=600,height=800,toolbars=true\');">Manage Events Categories</a><br />';

	$out.=pickDeviceTemplate(NULL,1,1,1,0,0,'deviceTemplates',$firstColLinks,$dbADO,1);
	
	$output->setNavigationMenu(array("Device Templates"=>'index.php?section=deviceTemplates'));
	

	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Device Templates');			
	$output->output();  		
}
?>