<?
$scriptID=$_REQUEST['script'];
switch($scriptID){
	case 1:
		$command='sudo -u root /usr/pluto/bin/Diskless_Setup.sh | /usr/pluto/bin/ansi2html';
		$title='Please wait for the setup process to complete, then do a ‘quick reload router’, and then you can bootup your new diskless media director.';
	break;
	default:
		$command='';
		$title='No command specified.';
	break;
}
print $title.'<br>
<pre style="background:black;color:white;">
';
print $command.'<br><br>';
if($command!=''){
	system($command);
}
?>
</pre>