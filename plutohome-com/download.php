<?
require('include/config/config.inc.php');
require('include/utils.inc.php');

$code=$_GET['code'];

header("Content-type: text/plain");

$signup_url = "http://www.plutohome.com/signup.html";

$code = $_GET["code"];

// sanity checks
if (! isset($code) || $code === "" || preg_match("/^\d+-\d+$/", $code) != 1 || ! CheckValidCode($code,$dbADO))
{
	$message = "ERROR. Invalid activation code. If you don't have an activation code, get yours by signing up at $signup_url. Thank you";
}
else // return
{
	$message = "OK\n" . implode("\n", (array)GetActivationSh($code));
}

header('Content-type: application/force-download');
header('Content-Transfer-Encoding: Binary');
header('Content-disposition: attachment; filename=activation.sh');
echo $message;


?>