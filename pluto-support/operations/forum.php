<?
session_start('Pluto');
	$oldParams=$_SERVER['QUERY_STRING'];
	$newParams=substr($oldParams,strpos($oldParams,'&')+1);

	if(!isset($_SESSION['userID'])){
		header("Location: ../phpbb2/login.php?".$newParams);
	}else{
		header("Location: ../phpbb2/login.php?username=".$_SESSION['username']."&password=".$_SESSION['extPassword']."&login=1&".$newParams);
	}
$out='

';
		
?>