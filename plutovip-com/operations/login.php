<?
function login($output,$dbADO) {
/* @var $dbADO ADOConnection */
$out='';
if (isset($_POST['submitX'])) {	
	
	$usernameForm = $_POST['username'];
	$username=$dbADO->qstr($usernameForm);
	$passwordForm = $_POST['password'];
	$password=$dbADO->qstr($passwordForm);
	$householdForm = $_POST['household'];
	$household=$dbADO->qstr($householdForm);
	
	$messages = '';
	
	if ($usernameForm=='')  {
		$messages.='Please enter your username';
	}
	
	if ($_POST['submitX']=='Log-in')
	{
		if ($passwordForm == '') {
			$messages.='Please enter your password';
		} else {
						
				$_SESSION['password'] = $passwordForm;
				$passwordForm = md5($passwordForm);
				$householdForm = strtolower($householdForm);
				
				$select = 'u.PKID_Users, u.isPowerUser, u.plutohomeAdminLevel';
				$from = 'Users AS u';
				$where = " u.UserName = ".$dbADO->qstr($usernameForm)." and u.Password = ".$dbADO->qstr($passwordForm);
				
				if ($householdForm=='') {
					$select.=', i.FKID_household, u.FKID_Installation_Main as PKID_Installation';
					$from.=' INNER JOIN Installation AS i ON u.FKID_Installation_Main=i.PKID_Installation';
					//$where.=" and u.isTopLevelUsername = 1 ";
				} elseif (is_numeric($householdForm) || substr($householdForm,0,5)==='pluto' && is_numeric(substr($householdForm,5))) {
					$select.=', i.FKID_household, i.PKID_Installation';
					$from.=' INNER JOIN Installation_Users AS iu ON iu.FKID_Users=u.PKID_Users';
					$from.=' INNER JOIN Installation AS i ON iu.FKID_Installation=i.PKID_Installation';
					$where.= " and i.PKID_Installation =  ".(is_numeric($householdForm) ? $dbADO->qstr($householdForm) : $dbADO->qstr(substr($householdForm,5)));					
				} else {
					$query = "select hu.FKID_household from Household where Username = ".$household;
					if ($resHH = $dbADO->GetRow($query)) {						
							$select.=', hu.FKID_household, u.FKID_Installation_Main as PKID_Installation';
							$from.=' INNER JOIN Household_Users AS hu ON u.PKID_Users=hu.FKID_Users';
							$where.= " and hu.FKID_household = ".$dbADO->qstr($resHH['PKID_household']);												
					} else {
						$messages.='Invalid username or password, try again';
					}
				}
				$select.=', h.Username';
				$from.=' INNER JOIN Household AS h ON FKID_Household=h.PKID_Household';
				
				if ($messages==='')	{
							/* @var $res_users ADORecordSet */
							$res_users = $dbADO->_Execute("select " . $select ." from ". $from . " WHERE " .$where);						
							if (!$res_users || $res_users->RecordCount()==0) {
									$messages.=$dbADO->ErrorMsg();
									$messages.='Invalid username or password, try again';
							} else {
								$installations=array();
								while (!$res_users->EOF) {
									if ($res_users->fields['isPowerUser']==0) {
										$messages.='Access Denied, you are not a power user.';
										break;
									}
									$installations[]=$res_users->fields['PKID_Installation'];
									$res_users->MoveNext();
								}
								$res_users->MoveFirst();
								$row_users=$res_users->FetchRow();
							}
				}
			
		}
		
		if ($messages==='') // TODO: User - Instalation - Household
		{
			$_SESSION['userID'] = (int)$row_users['PKID_Users'];
			$_SESSION['installationIDs'] = $installations;			
			$_SESSION['installationID'] = $installations[0];			
			$_SESSION['householdID'] = (int)$row_users['FKID_household'];
			$_SESSION['plutohomeAdminLevel'] =  (int)$row_users['plutohomeAdminLevel'];
			$_SESSION['hh_username'] = $row_users['Username'];
			$_SESSION['username'] = $usernameForm;
			if ($_SESSION['plutohomeAdminLevel']==3) {
				header("Location: index.php?section=userHome");
			} else {
				header("Location: index.php?section=adminHome");
			}
		} else {
			$out.=$messages;
			$_SESSION['password'] = null;
			header("Location: index.php?section=login&error=$messages");
		}
	}
}
	$out .= '
		
	
	<h1 align="center">Log - In</h1>
	<div align="center"></div>
	<form name="form1" id="form1" method="post" action="'.$_SERVER['PHP_SELF'].'">
	<input type="hidden" name="section" value="login">
	  <table border="0" align="center" cellpadding="5" cellspacing="0">
	    <tr> 
	      <td>Username</td>
	      <td>
	      	<input type="text" name="username" value="'.@$_POST['username'].'" />
	      	@ <input type="text" name="household" value="'.@$_POST['household'].'" />
	      	.plutohome.com</td>
	    </tr>
	    <tr> 
	      <td>Password</td>
	      <td><input type="password" name="password" value="" /></td>
	    </tr>
	    <tr>
	      <td colspan="2" align="center"><input type="submit" name="submitX" value="Log-in" /></td>
	    </tr>
	  </table>
	</form>
	<p align="center">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</p>
	
	';





$output->setScriptInBody("onload=\"document.form1.username.focus();\"");
//$output->setLeftMenu($leftMenu);
$output->setScriptCalendar('null');
$output->setScriptTRColor('null');		
$output->setBody($out);
$output->setTitle(APPLICATION_NAME);			
$output->output();  		
}
?>