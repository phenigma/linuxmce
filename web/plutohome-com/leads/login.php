<?
require_once('g/post_refresh.php');
require_once('leads/globals.php');
require_once('g/inst_messages.php');
require_once('leads/db_connect.php');

if (isset($_POST['login']))
{
	if ($_POST['user']==='1control' && $_POST['pwd']==='pluto')
	{
		$_SESSION['logged_in'] = true;
		redirect('default.php');
	}
}

?><html><head></head><body>

<form action="login.php" method="POST">
Username: <input type="text" name="user" /><br />
Password: <input type="password" name="pwd" />
<input type="submit" name="login" value="Login" />
</form>

</body></html>