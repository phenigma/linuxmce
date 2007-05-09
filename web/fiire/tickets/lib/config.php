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
$GLOBALS['db_user']="db_user";
$GLOBALS['db_pass']="db_pass";
$GLOBALS['db_database']="linuxmce_tickets";


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
$GLOBALS['website_url']='http://support.linuxmce.com/';

// path for uploaded pics
// need write permissions for web server and also be placed relative to web root
$GLOBALS['site_root']='/var/www/linux-mce/';
$GLOBALS['upload_path']=$GLOBALS['site_root'].'upload/';

// relative path to uploaded files
// need to be in sync with $GLOBALS['upload_path']
$GLOBALS['relative_upload_path']='upload/';

// the number of records per page in admin
$GLOBALS['records_per_page']=10;

// the number of seconds between posts: 0 = no check
$GLOBALS['time_between_comments']=30;

$GLOBALS['incident_status_array']=array(
	10=>'Issue open',
	20=>'Issue Closed - resolved',
	30=>'Issue Closed - unresolvable'
);


// override standard settings with local one
// to avoid saving in svn real users and passwords
if(file_exists('lib/config.local.php')){
	include('lib/config.local.php');
}
?>