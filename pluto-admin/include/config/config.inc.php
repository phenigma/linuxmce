<?PHP
	include_once($_SERVER['DOCUMENT_ROOT'].'/PlutoAdminConfig/config.inc.php');   
      
   error_reporting(E_ALL);
  //debuging
  //$GLOBALS['inDebug']=1;
  $GLOBALS['sendErrorsTo']='support@plutohome.com';

	$directory="/pluto-admin/";
	$domain="http://localhost";

//	$MasterUsersHost="https://plutohome.com/masterusers/";
	$MasterUsersHost="http://plutohome.com/masterusers/";
  	
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
  
  $GLOBALS['rootDCERouter'] = 1; //the PK_DeviceTemplate from DeviceTemplate table for the root DCE Router
  $GLOBALS['rootCoreID'] = 7; //the PK_DeviceTemplate from DeviceTemplate table for the root Core
  $GLOBALS['rootMediaDirectorsID'] = 28; //the PK_DeviceTemplate from DeviceTemplate table for the root Media Directors
  $GLOBALS['rootMediaPlugin']=2;		// the PK_DeviceTemplate for device template "Media Plug-in"
  
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
  
  $GLOBALS['MediaScenariosCommand']=43;			// the PK_Command for the command "MH Play Media" used in Media Scenarios
  $GLOBALS['commandParamPK_Device']=2;			// PK_CommandParameter from CommandParameter table for parameter "PK_Device"
  $GLOBALS['commandParamPK_DesignObj']=3;		// PK_CommandParameter from CommandParameter table for parameter "PK_DesignObj"
  $GLOBALS['commandParamFilename']=13;			// PK_CommandParameter from CommandParameter table for parameter "Filename"
  $GLOBALS['commandParamPK_MediaType']=29;		// PK_CommandParameter from CommandParameter table for parameter "PK_MediaType"
  $GLOBALS['commandParamPK_DeviceTemplate']=44;	// PK_CommandParameter from CommandParameter table for parameter "PK_DeviceTemplate"
  $GLOBALS['commandParamPK_EntertainArea']=45;	// PK_CommandParameter from CommandParameter table for parameter "PK_EntertainArea"
  
  $GLOBALS['commandSetVar']=27;				// FK_Command for "Set var to path"
  $GLOBALS['commandGotoScreen']=5;			// FK_Command for "Go to screen"
  
  $GLOBALS['mediaOptionsArray']=array('TV', 'playlists', 'music', 'movies', 'videos', 'pictures', 'documents');
  
  
  $GLOBALS['DeviceDataLinkedToTables']=array('PK_Distro','PK_FloorplanObjectType','PK_Skin','PK_Size','PK_Language','PK_Users');
  $GLOBALS['deviceTemplateOrbiter']=8;	// the PK_DeviceTemplate for 'Orbiter'

  
  // do not change the following lines 
  $addMasterUserUrl=$MasterUsersHost."index.php?section=add_master_user";
  $checkMasterUserUrl=$MasterUsersHost."index.php?section=check_master_user";
  $changePassMasterUserUrl=$MasterUsersHost."index.php?section=change_password";
  $updateMasterUserUrl=$MasterUsersHost."index.php?section=update_master_users";

?>