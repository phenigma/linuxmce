<?
session_name('Support');
session_start();

include('lib/config.php');
include('lib/utils.php');
include('lib/init.php');
include('lib/site.php');

$id=(int)@$_REQUEST['id'];

$title='Support tickets - Edit ticket #'.$id;

// main content
$page_template=implode('',file('templates/template.html'));
$content=edit_incident($conn);

// set dynamic fields
$variables=array();
$variables=set_variable($variables,'title',$title);
$variables=set_variable($variables,'content',$content);

outputHTML($variables,$page_template);
?>
