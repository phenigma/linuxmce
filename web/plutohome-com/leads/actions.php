<?
require_once('g/post_refresh.php');
require_once('leads/globals.php');
require_once('g/inst_messages.php');
require_once('leads/db_connect.php');

require_once('dbt_my_corp/lead_lead.php');
require_once('dbt_my_corp/lead_actionpending.php');
require_once('dbt_my_corp/lead_actionhistory.php');

verify_login();

$where = '';
if (isset_to_int($_GET['lead'])) $where .= '(FKID_Lead='.$_GET['lead'].')';

if (!isset($_GET['type']) || ($_GET['type']!=='p' && $_GET['type']!=='h')) $_GET['type'] = 'a';

if ($_GET['type'] === 'p' || $_GET['type'] === 'a')
{
	if ($_GET['type'] === 'p') $where = where_pending_action($where);
	$ap_keys = $DB_ActionPending_Table->P_load($where);
}
if ($_GET['type'] === 'h' || $_GET['type'] === 'a')
{
	$ah_keys = $DB_ActionHistory_Table->P_load($where);
}

?><html><head></head><body>

<h2 align="center"><a href="default.php">Home</a> - <a href="actions.php" title="view all actions">Actions</a></h2>
<h3 align="center"><? $messages->display(); ?></h3>

<?
$qs_lead = '';
if (isset_int($_GET['lead']))
{
	if ($l_row =& new DBT_Lead_Row($_GET['lead']))
	{
		$qs_lead = '&lead='.$_GET['lead'];
		?>
		<table border="1" cellpadding="5" cellspacing="0" align="center">
		<tr><th colspan="2"><a href="lead.php?id=<?= $_GET['lead'] ?>" title="click to edit">Lead</a></th></tr>
		<tr><th>Name</th><td><?= $l_row->GET_Name() ?></td></tr>
		<tr><th>Address</th><td><?= $l_row->GET_Full_Address() ?></td></tr>
		<tr><th>Comments</th><td><?= $l_row->GET_Comments() ?></td></tr>
		</table><br />
		<?
	}
}

if (isset($ap_keys))
{
	?><div align="center"><a href="action.php?type=p&id=0<?= $qs_lead ?>">New Action</a></div><br />
	<table border="1" cellpadding="5" cellspacing="0" align="center">
	<tr>
		<th colspan="8"><a href="actions.php?type=p" title="view all pending actions">Pending Actions</a></th>
	</tr>
	<tr>
		<th>Lead</th>
		<th>Action</th>
		<th>Employee</th>
		<th>Start Date</th>
		<th>Due Date</th>
		<th>Comments</th>
		<th>&nbsp;</th>
		<th>&nbsp;</th>
	</tr>
	<?
	foreach ($ap_keys as $ap_key)
	{
		$ap_row =& new DBT_ActionPending_Row($ap_key);
		?><tr><?
		?><td><a href="actions.php?lead=<?= $ap_row->GET_FKID_Lead() ?>"><?= $ap_row->GET_FKID_Lead(true) ?>&nbsp;</a></td><?
		?><td><?= $ap_row->GET_FKID_Action(true) ?>&nbsp;</td><?
		?><td><?= $ap_row->GET_FKID_Employee(true) ?>&nbsp;</td><?
		?><td><?= $ap_row->GET_StartDate(true) ?>&nbsp;</td><?
		?><td><?= $ap_row->GET_DueDate(true) ?>&nbsp;</td><?
		?><td><?= str_replace("\n",'<br />',$ap_row->GET_Comments(true)) ?>&nbsp;</td><?
		?><td><a href="action.php?type=c&id=<?= $ap_key ?>">Complete</a></td><?
		?><td><a href="action.php?type=p&id=<?= $ap_key ?>">Edit</a></td><?
		?></tr><?
	}
	?></table><br /><?
}

if (isset($ah_keys))
{
	?><table border="1" cellpadding="5" cellspacing="0" align="center">
	<tr>
		<th colspan="8"><a href="actions.php?type=h" title="view all action history">Action History</a></th>
	</tr>
	<tr>
		<th>Lead</th>
		<th>Action</th>
		<th>Employee</th>
		<th>Start Date</th>
		<th>Due Date</th>
		<th>Complete Date</th>
		<th>Comments</th>
		<th>&nbsp;</th>
	</tr>
	<?
	foreach ($ah_keys as $ah_key)
	{
		$ah_row =& new DBT_ActionHistory_Row($ah_key);
		?><tr><?
		?><td><a href="actions.php?lead=<?= $ah_row->GET_FKID_Lead() ?>"><?= $ah_row->GET_FKID_Lead(true) ?>&nbsp;</a></td><?
		?><td><?= $ah_row->GET_FKID_Action(true) ?>&nbsp;</td><?
		?><td><?= $ah_row->GET_FKID_Employee(true) ?>&nbsp;</td><?
		?><td><?= $ah_row->GET_StartDate(true) ?>&nbsp;</td><?
		?><td><?= $ah_row->GET_DueDate(true) ?>&nbsp;</td><?
		?><td><?= $ah_row->GET_CompleteDate(true) ?>&nbsp;</td><?
		?><td><?= str_replace("\n",'<br />',$ah_row->GET_Comments(true)) ?>&nbsp;</td><?
		?><td><a href="action.php?type=h&id=<?= $ah_key ?>">Edit</a></td><?
		?></tr><?
	}
	?></table><?
}


?></body></html>