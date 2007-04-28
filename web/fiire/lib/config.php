<?
// DO NOT CHANGE THIS FILE
// create a file config.local.php in the same directory and copy/paste the variables you want to change
// eg:
// $GLOBALS['db_user']="your_user";
// $GLOBALS['db_pass']="your_pass";


// GENERAL SETTINGS
// remove next line or set it to 0 to disable output to screen
$GLOBALS['IN_DEBUG']=1;

// set action on error
// 0 no action
// 1 die on error
// 2 log error
$GLOBALS['action_on_error']=1;


// database settings
$GLOBALS['db_server']="localhost";
$GLOBALS['db_port']="3306";
$GLOBALS['db_user']="user";
$GLOBALS['db_pass']="pass";
$GLOBALS['db_database']="fiire";

// SMF settings
$GLOBALS['smf_database']="fiire";
$GLOBALS['smf_db_prefix']='smf_';

// contact email
$GLOBALS['contact_email']='your_name@your_address.com';

// override PHP settings, if necessary
ini_set('max_execution_time',30000);
ini_set('display_errors',1);

// set to E_ALL ^ E_NOTICE for production site
error_reporting(E_ALL);

// set to 1 to dislay mysql queries
$GLOBALS['db_in_debug']=0;

// website url
$GLOBALS['website_url']='http://fiire.com/';

// path for uploaded pics
// need write permissions for web server and also be placed relative to web root
$GLOBALS['site_root']='/var/www/fiire/upload/';
$GLOBALS['upload_path']=$GLOBALS['site_root'].'upload/';

// relative path to uploaded files
// need to be in sync with $GLOBALS['upload_path']
$GLOBALS['relative_upload_path']='upload/';

// the number of records per page in admin
$GLOBALS['records_per_page']=10;

// the number of seconds between posts: 0 = no check
$GLOBALS['time_between_comments']=30;

$GLOBALS['fiire_engine_url']='http://fiireshop.com/product_info.php?products_id=566';
$GLOBALS['fiire_station_url']='http://fiireshop.com/product_info.php?products_id=567';
$GLOBALS['fiire_chief_url']='http://fiireshop.com/product_info.php?products_id=568';

// override standard settings with local one
// to avoid saving in svn real users and passwords
if(file_exists('lib/config.local.php')){
	include('lib/config.local.php');
}

?>