<?
/* Based on the "code" GET parameter, return a message and a one-per-row list of packages following it
 * if code is ok: return "OK\npackage1\npacket2\n..."
 * else: return "ERROR. Invalid code.\n"
 */

$signup_url = "http://www.plutohome.com/signup.html";

$code = $_GET["code"];

// sanity checks
if (! isset($code) || $code === "" || strlen($code) != 6 || ! CheckCode($code))
{
	$message = "ERROR. Invalid activation code. If you don't have an activation code, get yours by signing up at $signup_url. Thank you";
}
else // return
{
	$message = "OK\n" . implode("\n", (array)GetPackageList($code));
}

echo "$message";

// return true if code is valid, false if not
function CheckCode($code)
{
// TODO
	return true;
}

// return an array of packages
function GetPackageList($code)
{
// TODO
	$result[] = "package1.deb";
	$result[] = "package2.deb";

	return $result;
}
?>

