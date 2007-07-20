<?
session_name('FIIRE');
session_start();

include('lib/config.php');
include('lib/functions.php');
include('lib/init.php');

$id=(int)@$_REQUEST['uid'];

$title='FIIRE :: Sure Fiire';
$page_template=implode('',file('templates/template.html'));
$content=get_transactions_by_user($id,$conn);
$login_form=login_header($conn);
$navArray=array(
	'index.php'=>'Home',
	'myaccount.php'=>'My Account',
	'mycommisions.php'=>'My Commisions',
	'transactions.php?id='.$id=>'Transactions history'
);

// set dynamic fields
$variables=array();
$variables=set_variable($variables,'title',$title);
$variables=set_variable($variables,'content',$content);
$variables=set_variable($variables,'login_form',$login_form);
$variables=set_variable($variables,'nav_menu', nav_menu($navArray));

outputHTML($variables,$page_template);
?>
