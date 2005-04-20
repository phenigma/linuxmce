<?
require_once("/var/www/globalconfig/globalconfig.inc.php");

$esc = "mysql_real_escape_string";

$db = mysql_connect($dbServer, $dbUser, $dbPass, true);
$isSel=mysql_select_db("pluto_tracking",$db);

$EK_Installation = $esc($_REQUEST['Installation']);
$EK_Device = $esc($_REQUEST['Device']);
$EK_DeviceTemplate = $esc($_REQUEST['DeviceTemplate']);
$Uptime = $esc($_REQUEST['Uptime']);
$Orbiter = $esc($_REQUEST['Orbiter']);
$Router = $esc($_REQUEST['Router']);
$IP = $esc($_SERVER['REMOTE_ADDR']);

$q = "INSERT INTO Status(Date,EK_Installation,EK_Device,EK_DeviceTemplate,Uptime,Version_Orbiter,Version_DCERouter,IPaddress)
	VALUES(NOW(),'$EK_Installation','$EK_Device','$EK_DeviceTemplate','$Uptime','$Orbiter','$Router','$IP')";
mysql_query($q) or die('Err: '.mysql_error());
?>
