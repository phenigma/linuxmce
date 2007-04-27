<?
session_name('FIIRE');
session_start();

include('lib/config.php');
include('lib/functions.php');
include('lib/init.php');


$user=cleanString(@$_POST['user']);
$pass=cleanString(@$_POST['pass']);
$remember=(int)@$_POST['remember'];

do_login($user,$pass,$remember,$conn);

if(isset($_SESSION['uID'])){
	header("Location: myaccount.php");
}else{
	header("Location: ".$_SERVER['HTTP_REFERER']);
}
exit();
?>