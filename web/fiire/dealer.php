<?
session_name('FIIRE');
session_start();

include('lib/config.php');
include('lib/functions.php');
include('lib/init.php');
$titleOptions=array(
	0=>'Dealers, custom installers and open house events',
	1=>'Dealers',
	2=>'Custom installers',
	3=>'Open house events'
);



$title='FIIRE :: '.$titleOptions[(int)@$_REQUEST['dealerType']];
$page_template=implode('',file('templates/template.html'));
$content=get_dealer($conn);
$login_form=login_header($conn);
$navArray=array(
	'index.php'=>'Home',
	'dealer.php'=>'Dealer'
);

// set dynamic fields
$variables=array();
$variables=set_variable($variables,'title',$title);
$variables=set_variable($variables,'content',$content);
$variables=set_variable($variables,'login_form',$login_form);
$variables=set_variable($variables,'nav_menu', nav_menu($navArray));

outputHTML($variables,$page_template);
?>
