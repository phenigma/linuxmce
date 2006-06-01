<?
$scriptID=$_REQUEST['script'];
switch($scriptID){
	case 1:
		$command[]='sudo -u root /usr/pluto/bin/Diskless_Setup.sh';
		$command[]='sudo -u root /usr/pluto/bin/DHCP_config.sh';
		$command[]='sudo -u root /usr/pluto/bin/Diskless_ExportsNFS.sh';
		$title='Please wait for the setup process to complete, then do a "quick reload router", and then you can bootup your new diskless media director.';
		$end='
		<script language="JavaScript">
		<!--
			alert("@MESSAGE@");
		// -->
		</script>';
	break;
	case 2:
		$timestamp=time();
		$command[]=stripslashes(urldecode($_REQUEST['command'])).' 1>/dev/null 2>/tmp/sqlCVS-errors-'.$timestamp.'.log';
		$title='sqlCVS command';
		$showTmpFile=1;
	break;	
	case 3:
		$path=stripslashes($_REQUEST['path']);
		$command[]='sudo -u root /usr/pluto/bin/UpdateMedia -d "'.$path.'"';
		$title='Resynchronize directory '.$path;
	break;	
	case 4:
		$command[]=stripslashes($_REQUEST['cmd']);
		$title='Add software ';
	break;	
	default:
		$command[]='';
		$title='No command specified.';
	break;
}
print $title.'<br>
<pre style="background:black;color:white;">
';
$message = "Finished setting up diskless Media Directors";
for ($i = 0; $i < count($command); $i++)
{
	print $command[$i].'<br><br>';
	if($command[$i]!=''){
		if(!isset($showTmpFile)){
			system("bash -c '$command[$i] > >(tee -a /var/log/pluto/php-executeLog.log|/usr/pluto/bin/ansi2html)'", $retval);
			if ($retval != 0){
				$message = "Failed setting up diskless Media Directors";
				break;
			}
			print '<br><br>';
		}else{
			system("bash -c '$command[$i] > >(tee -a /var/log/pluto/php-executeLog.log|/usr/pluto/bin/ansi2html)'", $retval);
			if($retval==0){
				print 'Status: success';
			}else{
				exec('cat /tmp/sqlCVS-errors-'.$timestamp.'.log | /usr/pluto/bin/ansi2html',$retArray);
				print 'Error log: /tmp/sqlCVS-errors-'.$timestamp.'.log<br>'.join('<br>',$retArray);
			}
		}
	}
}

print(preg_replace("/@MESSAGE@/", $message, $end));
?>
</pre>
