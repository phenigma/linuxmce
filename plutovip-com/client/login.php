<?
function login($output) {
	global $checkMasterUserUrl;
	$out='';
	session_start();
	$usernameForm = $_POST['username'];
	$username=mysql_escape_string($usernameForm);
	$passwordForm = $_POST['password'];
	$password=mysql_escape_string($passwordForm);
	//	$householdForm = $_POST['household'];
	//	$household=$dbADO->qstr($householdForm);

	$messages = '';

	if ($usernameForm=='')  {
		$messages.='<br>Please enter your username<br><a href="index.php"> Go back</a>';
	}

	if ($passwordForm == '') {
		$messages.='<br>Please enter your password <br><a href="index.php"> Go back</a>';
	} else {

		$passwordForm = md5($passwordForm);

		$isMasterUsers=checkMasterUsers($username, $passwordForm,$checkMasterUserUrl,'&FirstAccount=&Email=&PlutoId=&Pin=');
		if(!$isMasterUsers[0])
			$messages.='<br>Invalid username or password, try again!!! <br><a href="index.php"> Go back</a>';
	}

	if($messages==''){
		parse_str($isMasterUsers[1]);
		$_SESSION['userID'] = $MasterUsersID;
		$_SESSION['PlutoId'] = $PlutoId;
		$_SESSION['Pin'] = $Pin;
		$_SESSION['username'] = $usernameForm;
		$_SESSION['userIsLogged']="yes";
		$_SESSION['categ']=$FirstAccount;
		$_SESSION['Email']=$Email;
		$_SESSION['Password']=$passwordForm;
		if($_SESSION['categ']==0)
			header("Location: index.php?section=settings");
		else
			header("Location: index.php?section=settings_b");
	}else
	{
		$out.=$messages;
	}

	$output->setScriptCalendar('null');
	$output->setScriptTRColor('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}
?>