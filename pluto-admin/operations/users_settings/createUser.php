<?php
function createUser($output,$dbADO) {
	global $addMasterUserUrl;
	$out='';
	$action = isset($_POST['action'])?cleanString($_POST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	
	$installationID = cleanInteger(@$_SESSION['installationID']);
	
	if ($action=='form') {
		
		$out.='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
			
			function enableMasterPass(){
				if(!document.createUser.theSamePassword.checked){
					document.createUser.userMasterPassword.disabled=false;
					document.createUser.userMasterPassword2.disabled=false;
				}else{
					document.createUser.userMasterPassword.disabled=true;
					document.createUser.userMasterPassword2.disabled=true;
				}
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
						<td colspan="2"><b>'.(isset($_GET['error'])?$_GET['error']:'').'</b></td>
					</tr>
					<tr valign="top">
						<td>Username</td>
						<td>
							<input type="text" name="userUserName" value="'.@$_SESSION['createUser']['userUserName'].'">
						</td>
					</tr>
					<tr valign="top">
						<td>Local password</td>
						<td>
							<input type="password" name="userPassword" value="">
						</td>
					</tr>
					<tr valign="top">
						<td>Re-Type local password</td>
						<td>
							<input type="password" name="userPassword2" value="">
						</td>
					</tr>
					<tr valign="top">
						<td>Use the same password for local and master account</td>
						<td><input type="checkbox" value="1" name="theSamePassword" checked onClick="enableMasterPass();"></td>
					</tr>
					<tr valign="top">
						<td>Master password</td>
						<td>
							<input type="password" name="userMasterPassword" value="" disabled="disabled">
						</td>
					</tr>
					<tr valign="top">
						<td>Re-Type master password</td>
						<td>
							<input type="password" name="userMasterPassword2" value="" disabled="disabled">
						</td>
					</tr>
					<tr valign="top">
						<td>Type</td>
						<td><input type="radio" value="Users" name="typeUser" checked>Individual<input type="radio" value="Establishment" name="typeUser">Business</td>
					</tr>			
					<tr valign="top">
						<td>Has Mailbox</td>
						<td><input type="checkbox" name="userHasMailbox" value="1" '.@($_SESSION['createUser']['userHasMailbox']==1?" checked='checked'":'').'></td>
					</tr>
					<tr valign="top">
						<td>Access General Mailbox</td>
						<td><input type="checkbox" name="userAccessGeneralMailbox" value="1" '.@($_SESSION['createUser']['userAccessGeneralMailbox']==1?" checked='checked'":'').'></td>
					</tr>
					<tr valign="top">
						<td>User extension</td>
						<td><input type="text" name="userExtension" value="'.@$_SESSION['createUser']['userExtension'].'"></td>
					</tr>
					<tr valign="top">
						<td>Name</td>
						<td>
							FirstName:<input type="text" name="userFirstName" value="'.@$_SESSION['createUser']['userFirstName'].'"><br />
							LastName :<input type="text" name="userLastName" value="'.@$_SESSION['createUser']['userLastName'].'"><br />
							NickName :<input type="text" name="userNickname" value="'.@$_SESSION['createUser']['userNickname'].'">
						</td>
					</tr>
					<tr valign="top">
						<td>Extension Ring Timeout</td>
						<td><input type="text" name="userExtensionRingTimeout" value="'.@$_SESSION['createUser']['userExtensionRingTimeout'].'"></td>
					</tr>
					<tr valign="top">
						<td>Forward email</td>
						<td><input type="text" name="userForwardEmail" value="'.@$_SESSION['createUser']['userForwardEmail'].'"></td>
					</tr>
					<tr valign="top">
						<td>Can modify installation</td>
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
								Language:
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
						<td>Main Installation</td>
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
						<td colspan="2" align="center"><input type="submit" name="submitX" value="Save"></td>
					</tr>
					<tr>
						<td colspan="2" align="left">* Internet connection is required.</td>
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
					frmvalidator.addValidation("userFirstName","req","Please enter a user first name");
					frmvalidator.addValidation("userLastName","req","Please enter a user last name");
					frmvalidator.addValidation("userForwardEmail","req","Please enter an email address");
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
		$userExtensionRingTimeout = $_SESSION['createUser']['userExtensionRingTimeout'] = cleanInteger($_POST['userExtensionRingTimeout']);
		
		$userForwardEmail = $_SESSION['createUser']['userForwardEmail'] = cleanString(@$_POST['userForwardEmail']);
		$userCanModifyInstallation = $_SESSION['createUser']['userCanModifyInstallation'] = cleanInteger(@$_POST['userCanModifyInstallation']);
		
		$userLanguage = $_SESSION['createUser']['userLanguage'] = cleanInteger(@$_POST['userLanguage']);
		
		$userMainInstallation = $_SESSION['createUser']['userMainInstallation'] = (cleanInteger($_POST['userMainInstallation'])==0?null:cleanInteger($_POST['userMainInstallation']));
		
		$userPassword = $_SESSION['createUser']['userPassword'] = cleanString($_POST['userPassword']);
		$userPassword2 = $_SESSION['createUser']['userPassword2'] = cleanString($_POST['userPassword2']);
		$passMd5=md5($userPassword);
		
		$theSamePassword=cleanInteger(@$_POST['theSamePassword']);
		if($theSamePassword!=1){
			$userMasterPassword = cleanString($_POST['userMasterPassword']);
			$userMasterPassword2 = cleanString($_POST['userMasterPassword2']);
			
			if ($userMasterPassword!=$userMasterPassword2) {
				header("Location: index.php?section=createUser&error=Master Passwords do not match&from=$from");
				exit();
			}	

			$masterPassMd5=md5($userMasterPassword);
		}else 
			$masterPassMd5=$passMd5;
			
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

		if ($username!='' && $userFirstName!='' && $userLastName!='' && $userPassword!='') {
			
			$userAddedtoMasterUsers=addtoMasterUsers($_POST['typeUser'],$userForwardEmail,$username,$_SESSION['userID'],$masterPassMd5,$addMasterUserUrl);
			if(!$userAddedtoMasterUsers[0]){
				header("Location: index.php?section=createUser&error=".$userAddedtoMasterUsers[1]."&from=$from");
				exit(0);
			}
			
			// insert the user into local table if he was added to MasterUsers table
			parse_str($userAddedtoMasterUsers[1]);
			$FK_MasterUsers=$MasterUsersID;
			
			$insertUser = '
					INSERT INTO Users (PK_Users,UserName,Password,samePasswordMasterUsers, HasMailbox,
					AccessGeneralMailbox,FirstName,
					LastName,Nickname,Extension,ExtensionRingTimeout,ForwardEmail,
					FK_Language,FK_Installation_Main) 
					values(?,?,?,?,?,?,?,?,?,?,?,?,?,?)';
			$query = $dbADO->Execute($insertUser,array($FK_MasterUsers,
					$username,$passMd5,$theSamePassword,$hasMailbox,$userAccessGeneralMailbox,$userFirstName,
					$userLastName,$userNickname,$userExtension,$userExtensionRingTimeout,$userForwardEmail,
					$userLanguage,$userMainInstallation
					));
			$insertID = $dbADO->Insert_ID();
			
					
			$insertUserToInstallation = "
			INSERT INTO Installation_Users(FK_Installation,FK_Users,userCanModifyInstallation)
				VALUES(?,?,?)
			";
			$_SESSION['createUser']=array();
			$query=$dbADO->Execute($insertUserToInstallation,array($installationID,$insertID,$userCanModifyInstallation));
	
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
			$out = 'You are not allowed to do that!';
	}
	
}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();
}
?>