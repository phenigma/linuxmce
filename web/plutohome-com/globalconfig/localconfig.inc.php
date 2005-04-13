<?
	// Web URLs
  	$forumHost="http://plutohome.com/support/phpbb2/";
  	$MantisHost="http://plutohome.com/support/mantis/";

  	$PlutoHomeHost="http://plutohome.com/";
  	$MasterUsersHost="http://plutohome.com/masterusers/";
	$PlutoSupportHost='http://plutohome.com/support/';

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

  	$GLOBALS['HTTPorFTP']=array(2,5,6);  	// the PK_RepositoryType for HTTP FTP and both from table RepositoryType
	$GLOBALS['CVSorSVN']=array(2,5,6);  	// the PK_RepositoryType for CVS or SVN  from table RepositoryType
	$GLOBALS['DHCPDeviceData']=28;			// PK_DeviceData for DHCP
	
	// pluto_main
	$dbPlutoMainUser = 'root';
	$dbPlutoMainPass = '';
	$dbPlutoMainDatabase = 'pluto_main';
	$dbPlutoMainServer = 'localhost';
	$dbPlutoMainType = 'mysql';
	
	// pluto admin database settings
	$dbPlutoAdminUser = 'root';
	$dbPlutoAdminPass = '';
	$dbPlutoAdminDatabase = 'pluto_main';
	$dbPlutoAdminServer = 'localhost';
	$dbPlutoAdminType = 'mysql';
	
	// pluto media
	$dbPlutoMediaType = "MySQL";
	$dbPlutoMediaUser = "plutomedia";
	$dbPlutoMediaPass = "";
	$dbPlutoMediaServer = "localhost";
	$dbPlutoMediaDatabase = "pluto_media";

	// pluto security
	$dbPlutoSecurityType = "MySQL";
	$dbPlutoSecurityUser = "plutosecurity";
	$dbPlutoSecurityPass = "";
	$dbPlutoSecurityServer = "localhost";
	$dbPlutoSecurityDatabase = "pluto_security";
?>
