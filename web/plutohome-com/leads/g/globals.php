<?
require_once('g/session_start.php');
require_once('g/fn_globals.php');

$_SESSION['last_page'] = isset($_SESSION['this_page']) ? $_SESSION['this_page'] : '';
$_SESSION['this_page'] = full_host().$_SERVER['PHP_SELF'];
//$_SESSION['this_page'] = "http://$_SERVER[HTTP_HOST]$_SERVER[PHP_SELF]";

// NOTE: following line is fix for multi-page debugging (with my 1st page redirect on default.php)
// if (isset($_GET['start_debug']) && $_SESSION['last_page']=='') refresh("?$_SERVER[QUERY_STRING]");