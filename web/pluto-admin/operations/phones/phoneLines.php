<?
function phoneLines($output,$astADO,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/phoneLines.lang.php');
	
	/* @var $astADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
	$providerData=array();
	$providerData['freeworddialup (free only)']['url']='http://www.freeworlddialup.com/';
	$providerData['freeworddialup (free only)']['script']='create_amp_fwd.pl';
	$providerData['freeworddialup (free only)']['keyword']='freeworddialup';
	
	$providerData['sipgate (try for free, pay as you go)']['url']='http://www.sipgate.co.uk/';
	$providerData['sipgate (try for free, pay as you go)']['script']='create_amp_sipgate.pl';
	$providerData['sipgate (try for free, pay as you go)']['keyword']='sipgate';
	
	$providerData['inphonex (try for free, pay as you go)']['url']='http://www.inphonex.com/';
	$providerData['inphonex (try for free, pay as you go)']['script']='create_amp_inphonex.pl';
	$providerData['inphonex (try for free, pay as you go)']['keyword']='inphonex';
	
	$providerData['e-fon (Switzerland)']['url']='http://www.e-fon.ch/';
	$providerData['e-fon (Switzerland)']['script']='create_amp_efon.pl';
	$providerData['e-fon (Switzerland)']['keyword']='efon';

	$providerData['broadvoice (US number, free incoming)']['url']='http://www.broadvoice.com/';
	$providerData['broadvoice (US number, free incoming)']['script']='create_amp_broadvoice.pl';
	$providerData['broadvoice (US number, free incoming)']['keyword']='broadvoice';

	$providerData['teliax (US number, pay incoming)']['url']='http://www.teliax.com/';
	$providerData['teliax (US number, pay incoming)']['script']='create_amp_teliax.pl';
	$providerData['teliax (US number, pay incoming)']['keyword']='teliax-out';

	$providerData['NuFone']['url']='http://www.nufone.net/';
	$providerData['NuFone']['script']='create_amp_nufone.pl';
	$providerData['NuFone']['keyword']='nufone-out';

	$keywords=array();
	foreach ($providerData AS $prID=>$providerArray){
		$keywords[$providerArray['keyword']]=$prID;
	}
	
	
	if(isset($_REQUEST['provider'])){
		$provider=$_REQUEST['provider'];
	}
	$editedID=@$_REQUEST['eid'];
	if($editedID!=0 && $editedID!=''){
		$phoneData=getPLDetails($editedID,$_REQUEST['type'],$astADO);
		$provider=$keywords[$phoneData['Data']];
	}

	$pulldownOptions='';
	foreach (array_keys($providerData) AS $option){
		$pulldownOptions.='<option value="'.$option.'" '.((@$provider==$option)?'selected':'').'>'.$option.'</option>';
	}

	if(isset($provider)){
		$providerUrl=$providerData[$_REQUEST['provider']]['url'];
		$providerScript=$providerData[$_REQUEST['provider']]['script'];
		$userBox='
		<input type="hidden" name="editedID" value="'.@$editedID.'">
		<table align="center" cellpadding="3" cellspacing="0">
			<tr>
				<td>'.$TEXT_URL_CONST.': </td>
				<td><a href="'.$providerUrl.'" target="_blank">'.$providerUrl.'</a></td>
			</tr>
			<tr>
				<td>&nbsp; </td>
				<td>&nbsp;</td>
			</tr>
			<tr bgcolor="#F0F3F8">
				<td><B>'.$TEXT_USERNAME_CONST.'</B> </td>
				<td><input type="text" name="username" value="'.@$phoneData['Username'].'"></td>
			</tr>
			<tr bgcolor="#F0F3F8">
				<td colspan="2">'.$TEXT_PHONE_LINE_USERNAME_NOTE_CONST.'</td>
			</tr>		
			<tr>
				<td><B>'.$TEXT_PASSWORD_CONST.'</B> </td>
				<td><input type="text" name="password" value="'.@$phoneData['Password'].'"></td>
			</tr>
			<tr>
				<td colspan="2">'.$TEXT_PHONE_LINE_PASSWORD_NOTE_CONST.'</td>
			</tr>
			<tr bgcolor="#F0F3F8">
				<td><B>'.$TEXT_PHONE_NUMBER_CONST.'</B> </td>
				<td><input type="text" name="phone" value="'.@$phoneData['PhoneNumber'].'"></td>
			</tr>
			<tr bgcolor="#F0F3F8">
				<td colspan="2">'.$TEXT_PHONE_LINE_NUMBER_NOTE_CONST.'</td>
			</tr>
			<tr>
				<td><B>'.$TEXT_HOST_CONST.'</B> </td>
				<td><input type="text" name="host" value="'.@$phoneData['Host'].'"> </td>
			</tr>		
			<tr>
				<td colspan="2">'.$TEXT_PHONE_LINE_HOST_NOTE_CONST.'</td>
			</tr>		
			<tr>
				<td colspan="2" align="center"><input type="submit" class="button" name="'.$TEXT_ADD_CONST.'" value="Submit"></td>
			</tr>
		
		</table>
		<script>
		 	var frmvalidator = new formValidator("phoneLines");
 			frmvalidator.addValidation("username","req","'.$TEXT_USERNAME_REQUIRED_CONST.'");			
			frmvalidator.addValidation("password","req","'.$TEXT_PASSWORD_REQUIRED_CONST.'");			
		</script>		
		';
	}
	
	if($action=='form'){
		
		$out.='
		<div align="center" class="err">'.stripslashes(@$_REQUEST['error']).'</div>
		<div align="center" class="confirm"><B>'.stripslashes(@$_REQUEST['msg']).'</B></div>
		
		'.phoneLinesLocalSettings($dbADO).'
		
		<form action="index.php" method="POST" name="phoneLines">
			<input type="hidden" name="section" value="phoneLines">
			<input type="hidden" name="action" value="update">
			
			<h3 align="center">'.$TEXT_PHONE_LINES_CONST.'</h3>
			'.phoneLinesTable($astADO).'
			<table align="center">
				<tr>
					<td>'.$TEXT_CHOOSE_PROVIDER_CONST.'</td>
					<td><select name="provider" onChange="document.phoneLines.action.value=\'form\';document.phoneLines.submit();">
						<option value=""> - '.$TEXT_PLEASE_SELECT_CONST.' -</option>
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
			$editedID=$_REQUEST['editedID'];
			if($editedID!=''){
				deletePhoneLine($editedID);
			}
			
			$username=cleanString($_POST['username']);
			$password=cleanString($_POST['password']);
			$phone=($_POST['phone']=='')?$username:cleanString($_POST['phone']);
			$host=cleanString($_POST['host']);

			$cmd='sudo -u root /usr/pluto/bin/'.$providerScript.' '.$username.' '.$password.' '.$phone.' '.$host;
			system($cmd,$cmdStatus);
			
			if($cmdStatus!=0){
				$suffix='&err='.$TEXT_ADD_PHONE_LINE_ERROR_CONST;
			}else{
				$suffix='&msg='.$TEXT_ADD_PHONE_LINE_CMD_SENT_CONST;
			}

			header('Location: index.php?section=phoneLines'.@$suffix);
			exit();
		}
		
		if(isset($_REQUEST['id'])){
			deletePhoneLine($id);
			
			header('Location: index.php?section=phoneLines&msg='.$TEXT_PHONE_LINE_DELETED_CONST);
			exit();
		}
		
		if(isset($_POST['update_settings'])){
			$val_141=(isset($_POST['ckb_141']))?$_POST['value_141']:0;
			$val_142=(isset($_POST['ckb_141']) && isset($_POST['ckb_142']))?$_POST['value_142']:-1;
			$val_143=$_POST['value_143'];
			$telecomPlugin=getTelecomPlugin($installationID,$dbADO);
			if(is_null($telecomPlugin)){
				header('Location: index.php?section=phoneLines&error='.$TEXT_ERROR_TELECOM_PLUGIN_NOT_FOUND_CONST);
				exit();
			}
			
			$query='UPDATE Device_DeviceData SET IK_DeviceData=? WHERE FK_Device=? AND FK_DeviceData=?';
			
			$dbADO->Execute($query,array($val_141,$telecomPlugin,$GLOBALS['TelecomLocalPrefix']));
			$dbADO->Execute($query,array($val_142,$telecomPlugin,$GLOBALS['TelecomPrependDigit']));
			$dbADO->Execute($query,array($val_143,$telecomPlugin,$GLOBALS['TelecomLocalNumberLength']));
			
			header('Location: index.php?section=phoneLines&msg='.$TEXT_PHONE_SETTINGS_UPDATED_CONST);
			exit();
		}
		
		header('Location: index.php?section=phoneLines');
	}
	
	$output->setScriptCalendar('null');
	$output->setNavigationMenu(array($TEXT_PHONE_LINES_CONST=>'index.php?section=phoneLines'));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_PHONE_LINES_CONST);
	$output->output();
}

function phoneLinesTable($astADO){
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/phoneLines.lang.php');
	
	$count=0;
	$res=$astADO->Execute("
		SELECT sip.id,sip.data,sips.data AS sdata, sipp.data AS pdata,siph.data AS hdata 
		FROM sip 
		INNER JOIN sip sips ON (sips.id=sip.id) AND (sips.keyword='secret')
		INNER JOIN sip sipp ON (sipp.id=sip.id) AND (sipp.keyword='username')
		INNER JOIN sip siph ON (siph.id=sip.id) AND (siph.keyword='host')
		WHERE (sip.keyword='account') AND ((sip.data='broadvoice') OR (sip.data='sipgate') OR (sip.data='inphonex'))");
	$out='
	<table align="center" cellpadding="3" cellspacing="0">
		<tr bgcolor="lightblue">
			<td align="center"><B>'.$TEXT_TYPE_CONST.'</B></td>
			<td align="center"><B>'.$TEXT_DATA_CONST.'</B></td>
			<td align="center"><B>'.$TEXT_PASSWORD_CONST.'</B></td>
			<td align="center"><B>'.$TEXT_USERNAME_CONST.'</B></td>
			<td align="center"><B>'.$TEXT_HOST_CONST.'</B></td>
			<td align="center"><B>'.$TEXT_PHONE_NUMBER_CONST.'</B></td>
			<td align="center"><B>'.$TEXT_ACTION_CONST.'</B></td>
		</tr>	';
	while($row=$res->FetchRow()){
		$incomingData=array_values(getAssocArray('incoming','destination','extension',$astADO,'WHERE destination=\'from-pluto-custom,10'.substr($row['id'],-1).',1\''));
		$phoneNumber=@$incomingData[0];
	
		$count++;
		$color=($count%2==0)?'#F0F3F8':'#FFFFFF';
		$out.='
		<tr bgcolor="'.$color.'">
			<td>SIP</td>
			<td>'.$row['data'].'</td>
			<td>'.$row['sdata'].'</td>
			<td>'.$row['pdata'].'</td>
			<td>'.$row['hdata'].'</td>
			<td>'.$phoneNumber.'</td>
			<td align="center">
				<a href="index.php?section=phoneLines&type=SIP&eid='.$row['id'].'">'.$TEXT_EDIT_CONST.'</a> 
				<a href="index.php?section=incomingCallsSettings&type=SIP&id='.$row['id'].'">'.$TEXT_SETTINGS_CONST.'</a> 
				<a href="javascript:if(confirm(\''.$TEXT_DELETE_PHONE_LINE_CONFIRMATION_CONST.'\'))self.location=\'index.php?section=phoneLines&action=del&id='.$row['id'].'\'">'.$TEXT_DELETE_CONST.'</a>
			</td>
		</tr>';
	}

	$res=$astADO->Execute("
		SELECT iax.id,iax.data,iaxs.data AS sdata, iaxp.data AS pdata,iaxh.data AS hdata 
		FROM iax 
		INNER JOIN iax iaxs ON (iaxs.id=iax.id) AND (iaxs.keyword='secret')
		INNER JOIN iax iaxp ON (iaxp.id=iax.id) AND (iaxp.keyword='username')
		INNER JOIN iax iaxh ON (iaxh.id=iax.id) AND (iaxh.keyword='host')
		WHERE (iax.keyword='account') AND ((iax.data='fwd') OR (iax.data='teliax-out') OR (iax.data='efon') OR (iax.data='nufone-out'))");
	while($row=$res->FetchRow()){
		$incomingData=array_values(getAssocArray('incoming','destination','extension',$astADO,'WHERE destination=\'from-pluto-custom,10'.substr($row['id'],-1).',1\''));
		$phoneNumber=@$incomingData[0];
		
		$count++;
		$color=($count%2==0)?'#F0F3F8':'#FFFFFF';
		$out.='
		<tr bgcolor="'.$color.'">
			<td>IAX</td>
			<td>'.$row['data'].'</td>
			<td>'.$row['sdata'].'</td>
			<td>'.$row['pdata'].'</td>
			<td>'.$row['hdata'].'</td>
			<td>'.$phoneNumber.'</td>
			<td align="center">
				<a href="index.php?section=phoneLines&type=IAX&eid='.$row['id'].'">'.$TEXT_EDIT_CONST.'</a> 
				<a href="index.php?section=incomingCallsSettings&type=IAX&id='.$row['id'].'">'.$TEXT_SETTINGS_CONST.'</a> 
				<a href="javascript:if(confirm(\''.$TEXT_DELETE_PHONE_LINE_CONFIRMATION_CONST.'\'))self.location=\'index.php?section=phoneLines&action=del&id='.$row['id'].'\'">'.$TEXT_DELETE_CONST.'</a>
			</td>
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
	curl_setopt($ch, CURLOPT_RETURNTRANSFER,1);
	
	// grab URL and pass it to the browser
	$result=curl_exec($ch);
	
	// close CURL resource, and free up system resources
	curl_close($ch);
	
	return $result;
}

function phoneLinesLocalSettings($dbADO){
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/phoneLines.lang.php');
	
	$installationID = (int)@$_SESSION['installationID'];	
	$telecomPlugin=getTelecomPlugin($installationID,$dbADO);
	if(is_null($telecomPlugin)){
		return '<span class="err">'.$TEXT_ERROR_TELECOM_PLUGIN_NOT_FOUND_CONST.'</span>';
	}
	
	$ddArray=getAssocArray('Device_DeviceData','FK_DeviceData','IK_DeviceData',$dbADO,'WHERE FK_Device='.$telecomPlugin);

	$out='
	<h3 align="center">'.$TEXT_LOCAL_PHONE_LINES_SETTINGS_CONST.'</h3>
	
	<form action="index.php" method="POST" name="phoneSettings">
	<input type="hidden" name="section" value="phoneLines">
	<input type="hidden" name="action" value="update">
	
	<table align="center" cellpadding="3" cellspacing="0" bgcolor="#F0F3F8">
		<tr>
			<td colspan="2"><input type="checkbox" name="ckb_141" value="1" '.((@$ddArray['141']!=0 && @$ddArray['141']!='')?'checked':'').'> '.$TEXT_PREPEND_AREA_CODE_CONST.' <input type="text" name="value_141" value="'.@$ddArray['141'].'" style="width:25px;"></td>
		</tr>
		<tr>
			<td><input type="checkbox" name="ckb_142" value="1" '.((@$ddArray['142']!=-1)?'checked':'').'> '.$TEXT_PREPEND_DIGIT_CONST.' <input type="text" name="value_142" value="'.((@$ddArray['142']==-1)?'':@$ddArray['142']).'" style="width:25px;"></td>
		</tr>
		<tr>
			<td>'.$TEXT_LOCAL_NUMBER_LENGTH_CONST.' <input type="text" name="value_143" value="'.@$ddArray['143'].'" style="width:25px;"></td>
		</tr>
		<tr>
			<td align="center"><input type="submit" class="button" name="update_settings" value="'.$TEXT_UPDATE_CONST.'"></td>
		</tr>	
	</table>
	</form>';
	
	return $out;
}

function getPLDetails($id,$type,$astADO){
	if($type=='SIP'){
		$res=$astADO->Execute("
			SELECT sip.id,sip.data,sips.data AS sdata, sipp.data AS pdata,siph.data AS hdata 
			FROM sip 
			INNER JOIN sip sips ON (sips.id=sip.id) AND (sips.keyword='secret')
			INNER JOIN sip sipp ON (sipp.id=sip.id) AND (sipp.keyword='username')
			INNER JOIN sip siph ON (siph.id=sip.id) AND (siph.keyword='host')
			WHERE (sip.keyword='account') AND ((sip.data='broadvoice') OR (sip.data='sipgate') OR (sip.data='inphonex')) AND sip.id='$id'");
	}else{
		$res=$astADO->Execute("
			SELECT iax.id,iax.data,iaxs.data AS sdata, iaxp.data AS pdata,iaxh.data AS hdata 
			FROM iax 
			INNER JOIN iax iaxs ON (iaxs.id=iax.id) AND (iaxs.keyword='secret')
			INNER JOIN iax iaxp ON (iaxp.id=iax.id) AND (iaxp.keyword='username')
			INNER JOIN iax iaxh ON (iaxh.id=iax.id) AND (iaxh.keyword='host')
			WHERE (iax.keyword='account') AND ((iax.data='fwd') OR (iax.data='teliax') OR (iax.data='efon')) AND iax.id='$id'");
	}
	$data=array();
	$row=$res->FetchRow();
	$data['Password']=$row['sdata'];
	$data['Username']=$row['pdata'];
	$data['Data']=$row['data'];
	$data['Host']=$row['hdata'];
	
	$incomingID='from-pluto-custom,10'.substr($id,-1).',1';
	$incomingData=array_values(getAssocArray('incoming','destination','extension',$astADO,'WHERE destination=\''.$incomingID.'\''));
	$data['PhoneNumber']=@$incomingData[0];
	
	return $data;
}

function deletePhoneLine($id){
	$id=str_replace('9999','',$id);

	$url='http://'.$_SERVER['SERVER_ADDR'].'/pluto-admin/amp/admin/config.php';
	$params='display=6&extdisplay=OUT_'.$id.'&action=deltrunk';
			
	$answer=queryExternalServer($url.'?'.$params);

	return $answer;
}
?>
