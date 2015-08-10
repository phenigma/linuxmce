<?

	// include language files
	includeLangFile('common.lang.php');
	includeLangFile('outsideAccess.lang.php');

function outsideAccess($output,$dbADO) {
	
	/* @var $dbADO ADOConnection */
	/* @var $res ADORecordSet */

	$accessFile=$GLOBALS['pluto.conf'];
	exec('cat '.$accessFile.' | grep -v -E "^#|^$" ',$retArray);	
	foreach ($retArray as $comf){
		parse_str(preg_replace('/ ?= ?/','=',$comf));
	}
	
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';

	$installationID = cleanInteger($_SESSION['installationID']);
	
	$queryDSS='
		SELECT Device_StartupScript.Enabled, PK_Device, Parameter,FK_Device 
		FROM Device 
		INNEr JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
		LEFT JOIN Device_StartupScript ON FK_Device=PK_Device AND FK_StartupScript=?
		WHERE FK_DeviceCategory=? AND FK_Installation=?';

	$resDSS=$dbADO->Execute($queryDSS,array($GLOBALS['ProcessLogs'],$GLOBALS['CategoryCore'],$installationID));
	if($resDSS->RecordCount()>0){
		$rowDSS=$resDSS->FetchRow();
		$sendErrorsToPluto=($rowDSS['Enabled']==1 && $rowDSS['Parameter']!='0')?1:0;
		$coreID=$rowDSS['PK_Device'];
	}
	
	$resAllow80=$dbADO->Execute('SELECT * FROM Firewall WHERE RuleType=? AND DestinationPort=80','input');
	$allowAccessOn80=($resAllow80->RecordCount()>0)?1:0;
		
	$resAllowPort=$dbADO->Execute('SELECT * FROM Firewall WHERE RuleType=? AND DestinationPort=80 AND DestinationIP=?',array('port_forward (NAT)','127.0.0.1'));
	$allowAccessOnPort=($resAllowPort->RecordCount()>0)?1:0;
	$rowAccessOnPort=$resAllowPort->FetchRow();
	$port=$rowAccessOnPort['SourcePort'];

	$resAllowSSL443=$dbADO->Execute('SELECT * FROM Firewall WHERE RuleType=? AND DestinationPort=443','input');
	$allowSSLAccessOn443=($resAllowSSL443->RecordCount()>0)?1:0;

	$resAllowSSLPort=$dbADO->Execute('SELECT * FROM Firewall WHERE RuleType=? AND DestinationPort=443 AND DestinationIP=?',array('port_forward (NAT)','127.0.0.1'));
	$allowSSLAccessOnPort=($resAllowSSLPort->RecordCount()>0)?1:0;
	$rowSSLAccessOnPort=$resAllowSSLPort->FetchRow();
	$sslport=$rowSSLAccessOnPort['SourcePort'];
	
	if ($action=='form') {
		

		if(count($retArray)==0){
			$out.='<div class="err">'.translate('TEXT_ERROR_OPENING_PLUTO_CONF_CONST').'</div>';
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
		portValue=parseInt(document.outsideAccess.sslport.value);
		if(document.outsideAccess.allowSSLOnPort.checked && (isNaN(portValue) || portValue==443 || portValue<=0)){
			alert("Please type the port, other value than 443.");
			document.outsideAccess.sslport.focus();
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
	<span class="err"><B>'.(isset($remote)?translate('TEXT_PROVIDE_PASSWORD_AND_INSTALLATION_CONST').' '.$installationID.'-'.$remote:'').'</B></span>
	<table width="600">
		<tr>
			<td colspan="3">'.translate('TEXT_REMOTE_ASSISTANCE_CONST').': <B>'.(isset($remote)?translate('TEXT_ENABLED_CONST'):translate('TEXT_DISABLED_CONST')).'</B> <input type="submit" name="'.(isset($remote)?'deactivate':'activate').'" class="button" value="'.(isset($remote)?translate('TEXT_DISABLE_CONST'):translate('TEXT_ENABLE_CONST')).'"></td>
		</tr>	
		<tr>
			<td><input type="checkbox" name="allow80" value="1" '.(($allowAccessOn80==1)?'checked':'').'></td>
			<td>'.translate('TEXT_ALLOW_OUTSIDE_ACCESS_80_CONST').'</td>
			<td>&nbsp;</td>
		</tr>		
		<tr>
			<td><input type="checkbox" name="allowOnPort" value="1" '.(($allowAccessOnPort==1)?'checked':'').'></td>
			<td>'.translate('TEXT_ALLOW_OUTSIDE_ACCESS_OTHER_CONST').'</td>
			<td><input type="text" name="port" value="'.@$port.'"></td>
		</tr>
		<tr>
			<td><input type="checkbox" name="allow443" value="1" '.(($allowSSLAccessOn443==1)?'checked':'').'></td>
			<td>'.translate('TEXT_ALLOW_OUTSIDE_ACCESS_SSL443_CONST').'</td>
			<td>&nbsp;</td>
		</tr>
		<tr>
			<td><input type="checkbox" name="allowSSLOnPort" value="1" '.(($allowSSLAccessOnPort==1)?'checked':'').'></td>
			<td>'.translate('TEXT_ALLOW_OUTSIDE_ACCESS_SSLOTHER_CONST').'</td>
			<td><input type="text" name="sslport" value="'.@$sslport.'"></td>
		</tr>
		<tr>
			<td><input type="checkbox" name="RAport" value="1" '.(((int)@$RAport==22)?'checked':'').'></td>
			<td>'.translate('TEXT_USE_PORT_22_FOR_REMOTE_ASSISTANCE_CONST').'</td>
			<td>&nbsp;</td>
		</tr>		
		<tr>
			<td><input type="checkbox" name="RA_CheckRemotePort" value="1" '.(((int)@$RA_CheckRemotePort==1)?'checked':'').'></td>
			<td>'.translate('TEXT_ENABLE_ANTI_HANGING_MEASURE_CONST').'</td>
			<td>&nbsp;</td>
		</tr>		
		<tr>
			<td><input type="checkbox" name="sendErrorsToPluto" value="1" '.((@$sendErrorsToPluto==1)?'checked':'').'></td>
			<td>'.translate('TEXT_SEND_ERRORS_TO_PLUTO_CONST').'</td>
			<td>&nbsp;</td>
		</tr>		
		<tr>
			<td>&nbsp;</td>
			<td colspan="2">'.translate('TEXT_OUTSIDE_ACCESS_NOTES_CONST').'</td>
		</tr>		
		<tr>
			<td colspan="3" align="center"><input type="submit" class="button" name="save" value="'.translate('TEXT_UPDATE_CONST').'"> <input type="reset" class="button" name="cancelBtn" value="'.translate('TEXT_CANCEL_CONST').'"></td>
		</tr>
	</table>
		<input type="hidden" name="oldAllow80" value="'.$allowAccessOn80.'">
		<input type="hidden" name="oldAllowOnPort" value="'.$allowAccessOnPort.'">
		<input type="hidden" name="oldPort" value="'.@$port.'">
		<input type="hidden" name="oldAllow443" value="'.$allowSSLAccessOn443.'">
		<input type="hidden" name="oldAllowSSLOnPort" value="'.$allowSSLAccessOnPort.'">
		<input type="hidden" name="oldSSLPort" value="'.@$sslport.'">
	</form>
		';
	} else {
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=outsideAcces&error=".translate('TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST'));
			exit();
		}
		
		if(isset($_POST['activate'])){
			// /usr/pluto/bin/RA-handler.sh {--enable|--disable
			$cmd='sudo -u root /usr/pluto/bin/RA-handler.sh --enable';
			exec_batch_command($cmd);
		}

		if(isset($_POST['deactivate'])){
			$cmd='sudo -u root /usr/pluto/bin/RA-handler.sh --disable';
			exec_batch_command($cmd);
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
			$allow443=(int)@$_POST['allow443'];
			$oldAllow443=(int)@$_POST['oldAllow443'];

			$oldAllowOnPort=@(int)$_POST['oldAllowOnPort'];
			$allowOnPort=@(int)$_POST['allowOnPort'];
			$oldPort=(int)@$_POST['oldPort'];
			$port=(int)@$_POST['port'];
			$oldAllowSSLOnPort=@(int)$_POST['oldAllowSSLOnPort'];
			$allowSSLOnPort=@(int)$_POST['allowSSLOnPort'];
			$oldSSLPort=(int)@$_POST['oldSSLPort'];
			$sslport=(int)@$_POST['sslport'];

			if($oldAllow80==0){
				if($allow80==1){
					$dbADO->Execute('INSERT INTO Firewall (Protocol,SourcePort,RuleType) VALUES (?,?,?)',array('tcp','80','input'));
				}
			}else{
				if($allow80==0){
					$dbADO->Execute('DELETE FROM Firewall WHERE RuleType=? AND SourcePort=?',array('input','80'));
				}
			}
			if($oldAllowOnPort==0){
				if($allowOnPort==1){
					$dbADO->Execute('INSERT INTO Firewall (Protocol,DestinationPort,RuleType,SourcePort,DestinationIP) VALUES (?,?,?,?,?)',array('tcp','80','port_forward (NAT)',$port,'127.0.0.1'));
					$dbADO->Execute('INSERT INTO Firewall (Protocol,DestinationPort,RuleType,SourcePort,DestinationIP) VALUES (?,?,?,?,?)',array('tcp','80','forward',$port,'127.0.0.1'));
				}
			}else{
				if($allowOnPort==0){
					$dbADO->Execute('DELETE FROM Firewall WHERE RuleType=? AND SourcePort=? AND DestinationPort=? AND DestinationIP=?',array('port_forward (NAT)',$oldPort,'80','127.0.0.1'));
					$dbADO->Execute('DELETE FROM Firewall WHERE RuleType=? AND SourcePort=? AND DestinationPort=? AND DestinationIP=?',array('forward',$oldPort,'80','127.0.0.1'));
				}elseif($port!=$oldPort){
					$dbADO->Execute('UPDATE Firewall SET SourcePort=? WHERE RuleType=? AND SourcePort=? AND DestinationIP=?',array($port,'port_forward (NAT)','80','127.0.0.1'));
					$dbADO->Execute('UPDATE Firewall SET SourcePort=? WHERE RuleType=? AND SourcePort=? AND DestinationIP=?',array($port,'forward','80','127.0.0.1')); 
				}
			}
			if($oldAllow443==0){
				if($allow443==1){
					$dbADO->Execute('INSERT INTO Firewall (Protocol,SourcePort,RuleType) VALUES (?,?,?)',array('tcp','443','input'));
				}
			}else{
				if($allow443==0){
					$dbADO->Execute('DELETE FROM Firewall WHERE RuleType=? AND SourcePort=?',array('input','443'));
				}
			}
			if($oldAllowSSLOnPort==0){
				if($allowSSLOnPort==1){
					$dbADO->Execute('INSERT INTO Firewall (Protocol,DestinationPort,RuleType,SourcePort,DestinationIP) VALUES (?,?,?,?,?)',array('tcp','443','port_forward (NAT)',$sslport,'127.0.0.1'));
					$dbADO->Execute('INSERT INTO Firewall (Protocol,DestinationPort,RuleType,SourcePort,DestinationIP) VALUES (?,?,?,?,?)',array('tcp','443','forward',$sslport,'127.0.0.1'));
				}
			}else{
				if($allowSSLOnPort==0){
					$dbADO->Execute('DELETE FROM Firewall WHERE RuleType=? AND SourcePort=? AND DestinationPort=? AND DestinationIP=?',array('port_forward (NAT)',$oldSSLPort,'443','127.0.0.1'));
					$dbADO->Execute('DELETE FROM Firewall WHERE RuleType=? AND SourcePort=? AND DestinationPort=? AND DestinationIP=?',array('forward',$oldSSLPort,'443','127.0.0.1'));
				}elseif($sslport!=$oldSSLPort){
					$dbADO->Execute('UPDATE Firewall SET SourcePort=? WHERE RuleType=? AND SourcePort=? AND DestinationIP=?',array($sslport,'port_forward (NAT)','443','127.0.0.1'));
					$dbADO->Execute('UPDATE Firewall SET SourcePort=? WHERE RuleType=? AND SourcePort=? AND DestinationIP=?',array($sslport,'forward','443','127.0.0.1'));
				}
			}
			exec('sudo -u root /usr/pluto/bin/Network_Firewall.sh');

			if(isset($_POST['RAport'])){
				if(@$RAport!=22){
					writeToFile($accessFile, 'RAport',0,22);
					$suffix=' restart';
				}
			}else{
				if(@$RAport==22){
					removeFromFile('RAport',$accessFile);
					$suffix=' restart';
				}
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
			$cmd='sudo -u root /usr/pluto/bin/SetupRemoteAccess.sh'.@$suffix;
			exec($cmd);
			
			header("Location: index.php?section=outsideAccess&msg=".translate('TEXT_REMOTE_ACCESS_UPDATED_CONST'));
			exit();
		}

		header("Location: index.php?section=outsideAccess");
	}

	$output->setMenuTitle(translate('TEXT_WIZARD_CONST').' |');
	$output->setPageTitle(translate('TEXT_OUTSIDE_ACCESS_CONST'));
	$output->setNavigationMenu(array(translate('TEXT_OUTSIDE_ACCESS_CONST')=>'index.php?section=outsideAccess'));	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.translate('TEXT_OUTSIDE_ACCESS_CONST'));			
	$output->output();  		
}

function writeToFile($accessFile, $variable,$oldValue,$newValue)
{	
	$oldFileArray=@file($accessFile);	
	if(!$oldFileArray){
		header("Location: index.php?section=outsideAccess&error=".translate('TEXT_ERROR_CANNOT_OPEN_FILE_FOR_READING_CONST')." ".$accessFile." ");
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
		header("Location: index.php?section=outsideAccess&error=".translate('TEXT_ERROR_CANNOT_WRITE_TO_FILE_CONST')." ".$accessFile);
		exit();
	}
	$handle = fopen($accessFile, 'w');

	if (!fwrite($handle, $newFile)) {
		header("Location: index.php?section=outsideAccess&error=".translate('TEXT_ERROR_CANNOT_WRITE_TO_FILE_CONST')." ".$accessFile);
		exit();
	}
	fclose($handle);
}

function removeFromFile($variable,$accessFile)
{
	$oldFileArray=@file($accessFile);	
	if(!$oldFileArray){
		header("Location: index.php?section=outsideAccess&error=".translate('TEXT_ERROR_CANNOT_OPEN_FILE_FOR_READING_CONST')." ".$accessFile);
		exit();
	}
	$oldFile='';
	foreach($oldFileArray AS $line)
		if(!ereg($variable,$line))
			$oldFile.=$line;
	if(!is_writable($accessFile)){
		header("Location: index.php?section=outsideAccess&error=".translate('TEXT_ERROR_CANNOT_WRITE_TO_FILE_CONST')." ".$accessFile);
		exit();
	}
	$handle = fopen($accessFile, 'w');

	if (!fwrite($handle, $oldFile)) {
		header("Location: index.php?section=outsideAccess&error=".translate('TEXT_ERROR_CANNOT_WRITE_TO_FILE_CONST')." ".$accessFile);
		exit();
	}
	fclose($handle);
}


function getRemotePorts($installationID){
	global $LinuxMCEHost;
	$cmd='wget \''.$LinuxMCEHost.'get_ra_ports.php?installationID='.$installationID.'\' --header=\'User-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.8.0.4) Gecko/20060508 Firefox/1.5.0.4\' -O -';
	$out=exec_batch_command($cmd,1);	
	
	// output like Ports=SSH=10003&Web=10004&SSHnomon=10005#
	if(substr($out,0,5)!='Ports' && substr($out,-1)!='#'){
		return false;
	}
	return substr($out,6,-1);
}

function freeRemotePorts($installationID){
	global $LinuxMCEHost;
	$cmd='wget \''.$LinuxMCEHost.'get_ra_ports.php?installationID='.$installationID.'&action=del\' --header=\'User-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.8.0.4) Gecko/20060508 Firefox/1.5.0.4\' -O -';
	$out=exec_batch_command($cmd,1);	
	
	return ($out=='Success')?true:false;
	
}
?>
