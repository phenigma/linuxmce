<?
require('include/config/config.inc.php');
require('include/utils.inc.php');
header("Content-type: text/plain");

$code = $_GET["code"];
$device = $_GET["device"];

$query = '
	SELECT PK_Installation 
	FROM Device JOIN Installation ON PK_Installation = Device.FK_Installation
	WHERE PK_Device=? AND ActivationCode=?';
//$dbADO->debug = true;
$res = $dbADO->Execute($query,array($device, $code));

$installation = "";
if ($row = $res->FetchRow())
	$installation = $row['PK_Installation'];

echo "$installation";
?>
