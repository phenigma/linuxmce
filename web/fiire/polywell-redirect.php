<?
session_name('FIIRE');
session_start();

include('lib/config.php');
include('lib/functions.php');
include('lib/init.php');

$url=@$_REQUEST['go'];

$title='FIIRE :: Sure Fiire';
$page_template=implode('',file('templates/template.html'));
$content=get_polywell_redirect($url);
$login_form=login_header($conn);

// set dynamic fields
$variables=array();
$variables=set_variable($variables,'title',$title);
$variables=set_variable($variables,'content',$content);
$variables=set_variable($variables,'login_form',$login_form);
//$variables=set_variable($variables,'meta_refresh','<meta http-equiv="refresh" content="2;url='.$url.'">');

outputHTML($variables,$page_template);
?>
