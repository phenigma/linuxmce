<?php
function userChangePassword($output,$dbADO) {
	global $changePassMasterUserUrl;
	$out='';
	$action = isset($_POST['action'])?cleanString($_POST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	$userID = isset($_REQUEST['userID'])?cleanInteger($_REQUEST['userID']):0;
	
	$installationID = cleanInteger($_SESSION['installationID']);
	$selectTheSamePassword ='SELECT samePasswordMasterUsers,PK_Users FROM Users WHERE PK_Users =?';
	$resTheSamePassword = $dbADO->Execute($selectTheSamePassword,array($userID));
	$rowTheSamePassword = $resTheSamePassword->FetchRow();
	$FK_MasterUsers=$rowTheSamePassword['PK_Users'];
	
	if ($action=='form') {
		
		$out.='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		</script>
		<div class="err">'.(isset($_GET['error'])&&$_GET['error']!='password'?strip_tags($_GET['error']):'').'</div>
		<form action="index.php" method="post" name="userChangePassword">
		<input type="hidden" name="section" value="userChangePassword">
		<input type="hidden" name="action" value="add">
		<input type="hidden" name="from" value="'.$from.'">
		<input type="hidden" name="userID" value="'.$userID.'">
		<table>	
				
		
		';
		
		
			$out.='
					<tr valign="top">
						<td>Password</td>
						<td>
							<input type="password" name="userPassword" value="">
						</td>
					</tr>
					<tr valign="top">
						<td>Re-Type Password</td>
						<td>
							<input type="password" name="userPassword2" value="">
						</td>
					</tr>					
			';
			if($rowTheSamePassword['samePasswordMasterUsers']==0){
				$out.='
					<tr valign="top">
						<td>Master Password</td>
						<td>
							<input type="password" name="userMasterPassword" value="">
							'.(@$_GET['error']=='password'?'<br /><b>Passwords do not match!</b>':'').'
						</td>
					</tr>
					<tr valign="top">
						<td>Re-Type Master Password</td>
						<td>
							<input type="password" name="userMasterPassword2" value="">
						</td>
					</tr>
				';
			
			}			
			$out.='
					<tr>
						<td colspan="2" align="center"><input type="submit" name="submitX" value="Save"></td>
					</tr>
					<tr>
						<td colspan="2" align="left">* Require Internet connection.</td>
					</tr>
			</table>
			</form>
			';
			
			$out.='
				<script>
		 			var frmvalidator = new formValidator("userChangePassword"); 		
					frmvalidator.addValidation("userPassword","req","Please enter a password");
					frmvalidator.addValidation("userPassword2","req","Please retype the password");';
			if($rowTheSamePassword['samePasswordMasterUsers']==0){
				$out.='
					frmvalidator.addValidation("userMasterPassword","req","Please enter a master password");
					frmvalidator.addValidation("userMasterPassword2","req","Please retype the master password");
				';
			}
			$out.='
				</script>
			';
		
	} else {
		
	$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$installationID,$dbADO);
	
		if ($canModifyInstallation) {		

			$userPassword = $_SESSION['userChangePassword']['userPassword'] = cleanString($_POST['userPassword']);
			$userPassword2 = $_SESSION['userChangePassword']['userPassword2'] = cleanString($_POST['userPassword2']);
	
			if ($userPassword!=$userPassword2) {
				header("Location: index.php?section=userChangePassword&error=Passwords do not match.&userID=$userID&from=$from");
				exit();
			}	

			$passMd5=md5($userPassword);
		
			if($rowTheSamePassword['samePasswordMasterUsers']==0){
				$userMasterPassword = cleanString($_POST['userMasterPassword']);
				$userMasterPassword2 = cleanString($_POST['userMasterPassword2']);

				if ($userMasterPassword!=$userMasterPassword2) {
					header("Location: index.php?section=userChangePassword&error=Master Passwords do not match.&userID=$userID&from=$from");
					exit();
				}

				$masterPassMd5=md5($userMasterPassword);
			}else
				$masterPassMd5=$passMd5;

			//check if is allowed to change password!if  is within that installation 

			$selectUserInst = 'SELECT * FROM Users WHERE PK_Users=?';
			$queryUserInst = $dbADO->Execute($selectUserInst,array($userID));
			
			if ($userPassword!='' && (($queryUserInst && $queryUserInst->RecordCount()==1) || $_SESSION['userID']==$userID)) {
				//(only the creator and the owner can change the password
				// attempt to change MasterUsers password
				if($rowTheSamePassword['samePasswordMasterUsers']!=2){
					$MasterUsersPasswordUpdated=updateMasterUsersPassword($FK_MasterUsers,$userMasterPassword,$changePassMasterUserUrl);
					if(!$MasterUsersPasswordUpdated[0]){
						header("Location: index.php?section=userChangePassword&error=".$MasterUsersPasswordUpdated[1]."&userID=$userID&from=$from");
						exit();
					}
				}				
				$insertUser = '
						UPDATE Users set Password = ? WHERE PK_Users = ?';
				$query = $dbADO->Execute($insertUser,array(md5($userPassword),$userID));
				
				$out.="
				<script>
					alert('User\'s password changed!');
				    opener.document.forms.{$from}.action.value='form';
					//opener.document.forms.{$from}.lastAction.value='xxx';
					opener.document.forms.{$from}.submit();
					self.close();
				</script>
				";			
			} else {				
				header("Location: index.php?section=userChangePassword&userID=$userID&from=$from");
			}		
		} else {
			header("Location: index.php?section=userChangePassword&userID=$userID&from=$from&error=You are not authorised to change the installation.");
		}
	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();
}
?>