<?php
function userChangePassword($output,$dbADO) {
	global $changePassMasterUserUrl;
	$out='';
	$action = isset($_POST['action'])?cleanString($_POST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	$userID = isset($_REQUEST['userID'])?cleanInteger($_REQUEST['userID']):0;
	
	$installationID = cleanInteger($_SESSION['installationID']);
	$selectTheSamePassword ='SELECT PK_Users FROM Users WHERE PK_Users =?';
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
		
			//check if is allowed to change password!if  is within that installation 

			$selectUserInst = 'SELECT * FROM Users WHERE PK_Users=?';
			$queryUserInst = $dbADO->Execute($selectUserInst,array($userID));
			
			if ($userPassword!='' && (($queryUserInst && $queryUserInst->RecordCount()==1) || $_SESSION['userID']==$userID)) {
				//(only the creator and the owner can change the password
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