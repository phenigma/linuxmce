<?
require_once('g/post_refresh.php');
require_once('leads/globals.php');
require_once('g/inst_messages.php');
require_once('leads/db_connect.php');

verify_login();

?><html><head></head><body>

<h2 align="center">Leads</h2>

<h3 align="center"><? $messages->display(); ?></h3>

<p><a href="leads.php">Leads (<?= $db_my->select_count('Lead') ?>)</a></p> 
<p><a href="actions.php?type=p">Pending Actions (<?= $db_my->select_count('ActionPending',where_pending_action()) ?>)</a></p>
<p><a href="actions.php?type=h">Action History</a></p>

</body></html>