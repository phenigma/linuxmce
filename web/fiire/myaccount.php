<?
session_name('FIIRE');
session_start();

include('lib/config.php');
include('lib/functions.php');
include('lib/init.php');

$title='FIIRE :: My Account';
$page_template=implode('',file('templates/template.html'));
$content=get_myaccount($conn);
$login_form=login_header($conn);
$navArray=array(
	'index.php'=>'Home',
	'myaccount.php'=>'My account'
);

// set dynamic fields
$variables=array();
$variables=set_variable($variables,'title',$title);
$variables=set_variable($variables,'content',$content);
$variables=set_variable($variables,'login_form',$login_form);
$variables=set_variable($variables,'nav_menu', nav_menu($navArray));

outputHTML($variables,$page_template);
?>
