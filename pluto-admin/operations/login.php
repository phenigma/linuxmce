<?
function login($output,$dbADO) {
/* @var $dbADO ADOConnection */
$out='';
$actionX = cleanString(@$_REQUEST['action']);

$loginFormBig= '
	<h1 align="center">Welcome to Pluto Admin.  Please log in:</h1>
	<div align="center"></div>	
	<form name="form1" id="form1" method="post" action="'.$_SERVER['PHP_SELF'].'">
	<input type="hidden" name="section" value="login">
	  <table border="0" align="center" cellpadding="5" cellspacing="0">
	    <tr> 
	      <td>Username</td>
	      <td>
	      	<input type="text" name="username" value="'.@$_POST['username'].'" /></td>
	    </tr>
	    <tr> 
	      <td>Password</td>
	      <td><input type="password" name="password" value="" /></td>
	    </tr>
	    <tr>
	      <td colspan="2" align="center"><input type="submit" name="submitX" value="Log-in" /></td>
	    </tr>
	    <tr>
	    &nbsp;&nbsp;
	    </tr>
	    <tr>
	      <td colspan="2" align="center">If you just installed Pluto, your username and password will be the same as the one you created on plutohome.com.</td>
	    </tr>
	    <tr>
	      <td colspan="2" align="center"><a href="/support/index.php?section=document&docID=35" target="_top">Help with the Pluto Admin site</a></td>
	    </tr>
	    <tr>
	      <td colspan="2" align="center">Learn about Pluto at <a href="http://www.plutohome.com" target="_top">www.plutohome.com</a></td>
	    </tr>
	  </table>
	</form>
	<p align="center">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</p>	
	';


if (isset($_POST['submitX'])) {	
	
	$usernameForm = cleanString($_POST['username']);
	$passwordForm = cleanString($_POST['password']);
	
	
	$messages = '';
	
	if ($usernameForm=='')  {
		$messages.='Please enter your username';
	}
	
	if ($_POST['submitX']=='Log-in')
	{
		if ($passwordForm == '') {
			$messages.='Please enter your password';
		} else {
						
				$passwordForm = md5($passwordForm);
				
				if ($messages==='')	{
							/* @var $res_users ADORecordSet */
							$res_users = $dbADO->Execute("select * from Users WHERE UserName = ? and Password = ?",array($usernameForm,$passwordForm));
							if (!$res_users || $res_users->RecordCount()==0) {									
									$messages.='Invalid username or password, try again';
									header("Location: index.php?section=login&error=User not found or wrong password");
									exit(0);
							} else {
								
								$row_users = $res_users->FetchRow();
								
								$_SESSION['userID'] = (int)$row_users['PK_Users'];
								$_SESSION['userLoggedIn'] = true;
								
								
								
								$_SESSION['hh_username'] = $row_users['FirstName'].' '.$row_users['LastName'];
								$_SESSION['username'] = $usernameForm;
								
								
								$query_installation = "SELECT * FROM Installation_Users WHERE FK_Users = ?";
								$res_installations = $dbADO->Execute($query_installation,array($_SESSION['userID']));
								
								$installations=array();
								while (!$res_installations->EOF) {
									$installations[]=$res_installations->fields['FK_Installation'];
									$res_installations->MoveNext();
								}
								
								if ($installations===array()) {
									//the user has no installation!!!
								} else {
									$_SESSION['installationIDs'] = $installations;			
									$_SESSION['installationID'] = $installations[0];
								}
								
								header("Location: index.php?section=userHome");
								exit(0);
								
							}
				} else {
					$out.=$messages;
					$_SESSION['password'] = null;
					$_SESSION['userLoggedIn'] = false;
					header("Location: index.php?section=login&error=$messages");
				}
			
		}
				
	}
} elseif ($actionX=='logout') {	
	$_SESSION = array();
	session_destroy();
	$out.='Thank you for your visit!';
	
	$scriptInHead='
		<script>
			if (parent.frames[\'treeframe\']) {
				parent.frames[\'treeframe\'].location.href=\'index.php?section=leftMenu&rightSection=login\';
			}		
		</script>
	';
	$output->setScriptInHead($scriptInHead);
} elseif (@$_SESSION['userLoggedIn']==false) {
	$output->setScriptInBody("onload=\"document.form1.username.focus();\"");
	$out.=$loginFormBig;
} else {
	header("Location: index.php?section=userHome");
	exit(0);
}
	


$output->setScriptCalendar('null');
$output->setBody($out);
$output->setTitle(APPLICATION_NAME);			
$output->output();  		
}
?>