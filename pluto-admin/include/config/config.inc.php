<?PHP
  	//debuging
  	$GLOBALS['inDebug']=1;
  	
  	if($GLOBALS['inDebug']!=1){
  		// production settings
  		error_reporting(E_ALL ^ E_NOTICE);
  		include_once('/var/www/globalconfig/globalconfig.inc.php');
  	}else{
  		error_reporting(E_ALL);
  		include_once('/var/www/test/globalconfig/globalconfig.inc.php');
  	}
	
	$GLOBALS['sendErrorsTo']='support@plutohome.com';
	
	$directory="/pluto-admin/";
	$domain="http://localhost";
	
	// path where the media pics are uploaded
	$GLOBALS['mediaPicsPath']=$_SERVER['DOCUMENT_ROOT'].'/pluto-admin/mediapics/';
	$GLOBALS['floorplansPath']='/var/www/test/floorplans';
	
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
	
	// hard-coded values
	// PK_DeviceCategory from table DeviceCategory
	
	$GLOBALS['rootOrbiterID'] = 5; 				// Orbiter
	$GLOBALS['rootComputerID'] = 6; 			// Computers
	$GLOBALS['rootLights'] = 73; 				// Lighting devices
	$GLOBALS['rootClimate'] = 83; 				// Climate devices
	$GLOBALS['rootSecurity'] = 84; 				// Security devices
	$GLOBALS['IntercomCategory']=94;			// Intercom 
	$GLOBALS['rootCameras']=93;					// Surveillance Cameras
	$GLOBALS['rootLightsInterfaces'] = 81;		// Light Interfaces
	$GLOBALS['rootClimateInterfaces'] = 82;		// Climate Interfaces
	$GLOBALS['rootSecurityInterfaces'] = 80;	// Security Interfaces
	
	// PK_DeviceTemplate from table DeviceTemplate
	
	$GLOBALS['rootDCERouter'] = 1; 				// DCE Router
	$GLOBALS['rootCoreID'] = 7; 				// Core
	$GLOBALS['rootMediaDirectorsID'] = 28; 		// Media Directors
	$GLOBALS['rootMediaPlugin']=2;				// Media Plug-in
	$GLOBALS['deviceTemplateOrbiter']=8;		// Orbiter
	
	// PK_Template from table Template
	
	$GLOBALS['LightingScenariosTemplate']=9;		// Lighting Scenarios
	$GLOBALS['ClimateScenariosTemplate']=10;		// Climate Scenarios
	$GLOBALS['securityScenariosTemplate']=12;		// Security Scenarios
	$GLOBALS['MediaScenariosTemplate']=1;			// Media Scenarios
	$GLOBALS['EventsHandlerTemplate']=11;			// Events Handler
	$GLOBALS['SecurityArmDisarmTemplate']=13;		// Security - Arm Disarm
	$GLOBALS['SecuritySOSTemplate']=14;				// Security - SOS
	$GLOBALS['SecurityViewCamerasTemplate']=15;		// Security - View Cameras
	$GLOBALS['SecurityViewOneCameraTemplate']=16;	// Security - View Camera

	// PK_Array from table Array
	
	$GLOBALS['ArrayIDForLight'] = 1;		// Lighting Scenarios
	$GLOBALS['ArrayIDForClimate'] = 2;		// Climate Scenarios
	$GLOBALS['ArrayIDForSecurity'] = 3;		// Security scenarios
	$GLOBALS['ArrayIDForMedia'] = 5;		// Media Scenarios
	$GLOBALS['EventsHandlerArray']=20;		// Events Handler
	
	// PK_Command from Command table
	
	$GLOBALS['genericONCommand']=192;				// Generic On
	$GLOBALS['genericOFFCommand']=193;				// Generic OFF
	$GLOBALS['genericSetLevelCommand']=184;			// Set Level
	$GLOBALS['genericHeatCommand']=197;				// Generic Heat
	$GLOBALS['genericCoolCommand']=196;				// Generic Cool
	$GLOBALS['MediaScenariosCommand']=43;			// MH Play Media	
	$GLOBALS['commandSetVar']=27;					// Set Variable
	$GLOBALS['commandGotoScreen']=5;				// Go to screen
	$GLOBALS['commandMHPlayMedia']=43;				// MH Play Media
	
	// PK_CommandParameters from table CommandParameters
	
	$GLOBALS['commandParamPK_Device']=2;			// PK_Device
	$GLOBALS['commandParamPK_DesignObj']=3;			// PK_DesignObj
	$GLOBALS['commandParameterVariableNumber']=4;	// Variable Number (PK_Variable)
	$GLOBALS['commandParameterValueToAsign']=5;		// Value to assign
	$GLOBALS['commandParamFilename']=13;			// Filename
	$GLOBALS['commandParamPK_MediaType']=29;		// PK_MediaType
	$GLOBALS['commandParamPK_DeviceTemplate']=44;	// PK_DeviceTemplate
	$GLOBALS['commandParamPK_EntertainArea']=45;	// PK_EntertainArea
	$GLOBALS['commandParamAbsoluteLevel']=76;		// Absolute Level
	
	
	$GLOBALS['commandParameterObjectScreen']=16;	// Object(Screen)
	
	// PK_DeviceData from table DeviceData
	$GLOBALS['FloorplanInfo']=10;				// FloorplanInfo (coordinates)
	$GLOBALS['FloorplanObjectType']=11;			// FloorplanObjectType
	$GLOBALS['securityAlert']=33;				// Alert 
	$GLOBALS['MobileOrbiterNotification']=34;	// Mobile Orbiter Notification
	$GLOBALS['OtherPhoneNotifications']=35;		// Other Phone Notifications 
	$GLOBALS['NeighborstoCall']=36;				// Neighbors to Call 
	
	// PK_FloorplanType from table FloorplanType 

	$GLOBALS['SecurityFoorplanType']=1;		// Security Zone
	$GLOBALS['LightingFoorplanType']=2;		// Lighting Zone
	$GLOBALS['ClimateFoorplanType']=3;		// Climate Zone
	$GLOBALS['CameraFoorplanType']=4;		// Camera Zone

	// PK_DesignObj from table DesignObj
	
	$GLOBALS['mnuSecurityCamerasDesignObj']=1414;		// Security cameras
	$GLOBALS['mnuSingleCameraViewOnlyDesignObj']=1405;	// Single Camera View Only
	$GLOBALS['mnuMediaFileListDesignObj']=2071;			// mnuMediaFileList
	$GLOBALS['mnuMediaFileList2DesignObj']=3226;		// mnuMediaFileList2
	
	// PK_CriteriaList from table CriteriaList
	
	$GLOBALS['EventCriteriaList']=2;			// Events

	// PK_Variable from table Variable
	
	$GLOBALS['camerasVariableNumbersArray']=array(18,19,20,21);
	$GLOBALS['cameraVariable']=22;
	
	// options to display in Media Wizard
	$GLOBALS['mediaOptionsArray']=array('TV', 'playlists', 'music', 'movies', 'videos', 'pictures', 'documents');
	
	// DeviceData displayed as pulldown menu
	$GLOBALS['DeviceDataLinkedToTables']=array('PK_Distro','PK_FloorplanObjectType','PK_Skin','PK_Size','PK_Language','PK_Users');
	
	// PK_Device for Local Orbiter
	$GLOBALS['localOrbiter']=-300;
	
	
	// do not change the following lines
	$addMasterUserUrl=$MasterUsersHost."index.php?section=add_master_user";
	$checkMasterUserUrl=$MasterUsersHost."index.php?section=check_master_user";
	$changePassMasterUserUrl=$MasterUsersHost."index.php?section=change_password";
	$updateMasterUserUrl=$MasterUsersHost."index.php?section=update_master_users";

?>