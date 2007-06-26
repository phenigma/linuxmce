<?
session_name('FIIRE');
session_start();

include('lib/config.php');
include('lib/functions.php');
include('lib/init.php');

$id=(int)@$_REQUEST['id'];
$data=getFields('dealers','WHERE id='.$id,$conn);
if(count($data)!=0){
	$content=get_dealer_comments($id,$conn);
	$title=$data[0]['title'];
}else{
	$content='Invalid ID';
	$title='ERROR';
}

$page_template=implode('',file('templates/template.html'));

$login_form=login_header($conn);
$navArray=array(
	'index.php'=>'Home',
	'dealer.php'=>'Dealer',
	'view-dealer.php'=>$title
);

// set dynamic fields
$variables=array();
$variables=set_variable($variables,'title','FIIRE :: '.$title);
$variables=set_variable($variables,'content',$content);
$variables=set_variable($variables,'login_form',$login_form);
$variables=set_variable($variables,'nav_menu', nav_menu($navArray));

outputHTML($variables,$page_template);
?>
