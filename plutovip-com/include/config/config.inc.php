<?PHP
   
      
  	$GLOBALS['inDebug']=1;
  	
  	if($GLOBALS['inDebug']!=1){
  		// production settings
  		error_reporting(E_ALL ^ E_NOTICE);
  		include_once('/var/www/globalconfig/globalconfig.inc.php');
  		$GLOBALS['sendNotificationsTo']='info@plutohome.com';
  	}else{
  		error_reporting(E_ALL);
  		include_once('/var/www/test/globalconfig/globalconfig.inc.php');
  		$GLOBALS['sendNotificationsTo']='valentin.g@newflavorstudio.com';
  	}
  	
  	
  	$GLOBALS['sendErrorsTo']='info@plutohome.com';
	
  	$MasterUsersHost="http://plutohome.com/masterusers/";
  	
	$directory="";
	$domain="http://plutovip.com/";
	$https="http://plutovip.com/";
	
	define("VERSION","0.1");
	define("APPLICATION_NAME","Plutovip.com");
	
	define("APPROOT",$_SERVER['DOCUMENT_ROOT']."".$directory."/"); 
	define("APPURL",$domain.$directory);
	define("APPDOMAIN",$domain);
	
	define("APPDIRECTORY",$directory);
	define("UPLOADDIR",'upload');		

    define("LANG","en-EN");
  
  
    setlocale(LC_ALL, LANG);
    
  	//database 
  	require_once("database.inc.php");

  	// do not change the following lines
  	$addMasterUserUrl=$MasterUsersHost."index.php?section=add_master_user";
  	$checkMasterUserUrl=$MasterUsersHost."index.php?section=check_master_user";
	$changePassMasterUserUrl=$MasterUsersHost."index.php?section=change_password";	
	$updateMasterUserUrl=$MasterUsersHost."index.php?section=update_master_users";

	$addReferrerUrl=$MasterUsersHost."index.php?section=add_referrer";
	$checkReferrerUrl=$MasterUsersHost."index.php?section=referral_summary";

?>