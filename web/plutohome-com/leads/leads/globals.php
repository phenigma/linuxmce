<?
require_once('g/globals.php');

define('APPLICATION_PATH',running_on_windows() ? 'pluto/leads' : 'leads');

function where_pending_action($where = '')
{
	global $db_my;
	if ($where !== '') $where = "($where) AND ";
	return $where.'(StartDate IS NULL or StartDate<=\''.$db_my->mysql_date().'\')';
}

function verify_login()
{
	if (!isset($_SESSION['logged_in'])) redirect('login.php');
}
?>