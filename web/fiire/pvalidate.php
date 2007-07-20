<?
session_name('FIIRE');
session_start();

include('lib/config.php');
include('lib/functions.php');
include('lib/init.php');


polywell_validate(@$_REQUEST['code'],$conn);

header("Location: myaccount.php");
exit();
?>