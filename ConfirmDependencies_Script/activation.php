<?
/* Based on the "code" GET parameter, return a message and a one-per-row list of packages following it
 * if code is ok: return "OK\npackage1\npacket2\n..."
 * else: return "ERROR. Invalid code.\n"
 */

require_once("database.inc.php");
header("Content-type: text/plain");

$signup_url = "http://www.plutohome.com/signup.html";

$code = $_GET["code"];

// sanity checks
if (! isset($code) || $code === "" || preg_match("/^\d+-\d+$/", $code) != 1 || ! CheckValidCode($code))
{
	$message = "ERROR. Invalid activation code. If you don't have an activation code, get yours by signing up at $signup_url. Thank you";
}
else // return
{
	$message = "OK\n" . implode("\n", (array)GetActivationSh($code));
}

echo "$message";

// return true if code is valid, false if not
function CheckValidCode($code)
{
	list($device, $pin) = explode("-", $code);
	$sql = "SELECT ActivationCode"
		. " FROM Device"
		. " JOIN Installation ON Device.FK_Installation = Installation.PK_Installation"
		. " WHERE Device.PK_Device = '$device' LIMIT 1";
	$res = @mysql_query($sql);
	if (mysql_num_rows($res) == 0)
		return false;
	$row = mysql_fetch_row($res);
	if ($row[0] !== $pin)
		return false;
	return true;
}

// return an array of packages
function GetActivationSh($code)
{
	list($device, $pin) = explode("-", $code);
	exec("/usr/pluto/bin/ConfirmDependencies -d $device install", $result) || die("ERROR. Can't generate answer: $device:$pin");

	return $result;
}
?>

