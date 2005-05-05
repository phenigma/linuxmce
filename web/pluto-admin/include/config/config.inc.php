<?PHP
  	//debuging
  	$GLOBALS['inDebug']=1;
  	
  	if($GLOBALS['inDebug']!=1){
  		// production settings
  		error_reporting(E_ALL ^ E_NOTICE);
  		$GLOBALS['globalConfigPath']='/var/www/globalconfig/';
  		$GLOBALS['floorplansPath']='/var/www/pluto-admin/floorplans';
  		$GLOBALS['usersPicsPath']='/var/www/pluto-admin/users/';
  		$GLOBALS['roomsPicsPath']='/var/www/pluto-admin/rooms/';
  		$GLOBALS['scenariosPicsPath']='/var/www/pluto-admin/scenarios/';
  		$GLOBALS['pluto.conf']='/etc/pluto.conf';
  	}else{
  		error_reporting(E_ALL);
  		$GLOBALS['globalConfigPath']='/home/users/vali/work/web/globalconfig/';
  		$GLOBALS['floorplansPath']='/home/users/vali/work/web/pluto-admin/floorplans/';
  		$GLOBALS['usersPicsPath']='/home/users/vali/work/web/pluto-admin/users/';
  		$GLOBALS['roomsPicsPath']='/home/users/vali/work/web/pluto-admin/rooms/';
  		$GLOBALS['scenariosPicsPath']='/home/users/vali/work/web/pluto-admin/scenarios/';
  		$GLOBALS['pluto.conf']='/home/users/vali/work/web/pluto.conf';
  	}

  	include_once($GLOBALS['globalConfigPath'].'globalconfig.inc.php');
  	
	$GLOBALS['sendErrorsTo']='support@plutohome.com';
	
	$directory="/pluto-admin/";
	$domain="http://localhost";
	
	// path where the media pics are uploaded
	$GLOBALS['mediaPicsPath']=$_SERVER['DOCUMENT_ROOT'].'/pluto-admin/mediapics/';
	
	
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
	
	// do not change the following lines
	$addMasterUserUrl=$MasterUsersHost."index.php?section=add_master_user";
	$checkMasterUserUrl=$MasterUsersHost."index.php?section=check_master_user";
	$changePassMasterUserUrl=$MasterUsersHost."index.php?section=change_password";
	$updateMasterUserUrl=$MasterUsersHost."index.php?section=update_master_users";

?>
