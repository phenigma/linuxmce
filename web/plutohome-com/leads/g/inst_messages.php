<?
require_once('g/session_start.php');

$messages =& $_SESSION['messages'];

if (!isset($_SESSION['messages'])) $messages = new Messages(); // can't do =& here, lose & to $_SESSION['messages'] ?
elseif (is_a($_SESSION['messages'],'__PHP_Incomplete_Class'))
{
	exit('Error: "Messages" class declared after session_start()');
}
elseif (!isset_object($_SESSION['messages'],'Messages')) $messages = new Messages(); // can't do =& here, lose & to $_SESSION['messages'] ?