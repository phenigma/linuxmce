<?
function outsideAccess($output,$dbADO) {
	/* @var $dbADO ADOConnection */
	/* @var $res ADORecordSet */
	
	// TODO: change path to /etc/pluto.comf
	$accessFile='/etc/pluto.conf';
	exec('cat '.$accessFile.' | grep -v -E "^#|^$" ',$retArray);	
	foreach ($retArray as $comf){
		parse_str($comf);
	}
	
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';

	$installationID = cleanInteger($_SESSION['installationID']);
	
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
	<table>
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
			if(isset($_POST['allowOnPort'])){
				$port=(int)$_POST['port'];
				writeToFile($accessFile, 'Website', @$Website,$port);
				
				if(!isset($Website))
					$dbADO->Execute('INSERT INTO Firewall (Protocol,SourcePort,RuleType) VALUES (?,?,?)',array('tcp',$port,'Website'));
				else 
					$dbADO->Execute('UPDATE Firewall SET SourcePort=? WHERE RuleType=?',array($port, 'Website'));
			}else {
				removeFromFile('Website',$accessFile);
				$dbADO->Execute('DELETE FROM Firewall WHERE RuleType=?','Website');
			}
			
			if(isset($_POST['allowOnPassword'])){
				$password=$_POST['password'];
				writeToFile($accessFile, 'remote',@$remote,$password);
			}else {
				removeFromFile('remote',$accessFile);
				$dbADO->Execute('DELETE FROM Firewall WHERE RuleType=?','Website');
			}

			exec('/usr/pluto/bin/SetupRemoteAcces.sh');
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
