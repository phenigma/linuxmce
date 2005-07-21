<?
function bookmarks($output,$mediadbADO,$dbADO) {
	/* @var $mediadbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	
	if($action=='form'){
		
		
		$out.='
			<div align="center" class="err">'.@$_REQUEST['error'].'</div>
			<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>

		<h3 align="center">Bookmarks</h3>
		<form action="index.php" method="POST" name="bookmarks">
			<input type="hidden" name="section" value="bookmarks">
			<input type="hidden" name="action" value="update">
		
		
		</form>';
	}else{
		
		$suffix=(isset($error))?'&error='.$error:'&msg='.@$msg;
		header('Location: index.php?section=bookmarks'.$suffix);
	}
	
	$output->setNavigationMenu(array("Media providers"=>'index.php?section=bookmarks'));
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}
?>