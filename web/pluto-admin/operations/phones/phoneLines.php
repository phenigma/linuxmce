<?
function phoneLines($output,$astADO) {
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

	$providerData['NuFone']['url']='http://www.nufone.net/';
	$providerData['NuFone']['script']='create_amp_nufone.pl';

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
		
		$out.='
		<div align="center" class="err">'.stripslashes(@$_REQUEST['error']).'</div>
		<div align="center" class="confirm"><B>'.stripslashes(@$_REQUEST['msg']).'</B></div>
		<h3 align="center">Phone Lines</h3>
		<form action="index.php" method="POST" name="phoneLines">
			<input type="hidden" name="section" value="phoneLines">
			<input type="hidden" name="action" value="update">
			'.phoneLinesTable($astADO).'
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
		
		if(isset($_REQUEST['id'])){
			$id=str_replace('9999','',$_REQUEST['id']);

			$url='http://'.$_SERVER['SERVER_ADDR'].'/pluto-admin/amp/admin/config.php';
			$params='display=6&extdisplay=OUT_'.$id.'&action=deltrunk';
			
			$answer=queryExternalServer($url.'?'.$params);
			
			header('Location: index.php?section=phoneLines&msg=The phone line was deleted.');
			exit();
		}
		
		header('Location: index.php?section=phoneLines');
	}
	
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}

function phoneLinesTable($astADO){
	$count=0;
	$res=$astADO->Execute("
		SELECT sip.id,sip.data,sips.data AS sdata, sipp.data AS pdata 
		FROM sip 
		INNER JOIN sip sips ON (sips.id=sip.id) AND (sips.keyword='secret')
		INNER JOIN sip sipp ON (sipp.id=sip.id) AND (sipp.keyword='username')
		WHERE (sip.keyword='account') AND ((sip.data='broadvoice') OR (sip.data='sipgate') OR (sip.data='inphonex'))");
	$out='
	<table align="center" cellpadding="3" cellspacing="0">
		<tr bgcolor="lightblue">
			<td align="center"><B>Type</B></td>
			<td align="center"><B>Data</B></td>
			<td align="center"><B>Password</B></td>
			<td align="center"><B>Username</B></td>
			<td align="center"><B>Actions</B></td>
		</tr>	';
	while($row=$res->FetchRow()){
		$count++;
		$color=($count%2==0)?'#F0F3F8':'#FFFFFF';
		$out.='
		<tr bgcolor="'.$color.'">
			<td>SIP</td>
			<td>'.$row['data'].'</td>
			<td>'.$row['sdata'].'</td>
			<td>'.$row['pdata'].'</td>
			<td align="center"><a href="index.php?section=incomingCallsSettings&type=SIP&id='.$row['id'].'">Settings</a> <a href="javascript:if(confirm(\'Are you sure you want to delete this line?\'))self.location=\'index.php?section=phoneLines&action=del&id='.$row['id'].'\'">Del</a></td>
		</tr>';
	}

	$res=$astADO->Execute("
		SELECT iax.id,iax.data,iaxs.data AS sdata, iaxp.data AS pdata 
		FROM iax 
		INNER JOIN iax iaxs ON (iaxs.id=iax.id) AND (iaxs.keyword='secret')
		INNER JOIN iax iaxp ON (iaxp.id=iax.id) AND (iaxp.keyword='username')
		WHERE (iax.keyword='account') AND ((iax.data='fwd') OR (iax.data='teliax-out') OR (iax.data='efon') OR (iax.data='nufone-out'))");
	while($row=$res->FetchRow()){
		$count++;
		$color=($count%2==0)?'#F0F3F8':'#FFFFFF';
		$out.='
		<tr bgcolor="'.$color.'">
			<td>IAX</td>
			<td>'.$row['data'].'</td>
			<td>'.$row['sdata'].'</td>
			<td>'.$row['pdata'].'</td>
			<td align="center"><a href="index.php?section=incomingCallsSettings&type=IAX&id='.$row['id'].'">Settings</a> <a href="javascript:if(confirm(\'Are you sure you want to delete this line?\'))self.location=\'index.php?section=phoneLines&action=del&id='.$row['id'].'\'">Del</a></td>
		</tr>';
	}
	
	$out.='</table><br>';
	if($count==0){
		$out='';
	}
	
	return $out;
}

function queryExternalServer($url){
	$ch = curl_init();
	
	// set URL and other appropriate options
	curl_setopt($ch, CURLOPT_URL, $url);
	curl_setopt($ch, CURLOPT_HEADER, false);
	
	// grab URL and pass it to the browser
	$result=curl_exec($ch);
	
	// close CURL resource, and free up system resources
	curl_close($ch);
	
	return $result;
}
?>
