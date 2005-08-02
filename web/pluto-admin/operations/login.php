<?
function login($output,$dbADO) {
/* @var $dbADO ADOConnection */
$out='';
$actionX = cleanString(@$_REQUEST['action']);

$loginFormBig= '
	<h1 align="center">'.((isset($_REQUEST['msg']))?$_REQUEST['msg']:'Welcome to Pluto Admin.  Please log in:').'</h1>
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
	      <td colspan="2" align="center"><input type="submit" class="button" name="submitX" value="Log-in" /></td>
	    </tr>
	    <tr>
	    &nbsp;&nbsp;
	    </tr>
	    <tr>
	      <td colspan="2" align="center">If you just installed Pluto, your username and password will be the same as the one you created on plutohome.com.</td>
	    </tr>
	    <tr>
	      <td colspan="2" align="center"><h1>IMPORTANT:</h1> You need to enable popups to use this site.  Some web browsers block them, and you need to specifically
	    add the IP address or domain name you use to access PlutoAdmin, such as 192.168.80.1, to the "allowed list".  If you\'re using the browser built into the
	      media director we did this for you already.  To allow popups for this site in <b>Firefox</b>, click Tools, Options, Web Features.  
	      If "Block Popup Windows" is checked, click "Allowed Sites".  In <b>Internet Explorer</b>, choose Tools, Pop-up Blocker, Popup-Blocker Settings.</td>
	    </tr>
	    <tr>
	      <td colspan="2" align="center"><a href="/support/index.php?section=document&docID=35" target="_top">Help with the Pluto Admin site</a></td>
	    </tr>
	    <tr>
	      <td colspan="2" align="center">Learn about Pluto at <a href="http://www.plutohome.com" target="_top">www.plutohome.com</a><br><br>Download <a href="index.php?section=orbitersWin">Orbiter Win Installer</a></td>
	    </tr>
	    <tr>
	      <td colspan="2" align="center">Version: <=version=></td>
	    </tr>
	  </table>
	</form>
	<p align="center" class="err"><B>'.(isset($_GET['error'])?strip_tags(stripslashes($_GET['error'])):'').'</B></p>	
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
			header("Location: index.php?section=login&error=Please enter your password");
			exit();			
		} else {
				$_SESSION['password']=$passwordForm;		
				$passwordForm = md5($passwordForm);
				
				if ($messages==='')	{
							/* @var $res_users ADORecordSet */
							$res_users = $dbADO->Execute("select * from Users WHERE UserName = ? and Password = ?",array($usernameForm,$passwordForm));
							if (!$res_users || $res_users->RecordCount()==0) {	
								if($usernameForm=='remote'){
					
									// remote acces: if you allowed remote acces in Wizard/Security/Outside Access the website can be accesed 
									// using username remote and the password you setup in Outside access page
									
									exec('cat '.$GLOBALS['pluto.conf'].' | grep -v -E "^#|^$" ',$retArray);	
									foreach ($retArray as $comf){
										parse_str($comf);
									}
									if(isset($remote) && $remote==$_SESSION['password']){
									// search for an user who can modify installation
										$resRemote=$dbADO->Execute('
											SELECT Users.* 
											FROM Users 
											INNER JOIN Installation_Users ON FK_Users=PK_Users
											WHERE userCanModifyInstallation=1 LIMIT 0,1');
										if($resRemote->RecordCount()>0){
											$rowRemote=$resRemote->FetchRow();
											
											$query_installation = "SELECT * FROM Installation_Users WHERE FK_Users = ?";
											$res_installations = $dbADO->Execute($query_installation,array((int)$rowRemote['PK_Users']));
											if($res_installations->RecordCount()==0){
												header('Location: index.php?section=login&error=You don\'t have any installation.');
												exit();
											}
				
											$_SESSION['userID'] = (int)$rowRemote['PK_Users'];
											$_SESSION['userLoggedIn'] = true;
												
											$_SESSION['username'] = $rowRemote['UserName'];
												
												
											$installations=array();
											while ($rowInstallations=$res_installations->FetchRow()) {
												$installations[]=$rowInstallations['FK_Installation'];
											}
											if ($installations===array()) {
												//the user has no installation!!!
												$messages="You don't have any installation.";
												header("Location: index.php?section=login&error=urlencode($messages)");
												exit();
											} else {
												$_SESSION['installationIDs'] = $installations;			
												$_SESSION['installationID'] = $installations[0];
											}	
											
											header("Location: index.php?section=userHome");
											exit();						
										}else{
											$_SESSION['password'] = null;
											$_SESSION['userLoggedIn'] = false;
											header("Location: index.php?section=login&error=Remote acces failed: there is no user who can modify installation");
											exit();
										}
									}
								}
															
								$messages.='Invalid username or password, try again';
								header("Location: index.php?section=login&error=User not found or wrong password");
								exit(0);
							} else {
								
								$row_users = $res_users->FetchRow();
								
								$query_installation = "SELECT * FROM Installation_Users WHERE FK_Users = ?";
								$res_installations = $dbADO->Execute($query_installation,array((int)$row_users['PK_Users']));
								if($res_installations->RecordCount()==0){
									header('Location: index.php?section=login&error=You don\'t have any installation.');
									exit();
								}

								$_SESSION['userID'] = (int)$row_users['PK_Users'];
								$_SESSION['userLoggedIn'] = true;
								
								$_SESSION['hh_username'] = $row_users['FirstName'].' '.$row_users['LastName'];
								$_SESSION['username'] = $usernameForm;
								
								
								$installations=array();
								while (!$res_installations->EOF) {
									$installations[]=$res_installations->fields['FK_Installation'];
									$res_installations->MoveNext();
								}
								
								if ($installations===array()) {
									//the user has no installation!!!
									$messages="You don't have any installation.";
									header("Location: index.php?section=login&error=urlencode($messages)");
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
	
	$scriptInHead='
		<script>
			parent.frames[\'basefrm\'].location.href="index.php?section=login&msg=Thank you for your visit!"
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
