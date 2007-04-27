<?
$conn=db_connect($GLOBALS['db_server'], $GLOBALS['db_port'], $GLOBALS['db_user'], $GLOBALS['db_pass'], $GLOBALS['db_database']);

do_cookie_autologin($conn);
?>