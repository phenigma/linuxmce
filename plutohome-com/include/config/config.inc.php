<?PHP
 	
  	//debuging
  	$GLOBALS['inDebug']=0;
  	
  	if($GLOBALS['inDebug']!=1){
  		// production settings
  		error_reporting(E_ALL ^ E_NOTICE);
  		include_once('/var/www/globalconfig/globalconfig.inc.php');
  	}else{
  		error_reporting(E_ALL);
  		include_once('/var/www/test/globalconfig/globalconfig.inc.php');
  	}
  	
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
    
  	//database 
  	require_once("database.inc.php");

  	
  	
  	// DO NOT CHANGE ANYTHING AFTER THIS LINE
  	$addMasterUserUrl=$MasterUsersHost."index.php?section=add_master_user";
  	$checkMasterUserUrl=$MasterUsersHost."index.php?section=check_master_user";
	$changePassMasterUserUrl=$MasterUsersHost."index.php?section=change_password";	
	$updateMasterUserUrl=$MasterUsersHost."index.php?section=update_master_users";
 ?>
