<?
require('include/config/config.inc.php');
require('include/utils.inc.php');

$code=$_GET['code'];
$distroInstaller=$_GET['distro'];
$name=cleanString($_GET['name']);

$out='<B>Auto install script</B><br>
The auto-install script comes in 2 parts.<br><br><br>

First, there are some scripts that are specific for your operating system and distribution platform.
These scripts are what know how to download and install software for your platform from http, ftp, packages, svn, cvs, etc.
Download the following archive and extract the files into /usr/pluto/install under Linux, or \pluto\install under Windows.<br>
<a href="'.$domain.$downloadDirectory.'installers/'.$distroInstaller.'">'.$domain.$downloadDirectory.'installers/'.$distroInstaller.'</a>
<br><br><br>
Second, this wizard creates a custom script for every computer within your installation.  We call this the "Wrapper".
The Wrapper has lists about all the software that you are going to need to run Pluto on the computer, and the
wrapper will call the install scripts above.  Since this wrapper is custom for <b>'.$name.'</b> be sure you
only run the wrapper on that computer.  Download the custom wrapper for each computer.  Save the Wrapper
into the same directory as the install scripts, and then run the Wrapper.  Everything will install 
automatically.<br>
<a href="download.php?code='.$code.'" target="_blank">Custom Wrapper for '.$name.'</a>.
';


// facelift is required
echo $out;

?>