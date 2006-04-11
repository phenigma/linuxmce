<?
function voicemail($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');

	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$dbADO->debug=false;
	$reloadTree = @$_REQUEST['reloadTree']=='false'?false:true;


	$out.=setLeftMenu($dbADO).'<iframe src="amp/recordings/index.php?s=voicemail" style="width:98%;height:600"></iframe>';

	$output->setMenuTitle($TEXT_TELECOM_CONST.' |');
	$output->setPageTitle($TEXT_VOICEMAIL_CONST);
	$output->setNavigationMenu(array($TEXT_VOICEMAIL_CONST=>'index.php?section=voicemail'));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_VOICEMAIL_CONST);
	$output->output();
}
?>