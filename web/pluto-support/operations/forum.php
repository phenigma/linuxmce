<?
session_name('Pluto');
session_start();
require('../include/config/config.inc.php');

	$oldParams=$_SERVER['QUERY_STRING'];
	$newParams=(strpos($oldParams,'&')!==false)?substr($oldParams,strpos($oldParams,'&')+1):$oldParams;

	if(!isset($_SESSION['userID'])){
		header("Location: ".$forumHost);
		exit();
	}else{
		header("Location: ../phpbb2/login.php?username=".$_SESSION['username']."&password=".$_SESSION['extPassword']."&login=1&".$newParams);
	}
$out='

';
		
?>