<?
session_name('FIIRE');
session_start();

include('lib/config.php');
include('lib/functions.php');
include('lib/init.php');

$title='FIIRE :: My bug reports';

$page_template=implode('',file('templates/template.html'));
$content=bug_report(@$_REQUEST['action'],$conn);
$login_form=login_header($conn);
$categ=(int)@$_REQUEST['categ'];

$navArray=array(
	'index.php'=>'Home',
	'myaccount.php'=>'My account',
	'bug_report.php'=>'My bug reports'
);

// set dynamic fields
$variables=array();
$variables=set_variable($variables,'title',$title);
$variables=set_variable($variables,'content',$content);
$variables=set_variable($variables,'login_form',$login_form);
$variables=set_variable($variables,'nav_menu', nav_menu($navArray));

outputHTML($variables,$page_template);
?>
