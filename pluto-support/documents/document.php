<?
function document($output,$dbADO) {
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$dbADO->debug=false;
	$reloadTree = @$_REQUEST['reloadTree']=='false'?false:true;
	
	$out.='
		Pluto Support Index page.
	';
	

	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}
?>