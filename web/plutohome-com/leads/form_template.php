<?
require_once('g/post_refresh.php');
require_once('leads/globals.php');
require_once('g/inst_messages.php');
require_once('leads/db_connect.php');
require_once('g/form_standard.php');

verify_login();

$form =& new Form_Standard();

if (isset_array($_POST['...']) && came_from_this_page())
{
	if ($messages->noErrors())
	{
		$messages->pass_in($form->process());
	}
}



?><html><head></head><body><?

?><h3 align="center"><? $messages->display(); ?></h3><?

$form->display();

?></body></html>