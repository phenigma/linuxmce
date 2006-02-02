<?php
require('../include/config/config.inc.php');
require('../include/utils.inc.php');

$path='/home/users/vali/work/web/pluto-admin/languages/en/';
$cmd="grep -H '^\\$' $path*.php";

$dbADO = &ADONewConnection('mysql');
$dbADO->NConnect($dbPlutoMainServer,urlencode($dbPlutoMainUser),urlencode($dbPlutoMainPass),'translation'); 
  
exec($cmd,$retArr);
$len=strlen($path);
foreach ($retArr AS $line){
	$pattern='/'.str_replace('/','\/',$path).'(.*).lang.php:\$TEXT_(.*)_CONST=\'(.*)\';/';
	preg_match($pattern,$line,$matches);
	if(isset($matches) && count($matches)==4){
		$filename=$matches[1];
		$variable=$matches[2];
		$value=$matches[3];
		$dbADO->Execute('INSERT IGNORE INTO Translation (Variable,Filename,Value) VALUES (?,?,?)',array($variable,$filename,$value));
	}else{
		echo $line.'<br>';
		print_array($matches);
	}	
	//echo $pattern.'<br>';
	//print_array($matches);
	
}
?>