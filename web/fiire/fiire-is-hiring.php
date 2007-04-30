<?
session_name('FIIRE');
session_start();

include('lib/config.php');
include('lib/functions.php');
include('lib/init.php');

$title='FIIRE :: Fiire is hiring';
$page_template=implode('',file('templates/template.html'));
$content=get_fiire_is_hiring($conn);
$content.='<br>'.contact('fiire-is-hiring',$conn);
$login_form=login_header($conn);
$navArray=array(
	'index.php'=>'Home',
	'fiire-is-hiring.php'=>'Fiire is hiring'
);

// set dynamic fields
$variables=array();
$variables=set_variable($variables,'title',$title);
$variables=set_variable($variables,'content',$content);
$variables=set_variable($variables,'login_form',$login_form);
$variables=set_variable($variables,'nav_menu', nav_menu($navArray));

outputHTML($variables,$page_template);
?>
