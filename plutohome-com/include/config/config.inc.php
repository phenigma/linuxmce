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
    
   	// hard-coded values
	$GLOBALS['rootOrbiterID'] = 5; 			// the PK_DeviceCategory from DeviceCategory table for the root orbiter
	$GLOBALS['rootCoreID'] = 7; 			// the PK_DeviceCategory from DeviceCategory table for the root Core
	$GLOBALS['rootMediaDirectorID'] = 8;	// the PK_DeviceCategory from DeviceCategory table for the root MediaDirector
	
	$GLOBALS['rootGenericMediaDirector']=28;// the PK_DeviceTemplate for Generic Media Director
	$GLOBALS['rootDCERouter'] = 1; 			// the PK_DeviceTemplate from DeviceTemplate table for the root DCE Router
	$GLOBALS['deviceTemplateOrbiter']=8;	// the PK_DeviceTemplate for 'Orbiter' 
	$GLOBALS['rootMediaPlugin']=2;				// Media Plug-in
	
	$GLOBALS['rootDevelopment']=8;			// the PK_DeviceData for description Development from table DeviceData
	$GLOBALS['rootPK_Distro']=7;			// the PK_DeviceData for PK_Distro from table DeviceData
	$GLOBALS['rootDisklessBoot']=9;			// the PK_DeviceData for Diskless Boot from table DeviceData
	$GLOBALS['DHCPDeviceData']=28;			// PK_DeviceData for DHCP
	
  	$GLOBALS['HTTPorFTP']=array(2,5,6);  	// the PK_RepositoryType for HTTP FTP and both from table RepositoryType
	$GLOBALS['CVSorSVN']=array(2,5,6);  	// the PK_RepositoryType for CVS or SVN  from table RepositoryType
		
	$GLOBALS['userIsAdmin']=array(33129,33130);
	
	$GLOBALS['MiscelaneousRoomType']=9;		// PK_RoomType for Miscelaneous

	// Array table
	$GLOBALS['ArrayIDForMedia'] = 5;		// Media Scenarios
	
	// Command table
	$GLOBALS['commandMHPlayMedia']=43;		// MH Play Media
	$GLOBALS['commandGotoScreen']=5;		// Go to screen
	$GLOBALS['commandSetVar']=27;			// Set Variable
	
	// CommandParameter table
	$GLOBALS['commandParamPK_MediaType']=29;		// PK_MediaType
	$GLOBALS['commandParamPK_DesignObj']=3;			// PK_DesignObj
	$GLOBALS['commandParameterVariableNumber']=4;	// Variable Number (PK_Variable)
	$GLOBALS['commandParameterValueToAsign']=5;		// Value to assign
	
	// PK_DesignObj from table DesignObj
	$GLOBALS['mnuMediaFileList2DesignObj']=3226;	// mnuMediaFileList2
	$GLOBALS['mnuMediaFileListDesignObj']=2071;		// mnuMediaFileList
	
	// PK_Device for Local Orbiter
	$GLOBALS['localOrbiter']=-300;

  	//database 
  	require_once("database.inc.php");

  	
  	
  	// DO NOT CHANGE ANYTHING AFTER THIS LINE
  	$addMasterUserUrl=$MasterUsersHost."index.php?section=add_master_user";
  	$checkMasterUserUrl=$MasterUsersHost."index.php?section=check_master_user";
	$changePassMasterUserUrl=$MasterUsersHost."index.php?section=change_password";	
	$updateMasterUserUrl=$MasterUsersHost."index.php?section=update_master_users";
 ?>
