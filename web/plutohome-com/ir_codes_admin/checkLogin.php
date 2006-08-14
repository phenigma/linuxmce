<?php
session_name('Pluto_master_sql_cvs');
session_start();

if(isset($_REQUEST['logout'])){
	unset($_SESSION['userIsLogged']);
}

if (@$_SESSION['userIsLogged']!="yes"){
	die(do_login());
}
if(@$_SESSION['sqlCvsAdmin']!=1){
	unset($_SESSION['userIsLogged']);
	die('Access denied!');
}

function do_login(){
	if(isset($_POST['submitX'])){
		return process_login();
	}else{
		return login_form();
	}
}

function login_form(){
	$out='
	<h3 align="center">Master sqlCVS login</h3>
	<form action="ir_codes_admin.php" method="post" name="loginForm">
		<table height="80" cellSpacing="0" cellPadding="5" border="0" align="center">
			<tr>
				<td height="20"><span class="insidetable">Username:&nbsp;</span></td>
				<td><input name="username" class="long"></td>
			</tr>
			<tr>
				<td height="20"><span class="insidetable">Password:&nbsp;</span></td>
				<td><input type="password" name="password" class="long"></td>
			</tr>
			<tr>
				<td align="middle" colSpan="2"><input type="submit" value="Log-In" name="submitX" class="long"></td>
			</tr>
		</table>
	</form>';
	return $out;
}

function process_login(){
	global $checkMasterUserUrl;
	$username = $_POST['username'];
	$password = $_POST['password'];
	if (isset($_POST['username']) && $_POST['username']!=''){
		// check master users database
		$isMasterUsers=checkMasterUsers($username, $password,$checkMasterUserUrl,'&FirstAccount=&Email=&PlutoId=&Pin=&sqlCvsAdmin=');
		if(!$isMasterUsers[0]){
			return 'Invalid username or password, <a href="ir_codes_admin.php">try again</a>.';
		}else{
			$_SESSION['userIsLogged']="yes";
			parse_str($isMasterUsers[1]);
			$_SESSION['sqlCvsAdmin']=$sqlCvsAdmin;
			$_SESSION['Username']=$username;
			header('Location: ir_codes_admin.php');
			exit();
		}
	}else{
		return 'Please enter your username.';
	}
}

?>