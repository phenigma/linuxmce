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
$categ=(int)@$_REQUEST['categ'];
$id=(int)@$_REQUEST['id'];

$page_template=implode('',file('templates/template.html'));
$content=edit_dealer($id,$categ_titles,$conn);
$login_form=login_header($conn);

$navArray=array(
	'index.php'=>'Home',
	'myaccount.php'=>'My account',
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
