<?
	include(APPROOT.'/include/config/config.inc.php');
	include(APPROOT.'/include/utils.inc.php');

	$configFile = $GLOBALS['plutoConfPath'];

	$DB_NAME = LoadConfig($configFile,"MySqlDBName");
	$DB_HOST = LoadConfig($configFile,"MySqlHost");
	$DB_PORT = LoadConfig($configFile,"MySqlPort");
	$DB_USER = LoadConfig($configFile,"MySqlUser");
	$DB_PASS = LoadConfig($configFile,"MySqlPassword");

  	$LinuxMCEHost="http://linuxmce.org/";
 	$wikiHost='http://wiki.linuxmce.org/';
 
	// pluto_main
	$dbPlutoMainType = 'mysql';
	$dbPlutoMainUser = $DB_USER;
	$dbPlutoMainPass = $DB_PASS;
	$dbPlutoMainDatabase = $DB_NAME;
	$dbPlutoMainServer = $DB_HOST;
	
	
	// pluto media
	$dbPlutoMediaType = "mysql";
	$dbPlutoMediaUser = "root";
	$dbPlutoMediaPass = "";
	$dbPlutoMediaServer = "localhost";
	$dbPlutoMediaDatabase = "pluto_media";	
	
	// pluto security
	$dbPlutoSecurityType = "mysql";
	$dbPlutoSecurityUser = "plutosecurity";
	$dbPlutoSecurityPass = "";
	$dbPlutoSecurityServer = "localhost";
	$dbPlutoSecurityDatabase = "pluto_security";
?>
