<?
function userHome($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/userHome.lang.php');
	
/* @var $dbADO ADOConnection */
/* @var $rs ADORecordSet */
$out='';
$dbADO->debug=false;
$reloadTree = @$_REQUEST['reloadTree']=='false'?false:true;

$scriptInHead='
<script>
	'.($reloadTree==true?'
	if (parent.frames[\'treeframe\']) {
		top.frames[\'treeframe\'].location=\'index.php?section=leftMenu\';
	}':'').'
</script>
';

$out.=$TEXT_USER_HOME_TEXT_CONST;

$output->setHelpSrc('/support/index.php?section=document&docID=35');
$output->setBody($out);
$output->setScriptInHead($scriptInHead);
$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_USER_HOME_CONST);			
$output->output();  		
}
?>