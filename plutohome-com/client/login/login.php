<?
function login($output,$dbADO) {
	global $forumHost;
/* @var $dbADO ADOConnection */
global $checkMasterUserUrl;

if(isset($_SESSION['userIsLogged']) && $_SESSION['userIsLogged']=="yes"){
	// the user is already logged
	header("Location: index.php?section=myPluto");
}

$out='';
if (isset($_POST['submitX'])) {	
	
	$username = $_POST['username'];
	$password = $_POST['password'];
	$autologin = @$_POST['autologin'];
	$_SESSION['username']=$username;
	$messages = '';
	
	if ($username=='')  {
		$messages.='Please enter your username';
	}
	
	if (isset($_POST['submitX']))
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
			
			$res=mysql_query("SELECT * FROM Users WHERE FK_MasterUsers='$MasterUsersID'");
			if(mysql_num_rows($res)!=0){
				$res=mysql_fetch_object($res);
				$_SESSION['extPassword']=$res->ExtPassword;
			}
			if($autologin==1){
				$_SESSION['setAutoCookie']=1;
				$_SESSION['password']=$password;
			}
			header("Location: index.php?section=myPluto");
		} else {
			$out.=$messages;
			header("Location: index.php?section=clientLoginForm&error=$messages");
		}
	}
}
	$out .= '<table width="100%" border="0" cellpadding="0" cellspacing="0" class="maintable"><td  valign="top" align="center" class="insidetable">
		
	
	<b>Log - In</b>
	<div align="center"></div>
	<form name="login" id="login" method="post" action="index.php">
	<input type="hidden" name="section" value="clientLoginForm">
		<span class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</span>
	  <table border="0" align="center" cellpadding="5" cellspacing="0" class="insidetable">
	    <tr> 
	      <td width="50%" align="right">Username</td>
	      <td>&nbsp;<input type="text" name="username" value="'.@$_SESSION['username'].'" /></td></tr>
	    <tr> 
	      <td align="right">Password</td>
	      <td>&nbsp;<input type="password" name="password" value="" /></td>
	    </tr>
        <tr>
        	<td height="20" align="right">Remember my password</td>
            <td align="left"><input type="checkbox" name="autologin" value="1"></td>
		</tr>	
	    <tr>
	      <td colspan="2" align="center"><input type="submit" name="submitX" value="Log-in" /></td>
	    </tr>
		<tr>
			<td colspan="2"><div align="center">If you already registered on <a href="http://www.plutovip.com">Plutovip.com</a>, your username and password are linked and will work on this site too.
						<br> 
						<br>
						Not already registered? <a href="index.php?section=register">Click here</a> to register.<br><br></div>
				We don’t require any confidential information, but we need you to pick a user name and password so our wizards can help you build and download a Pluto system.  If you’re a developer, this will also be used to check-in code and create new devices.  You can use the same username/password on <a href="http://www.plutovip.com">Plutovip.com</a>.
<br><br>
 

If you ever have a question, click Live chat to get instant support.
</td>
		</tr>
	  </table>
	</form>
	<script>
		 	var frmvalidator = new formValidator("login");
 			frmvalidator.addValidation("username","req","Please enter an username.");			
	 		frmvalidator.addValidation("password","req","Please enter your password.");			
	</script>
	
	
	</td></table>';





	$output->setScriptInBody("onload=\"document.login.username.focus();\"");
	//$output->setLeftMenu($leftMenu);
	$output->setScriptCalendar('null');
	$output->setScriptTRColor('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}
?>