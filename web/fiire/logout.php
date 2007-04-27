<?
session_name('FIIRE');
session_start();

include('lib/config.php');
include('lib/functions.php');
include('lib/init.php');

do_logout();

header("Location: index.php");
exit();
?>