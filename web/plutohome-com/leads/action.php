<?
require_once('g/post_refresh.php');
require_once('leads/globals.php');
require_once('g/inst_messages.php');
require_once('leads/db_connect.php');
require_once('g/form_standard.php');
require_once('dbt_my_corp/lead_actionhistory.php');
require_once('dbt_my_corp/lead_actionpending.php');

verify_login();

// TODO: Date form fields

if (!isset_to_int($_GET['id']) || !isset($_GET['type'])) redirect('actions.php?type=p');

$qs_lead = '';
if ($_GET['type'] === 'h')
{
	$can_delete = false;
	$redirect = 'actions.php?type=h';
	$tablename = 'ActionHistory';
	if ($_GET['id'] === 0)
	{
		$db_row =& new DBT_ActionHistory_Row();
	}
	elseif (!$db_row =& new DBT_ActionHistory_Row($_GET['id'])) redirect($redirect); 
	
	$fields = array(
		'FKID_Lead'=>'Lead',
		'FKID_Action'=>'Action',
		'FKID_Employee'=>'Employee',
		'StartDate'=>'Start Date (YYYY-MM-DD)', // TODO: static
		'DueDate'=>'Due Date (YYYY-MM-DD)', // TODO: static
		'CompleteDate'=>'Complete Date (YYYY-MM-DD)',
		'Comments'=>'Comments'
		);
}
elseif ($_GET['type'] === 'c')
{
	$can_delete = false;
	$redirect = 'actions.php?type=p';
	$tablename = 'ActionHistory';
	
	if (!$ap_row =& new DBT_ActionPending_Row($_GET['id'])) redirect($redirect);
	$db_row =& new DBT_ActionHistory_Row();
	
	$fields = array(
		'FKID_Lead'=>'Lead',
		'FKID_Action'=>'Action',
		'FKID_Employee'=>'Employee',
		'StartDate'=>'Start Date (YYYY-MM-DD)', // TODO: static
		'DueDate'=>'Due Date (YYYY-MM-DD)', // TODO: static
		'CompleteDate'=>'Complete Date (YYYY-MM-DD)',
		'Comments'=>'Comments'
		);
		
	$db_row->SET_CompleteDate($db_my->mysql_date(),false);
	$db_row->SET_Comments($ap_row->GET_Comments(),false);
	$db_row->SET_DueDate($ap_row->GET_DueDate(),false);
	$db_row->SET_FKID_Employee($ap_row->GET_FKID_Employee(),false);
	$db_row->SET_FKID_Action($ap_row->GET_FKID_Action(),false);
	$db_row->SET_FKID_Lead($ap_row->GET_FKID_Lead(),false);
	$db_row->SET_StartDate($ap_row->GET_StartDate(),false);
}
else // assume 'p'
{
	$can_delete = true;
	$_GET['type'] = 'p';
	$redirect = 'actions.php?type=p';
	$tablename = 'ActionPending';
	if ($_GET['id'] === 0)
	{
		$db_row =& new DBT_ActionPending_Row();
		if (isset_to_int($_GET['lead']))
		{
			$qs_lead = '&lead='.$_GET['lead'];
			$db_row->SET_FKID_Lead($_GET['lead']);
		}
	}
	elseif (!$db_row =& new DBT_ActionPending_Row($_GET['id'])) redirect($redirect); 
	
	$fields = array(
		'FKID_Lead'=>'Lead',
		'FKID_Action'=>'Action',
		'FKID_Employee'=>'Employee',
		'StartDate'=>'Start Date (YYYY-MM-DD)',
		'DueDate'=>'Due Date (YYYY-MM-DD)',
		'Comments'=>'Comments'
		);
}

$form =& new Form_Standard($db_row,$fields,$can_delete);

if (isset_array($_POST[$tablename]) && came_from_this_page())
{
	$messages->pass_in($form->process());
	if ($messages->noErrors())
	{
		if (!$form->cancelled())
		{
			if ($_GET['type']==='c')
			{
				if (!$ap_row->delete()) $messages->pass_in($ap_row->P_get_messages());
			}
			elseif ($_GET['type']==='p' && !$form->deleted_db_row())
			{
				require_once('dbt_my_corp/lead_lead.php');
				require_once('dbt_my_corp/lead_status.php');
				
				$lead_row =& new DBT_Lead_Row($db_row->GET_FKID_Lead());
				$lead_row->SET_FKID_Status($DB_Status->ACTIVE_ACTIONREQUIRED);
				$lead_row->update();
			}
		}
		redirect($redirect);
	}
}

?><html><head></head><body><?
?><h2 align="center">Action</h2><?
?><h3 align="center"><? $messages->display(); ?></h3><?

$form->display($_SERVER['PHP_SELF'].'?type='.$_GET['type'].'&id='.$_GET['id'].$qs_lead,'Update',true);

?></body></html>