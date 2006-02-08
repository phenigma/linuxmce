<?PHP
  	//debuging
  	$GLOBALS['inDebug']=1;
  	
  	// todo: set lang when translation is finished
  	$GLOBALS['lang']='en';
  	
  	if($GLOBALS['inDebug']!=1){
  		// production settings
  		error_reporting(E_ALL ^ E_NOTICE);
  		$GLOBALS['globalConfigPath']='/var/www/globalconfig/';
  		$GLOBALS['floorplansPath']='/var/www/pluto-admin/floorplans';
  		$GLOBALS['usersPicsPath']='/var/www/pluto-admin/users/';
  		$GLOBALS['roomsPicsPath']='/var/www/pluto-admin/rooms/';
  		$GLOBALS['scenariosPicsPath']='/var/www/pluto-admin/scenarios/';
  		$GLOBALS['pluto.conf']='/etc/pluto.conf';
  		$GLOBALS['wap.conf']='/etc/wap.conf';
  		$GLOBALS['ErrorLog']='/var/log/pluto/pluto-admin.log';
  	}else{
  		error_reporting(E_ALL);
  		$GLOBALS['globalConfigPath']='/home/users/vali/work/web/globalconfig/';
  		$GLOBALS['floorplansPath']='/home/users/vali/work/web/pluto-admin/floorplans/';
  		$GLOBALS['usersPicsPath']='/home/users/vali/work/web/pluto-admin/users/';
  		$GLOBALS['roomsPicsPath']='/home/users/vali/work/web/pluto-admin/rooms/';
  		$GLOBALS['scenariosPicsPath']='/home/users/vali/work/web/pluto-admin/scenarios/';
  		$GLOBALS['pluto.conf']='/home/users/vali/work/web/pluto.conf';
  		$GLOBALS['wap.conf']='/home/users/vali/work/web/wap.conf';
  		$GLOBALS['ErrorLog']='/var/log/pluto/pluto-admin.log';
  	}

  	include_once($GLOBALS['globalConfigPath'].'globalconfig.inc.php');
  	
	$GLOBALS['sendErrorsTo']='support@plutohome.com';
	
	$directory="/pluto-admin/";
	$domain="http://localhost";
	
	// path where the media pics and orbiter backgrounds are uploaded
	$GLOBALS['mediaPicsPath']=$_SERVER['DOCUMENT_ROOT'].'/pluto-admin/mediapics/';
	$GLOBALS['orbiterBackgroundPath']=$_SERVER['DOCUMENT_ROOT'].'/pluto-admin/orbiter_bg/';
	
	
	define("VERSION","0.1");
	define("APPLICATION_NAME","Pluto");
	
	define("APPROOT",$_SERVER['DOCUMENT_ROOT']."".$directory."/");
	define("APPURL",$domain.$directory);
	define("APPDOMAIN",$domain);
	
	define("APPDIRECTORY",$directory);
	define("UPLOADDIR",'upload');
	
	
	
	define("LANG","ro-RO");
	
	
	setlocale(LC_ALL, LANG);
	
	//database
	require_once("database.inc.php");
	
	// hard coded values
	require_once("hardCoded.php");
	
?>
