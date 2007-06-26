<?
session_name('FIIRE');
session_start();

include('lib/config.php');
include('lib/functions.php');
include('lib/init.php');

process_add_comment($conn);

header("Location: view-dealer.php?id=".(int)@$_REQUEST['id']);
exit();
?>
