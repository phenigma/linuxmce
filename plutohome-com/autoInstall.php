<?
require('include/config/config.inc.php');
require('include/utils.inc.php');

$code=$_GET['code'];
$distroInstaller=$_GET['distro'];
$name=cleanString($_GET['name']);

$out='<B>Auto install script</B><br>
The auto-install script comes in 2 parts.<br><br><br>

There is an installer that is specific for your operating system and distribution platform.
This is what knows how to download and install software for that platform.  First, download the installer and save it
to a folder on your hard drive with this link:<br>
<a href="'.$domain.$downloadDirectory.$distroInstaller.'">'.$domain.$downloadDirectory.$distroInstaller.'</a>
<br><br><br>
Next, there is a custom script created specifically for each computer in your installation.  We call this a "Wrapper".  The wrapper contains a list of
all the software that will need to be installed on that computer.  The wrapper will pass this information to the installer you downloaded above.  Note
that the wrapper is different for each computer in your installation.  Only run the wrapper on the computer for which it was intended.
After you downloaded the Installer above, download your wrapper with the following link, save it to the same directory
where you put the installer, and then run the Wrapper on the computer.<br>
<a href="download.php?code='.$code.'" target="_blank">Custom Wrapper for '.$name.'</a>.
';


// facelift is required
echo $out;

?>