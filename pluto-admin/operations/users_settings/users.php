<?
function users($output,$dbADO) {
/* @var $dbADO ADOConnection */
/* @var $rs ADORecordSet */
/* @var $resUserTypes ADORecordSet */
$out='';
$locationGoTo='';
$dbADO->debug=false;
$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
$lastAction = isset($_REQUEST['lastAction'])?cleanString($_REQUEST['lastAction']):'';

if (@$_SESSION['userLoggedIn']!=true) {
	header("Location: index.php?section=login&from=users");
}

$installationID = cleanInteger($_SESSION['installationID']);

$out.='<h3>Users</h3>';

if ($action=='form') {
$queryUsers = 'SELECT Users.*,Installation_Users.userCanModifyInstallation as canModifyInstallation FROM Users 
		INNER JOIN Installation_Users on FK_Users = PK_Users
		WHERE FK_Installation = ?';
$resUsers = $dbADO->Execute($queryUsers,array($installationID));


	if ($resUsers) {
		$out.="
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,'',attributes);
			}
		</script>
		
		<table border='0'>
			<form action='index.php' method='post' name='users'>
			<input type='hidden' name='section' value='users'>
			<input type='hidden' name='action' value='add'>
			<input type='hidden' name='lastAction' value=''>
		";
		
		$usersFormValidation='';
		$out.='<tr bgcolor="#AAAAAA"><td>Username</td><td>Has Mailbox</td><td>Access General</td><td>Extension</td>
					<td>Name</td><td># Extension Ring No</td><td>Forward Email</td>
					<td>Can Modify Installation</td>
					<td>Language</td>
					<td>Installation Main</td>
				</tr>';
		$displayedUsers=array();
		$i=0;

		$languagesTxt = '';
		$selectLanguages = 'SELECT * FROM Language ORDER BY Description ASC';
		$resLanguages = $dbADO->_Execute($selectLanguages);
		
		$queryHisInstallations = 'SELECT * FROM Installation 
		INNER JOIN Installation_Users on FK_Installation = PK_Installation
		WHERE FK_Users = ?';
		
		$resHisInstallations = $dbADO->Execute($queryHisInstallations,array($_SESSION['userID']));
		
		while ($rowUser = $resUsers->FetchRow()) {
			$displayedUsers[]=$rowUser['PK_Users'];
			$color=($i%2==1?"999999":"EEEEEE");
			$out.='<tr valign="top" bgcolor="#'.$color.'">
						<td>
							<input type="text" name="userUserName_'.$rowUser['PK_Users'].'" value="'.$rowUser['UserName'].'">							
							<a href="javascript:void(0);" onClick="if (confirm(\'Are you sure you want to remove user from this installation?\')) { windowOpen(\'index.php?section=removeUserFromInstallation&from=users&userID='.$rowUser['PK_Users'].'\',\'width=500,height=500,toolbars=true,resizable=yes\');}">Remove User From Installation</a><br />
							<a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=userChangePassword&from=users&userID='.$rowUser['PK_Users'].'\',\'width=400,height=400,toolbars=true,resizable=yes\');">Change Password</a>
						</td>
						<td><input type="checkbox" name="userHasMailbox_'.$rowUser['PK_Users'].'" value="1" '.($rowUser['HasMailbox']?" checked='checked' ":'').'></td>
						<td><input type="checkbox" name="userAccessGeneralMailbox_'.$rowUser['PK_Users'].'" value="1" '.($rowUser['AccessGeneralMailbox']?" checked='checked' ":'').'></td>
						<td><input type="text" name="userExtension_'.$rowUser['PK_Users'].'" value="'.$rowUser['Extension'].'"></td>
						
						<td>
							FirstName:<input type="text" name="userFirstName_'.$rowUser['PK_Users'].'" value="'.$rowUser['FirstName'].'"><br />
							LastName :<input type="text" name="userLastName_'.$rowUser['PK_Users'].'" value="'.$rowUser['LastName'].'"><br />
							NickName :<input type="text" name="userNickname_'.$rowUser['PK_Users'].'" value="'.$rowUser['Nickname'].'">
						</td>
						
						<td><input type="text" name="userExtensionRingTimeout_'.$rowUser['PK_Users'].'" value="'.$rowUser['ExtensionRingTimeout'].'"></td>
						<td><input type="text" name="userForwardEmail_'.$rowUser['PK_Users'].'" value="'.$rowUser['ForwardEmail'].'"></td>
						<td><input type="checkbox" name="userCanModifyInstallation_'.$rowUser['PK_Users'].'" value="1" '.($rowUser['canModifyInstallation']?" checked='checked' ":'').'></td>
			';
			
			if ($resLanguages) {
					$resLanguages->MoveFirst();
					$languagesTxt='';
					while ($rowLanguages=$resLanguages->fetchRow()) {
						$languagesTxt.='<option value="'.$rowLanguages['PK_Language'].'" '.($rowUser['FK_Language']==$rowLanguages['PK_Language']?"selected = 'selected'":"").'>'.$rowLanguages['Description'].'</option>';
					}
			}
			$out.='		<td>
							<select name="userLanguage_'.$rowUser['PK_Users'].'">
							'.$languagesTxt.'
							</select>
						</td>
			';
			
			if ($resHisInstallations) {
					$resHisInstallations->MoveFirst();
					$hisInstallationsTxt='';
					while ($rowHisInstallations=$resHisInstallations->fetchRow()) {
						$hisInstallationsTxt.='<option value="'.$rowHisInstallations['PK_Installation'].'" '.($rowUser['FK_Installation_Main']==$rowHisInstallations['PK_Installation']?"selected = 'selected'":"").'>'.$rowHisInstallations['Description'].'</option>';
					}
			}
			$out.='		<td>
							<select name="userMainInstallation_'.$rowUser['PK_Users'].'">
							<option value="0">-please select-</option>
							'.$hisInstallationsTxt.'
							</select>
						</td>
			';
			
			$out.='</tr>';
			$usersFormValidation.='
				frmvalidator.addValidation("userUserName_'.$rowUser['PK_Users'].'","req","Please enter a description");
			';
			$i++;
		}
		
		
		
		$out.='
			<tr>
				<td colspan="5"> 
					<input type="text" name="addUserToInstallation" value="" size="20">
					<input type="submit" name="submitX" value="Add"> 
						'.(@$_SESSION['users']['userNotValid'] == 1?'(<b>Invalid username</b>)':'').'
						'.(@$_SESSION['users']['userNotValid'] == 2?'(<b>User already in this installation</b>)':'').'
				</td>
			</tr>
			<tr><td colspan="2"><input type="submit" name="submitX" value="Submit">'.(isset($_GET['msg'])?"<br/><b>".strip_tags($_GET['msg']).'</b>':'').'</td></tr>
				<input type="hidden" name="displayedUsers" value="'.join(",",$displayedUsers).'">
			</form>

		<script>
		 	var frmvalidator = new formValidator("users"); 		
			'.$usersFormValidation.'
		</script>
			
		</table>
		<br />
		<a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=createUser&from=users\',\'width=600,height=650,toolbars=true\');">Create User</a>
		';
	}
} else {
	
	//check if current user canModifyInstallation
	$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$installationID,$dbADO);
	
	if ($canModifyInstallation) {		
			//process			
			$addNewUser = cleanString($_POST['addUserToInstallation'],30);
			$_SESSION['users']['userNotValid'] = 0;
			$displayedUsers = cleanString($_POST['displayedUsers']);	
			$displayedUsersArray = explode(",",$displayedUsers);
			
			if ($addNewUser!='') {
				$grabTheUserID = 'SELECT PK_Users From Users WHERE UserName = ?';
				$resGrabTheUserID = $dbADO->Execute($grabTheUserID,array($addNewUser));
				if ($resGrabTheUserID && $resGrabTheUserID->RecordCount()==1) {
								
					$rowUser = $resGrabTheUserID->FetchRow();
					if (!in_array($rowUser['PK_Users'],$displayedUsersArray)) {
						$insertUserToInstallation = "
						INSERT INTO Installation_Users(FK_Installation,FK_Users)
							VALUES(?,?)
						";			
						$query=$dbADO->Execute($insertUserToInstallation,array($installationID,$rowUser['PK_Users']));
						$lastInsert=$dbADO->Insert_ID();
						$locationGoTo = "userUserName_{$lastInsert}";		
						$_SESSION['users']['userNotValid'] = 0;
					} else {				
						$_SESSION['users']['userNotValid'] = 2;
					}			
				} else {
					$_SESSION['users']['userNotValid'] = 1;
				}
				
			}
			
			
			if (!is_array($displayedUsersArray) || $displayedUsersArray===array()) {
				$displayedUsersArray=array();
			}
			
			
			foreach ($displayedUsersArray as $user) {
				
				
				$username = cleanString($_POST['userUserName_'.$user]);

				$hasMailbox =   cleanInteger(@$_POST['userHasMailbox_'.$user]);
				$userAccessGeneralMailbox = cleanInteger(@$_POST['userAccessGeneralMailbox_'.$user]);

				$userFirstName = cleanString($_POST['userFirstName_'.$user]);
				$userLastName = cleanString($_POST['userLastName_'.$user]);
				$userNickname = cleanString($_POST['userNickname_'.$user]);

				$userExtension = cleanString($_POST['userExtension_'.$user]);

				$userForwardEmail = cleanString($_POST['userForwardEmail_'.$user]);

				$userCanModifyInstallation = cleanInteger(@$_POST['userCanModifyInstallation_'.$user]);

				$userLanguage = cleanInteger(@$_POST['userLanguage_'.$user]);

				$userMainInstallation = ((int)($_POST['userMainInstallation_'.$user])==0?NULL:cleanInteger($_POST['userMainInstallation_'.$user]));

				$userExtensionRingTimeout = cleanInteger($_POST['userExtensionRingTimeout_'.$user]);
				$query = 'UPDATE Users set
									UserName = ?,
									HasMailbox =?,
									AccessGeneralMailbox =?,
									Extension=?,
									FirstName=?,
									LastName=?,
									Nickname=?,
									ExtensionRingTimeout=?,
									ForwardEmail=?,
									FK_Language=?,
									FK_Installation_Main=?
							WHERE PK_Users = ?';
				$resUpdUser = $dbADO->Execute($query,array($username,$hasMailbox,$userAccessGeneralMailbox,
				$userExtension,$userFirstName,$userLastName,$userNickname,$userExtensionRingTimeout,
				$userForwardEmail,$userLanguage,$userMainInstallation,$user));

				// check if is the last user who could modify this installation
				$resLastUser=$dbADO->Execute('SELECT FK_Users FROM Installation_Users WHERE userCanModifyInstallation=1 AND FK_Installation =?',array($installationID));
				$rowLastUser=$resLastUser->FetchRow();
				if($resLastUser && $resLastUser->RecordCount()==1 && $userCanModifyInstallation==0 && $rowLastUser['FK_Users']==$user){
					header("Location: index.php?section=users&msg=At least one user could be able to modify installation!&lastAction=".$locationGoTo);
					exit();
				}

				$updateInstallationUserCanModify = 'UPDATE Installation_Users set userCanModifyInstallation = '.$userCanModifyInstallation.' WHERE FK_Users = ? and FK_Installation = ? ';
				$dbADO->Execute($updateInstallationUserCanModify,array($user,$installationID));
				$locationGoTo = "userDesc_".$user;

				
			}
			
			if (strstr($locationGoTo,"#")) {
				header("Location: index.php?section=users&msg=Saved!".$locationGoTo);
			} else {
				header("Location: index.php?section=users&msg=Saved!&lastAction=".$locationGoTo);
			}
	} else {
			header("Location: index.php?section=users&msg=Not allowed!&lastAction=".$locationGoTo);
	}
	
}

	$onLoad='';
	if ($lastAction!=''?$onLoad.="if (document.forms.users.{$lastAction}) {document.forms.users.{$lastAction}.focus();} ":$onLoad.="")
	if (strlen($onLoad)>2) {
		$output->setScriptInBody("onLoad=\"javascript:eval('$onLoad');\"");
	}
	
	$output->setNavigationMenu(array("Settings"=>'index.php?section=installationSettings',"Users"=>'index.php?section=users'));
	
$output->setBody($out);
$output->setTitle(APPLICATION_NAME);			
$output->output();  		
}
?>