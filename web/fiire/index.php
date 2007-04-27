<?
session_name('FIIRE');
session_start();

include('lib/config.php');
include('lib/functions.php');
include('lib/init.php');

$title='FIIRE';
$page_template=implode('',file('templates/template.html'));
$content=get_main();
$login_form=login_header($conn);

// set dynamic fields
$variables=array();
$variables=set_variable($variables,'title',$title);
$variables=set_variable($variables,'content',$content);
$variables=set_variable($variables,'login_form',$login_form);

outputHTML($variables,$page_template);
?>
