<?
/* Based on the "code" GET parameter, return a message and a one-per-row list of packages following it
 * if code is ok: return "OK\npackage1\npacket2\n..."
 * else: return "ERROR. Invalid code.\n"
 */

require('include/config/config.inc.php');
require('include/utils.inc.php');
header("Content-type: text/plain");

$code = $_GET["code"];
$device = $_GET["device"];

$query='
SELECT PK_Installation,FK_Users 
FROM Device JOIN Installation ON PK_Installation = Device.FK_Installation
JOIN Installation_Users ON Installation_Users.FK_Installation=PK_Installation
WHERE PK_Device=? AND ActivationCode=?';
//$dbADO->debug=true;
$res=$dbADO->Execute($query,array($device,$code));
$installation = "";
while($row=$res->FetchRow()){
	$installation=$row['PK_Installation'];
	if($user!=''){
		$user=$user.',';
	}
	$user=$user.$row['FK_Users'];
}

if ($installation === "")
{
	$header = "-- ERROR. Invalid Device or Activation Code. If you see this, something changed since you started the installation";
	$message = "";
}
else
{
	$header = "-- OK. SQL data follows";
	$message = implode("\n", (array)GetInitialData($installation,$user)) . "\n-- EOF";
}

echo "$header\n$message";
?>

