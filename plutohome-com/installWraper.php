<?
require('include/config/config.inc.php');
require('include/utils.inc.php');

$code=$_GET['code'];
$signup_url = "http://www.plutohome.com/signup.html";


$out='If there is no installer for your platform, you can download just the Custom Wrapper for this device. This wrapper will have a list of every piece of software that needs to be installed on this computer. You can use it for a reference if you want to install the software by hand. However, if you know scripting, it’s probably easier to just make a script for your platform. You can use one of the install scripts we already have as a template. The install script is really quite simple.';


// sanity checks
if (! isset($code) || $code === "" || preg_match("/^\d+-\d+$/", $code) != 1 || ! CheckValidCode($code,$dbADO))
{
	$message = "ERROR. Invalid activation code. If you don't have an activation code, get yours by signing up at $signup_url. Thank you";
}
else // return
{
	$message = "OK\n" . implode("\n", (array)GetActivationSh($code));
}

$out.='<br><br>'.$message;


echo $out;


?>