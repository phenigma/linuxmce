<?PHP
   
      
   error_reporting(E_ALL);
  //debuging
  //$GLOBALS['inDebug']=1;
  	
  // PlutoHome website
  	$GLOBALS['sendErrorsTo']='dragos.grosu@gmail.com';

	$directory="/support/";
	$domain="http://plutohome.com/";

//	$MasterUsersHost="https://localhost/masterusers/";
	$MasterUsersHost="http://plutohome.com/masterusers/";
	$PlutoHomeHost="http://plutohome.com/";
	$PlutoSupportHost='http://plutohome.com/support/';
	
	$defaultHelpUrl='index.php?section=help';
	
	
	define("VERSION","0.1");
	define("APPLICATION_NAME","Pluto support");
	
	define("APPROOT",$_SERVER['DOCUMENT_ROOT']."".$directory."/"); 
	define("APPURL",$domain.$directory);
	define("APPDOMAIN",$domain);
	
	define("APPDIRECTORY",$directory);
	define("UPLOADDIR",'upload');		

    define("LANG","ro-RO");
  
  
    setlocale(LC_ALL, LANG);
    
  	//database 
  	require_once("database.inc.php");
    
  	$GLOBALS['HTTPorFTP']=array(2,5,6);  	// the PK_RepositoryType for HTTP FTP and both from table RepositoryType
	$GLOBALS['CVSorSVN']=array(2,5,6);  	// the PK_RepositoryType for CVS or SVN  from table RepositoryType
	
	$GLOBALS['userIsAdmin']=array(33129);
  
  // do not change the following lines
   	$plutoHomeRegister=$PlutoHomeHost.'index.php?section=register';
	$loginUrl=$PlutoHomeHost.'index.php?section=clientLoginForm';
	$logoutUrl=$PlutoHomeHost.'index.php?section=clientLoggedOut';

  	$addMasterUserUrl=$MasterUsersHost."index.php?section=add_master_user";
  	$checkMasterUserUrl=$MasterUsersHost."index.php?section=check_master_user";
	$changePassMasterUserUrl=$MasterUsersHost."index.php?section=change_password";	
	$updateMasterUserUrl=$MasterUsersHost."index.php?section=update_master_users";
?>
