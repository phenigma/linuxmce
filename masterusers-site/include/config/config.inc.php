<?PHP
   
      
 //  error_reporting(E_ALL~E_NOTICE);
  //debuging
  	$GLOBALS['inDebug']=1;
  	$GLOBALS['sendErrorsTo']='dragos.grosu@gmail.com';

	$directory="/masterusers/";
	$domain="http://localhost";
	$https="https://10.0.0.150/masterusers/";
	
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