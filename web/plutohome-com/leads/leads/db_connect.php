<?
  	$GLOBALS['inDebug']=0;
  	
  	if($GLOBALS['inDebug']!=1){
  		// production settings
  		error_reporting(E_ALL ^ E_NOTICE);
  		include_once('/var/www/globalconfig/globalconfig.inc.php');
  	}else{
  		error_reporting(E_ALL);
  		include_once('/home/users/vali/work/web/globalconfig/globalconfig.inc.php');
  	}
  	
require_once('g/fn_globals.php');
require_once('g/db_mysql.php');

if (running_on_windows()) // testing
{
	$db_my =& new Mysql_Database('localhost','1control','pluto','hayley'); // Local version of Hayley
}
else // Production
{
	$db_my =& new Mysql_Database($dbServer,$dbUser,$dbPass,'leads'); //hayley
}
