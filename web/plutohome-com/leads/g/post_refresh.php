<?
// this file provides a seamless mechanism to refresh pages with $_POST data.

require_once('g/session_start.php');
/*
if ($_SERVER['REQUEST_METHOD']==='POST')
{
	if (isset($_SERVER['HTTP_HOST']))
	{
		$_SESSION['POST'] = array();
		foreach ($_POST as $key=>$val)
		{
			$_SESSION['POST'][$key] = $val;
		}
		if(!isset($_SERVER['REQUEST_URI']))
		{
			$_SERVER['REQUEST_URI'] = $_SERVER['URL'] . ((isset($_SERVER['QUERY_STRING']) && $_SERVER['QUERY_STRING'] != '') ? "?$_SERVER[QUERY_STRING]" : '');
		}
		$location = 'http'.((isset($_SERVER['SERVER_PORT']) && $_SERVER['SERVER_PORT']==443) ? 's' : '')."://$_SERVER[HTTP_HOST]$_SERVER[REQUEST_URI]";
		header("Location: $location");
		exit();
	}
}
elseif (isset($_SESSION['POST']))
{
	foreach ($_SESSION['POST'] as $key=>$val)
	{
		$_POST[$key] = $val;
		$_REQUEST[$key] = $val;
	}
	$_SESSION['POST'] = null; // for 4.2 compatibility // was unset($_SESSION['POST']);
	$_SERVER['REQUEST_METHOD'] = 'POST';
}
*/
