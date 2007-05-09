<?
$conn=db_connect($GLOBALS['db_server'], $GLOBALS['db_port'], $GLOBALS['db_user'], $GLOBALS['db_pass'], $GLOBALS['db_database']);

if(isset($_REQUEST['admin']) && @$_REQUEST['admin']==@$GLOBALS['access_string']){
	$_SESSION['admin_links']=1;
}

if(isset($_REQUEST['admin']) && @$_REQUEST['no']==1){
	unset($_SESSION['admin_links']);
}

?>