<?
function deleteCriticalDevice($output,$dbADO) {
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';

	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$deviceID = isset($_REQUEST['deviceID'])?cleanInteger($_REQUEST['deviceID']):0;
	if($deviceID==0){
		header("Location: index.php?section=editDeviceParams&deviceID=$deviceID&error=Invalid device ID specified.");
		exit();
	}
	
	// check if the user has the right to modify installation
	$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
	if (!$canModifyInstallation){
		header("Location: index.php?section=editDeviceParams&deviceID=$deviceID&error=You are not authorised to change the installation.");
		exit();
	}

	if($action=='form'){
		$out.='
	<div class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
	<form action="index.php" method="POST" name="deleteCriticalDevice">
	<input type="hidden" name="section" value="deleteCriticalDevice">
	<input type="hidden" name="action" value="del">	
	<input type="hidden" name="deviceID" value="'.$deviceID.'">
		<font color="red">
		<h2 align="center">This is a critical device !!! If you delete it, you will break you installation.</h2><br>
		<h3 align="center"><B>Do you still want to delete it?</B></h3>
		<table width="50%" align="center">
			<tr>
				<td align="center">Please confirm the operation by typing your password below:<br><br> <input type="password" name="confirmPassword"><br><br></td>
			</tr>
			<tr>
				<td align="center"><input type="submit" class="button" name="del" value="Yes, delete it"> <input type="button" class="button" name="cancel" value="No way !!!" onClick="self.location=\'index.php?section=editDeviceParams&deviceID='.$deviceID.'\'"></td>
			</tr>
		</table>
		</font>
	</form>';
		
	}else{
		$confirmPassword=$_POST['confirmPassword'];
		if($confirmPassword!=$_SESSION['password']){
			header("Location: index.php?section=editDeviceParams&deviceID=$deviceID&error=Password does not match.");
			exit();
		}
		$deviceFields=getFieldsAsArray('Device','FK_DeviceTemplate,IPaddress,MACaddress',$dbADO,'WHERE PK_Device='.$deviceID);
		if($deviceFields['FK_DeviceTemplate'][0]==$GLOBALS['rootMediaDirectorsID']){
			$cmd='sudo -u root /usr/pluto/bin/DeleteMD.sh "'.$deviceFields['IPaddress'][0].'" "'.$deviceFields['MACaddress'][0].'"';
			exec($cmd);
		}
		
		deleteDevice($deviceID,$dbADO);
		setDCERouterNeedConfigure($_SESSION['installationID'],$dbADO);

		$out.='
	<script>
		alert("Device(s) deleted!");
		top.frames["treeframe"].location="index.php?section=leftMenu";
		document.location.href="index.php?section=myDevices&action=showBasicInfo";
	</script>
	<noscript>
		Device deleted! Please <a href="index.php?section=myDevices&action=showBasicInfo">click here</a> to go to main area!
	</noscript>
	';
	}

	$output->setScriptCalendar('null');

	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}