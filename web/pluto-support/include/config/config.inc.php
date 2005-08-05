<?PHP
  	$GLOBALS['inDebug']=1;
  	
  	if($GLOBALS['inDebug']!=1){
  		// production settings
  		error_reporting(E_ALL ^ E_NOTICE);
  		include_once('/var/www/globalconfig/globalconfig.inc.php');
  	}else{
  		error_reporting(E_ALL);
  		include_once('/home/users/vali/work/web/globalconfig/globalconfig.inc.php');
  	}
  	
  // PlutoHome website
  	$GLOBALS['sendErrorsTo']='info@plutohome.com';
	
  	$GLOBALS['HTTPorFTP']=array(2,5,6);  	// the PK_RepositoryType for HTTP FTP and both from table RepositoryType
	$GLOBALS['CVSorSVN']=array(2,5,6);  	// the PK_RepositoryType for CVS or SVN  from table RepositoryType

	$directory="/support/";
	$domain="http://plutohome.com/";


	$defaultHelpUrl='index.php?section=help';
	
	
	define("VERSION","0.1");
	define("APPLICATION_NAME","Pluto support");
	
	define("APPROOT",$_SERVER['DOCUMENT_ROOT']."".$directory); 
	define("APPURL",$domain.$directory);
	define("APPDOMAIN",$domain);
	
	define("APPDIRECTORY",$directory);
	define("UPLOADDIR",'upload');		

    define("LANG","ro-RO");
  
  
    setlocale(LC_ALL, LANG);
    
  	//database 
  	require_once("database.inc.php");
    
  	// hard coded values
	require_once("hardCoded.php");
  
  // do not change the following lines
   	$plutoHomeRegister=$PlutoHomeHost.'index.php?section=register';
	$loginUrl=$PlutoHomeHost.'index.php?section=clientLoginForm';
	$logoutUrl=$PlutoHomeHost.'index.php?section=clientLoggedOut';

  	$addMasterUserUrl=$MasterUsersHost."index.php?section=add_master_user";
  	$checkMasterUserUrl=$MasterUsersHost."index.php?section=check_master_user";
	$changePassMasterUserUrl=$MasterUsersHost."index.php?section=change_password";	
	$updateMasterUserUrl=$MasterUsersHost."index.php?section=update_master_users";
?>