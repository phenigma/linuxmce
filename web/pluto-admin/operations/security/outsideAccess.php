<?
function outsideAccess($output,$dbADO) {
	/* @var $dbADO ADOConnection */
	/* @var $res ADORecordSet */
	
	$accessFile='/etc/pluto.conf';
	exec('cat '.$accessFile.' | grep -v -E "^#|^$" ',$retArray);	
	foreach ($retArray as $comf){
		parse_str($comf);
	}
	
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';

	$installationID = cleanInteger($_SESSION['installationID']);
	
	$queryDSS='
		SELECT Device_StartupScript.Enabled, FK_Device,Parameter
		FROM Device_StartupScript
		INNER JOIN Device ON FK_Device=PK_Device
		WHERE FK_DeviceTemplate=? AND FK_Installation=? AND FK_StartupScript=?';
	$resDSS=$dbADO->Execute($queryDSS,array($GLOBALS['rootCoreID'],$installationID,$GLOBALS['ProcessLogs']));
	if($resDSS->RecordCount()>0){
		$rowDSS=$resDSS->FetchRow();
		$sendErrorsToPluto=($rowDSS['Enabled']==1 && $rowDSS['Parameter']!='0')?1:0;
		$coreID=$rowDSS['FK_Device'];
	}
	$out.='<div align="left"><h3>Outside Access</h3></div>';
	
	if ($action=='form') {
		

		if(count($retArray)==0){
			$out.='<div class="err">Insuffient rights: pluto.conf file cannot be opened.</div>';
		}
		$out.='<div class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>';
		$out.='
	<div class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
	<form action="index.php" method="post" name="outsideAccess">
	<input type="hidden" name="section" value="outsideAccess">
	<input type="hidden" name="action" value="add">
	<table width="500">
		<tr>
			<td><input type="checkbox" name="allowOnPort" value="1" '.(isset($Website)?'checked':'').'></td>
			<td>Allow outside to tthe website on port</td>
			<td><input type="text" name="port" value="'.(isset($Website)?$Website:'80').'"></td>
		</tr>
		<tr>
			<td><input type="checkbox" name="allowOnPassword" value="1" '.(isset($remote)?'checked':'').'></td>
			<td>Allow outside access with password</td>
			<td><input type="password" name="password" value="'.(isset($remote)?$remote:'').'"></td>
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
					if(isset($coreID)){
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
		
			
			if(isset($_POST['allowOnPort'])){
				$port=(int)$_POST['port'];
				writeToFile($accessFile, 'Website', @$Website,$port);
				
				if(!isset($Website))
					$dbADO->Execute('INSERT INTO Firewall (Protocol,SourcePort,RuleType) VALUES (?,?,?)',array('tcp',$port,'core_input'));
				else 
					$dbADO->Execute('UPDATE Firewall SET SourcePort=? WHERE RuleType=? AND SourcePort=?',array($port, 'core_input',$Website));
			}else {
				removeFromFile('Website',$accessFile);
				$dbADO->Execute('DELETE FROM Firewall WHERE RuleType=? AND SourcePort=?',array('core_input',$Website));
			}
			
			if(isset($_POST['allowOnPassword'])){
				$password=$_POST['password'];
				writeToFile($accessFile, 'remote',@$remote,$password);
			}else {
				removeFromFile('remote',$accessFile);
				$dbADO->Execute('DELETE FROM Firewall WHERE RuleType=?','Website');
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
	if($oldValue!=''){
		$stringToReplace=$variable.'='.$oldValue;
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
