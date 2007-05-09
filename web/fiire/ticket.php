<?
session_name('FIIRE');
session_start();

include('lib/config.php');
include('lib/functions.php');
include('lib/init.php');

$title='FIIRE :: Ticket history';

$id=(int)@$_REQUEST['id'];

$page_template=implode('',file('templates/template.html'));
$content=ticket_history($id,$conn);
$login_form=login_header($conn);

$navArray=array(
	'index.php'=>'Home',
	'myaccount.php'=>'My account',
	'ticket.php?id='.$id=>'Ticket #'.$id
);

if(isset($_SESSION['uID']) && isset($_REQUEST['categ'])){
	$navArray['edit_dealer.php?id='.$id]=@$categ_titles[$categ];
}

// set dynamic fields
$variables=array();
$variables=set_variable($variables,'title',$title);
$variables=set_variable($variables,'content',$content);
$variables=set_variable($variables,'login_form',$login_form);
$variables=set_variable($variables,'nav_menu', nav_menu($navArray));

outputHTML($variables,$page_template);
?>
