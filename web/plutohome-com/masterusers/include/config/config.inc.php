<?PHP
	include_once('/var/www/globalconfig/globalconfig.inc.php');  
      
 //  error_reporting(E_ALL~E_NOTICE);
  //debuging
  	$GLOBALS['inDebug']=0;
  	$GLOBALS['sendErrorsTo']='info@plutohome.com';

	$directory="/masterusers/";
	$domain="http://plutohome.com";
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
	$GLOBALS['testersRefArray']=array(33219,33160);
    
  	//database 
  	require_once("database.inc.php");
      
	
?>