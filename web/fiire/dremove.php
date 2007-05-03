<?
session_name('FIIRE');
session_start();

include('lib/config.php');
include('lib/functions.php');
include('lib/init.php');

delete_dealer((int)@$_REQUEST['id'],$conn);

header("Location: myaccount.php#dealers");
exit();
?>
