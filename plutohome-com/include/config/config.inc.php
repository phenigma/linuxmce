<?PHP
   
      
 //  error_reporting(E_ALL~E_NOTICE);
  //debuging
  	$GLOBALS['inDebug']=1;
  	$GLOBALS['sendErrorsTo']='info@plutohome.com';

// 	$MasterUsersHost="https://plutohome.com/masterusers/";
  	$MasterUsersHost="http://plutohome.com/masterusers/";
  	$forumHost="http://plutohome.com/support/phpbb2/";
  	$MantisHost="http://plutohome.com/support/mantis/";
  	
	// hard-coded values
	$GLOBALS['rootOrbiterID'] = 5; 			// the PK_DeviceCategory from DeviceCategory table for the root orbiter
	$GLOBALS['rootCoreID'] = 7; 			// the PK_DeviceCategory from DeviceCategory table for the root Core
	$GLOBALS['rootMediaDirectorID'] = 8;	// the PK_DeviceCategory from DeviceCategory table for the root MediaDirector
	
	$GLOBALS['rootGenericMediaDirector']=28;// the PK_DeviceTemplate for Generic Media Director
	$GLOBALS['rootDCERouter'] = 1; 			// the PK_DeviceTemplate from DeviceTemplate table for the root DCE Router
	$GLOBALS['deviceTemplateOrbiter']=8;	// the PK_DeviceTemplate for 'Orbiter' 
	
	$GLOBALS['rootDevelopment']=8;			// the PK_DeviceData for description Development from table DeviceData
	$GLOBALS['rootPK_Distro']=7;			// the PK_DeviceData for PK_Distro from table DeviceData
	$GLOBALS['rootDisklessBoot']=9;			// the PK_DeviceData for Diskless Boot from table DeviceData
	
	$directory="";
	$downloadDirectory=$directory."downloads/";
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
