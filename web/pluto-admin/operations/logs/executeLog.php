<?
$scriptID=$_REQUEST['script'];
switch($scriptID){
	case 1:
		$command[]='sudo -u root /usr/pluto/bin/Diskless_Setup.sh';
		$command[]='sudo -u root /usr/pluto/bin/Update_StartupScrips.sh';
		$title='Please wait for the setup process to complete, then do a ‘quick reload router’, and then you can bootup your new diskless media director.';
	break;
	case 2:
		$command[]=stripslashes(urldecode($_REQUEST['command']));
		$title='sqlCVS update';
	break;	
	default:
		$command[]='';
		$title='No command specified.';
	break;
}
print $title.'<br>
<pre style="background:black;color:white;">
';
for ($i = 0; $i < count($command); $i++)
{
	print $command[$i].'<br><br>';
	if($command[$i]!=''){
		system($command[$i] . '|/usr/pluto/bin/ansi2html');
		print '<br><br>';
	}
}
?>
</pre>
