<?
function users($output,$dbADO) {
	global $checkMasterUserUrl;
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

$out.='<p>Add all the users, or family members, who will be using Pluto.</p>';


if ($action=='form') {
$queryUsers = 'SELECT Users.*,Installation_Users.userCanModifyInstallation as canModifyInstallation FROM Users 
		INNER JOIN Installation_Users on FK_Users = PK_Users
		WHERE FK_Installation = ?';
$resUsers = $dbADO->Execute($queryUsers,array($installationID));


	if ($resUsers) {
		$out.=setLeftMenu($dbADO)."
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
		$out.='<tr bgcolor="#CCCCCC">
				<td align="center"><B>Username</B></td>
				<td align="center"><B>Voicemail<br>+Email</B></td>
				<td align="center"><B>Access general<br>mailbox</B></td>
				<td align="center"><B>Extension<br>for intercom</B></td>
				<td align="center"><B>Name</b></td>
				<td align="center"><b>Email</B></td>
				<td align="center"><B>Can modify<br>configuration?</B></td>
				<td align="center"><B>Language</B></td>
				<td align="center"><B>Primary<br>Installation</B></td>
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
			$color=($i%2==1?"CCCCCC":"EEEEEE");
			$out.='<tr valign="top" bgcolor="#'.$color.'">
						<td>
							<B>'.$rowUser['UserName'].'</B><br>
							<a href="javascript:void(0);" onClick="if (confirm(\'Are you sure you want to remove user from this installation?\')) { windowOpen(\'index.php?section=removeUserFromInstallation&from=users&userID='.$rowUser['PK_Users'].'\',\'width=500,height=500,toolbars=true,resizable=yes\');}">Remove User From Installation</a><br />
							<a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=userChangePassword&from=users&userID='.$rowUser['PK_Users'].'\',\'width=400,height=400,toolbars=true,resizable=yes\');">Change Password</a><br>
							<a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=userChangePIN&from=users&userID='.$rowUser['PK_Users'].'\',\'width=400,height=200,toolbars=true,resizable=yes\');">Change PIN</a>
						</td>
						<td align="center"><input type="checkbox" name="userHasMailbox_'.$rowUser['PK_Users'].'" value="1" '.($rowUser['HasMailbox']?" checked='checked' ":'').'></td>
						<td align="center"><input type="checkbox" name="userAccessGeneralMailbox_'.$rowUser['PK_Users'].'" value="1" '.($rowUser['AccessGeneralMailbox']?" checked='checked' ":'').'></td>
						<td><input type="text" name="userExtension_'.$rowUser['PK_Users'].'" value="'.$rowUser['Extension'].'"></td>
						
						<td>
							FirstName:<input type="text" name="userFirstName_'.$rowUser['PK_Users'].'" value="'.$rowUser['FirstName'].'"><br />
							LastName :<input type="text" name="userLastName_'.$rowUser['PK_Users'].'" value="'.$rowUser['LastName'].'"><br />
							NickName :<input type="text" name="userNickname_'.$rowUser['PK_Users'].'" value="'.$rowUser['Nickname'].'">
						</td>
						
						<td><input type="text" name="userForwardEmail_'.$rowUser['PK_Users'].'" value="'.$rowUser['ForwardEmail'].'"></td>
						<td align="center"><input type="checkbox" name="userCanModifyInstallation_'.$rowUser['PK_Users'].'" value="1" '.($rowUser['canModifyInstallation']?" checked='checked' ":'').'></td>
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
			$i++;
		}
		
		
		
		$out.='
			<tr>
				<td colspan="9"><span class="err">'.stripslashes(@$_REQUEST['error']).'</span></td>
			</tr>
			<tr>
				<td colspan="9"> ';
				if(!isset($_SESSION['masterUserData'])){
					$out.='Add an existing plutohome.com user to this installation - username: 
					<input type="text" name="addUserToInstallation" value="" size="20">
					<input type="submit" class="button" name="addUser" value="Add"  > 
						'.(@$_SESSION['users']['userNotValid'] == 1?'(<b>Invalid username</b>)':'').'
						'.(@$_SESSION['users']['userNotValid'] == 2?'(<b>User already in this installation</b>)':'');
					}else{
						parse_str($_SESSION['masterUserData']);
						$out.='
							<input type="hidden" name="addUserToInstallation" value="'.$_SESSION['masterUserName'].'">
							<span class="confirm"><B>User found in Pluto database. Please type his email <input type="email" name="masterEmail"> and the local password for him:</B></span>
							<input type="password" name="masterUserPas"> <input type="submit" class="button" name="addtoInst" value="Save"  > <input type="button" class="button" name="cancel" value="Cancel" onClick="self.location=\'index.php?section=users&action=cancel&cancel=1\'">
						';
						$usersFormValidation.='
							frmvalidator.addValidation("masterUserPas","req","Please enter a password");';
					}
				$out.='<p><a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=createUser&from=users\',\'width=600,height=650,toolbars=true, resizable=1\');">Create a new user/family member</a><p>
				</td>
			</tr>
			<tr><td colspan="2"><input type="submit" class="button" name="submitX" value="Save"  >'.(isset($_GET['msg'])?"<br/><b>".strip_tags($_GET['msg']).'</b>':'').'</td></tr>
				<input type="hidden" name="displayedUsers" value="'.join(",",$displayedUsers).'">
			</form>

		<script>
		 	var frmvalidator = new formValidator("users"); 		
			'.$usersFormValidation.'
		</script>
			
		</table>
		<br />
		';
	}
} else {
	//check if current user canModifyInstallation
	$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$installationID,$dbADO);
	if(isset($_REQUEST['cancel'])){
		unset($_SESSION['masterUserData']);
		unset($_SESSION['masterUserName']);
		header("Location: index.php?section=users");
		exit();
	}

	
	if ($canModifyInstallation) {	
			//process			
			$addNewUser = cleanString($_POST['addUserToInstallation'],30);
			$_SESSION['users']['userNotValid'] = 0;
			$displayedUsers = cleanString($_POST['displayedUsers']);	
			$displayedUsersArray = explode(",",$displayedUsers);
			
			if ($addNewUser!='' && isset($_POST['addUser'])) {
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
					// check MasterUser database and if user exist, prompt password field for him
					$isMasterUsers=checkMasterUsers($addNewUser, '',$checkMasterUserUrl,'&FirstAccount=&Email=&PlutoId=&Pin=');
					if(!$isMasterUsers[0]){
						$_SESSION['users']['userNotValid'] = 1;
					}
					else {
						$_SESSION['masterUserData']=$isMasterUsers[1];
						$_SESSION['masterUserName']=$addNewUser;
					}
				}
				$msg='';
				header("Location: index.php?section=users&msg=".$msg.$locationGoTo);
			}
			
			if(isset($_POST['addtoInst'])){
				$md5Pass=md5($_POST['masterUserPas']);
				parse_str($_SESSION['masterUserData']);
				if($Email!=$_REQUEST['masterEmail']){
					header("Location: index.php?section=users&error=Email doesn't match");
					exit();
				}
				$insertUser = '
					INSERT INTO Users (PK_Users,UserName,Password, ForwardEmail) 
					values(?,?,?,?)';
				$query = $dbADO->Execute($insertUser,array($MasterUsersID,$_SESSION['masterUserName'],$md5Pass,$Email));
				$insertUserToInstallation = "
					INSERT INTO Installation_Users(FK_Installation,FK_Users) VALUES(?,?)
						";			
				$query=$dbADO->Execute($insertUserToInstallation,array($installationID,$MasterUsersID));
				unset($_SESSION['masterUserData']);
				unset($_SESSION['masterUserName']);
				header("Location: index.php?section=users&msg=User added.".$locationGoTo);
			}
			
			
			if (!is_array($displayedUsersArray) || $displayedUsersArray===array()) {
				$displayedUsersArray=array();
			}

			foreach ($displayedUsersArray as $user) {
				
				
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
				$query = 'UPDATE Users set
									HasMailbox =?,
									AccessGeneralMailbox =?,
									Extension=?,
									FirstName=?,
									LastName=?,
									Nickname=?,
									ForwardEmail=?,
									FK_Language=?,
									FK_Installation_Main=?
							WHERE PK_Users = ?';
				$resUpdUser = $dbADO->Execute($query,array($hasMailbox,$userAccessGeneralMailbox,
				$userExtension,$userFirstName,$userLastName,$userNickname,
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
			$commandToSend='/usr/pluto/bin/SetupUsers.sh';
			system($commandToSend);
			
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
	$output->setTitle(APPLICATION_NAME.' :: Users');			
	$output->output();  		
}
?>
