<?
include("include/config/database.inc.php");
$user="jose";
$sql="select * from MasterUsers where Username='$user'";
$r=mysql_query($sql)or die("sdasdasd".mysql_error());
$row=mysql_fetch_array($r);
$pass=$row['Password'];
//$pass=md5($pass);
echo $pass;
?>