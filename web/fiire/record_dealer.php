<?
session_name('FIIRE');
session_start();

include('lib/config.php');
include('lib/functions.php');
include('lib/init.php');

$title='FIIRE :: Dealer';
$categ_titles=array(
	1=>'Become a dealer',
	2=>'Become a custom installer',
	3=>'Announce an open house event'
);

$page_template=implode('',file('templates/template.html'));
$content=record_dealer($categ_titles,$conn);
$login_form=login_header($conn);
$categ=(int)@$_REQUEST['categ'];

$navArray=array(
	'index.php'=>'Home',
	'myaccount.php'=>'My account',
	'record_dealer.php'=>@$categ_titles[$categ]
);

// set dynamic fields
$variables=array();
$variables=set_variable($variables,'title',$title);
$variables=set_variable($variables,'content',$content);
$variables=set_variable($variables,'login_form',$login_form);
$variables=set_variable($variables,'nav_menu', nav_menu($navArray));

outputHTML($variables,$page_template);
?>
