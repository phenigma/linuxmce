<?
require('include/config/config.inc.php');
require('include/utils.inc.php');

$device=$_GET['device'];
$out='Here is all the software required for this device in the general order it will need to be installed<br><br>
	If you see bold lines with *ERROR* it means the indicated software is required for this computer, but we don\'t have the information in our database yet
	indicating where to download the software for the operating system and platform you need, and can only provide a link to some download info.  We do our in-house development with Linux, Debian / Sarge,
	so our database is quite complete and we have sources for all the software Pluto will require.  If you want to add support for your platform,
	just send us an email or live chat and we will give you a link you can use to specify where to get each piece of software for your platform.<br><br>
	All the software we do have in our database for your platform will appear as a clickable link that will take you to the download page.<br><br>
';



exec("/usr/pluto/bin/ConfirmDependencies -h localhost -u $dbUser -p $dbPass -d $device list", $results);

foreach($results AS $line){
	if(strpos($line,'ConfirmDependencies')===false && strpos($line,'*ERROR*')===false){
		if($line[0]=='#')
			$out.=$line.'<br>';
		else{
			$parts=explode('|',$line);
			$out.='<a href="support/index.php?section=packageDownload&pkid='.$parts[0].'" target="_blank">'.$parts[1].'</a><br>';
		}
	}
	else if(strpos($line,'ConfirmDependencies')===false)
	{
		$parts=explode('|',$line);
		$out.='<b>'.$parts[1].'</b> <a href="support/index.php?section=packageDownload&pkid='.$parts[0].'" target="_blank">download info</a><br>';
	}
	else
		$out.='&nbsp;&nbsp;List provided by: '.$line.'<br><br>';
}

echo $out;
?>
