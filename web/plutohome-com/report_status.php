<?
require_once("/var/www/globalconfig/globalconfig.inc.php");

$esc = "mysql_real_escape_string";

$EK_Installation = $esc($_REQUEST['Installation']);
$EK_Device = $esc($_REQUEST['Device']);
$EK_DeviceTemplate = $esc($_REQUEST['DeviceTemplate']);
$Uptime = $esc($_REQUEST['Uptime']);
$Orbiter = $esc($_REQUEST['Orbiter']);
$Router = $esc($_REQUEST['Router']);

$db = mysql_connect($dbServer, $dbUser, $dbPass, true);
$isSel=mysql_select_db("pluto_tracking",$db);

$q = "INSERT INTO Status(EK_Installation,EK_Device,EK_DeviceTemplate,Uptime,Version_Orbiter,Version_DCERouter,Date)
	VALUES('$EK_Installation','$EK_Device','$EK_DeviceTemplate','$Uptime','$Orbiter','$Router',NOW())";
mysql_query($q) or die('Err: '.mysql_error());
?>
