<?php
function userChangePIN($output,$dbADO) {
	global $changePassMasterUserUrl;
	$out='';
	$action = isset($_POST['action'])?cleanString($_POST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	$userID = isset($_REQUEST['userID'])?cleanInteger($_REQUEST['userID']):0;
	
	$installationID = cleanInteger($_SESSION['installationID']);
	
	if ($action=='form') {
		
		$out.='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		</script>
		<div class="err">'.(isset($_GET['error'])&&$_GET['error']!='password'?strip_tags($_GET['error']):'').'</div>
		<form action="index.php" method="post" name="userChangePIN">
		<input type="hidden" name="section" value="userChangePIN">
		<input type="hidden" name="action" value="add">
		<input type="hidden" name="from" value="'.$from.'">
		<input type="hidden" name="userID" value="'.$userID.'">
		<h3>Change PIN Code</h3>
		<table>	
		';
			$out.='
					<tr valign="top">
						<td>Old password or PIN</td>
						<td>
							<input type="password" name="passOrPIN" value="">
						</td>
					</tr>
					<tr valign="top">
						<td>New PIN</td>
						<td>
							<input type="text" name="newPIN" value="">
						</td>
					</tr>					
					<tr>
						<td colspan="2" align="center"><input type="submit" class="button" name="submitX" value="Save"  ></td>
					</tr>
			</table>
			</form>
			';
			
			$out.='
				<script>
		 			var frmvalidator = new formValidator("userChangePIN"); 		
					frmvalidator.addValidation("passOrPIN","req","Please enter old password or PIN");
					frmvalidator.addValidation("newPIN","req","Please enter a number for PIN code.");
			 		frmvalidator.addValidation("newPIN","numeric","PIN code must be a number.");
				</script>
			';
		
	} else {
		
	$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$installationID,$dbADO);
		
		$passOrPIN=$_POST['passOrPIN'];
		$newPIN=(int)$_POST['newPIN'];
		
		if (!$canModifyInstallation) {		
			header("Location: index.php?section=userChangePIN&userID=$userID&from=$from&error=You are not authorised to change the installation.");
			exit();
		}
		
		$selectUserInst = 'SELECT * FROM Users WHERE PK_Users=?';
		$queryUserInst = $dbADO->Execute($selectUserInst,array($userID));
		if($queryUserInst->RecordCount()==0){
			header("Location: index.php?section=userChangePIN&error=Invalid user specified.&userID=$userID&from=$from");
			exit();
		}
		$row=$queryUserInst->FetchRow();
		if(md5($passOrPIN)==$row['Password'] || md5($passOrPIN)==$row['PINCode']){
			$dbADO->Execute('UPDATE Users SET PINCode = ? WHERE PK_Users = ?',array(md5($newPIN),$userID));
			$out.="
				<script>
					alert('PIN code was changed!');
				    opener.document.forms.{$from}.action.value='form';
					opener.document.forms.{$from}.submit();
					self.close();
				</script>
				";			
		}else{
			header("Location: index.php?section=userChangePIN&error=Neither password or PIN code match.&userID=$userID&from=$from");
			exit();
		}
	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Change password');			
	$output->output();
}
?>
