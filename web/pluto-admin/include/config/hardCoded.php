<?
	// hard-coded values
	// PK_DeviceCategory from table DeviceCategory
	
	$GLOBALS['rootOrbiterID'] = 5; 				// Orbiter
	$GLOBALS['rootComputerID'] = 6; 			// Computers
	$GLOBALS['rootMediaDirectors'] = 8; 		// Media Directors
	$GLOBALS['rootInterfaces'] = 71; 			// Interfaces
	$GLOBALS['rootLights'] = 73; 				// Lighting devices
	$GLOBALS['rootAVEquipment'] = 76; 			// AV Equipment
	$GLOBALS['rootClimate'] = 83; 				// Climate devices
	$GLOBALS['rootGenericIO'] = 79; 			// Generic I/O
	$GLOBALS['rootSecurity'] = 84; 				// Security devices
	$GLOBALS['IntercomCategory']=94;			// Intercom 
	$GLOBALS['rootCameras']=93;					// Surveillance Cameras
	$GLOBALS['rootLightsInterfaces'] = 81;		// Light Interfaces
	$GLOBALS['rootClimateInterfaces'] = 82;		// Climate Interfaces
	$GLOBALS['rootSecurityInterfaces'] = 80;	// Security Interfaces
	$GLOBALS['rootPhones'] = 89;				// Phones
	$GLOBALS['specialized']=97;					// Specialized
	$GLOBALS['InfraredInterface']=114;			// Infrared Interface
	$GLOBALS['TVdevices']=77;					// TV's
	$GLOBALS['CableBoxes']=105;					// Cable boxes
	$GLOBALS['SatelliteBoxes']=108;				// Satellite Boxes
	$GLOBALS['TVVCRCombo']=106;					// TV VCR Combo
	$GLOBALS['AmpsReceivers']=103;				// Amps/Receivers
	$GLOBALS['RemoteControlls']=117;			// Remote Controlls
	$GLOBALS['GenericSerialDevices']=119;		// GenericSerialDevices
	$GLOBALS['PVRCaptureCards']=75;				// PVR Capture Cards
	$GLOBALS['SoundCards']=124;					// Sound Cards
	$GLOBALS['VideoCards']=125;					// Video Cards
	$GLOBALS['InfraredReceivers']=132;			// Infrared receivers

	$GLOBALS['TVdevicesArray']=array($GLOBALS['TVdevices'],$GLOBALS['TVVCRCombo'],$GLOBALS['AmpsReceivers']);
	
	// PK_DeviceTemplate from table DeviceTemplate
	
	$GLOBALS['rootDCERouter'] = 1; 				// DCE Router
	$GLOBALS['rootMediaPlugin']=2;				// Media Plug-in
	$GLOBALS['rootCoreID'] = 7; 				// Core
	$GLOBALS['deviceTemplateOrbiter']=8;		// Orbiter
	$GLOBALS['OrbiterPlugIn'] = 12; 			// Orbiter Plug In
	$GLOBALS['rootMediaDirectorsID'] = 28; 		// Media Directors
	$GLOBALS['SecurityPlugin'] = 33;	 		// Security plugin
	$GLOBALS['LightSwitchOnOff']=37;			// Light Switch (on/off)
	$GLOBALS['LightSwitchDimmable']=38;			// Light Switch (dimmable)
	$GLOBALS['InfraredPlugIn'] = 39; 			// Infrared Plug In
	$GLOBALS['EIB']=49;							// EIB
	$GLOBALS['GenericInputOuput']=43;			// Generic Input Ouput 
	$GLOBALS['DrapesSwitch']=68;				// Drapes Switch
	$GLOBALS['StandardThermostat']=4;			// Standard Thermostat
	$GLOBALS['Tuner']=75;						// Generic Tuner
	
	
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
	$GLOBALS['TelecomScenariosTemplate']=17;		// Telecom Scenarios
	
	// PK_Array from table Array
	
	$GLOBALS['ArrayIDForLight'] = 1;				// Lighting Scenarios
	$GLOBALS['ArrayIDForClimate'] = 2;				// Climate Scenarios
	$GLOBALS['ArrayIDForSecurity'] = 3;				// Security scenarios
	$GLOBALS['ArrayIDCommunicationScenarios']=4; 	// Communication scenarios
	$GLOBALS['ArrayIDForMedia'] = 5;				// Media Scenarios
	$GLOBALS['EventsHandlerArray']=20;				// Events Handler
	$GLOBALS['ArrayIDForMobileOrbiterScenarios']=23;// Mobile Orbiter Scenarios	
	$GLOBALS['ArrayIDSleepingScenarios']=21;		// Sleeping Scenarios
	
	// PK_Command from Command table
	
	$GLOBALS['inputSelectCommand']=91;				// Input select
	$GLOBALS['DSPModeCommand']=116;					// DSP Mode
	$GLOBALS['genericONCommand']=192;				// Generic On
	$GLOBALS['genericOFFCommand']=193;				// Generic OFF
	$GLOBALS['powerCommand']=194;					// Generic OFF
	$GLOBALS['genericSetLevelCommand']=184;			// Set Level
	$GLOBALS['genericHeatCommand']=197;				// Generic Heat
	$GLOBALS['genericCoolCommand']=196;				// Generic Cool
	$GLOBALS['setCoolHeat']=280;					// Set cool/Heat, with parm C for Cool and H for Heat
	$GLOBALS['MediaScenariosCommand']=43;			// MH Play Media
	$GLOBALS['commandSetUserMode']=58;				// Set User Mode	
	$GLOBALS['commandSetVar']=27;					// Set Variable
	$GLOBALS['commandGotoScreen']=5;				// Go to screen
	$GLOBALS['commandMHPlayMedia']=43;				// MH Play Media
	$GLOBALS['commandPL_Originate']=232;			// PL_Originate
	$GLOBALS['outputSelectCommand']=244;			// Output select
	$GLOBALS['tuner_1']=166;						// Tuner 1
	$GLOBALS['tuner_2']=175;						// Tuner 2
	$GLOBALS['tuner_3']=326;						// Tuner 3
	$GLOBALS['AM_Command']=169;						// AM
	$GLOBALS['FM_Command']=170;						// FM
	$GLOBALS['SW_Command']=328;						// SW
	$GLOBALS['WB_Command']=327;						// WB
	
	// PK_CommandParameters from table CommandParameters
	
	$GLOBALS['commandParamPK_Device']=2;			// PK_Device
	$GLOBALS['commandParamPK_DesignObj']=3;			// PK_DesignObj
	$GLOBALS['commandParameterVariableNumber']=4;	// Variable Number (PK_Variable)
	$GLOBALS['commandParameterValueToAsign']=5;		// Value to assign
	$GLOBALS['commandParameterOnOff']=8;			// On/Off
	$GLOBALS['commandParamID']=10;					// ID
	$GLOBALS['commandParamFilename']=13;			// Filename
	$GLOBALS['commandParamDesignObjCurrentScreen']=16;	// PK_DesignObj_CurrentScreen
	$GLOBALS['commandParamPKUsers']=17;				// PK_Users
	$GLOBALS['commandParamStoreVariables']=22;		// Store Variables
	$GLOBALS['commandParamPK_MediaType']=29;		// PK_MediaType
	$GLOBALS['commandParamPK_DeviceTemplate']=44;	// PK_DeviceTemplate
	$GLOBALS['commandParamPK_EntertainArea']=45;	// PK_EntertainArea
	$GLOBALS['commandParamAbsoluteLevel']=76;		// Absolute Level
	$GLOBALS['commandPhoneExtension']=83;			// PhoneExtension
	
	$GLOBALS['commandParameterObjectScreen']=16;	// Object(Screen)
	
	// PK_DeviceData from table DeviceData
	$GLOBALS['Path']=2;							// the Path
	$GLOBALS['rootPK_Distro']=7;				// the PK_Distro
	$GLOBALS['FloorplanInfo']=10;				// FloorplanInfo (coordinates)
	$GLOBALS['FloorplanObjectType']=11;			// FloorplanObjectType
	$GLOBALS['Channel']=12;						// Channel
	$GLOBALS['InputOrOutput']=18;				// InputOrOutput
	$GLOBALS['Language']=26;					// Language
	$GLOBALS['DHCPDeviceData']=28;				// DHCP
	$GLOBALS['PhoneNumber']=31;					// Phone Number
	$GLOBALS['NetworkInterfaces']=32;			// Network Interfaces
	$GLOBALS['securityAlert']=33;				// Alert 
	$GLOBALS['MobileOrbiterNotification']=34;	// Mobile Orbiter Notification
	$GLOBALS['OtherPhoneNotifications']=35;		// Other Phone Notifications 
	$GLOBALS['NeighborstoCall']=36;				// Neighbors to Call 
	$GLOBALS['Port']=37;						// Port
	$GLOBALS['securityAlertType']=45;			// Alert type 
	$GLOBALS['InfraredPort']=12;				// Infrared Port
	$GLOBALS['PK_Device']=77;					// PK_Device
	$GLOBALS['AudioSettings']=88;				// Audio settings
	$GLOBALS['VideoSettings']=89;				// Video settings
	$GLOBALS['EmergencyCalls']=96;				// Emergency Calls
	
	// PK_FloorplanType from table FloorplanType 

	$GLOBALS['SecurityFoorplanType']=1;		// Security Zone
	$GLOBALS['LightingFoorplanType']=2;		// Lighting Zone
	$GLOBALS['ClimateFoorplanType']=3;		// Climate Zone
	$GLOBALS['CameraFoorplanType']=4;		// Camera Zone
	$GLOBALS['EntertainmentZone']=5;		// Entertainment Zone
	$GLOBALS['AVEquipmentFlorplanType']=6;	// AV Equipment
	$GLOBALS['PhonesFoorplanType']=7;		// Phones
	
	// PK_DesignObj from table DesignObj
	
	$GLOBALS['mnuSecurityCamerasDesignObj']=1414;		// Security cameras
	$GLOBALS['mnuNoMediaInDrive']=1696;					// mnuNoMediaInDrive
	$GLOBALS['mnuSingleCameraViewOnlyDesignObj']=1405;	// Single Camera View Only
	$GLOBALS['mnuMediaFileListDesignObj']=2071;			// mnuMediaFileList
	$GLOBALS['mnuMediaFileList2DesignObj']=3226;		// mnuMediaFileList2
	$GLOBALS['butViewDesignObj']=3283;					// butView
	$GLOBALS['TelecomMenu']=1323;						// Telecom Menu mnuMakeCallFavorites
	$GLOBALS['mnuPicturesDocumentsList']=3282;			// mnuPicturesDocumentsList
	$GLOBALS['mnuSecurityPanel']=1389;					// mnuSecurityPanel for Arm/Disarm
	
	// PK_CriteriaList from table CriteriaList
	
	$GLOBALS['EventCriteriaList']=2;			// Events

	// PK_CriteriaParmList from table CriteriaParmList
	
	$GLOBALS['TimeOfDayParmList']=11;					// Interval and also Time of Day
	$GLOBALS['DayOfWeekParmList']=17;					// Day Of Week
	$GLOBALS['DayOfMonthParmList']=18;					// Day Of Month
	$GLOBALS['SpecificDateParmList']=11;				// Specific Date or Absolute
	$GLOBALS['PK_EventParameterParmList']=24;			// PK_EventParameter

	// PK_Variable from table Variable
	
	$GLOBALS['camerasVariableNumbersArray']=array(18,19,20,21);
	$GLOBALS['cameraVariable']=22;
	$GLOBALS['PK_DeviceGroup']=32;
	
	// PK_StartupScript from table StartupScript
	$GLOBALS['ProcessLogs']=45;					// Process_Logs.sh
	$GLOBALS['ShareIRCodes']=49;				// Share IR codes

	// PK_CommandCategory
	$GLOBALS['AVCommandCategory']=12;			// AV commands
	$GLOBALS['DSPModeCommandCategory']=21;		// DSP Modes
	$GLOBALS['InputsCommandCategory']=22;		// Inputs
	$GLOBALS['OutputsCommandCategory']=27;		// Outputs
	$GLOBALS['GeneralInternal']=55;				// General/Internal
	
	// options to display in Media Wizard
	
	$GLOBALS['mediaOptionsArray']=array('TV', 'playlists', 'music', 'movies', 'videos', 'pictures', 'documents', 'Disc CD/DVD');
	
	// DeviceData displayed as pulldown menu
	
	$GLOBALS['DeviceDataLinkedToTables']=array('PK_Distro','PK_FloorplanObjectType','PK_Skin','PK_Size','PK_Language','PK_Users','PK_AlertType');
	
	// PK_Device for Local Orbiter
	
	$GLOBALS['localOrbiter']=-300;
	
	// PK_Pipe from table Pipe
	
	$GLOBALS['AudioPipe']=1;		// Audio Pipe
	$GLOBALS['VideoPipe']=2;		// Video Pipe
	
	// PK_PageSetup from table PageSetup
	$GLOBALS['InstallationPage']=11;	// Installation Settings

	// PK_Package from table Package
	$GLOBALS['orbiterInstallerPackage']=363;

	// DeviceCommandGroups
	$GLOBALS['ThermostatCommands']=36;
	
	// date format
	$GLOBALS['defaultDateFormat']='Y-m-d h:i:s';

	$GLOBALS['GenericSerialDeviceCommandLine']='Generic_Serial_Device';		// the command line used to identify generic serial devices
	
	$GLOBALS['SecurityPicsPath']='/home/securitypic/';
	
	// criticals device templates; user will be notified not to delete them
	$GLOBALS['CriticalDeviceTemplates']=array(1,2,6,7,8,9,10,12,27,28,31,32,33,34,36,39);
	
?>
