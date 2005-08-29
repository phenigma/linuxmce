<?PHP
 	
  	//debuging
  	$GLOBALS['inDebug']=0;
  	
  	if($GLOBALS['inDebug']!=1){
  		// production settings
  		error_reporting(E_ALL ^ E_NOTICE);
  		$GLOBALS['globalconfigPath']='/var/www/globalconfig/';
  		$GLOBALS['sendNotificationsTo']='info@plutohome.com';
  	}else{
  		error_reporting(E_ALL);
  		$GLOBALS['globalconfigPath']='/home/users/vali/work/web/globalconfig/';
  		$GLOBALS['sendNotificationsTo']='valentin.g@newflavorstudio.com';
  	}
  	
  	include_once($GLOBALS['globalconfigPath'].'globalconfig.inc.php');
  	
  	$GLOBALS['sendErrorsTo']='info@plutohome.com';

	$directory="";
	$downloadDirectory=$directory."download/";
	$domain="http://plutohome.com/";
	
	
	define("VERSION","0.1");
	define("APPLICATION_NAME","Plutohome.com");
	
	define("APPROOT",$_SERVER['DOCUMENT_ROOT']."".$directory."/"); 
	define("APPURL",$domain.$directory);
	define("APPDOMAIN",$domain);
	
	define("APPDIRECTORY",$directory);
	define("UPLOADDIR",'upload');		

    define("LANG","en-EN");
  
  
    setlocale(LC_ALL, LANG);
    
    // hard coded values
	require_once("hardCoded.php");
	
  	//database 
  	require_once("database.inc.php");

	@include_once('include/logsDB.inc.php');

  	
  	// DO NOT CHANGE ANYTHING AFTER THIS LINE
  	$addMasterUserUrl=$MasterUsersHost."index.php?section=add_master_user";
  	$checkMasterUserUrl=$MasterUsersHost."index.php?section=check_master_user";
	$changePassMasterUserUrl=$MasterUsersHost."index.php?section=change_password";	
	$updateMasterUserUrl=$MasterUsersHost."index.php?section=update_master_users";
 ?>
