<?
require('include/config/config.inc.php');
require('include/utils.inc.php');

$code=$_GET['code'];
$signup_url = "http://plutohome.com/signup.html";


$out='Pluto\'s auto-install script comes in 2 parts.  First there are scripts specific to the operating system which handle getting software from http, ftp, packages, svn, cvs, etc.
Then there is a wrapper that this wizard builds custom for each computer.  This wrapper calls the install scripts passing information about what software to get.<br><br>

If there are no install scripts for your platform and you know some scripting, like shell or bash, it might be faster just to write the install script then to install
everything by hand.  The install scripts we created for Debian were very simple, and most of them will work on any Linux Distro.  Getting software from http, ftp, svn and cvs
should be the same on every distro.  The only script that would need to be modified for your distro is the one that gets packages.  The script we wrote for Debian
uses apt-get.  Also the package names may be different for your distro than for debian.  If you want to contribute an install script for your distro, we can
give you a link to a web page that will show all the package names we already have (like \'mysql-dev\'), and let you change it for your distro if it\'s different.
Send us an email or a live chat for more info.<br><br>

Here is a link to the install scripts we wrote for Debian: <a href="http://plutohome.com/download/installers/DebianInstallers.tar.gz">http://plutohome.com/download/installers/DebianInstallers.tar.gz</a><br><br>

And here is the custom wrapper that this wizard generated for your particular computer along with the command line to the ConfirmDependencies program which created it.  This lists all the software you will need:<br><br>';

// sanity checks
if (! isset($code) || $code === "" || preg_match("/^\d+-\d+$/", $code) != 1 || ! CheckValidCode($code,$dbADO))
{
	$message = "ERROR. Invalid activation code. If you don't have an activation code, get yours by signing up at $signup_url. Thank you";
}
else // return
{
	$message = "OK\n" . implode("<br>", (array)GetActivationSh($code));
}

$out.='<br><br>'.$message;


echo $out;


?>