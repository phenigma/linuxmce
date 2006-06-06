<?PHP
  	//debuging
  	$GLOBALS['inDebug']=0;
  	
  	// todo: set lang when translation is finished
  	$GLOBALS['lang']='en';
  	$_SESSION['skin']=0;
  	
  	if($GLOBALS['inDebug']!=1){
  		// production settings
  		error_reporting(E_ALL ^ E_NOTICE);
  		$GLOBALS['globalConfigPath']='/var/www/globalconfig/';
  		$GLOBALS['floorplansPath']='/var/www/pluto-admin/floorplans';
  		$GLOBALS['usersPicsPath']='/var/www/pluto-admin/users/';
  		$GLOBALS['roomsPicsPath']='/var/www/pluto-admin/rooms/';
  		$GLOBALS['scenariosPicsPath']='/var/www/pluto-admin/scenarios/';
  		$GLOBALS['pluto.conf']='/etc/pluto.conf';
  		$GLOBALS['wap.conf']='/etc/wap.conf';
  		$GLOBALS['pluto_callerid.conf']='/etc/pluto-callerid.conf';
  		$GLOBALS['ErrorLog']='/var/log/pluto/pluto-admin.log';
  		$GLOBALS['databasesArray']=array('pluto_main_devel'=>'pluto_main','pluto_myth_devel'=>'pluto_myth');
  		
  		// display loading time at the bottom of the page
  		$GLOBALS['display_loading_time']=0;
  		
  		// set it to path with write permission to enable loading time logging orr leave blank to disable
  		$GLOBALS['save_loading_time']='/var/www/pluto-admin/cached/';
  		
  		// log file for external calls
  		$GLOBALS['WebExecLogFile']='/var/log/pluto/webExecLog.log';
  	}else{
  		error_reporting(E_ALL);
  		$GLOBALS['globalConfigPath']='/home/users/vali/work/web/globalconfig/';
  		$GLOBALS['floorplansPath']='/home/users/vali/work/web/pluto-admin/floorplans/';
  		$GLOBALS['usersPicsPath']='/home/users/vali/work/web/pluto-admin/users/';
  		$GLOBALS['roomsPicsPath']='/home/users/vali/work/web/pluto-admin/rooms/';
  		$GLOBALS['scenariosPicsPath']='/home/users/vali/work/web/pluto-admin/scenarios/';
  		$GLOBALS['pluto.conf']='/home/users/vali/work/web/pluto.conf';
  		$GLOBALS['wap.conf']='/home/users/vali/work/web/wap.conf';
  		$GLOBALS['pluto_callerid.conf']='/home/users/vali/work/web/pluto-callerid.conf';
  		$GLOBALS['ErrorLog']='/var/log/pluto/pluto-admin.log';
  		$GLOBALS['databasesArray']=array('pluto_main'=>'pluto_main','pluto_myth'=>'pluto_myth');
  		
  		// display loading time at the bottom of the page
  		$GLOBALS['display_loading_time']=1;
  		
  		// set it to path with write permission to enable loading time logging orr leave blank to disable
  		$GLOBALS['save_loading_time']='/home/users/vali/work/web/pluto-admin/cached/';

  		// log file for external calls
  		$GLOBALS['WebExecLogFile']='/var/log/pluto/webExecLog.log';

  		// todo: remove
  		// setting get get parameter &red=1 set css style to red stylesheet and preserve it, set to 0 to return to pluto
  		if(isset($_REQUEST['red'])){
  			$_SESSION['skin']=(int)$_REQUEST['red'];
  		}
  	}

  	if($_SESSION['skin']==0){
		$_SESSION['cssFile']='include/styles/customer_setup_style.css';
		$_SESSION['jsForMenu']='scripts/menu_settings.js';
	}else{
		$_SESSION['cssFile']='include/styles/customer_setup_style_red.css';
		$_SESSION['jsForMenu']='scripts/menu_settings_red.js';
  	}
  	
  	include_once($GLOBALS['globalConfigPath'].'globalconfig.inc.php');
  	
	$GLOBALS['sendErrorsTo']='support@plutohome.com';
	
	$directory="/pluto-admin/";
	$domain="http://localhost";
	
	// path where the media pics and orbiter backgrounds are uploaded
	$GLOBALS['mediaPicsPath']=$_SERVER['DOCUMENT_ROOT'].'/pluto-admin/mediapics/';
	$GLOBALS['orbiterBackgroundPath']=$_SERVER['DOCUMENT_ROOT'].'/pluto-admin/orbiter_bg/';
	
	
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
	
	// hard coded values
	require_once("hardCoded.php");
	
?>
