<?PHP
   
      
 //  error_reporting(E_ALL~E_NOTICE);
  //debuging
  	$GLOBALS['inDebug']=1;
  	$GLOBALS['sendErrorsTo']='dragos.grosu@gmail.com';
	
  	$MasterUsersHost="https://10.0.0.150/masterusers/";
  	
  	$addMasterUserUrl=$MasterUsersHost."index.php?section=add_master_user";
  	$checkMasterUserUrl=$MasterUsersHost."index.php?section=check_master_user";
	$changePassMasterUserUrl=$MasterUsersHost."index.php?section=change_password";	
	$updateMasterUserUrl=$MasterUsersHost."index.php?section=update_master_users";
	
	$addReferrerUrl="https://10.0.0.150/masterusers/index.php?section=add_referrer";
	$checkReferrerUrl="https://10.0.0.150/masterusers/index.php?section=referral_summary";
	
	$directory="/pluto_vip/";
	$domain="http://localhost";
	$https="https://10.0.0.150/pluto_vip/";
	
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
      
	
?>