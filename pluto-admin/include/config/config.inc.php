<?PHP
   
      
   error_reporting(E_ALL);
  //debuging
  //$GLOBALS['inDebug']=1;
  $GLOBALS['sendErrorsTo']='dragos.grosu@gmail.com';

	$directory="/customer_setup6/";
	$domain="http://localhost";

	$MasterUsersHost="https://10.0.0.150/masterusers/";
  	
  	$addMasterUserUrl=$MasterUsersHost."index.php?section=add_master_user";
  	$checkMasterUserUrl=$MasterUsersHost."index.php?section=check_master_user";
	$changePassMasterUserUrl=$MasterUsersHost."index.php?section=change_password";	
	$updateMasterUserUrl=$MasterUsersHost."index.php?section=update_master_users";

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
      
	
  $GLOBALS['rootOrbiterID'] = 5; //the PK_DeviceCategory from DeviceCategory table for the root orbiter
  $GLOBALS['rootComputerID'] = 6; //the PK_DeviceCategory from DeviceCategory table for the root Computers
  
  $GLOBALS['rootLights'] = 71; //the PK_DeviceCategory from DeviceCategory table for the root Lightning devices
  $GLOBALS['ArrayIDForLight'] = 1;
  
  $GLOBALS['rootClimate'] = 76; //the PK_DeviceCategory from DeviceCategory table for the root Climate devices
  $GLOBALS['ArrayIDForClimate'] = 2;
  
  $GLOBALS['rootSecurity'] = 77; //the PK_DeviceCategory from DeviceCategory table for the root Security devices
  $GLOBALS['ArrayIDForSecurity'] = 3;
  $GLOBALS['ArrayIDEntertainment'] = 5;
  
  $GLOBALS['rootLightsInterfaces'] = 75;
  $GLOBALS['rootClimateInterfaces'] = 78;
  $GLOBALS['rootSecurityInterfaces'] = 79;
  
  $GLOBALS['DeviceDataLinkedToTables']=array('PK_Distro','PK_FloorplanObjectType');
  $GLOBALS['deviceTemplateOrbiter']=8;	// the PK_DeviceTemplate for 'Orbiter'
?>