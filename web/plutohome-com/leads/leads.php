<?
require_once('g/post_refresh.php');
require_once('leads/globals.php');
require_once('g/inst_messages.php');
require_once('leads/db_connect.php');

require_once('dbt_my_corp/lead_lead.php');

verify_login();

$l_keys = $DB_Lead_Table->P_load();

?><html><head></head><body>

<h2 align="center"><a href="default.php">Home</a> - Leads</h2>
<div align="center"><a href="lead.php?id=0">New Lead</a></div>
<h3 align="center"><? $messages->display(); ?></h3>

<table border="1" cellpadding="5" cellspacing="0" align="center">
<tr>
	<th>Name</th>
	<th>City</th>
	<th>State</th>
	<th>Country</th>
	<th>Phone</th>
	<th>Email</th>
	<th>Website</th>
	<th>Comments</th>
	<th>Type</th>
	<th>Status</th>
	<th>&nbsp;</th>
</tr>

<?
foreach ($l_keys as $l_key)
{
	$l_row =& new DBT_Lead_Row($l_key);
	
	?><tr><?
	?><td><a href="lead.php?id=<?= $l_key ?>" title="click to edit"><?= $l_row->GET_Name() ?></a></td><?
	?><td><?= $l_row->GET_City() ?>&nbsp;</td><?
	?><td><?= $l_row->GET_State() ?>&nbsp;</td><?
	?><td><?= $l_row->GET_Country() ?>&nbsp;</td><?
	?><td><?= $l_row->GET_Phone() ?>&nbsp;</td><?
	?><td><a href="mailto:<?= $l_row->GET_Email() ?>"><?= $l_row->GET_Email() ?></a>&nbsp;</td><?
	?><td><a href="http://<?= $l_row->GET_website() ?>"><?= $l_row->GET_website() ?></a>&nbsp;</td><?
	?><td><?= str_replace("\n",'<br />',$l_row->GET_Comments()) ?>&nbsp;</td><?
	?><td><?= $l_row->GET_FKID_LeadType(true) ?></td><?
	?><td><?= $l_row->GET_FKID_Status(true) ?></td><?
	?><td><a href="actions.php?lead=<?= $l_key ?>">Actions</a></td><?
	?></tr><?
}
?>

</table></body></html>