<?
function login($output,$dbADO,$conn) {
	global $forumHost;
/* @var $dbADO ADOConnection */
global $checkMasterUserUrl;
$return=@$_REQUEST['return'];

if(isset($_SESSION['userIsLogged']) && $_SESSION['userIsLogged']=="yes"){
	// the user is already logged
	header("Location: index.php?section=myPluto");
}

$out='';
if (isset($_POST['username']) && $_POST['username']!='') {	
	$username = $_POST['username'];
	$password = $_POST['password'];
	$autologin = @$_POST['autologin'];
	$_SESSION['username']=$username;
	$messages = '';
	
	if ($username=='')  {
		$messages.='Please enter your username';
	}
	
	if (isset($_POST['username']) && $_POST['username']!='')
	{
		if ($password == '') {
			$messages.='Please enter your password';
		} else {
				$passwordMD5 = md5($password);
				// check master users database
				$isMasterUsers=checkMasterUsers($username, $password,$checkMasterUserUrl,'&FirstAccount=&Email=&PlutoId=&Pin=');
				if(!$isMasterUsers[0])
					$messages.='Invalid username or password, try again!!!</a>';
		}
		if ($messages=='') 
		{
			parse_str($isMasterUsers[1]);
			$_SESSION['userID'] = $MasterUsersID;
			$_SESSION['PlutoId'] = $PlutoId;
			$_SESSION['Pin'] = $Pin;
			$_SESSION['username'] = $username;
			$_SESSION['userIsLogged']="yes";
			$_SESSION['categ']=$FirstAccount;
			$_SESSION['Email']=$Email;
			$_SESSION['userLoggedIn'] = true;
			
			$query_installation = "SELECT * FROM Installation_Users WHERE FK_Users = ?";
			$res_installations = $dbADO->Execute($query_installation,$MasterUsersID);
			$installations=array();
			while ($row=$res_installations->FetchRow()) {
				$installations[]=$row['FK_Installation'];
			}

			$_SESSION['installationIDs'] = $installations;
			$_SESSION['installationID'] = $installations[0];
			
			$res=mysql_query("SELECT * FROM Users WHERE FK_MasterUsers='$MasterUsersID'",$conn);
			if(mysql_num_rows($res)!=0){
				$res=mysql_fetch_object($res);
				$_SESSION['extPassword']=$res->ExtPassword;
			}
			if($autologin==1){
				$_SESSION['setAutoCookie']=1;
				$_SESSION['password']=$password;
			}
			if($return!=''){
				header("Location: support/mantis/login.php?username=".$_SESSION['username']."&password=".$_SESSION['extPassword']."&login=1&return=".$return);
				exit();
			}
			header("Location: index.php?section=myPluto");
			exit();
		} else {
			$out.=$messages;
			header("Location: index.php?section=login&error=$messages");
			exit();
		}
	}
}
	$out .= '
	<script>
	var setFocus=0
	</script>
	<form name="login" id="login" method="post" action="index.php">
	<input type="hidden" name="section" value="login">
	<input type="hidden" name="return" value="'.$return.'">
		<table width="100%">
     		<tr>
				<td align="left" colspan="2" class="insidetable"><img src="images/titles/login.gif" border="0"> <span class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</span></td>
			</tr>
     		<tr>
				<td align="left" colspan="2" class="insidetable2">&nbsp;</td>
			</tr>	
	    <tr class="insidetable"> 
	      <td width="30%" align="left"><B>Username</B></td>
	      <td>&nbsp;<input type="text" class="textfield" name="username" value="'.@$_SESSION['username'].'" /></td></tr>
	    <tr class="insidetable"> 
	      <td align="left"><B>Password</B></td>
	      <td>&nbsp;<input type="password" class="textfield" name="password" value="" onkeypress="assignVal(1)"/> <a href="index.php?section=forgotPassword">Forgot password ?</a></td>
	    </tr>
        <tr class="insidetable">
        	<td height="20" align="left"><B>Remember my password</B></td>
            <td align="left"><input type="checkbox" name="autologin" value="1"></td>
		</tr>	
	    <tr class="insidetable">
	      <td colspan="2" align="left">* You must enable cookies and javascript in order to login.<br><input type="image" name="submitX" src="images/buttons/buton_login.gif" /></td>
	    </tr>
		<tr class="insidetable">
			<td colspan="2"><br><div align="left">If you already registered on <a href="http://www.plutovip.com">Plutovip.com</a>, your username and password are linked and will work on this site too.
						<br> 
						<br>
						Not already registered? <a href="index.php?section=register">Click here</a> to register.<br><br></div>
				We don’t require any confidential information, but we need you to pick a user name and password so our wizards can help you build and download a Pluto system.  If you’re a developer, this will also be used to check-in code and create new devices.  You can use the same username/password on <a href="http://www.plutovip.com">Plutovip.com</a>.
<br><br>
 

If you ever have a question, click "Live Support" button from top left corner to get instant assistance.
			</td>
		</tr>
	  </table>
	</form>
	<script>
		function assignVal(val)
		{
			setFocus=val;
		}
			
		var frmvalidator = new formValidator("login");
 		frmvalidator.addValidation("username","req","Please enter an username.");			
	 	frmvalidator.addValidation("password","req","Please enter your password.");			
	</script>
';




	$output->setNavigationMenu(array("Login"=>"index.php?section=login"));
	$output->setScriptInBody("onload=\"if(setFocus==0);document.login.username.focus();\"");
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}
?>