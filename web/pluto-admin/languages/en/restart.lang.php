<?
function restart($output,$dbADO) {
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';

	
	if($action=='form'){
		$queryDevice='
			SELECT Device.*
			FROM Device
			INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
			WHERE FK_DeviceCategory=? AND Device.FK_Installation=?';
		$resDevice=$dbADO->Execute($queryDevice,array($GLOBALS['rootCoreID'],$_SESSION['installationID']));
		if($resDevice->RecordCount()!=0){
			$rowDevice=$resDevice->FetchRow();
			$coreID=$rowDevice['PK_Device'];
		}

		$MDArray=getDevicesArrayFromCategory($GLOBALS['rootMediaDirectors'],$dbADO);
		$out.=setLeftMenu($dbADO).'
		<script>
		function confirmAction()
		{
			if(confirm(\'Do you confirm the action?\')){
				return true;
			}else
				return false;
		}
		</script>
		
		<div align="center" class="err">'.@$_REQUEST['error'].'</div>
		<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
		<h3 align="center">Restart</h3>
		<form action="index.php" method="POST" name="restart" onSubmit="return confirmAction();">
		<input type="hidden" name="section" value="restart">
		<input type="hidden" name="action" value="update">
		
		<table cellpadding="3" align="center">
			<tr>
				<td colspan="6" align="center"><input type="submit" class="button" name="quick_reload" value="Quick Reload Router"></td>
			</tr>
			<tr>
				<td colspan="6" align="center">&nbsp;</td>
			</tr>		
			<tr bgcolor="#EEEEEE">
				<td colspan="3"><B>Core (reset everything)</B></td>
				<td><input type="submit" class="button" name="halt_core" value="Halt"></td>
				<td><input type="submit" class="button" name="suspend_core" value="Suspend"></td>
				<td><input type="submit" class="button" name="reboot_core" value="Reboot"></td>
			</tr>
			<tr bgcolor="#DDDDDD">
				<td colspan="6" align="center"><B>Media Directors</B></td>
			</tr>		
			<tr bgcolor="#EEEEEE">
				<td>&nbsp;</td>
				<td colspan="2" align="center"><B>Boot from</B></td>
				<td colspan="3"></td>
			</tr>		
		
		';
		foreach($MDArray AS $mdID=>$description){
			$out.='
			<tr>
				<td><B>'.$description.'</B></td>
				<td><input type="radio" name="boot_'.$mdID.'" value="V" onClick="self.location=\'index.php?section=restart&action=V&device='.$mdID.'\'"> Hdd </td>
				<td><input type="radio" name="boot_'.$mdID.'" value="net" onClick="self.location=\'index.php?section=restart&action=N&device='.$mdID.'\'"> Net</td>
				<td><input type="submit" class="button" name="halt_'.$mdID.'" value="Halt"></td>
				<td><input type="submit" class="button" name="suspend_'.$mdID.'" value="Suspend"></td>
				<td><input type="submit" class="button" name="reboot_'.$mdID.'" value="Reboot"></td>
			</tr>			
			';
		}
		$out.='
		</table>
			<input type="hidden" name="coreID" value="'.$coreID.'">
			<input type="hidden" name="MDArray" value="'.join(',',array_keys($MDArray)).'">
		</form>';
	}else{
	// process area
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation) {	
			header('Location: index.php?section=restart&error=You are not allowed to change the installation');
			exit();
		}
		$coreID=(int)@$_POST['coreID'];
		$MDArray=explode(',',@$_POST['MDArray']);

		if(isset($_POST['halt_core'])){
			$command='/usr/pluto/bin/MessageSend -targetType template 0 27 1 268 21 H 2 '.$coreID;
			exec($command);
		}

		if(isset($_POST['suspend_core'])){
			$command='/usr/pluto/bin/MessageSend -targetType template 0 27 1 268 21 S 2 '.$coreID;
			exec($command);
		}

		if(isset($_POST['reboot_core'])){
			$command='sudo -u root reboot';
			exec($command);
		}

		foreach ($MDArray AS $mdID){
			if(isset($_POST['halt_'.$mdID])){
				$command='/usr/pluto/bin/MessageSend -targetType template 0 27 1 268 21 H 2 '.$mdID;
				exec($command);
			}
	
			if(isset($_POST['suspend_'.$mdID])){
				$command='/usr/pluto/bin/MessageSend -targetType template 0 27 1 268 21 S 2 '.$mdID;
				exec($command);
			}
	
			if(isset($_POST['reboot_'.$mdID])){
				// extract IP
				$mdData=getAssocArray('Device','PK_Device','IPAddress',$dbADO,'WHERE PK_Device='.$mdID);
				$command='ssh root@'.$mdData[$mdID].' reboot';
				exec($command);
			}
		}
		
		if(isset($_REQUEST['device']) && (int)$_REQUEST['device']!=0){
			$device=(int)$_REQUEST['device'];
			$command='/usr/pluto/bin/MessageSend -targetType template 0 27 1 268 21 '.$action.' 2 '.$device;
			exec($command);
		}

		if(isset($_POST['quick_reload'])){
			$command='/usr/pluto/bin/MessageSend localhost 0 -1000 7 1';
			exec($command);
		}

		header('Location: index.php?section=restart&msg=The command '.$command.' was sent');
	}
	
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}


?>