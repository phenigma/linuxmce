<?php
function createUser($output,$dbADO) {
	global $addMasterUserUrl;
	$out='<h3>Add new user/family member</h3>Pick a username.  It must be unique among all Pluto users because you will have the option of using it
	as a substitute for your phone number and for instant messaging.
	If you specify a "Nickname", that name will appear on all the screens instead of the username.  The nickname can be anything and does not need to be unique.';
	$action = isset($_POST['action'])?cleanString($_POST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	
	$installationID = cleanInteger(@$_SESSION['installationID']);
	
	if ($action=='form') {
		
		$out.='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
			
		</script>
		<form action="index.php" method="post" name="createUser">
		<input type="hidden" name="section" value="createUser">
		<input type="hidden" name="action" value="add">
		<input type="hidden" name="from" value="'.$from.'">
		<table>	
				
		
		';
		
		$i=0;

		$languagesTxt = '';
		$selectLanguages = 'SELECT * FROM Language ORDER BY Description ASC';
		$resLanguages = $dbADO->_Execute($selectLanguages);
		
		$queryHisInstallations = 'SELECT * FROM Installation 
		INNER JOIN Installation_Users on FK_Installation = PK_Installation
		WHERE FK_Users = ? and PK_Installation != ?';
		
		$resHisInstallations = $dbADO->Execute($queryHisInstallations,array($_SESSION['userID'],$installationID));
		
			$out.='
					<tr valign="top">
						<td colspan="2"><h3 class="err">'.(isset($_GET['error'])?$_GET['error']:'').'</h3></td>
					</tr>
					<tr valign="top" bgcolor="#EEEEEE">
						<td><B>Username</B>*</td>
						<td>
							<input type="text" name="userUserName" value="'.@$_SESSION['createUser']['userUserName'].'">
						</td>
					</tr>
					<tr valign="top">
						<td><B>Password</B>*</td>
						<td>
							<input type="password" name="userPassword" value="">
						</td>
					</tr>
					<tr valign="top">
						<td><B>Re-Type password</B>*</td>
						<td>
							<input type="password" name="userPassword2" value="">
						</td>
					</tr>
					<tr valign="top" bgcolor="#EEEEEE">
						<td><B>PIN Code</B>* (numeric only)</td>
						<td>
							<input type="text" name="pinCode" value="">
						</td>
					</tr>			
					<tr valign="top">
						<td><B>Create a voicemail box and an email account for the user?</B><br>This allows the user to check his messages/email on the Orbiters.</td>
						<td><input type="checkbox" name="userHasMailbox" value="1" '.@($_SESSION['createUser']['userHasMailbox']==1?" checked='checked'":'').'></td>
					</tr>
					<tr valign="top">
						<td><B>Access General Mailbox?</B><br>This allows the user to review messages left in the family\'s general mailbox.  Otherwise the user can only access his own private messages.</td>
						<td><input type="checkbox" name="userAccessGeneralMailbox" value="1" '.@($_SESSION['createUser']['userAccessGeneralMailbox']==1?" checked='checked'":'').'></td>
					</tr>
					<tr valign="top">
						<td><B>Extension for intercom</B><br>Dial this number from any phone in the house to quickly reach the user.  Outside callers can dial it also to be directly connected.</td>
						<td><input type="text" name="userExtension" value="'.@$_SESSION['createUser']['userExtension'].'"></td>
					</tr>
					<tr valign="top">
						<td><B>Name</B></td>
						<td>
							<B>FirstName:</B><input type="text" name="userFirstName" value="'.@$_SESSION['createUser']['userFirstName'].'"><br />
							<B>LastName:</B> <input type="text" name="userLastName" value="'.@$_SESSION['createUser']['userLastName'].'"><br />
							<B>NickName:</B> <input type="text" name="userNickname" value="'.@$_SESSION['createUser']['userNickname'].'">
						</td>
					</tr>
					<tr valign="top" bgcolor="#EEEEEE">
						<td><B>Email</B>*<br>This user will get an email whenever he has new voicemail</td>
						<td><input type="text" name="userForwardEmail" value="'.@$_SESSION['createUser']['userForwardEmail'].'"></td>
					</tr>
					<tr valign="top">
						<td><B>Can modify configuration</B><br>If checked, the user will be able to access this Pluto Admin site and change the configuration of Pluto</td>
						<td><input type="checkbox" name="userCanModifyInstallation" value="1" '.(@$_SESSION['createUser']['userCanModifyInstallation']==1?" checked='checked'":'').'></td>
					</tr>
					
			';
			
			if ($resLanguages) {
					$resLanguages->MoveFirst();
					$languagesTxt='';
					while ($rowLanguages=$resLanguages->fetchRow()) {
						$languagesTxt.='<option '.(@$_SESSION['createUser']['userLanguage']==$rowLanguages['PK_Language']?"selected='selected'":'').' value="'.$rowLanguages['PK_Language'].'">'.$rowLanguages['Description'].'</option>';
					}
			}
			$out.='
						<tr>
							<td>
								<B>Language</B>
							</td>
							<td>
								<select name="userLanguage">
								'.$languagesTxt.'
								</select>
							</td>
						</tr>
			';
			
			if ($resHisInstallations) {
					$resHisInstallations->MoveFirst();
					$hisInstallationsTxt='';
					while ($rowHisInstallations=$resHisInstallations->fetchRow()) {
						$hisInstallationsTxt.='<option '.(@$_SESSION['createUser']['userMainInstallation']==$rowHisInstallations['PK_Installation']?"selected='selected'":'').' value="'.$rowHisInstallations['PK_Installation'].'">'.$rowHisInstallations['Description'].'</option>';
					}
			}
			$out.='<tr>
						<td><B>Main Installation</B><br>If you have multiple homes, or installations joined, which is this user\'s primary residence?</td>
						<td>
							<select name="userMainInstallation">
							<option value="0">-please select-</option>
							'.$hisInstallationsTxt.'
							</select>
						</td>
					</tr>
			';
			
			$out.='</tr>';
			
			$out.='
					<tr>
						<td colspan="2" align="left">* Required fields.</td>
					</tr>			
					<tr>
						<td colspan="2" align="center"><input type="submit" class="button" name="submitX" value="Save"  ></td>
					</tr>
			</table>
			</form>
			';
			
			$out.='
				<script>
		 			var frmvalidator = new formValidator("createUser"); 		
					frmvalidator.addValidation("userUserName","req","Please enter a user name");
					frmvalidator.addValidation("userPassword","req","Please enter a password");
					frmvalidator.addValidation("userPassword2","req","Please retype the password");
					//frmvalidator.addValidation("userFirstName","req","Please enter a user first name");
					//frmvalidator.addValidation("userLastName","req","Please enter a user last name");
					frmvalidator.addValidation("pinCode","req","Please enter a number for PIN code.");
			 		frmvalidator.addValidation("pinCode","numeric","PIN code must be a number.");
					frmvalidator.addValidation("userForwardEmail","email","Please enter an email address");
				</script>
			';
		
	} else {
		
	//check if current user canModifyInstallation
	$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$installationID,$dbADO);
	
	if ($canModifyInstallation) {		
		
		$username = $_SESSION['createUser']['userUserName'] = cleanString($_POST['userUserName']);
		
		$hasMailbox = $_SESSION['createUser']['userHasMailbox'] =  cleanInteger(@$_POST['userHasMailbox']);
		$userAccessGeneralMailbox = $_SESSION['createUser']['userAccessGeneralMailbox'] = cleanInteger(@$_POST['userAccessGeneralMailbox']);
		
		$userFirstName =  $_SESSION['createUser']['userFirstName'] = cleanString($_POST['userFirstName']);
		$userLastName = $_SESSION['createUser']['userLastName'] = cleanString($_POST['userLastName']);
		$userNickname = $_SESSION['createUser']['userNickname'] = cleanString($_POST['userNickname']);
		
		$userExtension = $_SESSION['createUser']['userExtension'] = cleanString($_POST['userExtension']);
		
		$userForwardEmail = $_SESSION['createUser']['userForwardEmail'] = cleanString(@$_POST['userForwardEmail']);
		$userCanModifyInstallation = $_SESSION['createUser']['userCanModifyInstallation'] = cleanInteger(@$_POST['userCanModifyInstallation']);
		
		$userLanguage = $_SESSION['createUser']['userLanguage'] = cleanInteger(@$_POST['userLanguage']);
		
		$userMainInstallation = $_SESSION['createUser']['userMainInstallation'] = (cleanInteger($_POST['userMainInstallation'])==0?null:cleanInteger($_POST['userMainInstallation']));
		
		$userPassword = $_SESSION['createUser']['userPassword'] = cleanString($_POST['userPassword']);
		$userPassword2 = $_SESSION['createUser']['userPassword2'] = cleanString($_POST['userPassword2']);
		$passMd5=md5($userPassword);
		
		$masterPassMd5=$passMd5;
		$userMasterPassword=$userPassword;

		$pinCode=(int)$_POST['pinCode'];
		$pinCodeMd5=md5($pinCode);
		
		if ($userPassword!=$userPassword2) {
			header("Location: index.php?section=createUser&error=Passwords do not match!&from=$from");
			exit();
		}	

		$selectUsername = "SELECT UserName from Users where UserName = ?";
		$resUsername = $dbADO->Execute($selectUsername,array($username));
		if ($resUsername && $resUsername->RecordCount()>0) {
			header("Location: index.php?section=createUser&error=Username already taken!&from=$from");
			exit();
		}

		if ($username!='' && $userPassword!='') {
			$userAddedtoMasterUsers=addtoMasterUsers('Users',$userForwardEmail,$username,$_SESSION['userID'],$userMasterPassword,$addMasterUserUrl);
			if(!$userAddedtoMasterUsers[0]){
				header("Location: index.php?section=createUser&error=".$userAddedtoMasterUsers[1]."&from=$from");
				exit(0);
			}
			
			// insert the user into local table if he was added to MasterUsers table
			parse_str($userAddedtoMasterUsers[1]);
			$FK_MasterUsers=$MasterUsersID;
			
			$insertUser = '
					INSERT INTO Users (PK_Users,UserName,Password, HasMailbox,
					AccessGeneralMailbox,FirstName,
					LastName,Nickname,Extension,ForwardEmail,
					FK_Language,FK_Installation_Main,PINCode) 
					values(?,?,?,?,?,?,?,?,?,?,?,?,?)';
			$query = $dbADO->Execute($insertUser,array($FK_MasterUsers,
					$username,$passMd5,$hasMailbox,$userAccessGeneralMailbox,$userFirstName,
					$userLastName,$userNickname,$userExtension,$userForwardEmail,
					$userLanguage,$userMainInstallation,$pinCodeMd5
					));
			$insertID = $dbADO->Insert_ID();
			
					
			$insertUserToInstallation = "
			INSERT INTO Installation_Users(FK_Installation,FK_Users,userCanModifyInstallation)
				VALUES(?,?,?)
			";
			$_SESSION['createUser']=array();
			$query=$dbADO->Execute($insertUserToInstallation,array($installationID,$insertID,$userCanModifyInstallation));

			$commandToSend='/usr/pluto/bin/SetupUsers.sh';
			system($commandToSend);
			
			$out.="
			<script>
				alert('User created!');
			    opener.document.forms.{$from}.action.value='form';
				opener.document.forms.{$from}.lastAction.value='newEventToMasterDevice';
				opener.document.forms.{$from}.submit();
				self.close();
			</script>
			";			
		} else {				
			header("Location: index.php?section=createUser&from=$from");
		}		
		
	} else {
			header("Location: index.php?section=createUser&error=You are not allowed to modify installation.");
	}
	
}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();
}
?>
