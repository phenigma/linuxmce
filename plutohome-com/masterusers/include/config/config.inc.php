<?PHP
	include_once('/var/www/globalconfig/globalconfig.inc.php');  
      
 //  error_reporting(E_ALL~E_NOTICE);
  //debuging
  	$GLOBALS['inDebug']=1;
  	$GLOBALS['sendErrorsTo']='info@plutohome.com';

	$directory="/masterusers/";
	$domain="http://plutohome.com";
//	$https="https://plutohome.com/masterusers/";
	$https="http://plutohome.com/masterusers/";
	
	define("VERSION","0.1");
	define("APPLICATION_NAME","Plutovip.com");
	
	define("APPROOT",$_SERVER['DOCUMENT_ROOT']."".$directory."/"); 
	define("APPURL",$domain.$directory);
	define("APPDOMAIN",$domain);
	
	define("APPDIRECTORY",$directory);
	define("UPLOADDIR",'upload');		

    define("LANG","en-EN");
  
  
    setlocale(LC_ALL, LANG);
    
  	//database 
  	require_once("database.inc.php");
      
	
?>