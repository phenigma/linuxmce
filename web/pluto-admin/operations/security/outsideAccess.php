<?
function outsideAccess($output,$dbADO) {
	/* @var $dbADO ADOConnection */
	/* @var $res ADORecordSet */

	$accessFile=$GLOBALS['pluto.conf'];
	exec('cat '.$accessFile.' | grep -v -E "^#|^$" ',$retArray);	
	foreach ($retArray as $comf){
		parse_str($comf);
	}
	
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';

	$installationID = cleanInteger($_SESSION['installationID']);
	
	$queryDSS='
		SELECT Device_StartupScript.Enabled, PK_Device, Parameter,FK_Device 
		FROM Device 
		LEFT JOIN Device_StartupScript ON FK_Device=PK_Device AND FK_StartupScript=?
		WHERE FK_DeviceTemplate=? AND FK_Installation=?';

	$resDSS=$dbADO->Execute($queryDSS,array($GLOBALS['ProcessLogs'],$GLOBALS['rootCoreID'],$installationID));
	if($resDSS->RecordCount()>0){
		$rowDSS=$resDSS->FetchRow();
		$sendErrorsToPluto=($rowDSS['Enabled']==1 && $rowDSS['Parameter']!='0')?1:0;
		$coreID=$rowDSS['PK_Device'];
	}
	
	$resAllow80=$dbADO->Execute('SELECT * FROM Firewall WHERE RuleType=? AND SourcePort=80','core_input');
	$allowAccessOn80=($resAllow80->RecordCount()>0)?1:0;
		
	$resAllowPort=$dbADO->Execute('SELECT * FROM Firewall WHERE RuleType=? AND DestinationPort=80 AND DestinationIP=?',array('port_forward','127.0.0.1'));
	$allowAccessOnPort=($resAllowPort->RecordCount()>0)?1:0;
	$rowAccessOnPort=$resAllowPort->FetchRow();
	$port=$rowAccessOnPort['SourcePort'];
	
	$out.='<div align="left"><h3>Outside Access</h3></div>';
	
	if ($action=='form') {
		

		if(count($retArray)==0){
			$out.='<div class="err">Insuffient rights: pluto.conf file cannot be opened.</div>';
		}
		$out.=setLeftMenu($dbADO).'<div class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>';
		$out.='
	<script>
	function validateInput()
	{
		portValue=parseInt(document.outsideAccess.port.value);
		if(document.outsideAccess.allowOnPort.checked && (isNaN(portValue) || portValue==80 || portValue<=0)){
			alert("Please type the port, other value than 80.");
			document.outsideAccess.port.focus();
			return false;
		}
		
		if(document.outsideAccess.allowOnPassword.checked && document.outsideAccess.password.value!=document.outsideAccess.password1.value){
			alert("Please type the same password.");
			document.outsideAccess.password.focus();
			return false;
		}
		return true;
	}
		
	function setAntiHanging()
	{
		document.outsideAccess.RA_CheckRemotePort.checked=(document.outsideAccess.allowOnPassword.checked)?true:false;
	}
	</script>	
		
	<div class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
	<form action="index.php" method="post" name="outsideAccess" onSubmit="return validateInput();">
	<input type="hidden" name="section" value="outsideAccess">
	<input type="hidden" name="action" value="add">
	<table width="600">
		<tr>
			<td><input type="checkbox" name="allow80" value="1" '.(($allowAccessOn80==1)?'checked':'').'></td>
			<td>Allow outside access to the website on port 80</td>
			<td>&nbsp;</td>
		</tr>		
		<tr>
			<td><input type="checkbox" name="allowOnPort" value="1" '.(($allowAccessOnPort==1)?'checked':'').'></td>
			<td>Allow outside to the website on port</td>
			<td><input type="text" name="port" value="'.@$port.'"></td>
		</tr>
		<tr>
			<td><input type="checkbox" name="allowOnPassword" value="1" '.(isset($remote)?'checked':'').' onClick="setAntiHanging();"></td>
			<td>Allow Pluto tech support temporary access using this password</td>
			<td><input type="password" name="password" value="'.(isset($remote)?$remote:'').'"></td>
		</tr>
		<tr>
			<td>&nbsp;</td>
			<td>Confirm password</td>
			<td><input type="password" name="password1" value="'.(isset($remote)?$remote:'').'"></td>
		</tr>
		<tr>
			<td><input type="checkbox" name="RA_CheckRemotePort" value="1" '.(((int)@$RA_CheckRemotePort==1)?'checked':'').'></td>
			<td>Enable anti-hanging measure for Remote Assistance</td>
			<td>&nbsp;</td>
		</tr>		
		<tr>
			<td colspan="3">You will need to provide your support rep the password and the following installation number: <b>'.$_SESSION['installationID'].'</b></td>
		</tr>
		<tr>
			<td><input type="checkbox" name="sendErrorsToPluto" value="1" '.((@$sendErrorsToPluto==1)?'checked':'').'></td>
			<td>Send errors to pluto</td>
			<td>&nbsp;</td>
		</tr>		
		<tr>
			<td>&nbsp;</td>
			<td colspan="2">Pluto monitors itself and prepares reports if there are any software bugs or crashes. There is no confidential information in these reports--just technical stuff about the cause of the crash (core dumps). By sharing this information Pluto can make the product any better. Also, if your system has a problem that we have fixed we will send you an email and let you know.</td>
		</tr>		
		<tr>
			<td colspan="3" align="center"><input type="submit" class="button" name="save" value="Update"></td>
		</tr>
	</table>
		<input type="hidden" name="oldAllow80" value="'.$allowAccessOn80.'">
		<input type="hidden" name="oldAllowOnPort" value="'.$allowAccessOnPort.'">
		<input type="hidden" name="oldPort" value="'.@$port.'">
	</form>
		';
	} else {
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=outsideAcces&error=You are not authorised to change the installation.");
			exit();
		}
		
		if(isset($_POST['save'])){
			
			$oldSendErrorsToPluto=(int)@$sendErrorsToPluto;
			$newSendErrorsToPluto=(int)@$_POST['sendErrorsToPluto'];
			if($oldSendErrorsToPluto==0){
				if($newSendErrorsToPluto==1){
					if($rowDSS['FK_Device']!=''){
						$dbADO->Execute('UPDATE Device_StartupScript SET Enabled=1, Parameter=? WHERE FK_Device=? AND FK_StartupScript=?',array('',$coreID,$GLOBALS['ProcessLogs']));
					}else{
						$dbADO->Execute('INSERT INTO Device_StartupScript (FK_Device, FK_StartupScript,Enabled,Parameter) VALUES (?,?,1,?)',array($coreID,$GLOBALS['ProcessLogs'],''));
					}
				}
			}else{
				if($newSendErrorsToPluto==0){
					if(isset($coreID)){
						$dbADO->Execute('UPDATE Device_StartupScript SET Parameter=0 WHERE FK_Device=? AND FK_StartupScript=?',array($coreID,$GLOBALS['ProcessLogs']));
					}
				}
			}
		
			

			$allow80=(int)@$_POST['allow80'];
			$oldAllow80=(int)@$_POST['oldAllow80'];

			$oldAllowOnPort=@(int)$_POST['oldAllowOnPort'];
			$allowOnPort=@(int)$_POST['allowOnPort'];
			$oldPort=(int)@$_POST['oldPort'];
			$port=(int)@$_POST['port'];

			if($oldAllow80==0){
				if($allow80==1){
					$dbADO->Execute('INSERT INTO Firewall (Protocol,SourcePort,RuleType) VALUES (?,?,?)',array('tcp','80','core_input'));
				}
			}else{
				if($allow80==0){
					$dbADO->Execute('DELETE FROM Firewall WHERE RuleType=? AND SourcePort=?',array('core_input','80'));
				}
			}
			if($oldAllowOnPort==0){
				if($allowOnPort==1){
					$dbADO->Execute('INSERT INTO Firewall (Protocol,DestinationPort,RuleType,SourcePort,DestinationIP) VALUES (?,?,?,?,?)',array('tcp','80','port_forward',$port,'127.0.0.1'));
				}
			}else{
				if($allowOnPort==0){
					$dbADO->Execute('DELETE FROM Firewall WHERE RuleType=? AND SourcePort=? AND DestinationPort=? AND DestinationIP=?',array('port_forward','80',$oldPort,'127.0.0.1'));
				}elseif($port!=$oldPort){
					$dbADO->Execute('UPDATE Firewall SET SourcePort=? WHERE RuleType=? AND SourcePort=? AND DestinationIP=?',array($port,'port_forward','80','127.0.0.1'));
				}
			}
			exec('sudo -u root /usr/pluto/bin/Network_Firewall.sh');

			if(isset($_POST['allowOnPassword'])){
				$password=$_POST['password'];
				writeToFile($accessFile, 'remote',@$remote,$password);
			}else {
				removeFromFile('remote',$accessFile);
			}
			
			if((int)@$RA_CheckRemotePort==1){
				if(!isset($_POST['RA_CheckRemotePort'])){
					writeToFile($accessFile, 'RA_CheckRemotePort',1,0);	
				}
			}else{
				if(isset($_POST['RA_CheckRemotePort'])){
					writeToFile($accessFile, 'RA_CheckRemotePort',0,1);
				}
			}

			exec('sudo -u root /usr/pluto/bin/SetupRemoteAccess.sh');
			header("Location: index.php?section=outsideAccess&msg=Remote access was updated.");
			exit();
		}

		header("Location: index.php?section=outsideAccess");
	}
	
		
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Outside Access');			
	$output->output();  		
}

function writeToFile($accessFile, $variable,$oldValue,$newValue)
{
	$oldFileArray=@file($accessFile);	
	if(!$oldFileArray){
		header("Location: index.php?section=outsideAccess&error=Cannot open ".$accessFile." for reading.");
		exit();
	}
	$oldFile=implode('',$oldFileArray);
	$stringToReplace=$variable.'='.$oldValue;
	if(ereg($stringToReplace,$oldFile)){
		$newFile=str_replace($stringToReplace,$variable.'='.$newValue,$oldFile);
	}
	else
		$newFile=$oldFile.$variable.'='.$newValue."\n";
	if(!is_writable($accessFile)){
		header("Location: index.php?section=outsideAccess&error=Cannot write to ".$accessFile);
		exit();
	}
	$handle = fopen($accessFile, 'w');

	if (!fwrite($handle, $newFile)) {
		header("Location: index.php?section=outsideAccess&error=Cannot write to ".$accessFile);
		exit();
	}
	fclose($handle);
}

function removeFromFile($variable,$accessFile)
{
	$oldFileArray=@file($accessFile);	
	if(!$oldFileArray){
		header("Location: index.php?section=outsideAccess&error=Cannot open ".$accessFile." for reading.");
		exit();
	}
	$oldFile='';
	foreach($oldFileArray AS $line)
		if(!ereg($variable,$line))
			$oldFile.=$line;
	if(!is_writable($accessFile)){
		header("Location: index.php?section=outsideAccess&error=Cannot write to ".$accessFile);
		exit();
	}
	$handle = fopen($accessFile, 'w');

	if (!fwrite($handle, $oldFile)) {
		header("Location: index.php?section=outsideAccess&error=Cannot write to ".$accessFile);
		exit();
	}
	fclose($handle);
}

?>
