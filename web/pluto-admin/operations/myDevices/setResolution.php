<?php
function setResolution($output,$dbADO) {
	//$dbADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$mdID=(int)$_REQUEST['mdID'];
	$installationID = (int)@$_SESSION['installationID'];
	$videoSettingsArray=array('M'=>'Manual', '640'=>'640x480', '800'=>'800x600', '1024'=>'1024x768', '1224'=>'1280x1024', '1600'=>'1600x1200', '480P'=>'480p', '720P'=>'720p', '1280I'=>'1280i', '1280P'=>'1280P');
	$refreshArray=array('50'=>'50 hz', '60'=>'60 hz', '72'=>'72 hz','75'=>'75 hz', '80'=>'80 hz');
	
	$oldValues=getFieldsAsArray('Device_DeviceData','IK_DeviceData',$dbADO,'WHERE FK_Device='.$mdID.' AND FK_DeviceData='.$GLOBALS['VideoSettings']);
	if(!is_null($oldValues['IK_DeviceData'])){
		$oldDD=explode('/',$oldValues['IK_DeviceData'][0]);
		$oldResolution=@$oldDD[0];
		$oldRefresh=@$oldDD[1];
	}
	
	if ($action=='form') {

		$out.='

		<div class="err">'.@$_GET['error'].'</div>
		<div class="confirm" align="center"><B>'.strip_tags(@$_GET['msg']).'</B></div>
		
		<form action="index.php" method="post" name="setResolution">
		<input type="hidden" name="section" value="setResolution">
		<input type="hidden" name="action" value="confirm">
		<input type="hidden" name="mdID" value="'.$mdID.'">
		
<span style="padding-left:10px; padding-right:10px;">
<h3>Change resolution and refresh</h3>		
Choose resolution and refresh and click "test resolution" button.
</span>
<br><br>';



$out.='
<table>
	<tr>
		<td><B>Resolution</B></td>
		<td>'.pulldownFromArray($videoSettingsArray,'resolution',@$oldResolution).'</td>
	</tr>
	<tr>
		<td><B>Refresh</B></td>
		<td>'.pulldownFromArray($refreshArray,'refresh',@$oldRefresh).'</td>
	</tr>
	<tr>
		<td colspan="2" align="center"><input type="submit" class="button" name="test" value="Test resolution and refresh"></td>
	</tr>
</table>		
		</form>
		';
		
	} elseif($action=='confirm'){
		$resolution=$_POST['resolution'];
		$refresh=$_POST['refresh'];
		$mdDetails=getFieldsAsArray('Device','IPaddress',$dbADO,'WHERE PK_Device='.$mdID);
		$ipAddress=$mdDetails['IPaddress'][0];
		
		// TODO for Radu: put correct script
		exec('pathto/script_of_radu',$retArray);
		
		$out.='
		<form action="index.php" method="post" name="setResolution">
		<input type="hidden" name="section" value="setResolution">
		<input type="hidden" name="action" value="update">
		<input type="hidden" name="mdID" value="'.$mdID.'">
		<input type="hidden" name="resolution" value="'.$resolution.'">
		<input type="hidden" name="refresh" value="'.$refresh.'">
		
<span style="padding-left:10px; padding-right:10px;">
<h3>Do you want to keep this settings?</h3>		
</span>
<br><br>';



$out.='
<table>
	<tr>
		<td><B>Resolution</B></td>
		<td>'.$videoSettingsArray[$resolution].'</td>
	</tr>
	<tr>
		<td><B>Refresh</B></td>
		<td>'.$refreshArray[$refresh].'</td>
	</tr>
	<tr>
		<td colspan="2" bgcolor="#F0F3F8">'.join('<br>',$retArray).'</td>
	</tr>
	<tr>
		<td colspan="2" align="center"><input type="submit" class="button" name="submit" value="Yes"> <input type="button" class="button" name="submit" value="No" onClick="self.location=\'index.php?section=setResolution&mdID='.$mdID.'\'"></td>
	</tr>
</table>		
</form>';
			
	}else {

		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=setResolution&error=You are not authorised to change the installation.");
			exit(0);
		}

		$resolution=$_POST['resolution'];
		$refresh=$_POST['refresh'];
		$newVS=$resolution.'/'.$refresh;
		$dbADO->Execute('UPDATE Device_DeviceData SET IK_DeviceData=? WHERE FK_Device=? AND FK_DeviceData=?',array($newVS,$mdID,$GLOBALS['VideoSettings']));

		header("Location: index.php?section=setResolution&mdID=$mdID&msg=Resolution and refresh updated.");
	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Setup resolution and refresh');			
	$output->output();
}
?>
