<?
require_once('g/post_refresh.php');
require_once('leads/globals.php');
require_once('g/inst_messages.php');
require_once('leads/db_connect.php');
require_once('g/form_standard.php');

require_once('dbt_my_corp/lead_lead.php');

verify_login();

if (!isset_to_int($_GET['id'])) redirect('leads.php');

if ($_GET['id'] === 0)
{
	$db_row =& new DBT_Lead_Row();	
}
elseif (!$db_row =& new DBT_Lead_Row($_GET['id'])) redirect('leads.php'); 

$fields = array(
	'Name'=>'Name',
	'FKID_LeadType'=>'Type',
	'FKID_Status'=>'Status',
	'Email'=>'Email',
	'Phone'=>'Phone',
	'Fax'=>'Fax',
	'website'=>'Website',
	'Address1'=>'Address1',
	'Address2'=>'Address2',
	'City'=>'City',
	'State'=>'State',
	'Postal_Code'=>'Postal Code',
	'Country'=>'Country',
	'Comments'=>'Comments',
	);
$form =& new Form_Standard($db_row,$fields,true);

if (isset_array($_POST['Lead']) && came_from_this_page())
{
	$messages->pass_in($form->process());
	if ($messages->noErrors()) redirect('leads.php');
}

?><html><head></head><body><?
?><h2 align="center">Lead</h2><?
?><h3 align="center"><? $messages->display(); ?></h3><?

$form->display($_SERVER['PHP_SELF'].'?id='.$_GET['id'],'Update',true);

?></body></html>