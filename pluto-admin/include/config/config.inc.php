<?PHP
	include_once($_SERVER['DOCUMENT_ROOT'].'/PlutoAdminConfig/config.inc.php');   
      
  	error_reporting(E_ALL);
    
  	$GLOBALS['sendErrorsTo']='support@plutohome.com';
 	
	$directory="/pluto-admin/";
	$domain="http://localhost";

//	$MasterUsersHost="https://plutohome.com/masterusers/";
	$MasterUsersHost="http://plutohome.com/masterusers/";
  	
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
      
	
  $GLOBALS['rootOrbiterID'] = 5; //the PK_DeviceCategory from DeviceCategory table for the root orbiter
  $GLOBALS['rootComputerID'] = 6; //the PK_DeviceCategory from DeviceCategory table for the root Computers
  
  $GLOBALS['rootDCERouter'] = 1; //the PK_DeviceTemplate from DeviceTemplate table for the root DCE Router
  $GLOBALS['rootCoreID'] = 7; //the PK_DeviceTemplate from DeviceTemplate table for the root Core
  $GLOBALS['rootMediaDirectorsID'] = 28; //the PK_DeviceTemplate from DeviceTemplate table for the root Media Directors
  $GLOBALS['rootMediaPlugin']=2;		// the PK_DeviceTemplate for device template "Media Plug-in"
  
  $GLOBALS['LightingScenariosTemplate']=9;	// PK_Template for Lighting Scenarios
  $GLOBALS['rootLights'] = 73; //the PK_DeviceCategory from DeviceCategory table for the root Lighting devices
  $GLOBALS['ArrayIDForLight'] = 1;
  $GLOBALS['genericONCommand']=192;	// PK_Command for "Generic On", "Generic OFF", "Set Level"
  $GLOBALS['genericOFFCommand']=193;
  $GLOBALS['genericSetLevelCommand']=184;
  $GLOBALS['commandParamAbsoluteLevel']=76;	// PK_CommandType for "Absolute Level"
  
  
  $GLOBALS['ClimateScenariosTemplate']=10;	// PK_Template for Climate Scenarios
  $GLOBALS['rootClimate'] = 83; //the PK_DeviceCategory from DeviceCategory table for the root Climate devices
  $GLOBALS['ArrayIDForClimate'] = 2;
  $GLOBALS['genericHeatCommand']=197;
  $GLOBALS['genericCoolCommand']=196;
    
  $GLOBALS['rootSecurity'] = 84; //the PK_DeviceCategory from DeviceCategory table for the root Security devices
  $GLOBALS['ArrayIDForSecurity'] = 3;
  $GLOBALS['ArrayIDForMedia'] = 5;
  $GLOBALS['securityScenariosTemplate']=12;
  
  $GLOBALS['rootLightsInterfaces'] = 75;
  $GLOBALS['rootClimateInterfaces'] = 78;
  $GLOBALS['rootSecurityInterfaces'] = 80;
  
  $GLOBALS['MediaScenariosTemplate']=1;			// PK_Template for the template "Media Scenarios"
  $GLOBALS['MediaScenariosCommand']=43;			// the PK_Command for the command "MH Play Media" used in Media Scenarios
  $GLOBALS['commandParamPK_Device']=2;			// PK_CommandParameter from CommandParameter table for parameter "PK_Device"
  $GLOBALS['commandParamPK_DesignObj']=3;		// PK_CommandParameter from CommandParameter table for parameter "PK_DesignObj"
  $GLOBALS['commandParamFilename']=13;			// PK_CommandParameter from CommandParameter table for parameter "Filename"
  $GLOBALS['commandParamPK_MediaType']=29;		// PK_CommandParameter from CommandParameter table for parameter "PK_MediaType"
  $GLOBALS['commandParamPK_DeviceTemplate']=44;	// PK_CommandParameter from CommandParameter table for parameter "PK_DeviceTemplate"
  $GLOBALS['commandParamPK_EntertainArea']=45;	// PK_CommandParameter from CommandParameter table for parameter "PK_EntertainArea"
  
  $GLOBALS['commandSetVar']=27;					// PK_Command for "Set var to path"
  $GLOBALS['commandGotoScreen']=5;				// PK_Command for "Go to screen"
  $GLOBALS['commandVariableNumber']=86;			// PK_Command for Variable Number
  
  $GLOBALS['mediaOptionsArray']=array('TV', 'playlists', 'music', 'movies', 'videos', 'pictures', 'documents');
  
  
  $GLOBALS['DeviceDataLinkedToTables']=array('PK_Distro','PK_FloorplanObjectType','PK_Skin','PK_Size','PK_Language','PK_Users');
  $GLOBALS['deviceTemplateOrbiter']=8;		// the PK_DeviceTemplate for 'Orbiter'

  $GLOBALS['FloorplanObjectType']=11;		// PK_DeviceData for FloorplanObjectType from DeviceData
  $GLOBALS['FloorplanInfo']=10;				// PK_DeviceData for FloorplanInfo (coordinates) from DeviceData

  $GLOBALS['EventCriteriaList']=2;			// PK_CriteriaList for 'Events'
  $GLOBALS['EventsHandlerArray']=20;		// PK_Array for Events Handler
  $GLOBALS['EventsHandlerTemplate']=11;		// PK_Template for Events Handler
  
  $GLOBALS['rootCameras']=93;				// PK_DeviceCategory for Surveillance Cameras
  $GLOBALS['securityAlert']=33;				// PK_DeviceData for Alert from DeviceData table
  $GLOBALS['IntercomCategory']=94;			// PK_DeviceCategory for Intercom from DeviceCategory table
  
  $GLOBALS['MobileOrbiterNotification']=34;	// PK_DeviceData for Mobile Orbiter Notification from DeviceData table
  $GLOBALS['OtherPhoneNotifications']=35;	// PK_DeviceData for Other Phone Notifications from DeviceData table
  $GLOBALS['NeighborstoCall']=36;			// PK_DeviceData for Neighbors to Call from DeviceData table
  
  $GLOBALS['camerasVariableNumbersArray']=array(18,19,20,21);
  $GLOBALS['commandParameterValueToAsign']=5;
  $GLOBALS['commandParameterObjectScreen']=87;
  $GLOBALS['mnuSecurityCamerasDesignObj']=1414;	// PK_DesignObj for Security cameras
  
  $GLOBALS['SecurityArmDisarmTemplate']=13;		// PK_Template for Security - Arm Disarm
  $GLOBALS['SecuritySOSTemplate']=14;			// PK_Template for Security - SOS
  $GLOBALS['SecurityViewCamerasTemplate']=15;	// PK_Template for Security - View Cameras
  
  // do not change the following lines 
  $addMasterUserUrl=$MasterUsersHost."index.php?section=add_master_user";
  $checkMasterUserUrl=$MasterUsersHost."index.php?section=check_master_user";
  $changePassMasterUserUrl=$MasterUsersHost."index.php?section=change_password";
  $updateMasterUserUrl=$MasterUsersHost."index.php?section=update_master_users";

?>