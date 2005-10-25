<pre style="background:black;color:white;">
<?
$logname=$_REQUEST['log'];
print $logname.'<br><br>';
if($logname!=''){
	$cmd='tail -f '.$logname.' | /usr/pluto/bin/ansi2html';
	system($cmd);
}
?>
</pre>