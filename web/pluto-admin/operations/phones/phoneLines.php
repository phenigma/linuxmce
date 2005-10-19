<?
function phoneLines($output,$astADO,$dbADO) {
	/* @var $astADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
	$providerData=array();
	$providerData['freeworddialup (free only)']['url']='http://www.freeworlddialup.com/';
	$providerData['freeworddialup (free only)']['script']='create_amp_fwd.pl';
	
	$providerData['sipgate (try for free, pay as you go)']['url']='http://www.sipgate.co.uk/';
	$providerData['sipgate (try for free, pay as you go)']['script']='create_amp_sipgate.pl';
	
	$providerData['inphonex (try for free, pay as you go)']['url']='http://www.inphonex.com/';
	$providerData['inphonex (try for free, pay as you go)']['script']='create_amp_inphonex.pl';

	$providerData['e-fon (Switzerland)']['url']='http://www.e-fon.ch/';
	$providerData['e-fon (Switzerland)']['script']='create_amp_efon.pl';

	$providerData['broadvoice (US number, free incoming)']['url']='http://www.broadvoice.com/';
	$providerData['broadvoice (US number, free incoming)']['script']='create_amp_broadvoice.pl';

	$providerData['teliax (US number, pay incoming)']['url']='http://www.teliax.com/';
	$providerData['teliax (US number, pay incoming)']['script']='create_amp_teliax.pl';

	$pulldownOptions='';
	foreach (array_keys($providerData) AS $option){
		$pulldownOptions.='<option value="'.$option.'" '.((@$_REQUEST['provider']==$option)?'selected':'').'>'.$option.'</option>
		';
	}
	
	if(@$_REQUEST['provider']!=''){
		$providerUrl=$providerData[$_REQUEST['provider']]['url'];
		$providerScript=$providerData[$_REQUEST['provider']]['script'];
		$userBox='
		<table align="center">
			<tr>
				<td>Url: </td>
				<td><a href="'.$providerUrl.'" target="_blank">'.$providerUrl.'</a></td>
			</tr>
			<tr>
				<td>&nbsp; </td>
				<td>&nbsp;</td>
			</tr>
			<tr>
				<td>Username </td>
				<td><input type="text" name="username" value=""></td>
			</tr>
			<tr>
				<td>Password </td>
				<td><input type="text" name="password" value=""></td>
			</tr>
			<tr>
				<td>Phone number </td>
				<td><input type="text" name="phone" value=""></td>
			</tr>
			<tr>
				<td colspan="2" align="center"><input type="submit" class="button" name="Add" value="Submit"></td>
			</tr>
		
		</table>
		<script>
		 	var frmvalidator = new formValidator("phoneLines");
 			frmvalidator.addValidation("username","req","Please provide an username");			
			frmvalidator.addValidation("password","req","Please provide a password");			
		</script>		
		';
	}
	
	if($action=='form'){
		
		$out.=setLeftMenu($dbADO).'
		<div align="center" class="err">'.stripslashes(@$_REQUEST['error']).'</div>
		<div align="center" class="confirm"><B>'.stripslashes(@$_REQUEST['msg']).'</B></div>
		<h3 align="center">Phone Lines</h3>
		<form action="index.php" method="POST" name="phoneLines">
			<input type="hidden" name="section" value="phoneLines">
			<input type="hidden" name="action" value="update">

			<table align="center">
				<tr>
					<td>Choose provider</td>
					<td><select name="provider" onChange="document.phoneLines.action.value=\'form\';document.phoneLines.submit();">
						<option value=""> - Please select -</option>
						'.$pulldownOptions.'
					</select></td>
				</tr>
				<tr>
					<td colspan="2">'.@$userBox.'</td>
				</tr>		
			</table>
		
		</form>
		';
	}else{
	// process area
		if(isset($_POST['Add'])){
			$username=cleanString($_POST['username']);
			$password=cleanString($_POST['password']);
			$phone=($_POST['phone']=='')?$username:cleanString($_POST['phone']);

			$cmd='sudo -u root /usr/pluto/bin/'.$providerScript.' '.$username.' '.$password.' '.$phone;
			system($cmd,$cmdStatus);
			
			if($cmdStatus!=0){
				$suffix='&err=An error has occured.';
			}else{
				$suffix='&msg=The command was sent.';
			}

			header('Location: index.php?section=phoneLines'.@$suffix);
			exit();
		}
		
		
		header('Location: index.php?section=phoneLines');
	}
	
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}
?>
