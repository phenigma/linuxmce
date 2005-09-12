<pre style="background:black;color:white;">
<?
$logname=$_REQUEST['log'];
print $logname.'<br><br>';
if($logname!=''){
	system('sudo -u root tail -f '.$logname.' | /usr/pluto/bin/ansi2html');
}
?>
</pre>