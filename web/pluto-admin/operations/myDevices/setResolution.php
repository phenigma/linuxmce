<?php
function setResolution($output,$dbADO) {
	//$dbADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$mdID=(int)$_REQUEST['mdID'];
	$installationID = (int)@$_SESSION['installationID'];
	$videoSettingsArray=array('M'=>'Manual', '640 480'=>'640x480', '800 600'=>'800x600', '1024 768'=>'1024x768', '1280 1024'=>'1280x1024', '1600 1200'=>'1600x1200', '720 480 p'=>'480p', '1280 720 p'=>'720p', '1920 1080 i'=>'1080i', '1920 1080 p'=>'1080p');
	$refreshArray=array('50'=>'50 hz', '60'=>'60 hz', '65'=>'65 hz', '72'=>'72 hz','75'=>'75 hz', '80'=>'80 hz', '100'=>'100 hz', '120'=>'120 hz');
	
	$oldValues=getFieldsAsArray('Device_DeviceData','IK_DeviceData',$dbADO,'WHERE FK_Device='.$mdID.' AND FK_DeviceData='.$GLOBALS['VideoSettings']);
	if(!is_null($oldValues['IK_DeviceData'])){
		$oldDD=explode('/',$oldValues['IK_DeviceData'][0]);
		$oldResolution=@$oldDD[0];
		$oldRefresh=@$oldDD[1];
	}
	$mdDetails=getFieldsAsArray('Device','IPaddress,FK_Device_ControlledVia',$dbADO,'WHERE PK_Device='.$mdID);
	$ipAddress=(is_null($mdDetails['FK_Device_ControlledVia'][0]))?$mdDetails['IPaddress'][0]:'127.0.0.1';

	
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

		@list($resX, $resY, $resType) = explode(' ', $resolution);
		if ($resType === "i")
		{
			$resType = "interlace";
		}
		else
		{
			$resType = "";
		}
		$force = "-f"; $force = ""; // XXX
		$cmd="sudo -u root /usr/pluto/bin/LaunchRemoteCmd.sh '$ipAddress' '/usr/pluto/bin/Xres_config.sh $resX $resY $refresh $resType $force'";
		exec($cmd, $retArray);
		
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


$answer=join(' ',$retArray);
if((ereg('Failed',$answer))){
	$out.='
	<table>
	<tr>
		<td colspan="2" bgcolor="#F0F3F8">'.$answer.'</td>
	</tr>
	<tr>
		<td colspan="2" align="center"><input type="button" class="button" name="retry" value="Try again" onClick="self.location=\'index.php?section=setResolution&mdID='.$mdID.'\'"> </td>
	</tr>
</table>';
	$answer='<span class="err">'.$answer.'</span>';
	
}else{
	preg_match("/Current resolution: *([0-9]+) *x *([0-9]+) *@ *([0-9]+) *Hz/",$answer,$matches);
	if(count($matches)!=4){
		$answer='<span class="err">Cannot retrieve valid resolution and refresh: '.$answer.'</span>';
		$noUpdate=1;
	}
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
			<td colspan="2" bgcolor="#F0F3F8">'.$answer.'</td>
		</tr>';
	if(!isset($noUpdate)){
		$out.='
		<input type="hidden" name="realResolution" value="'.@$matches[1].' '.@$matches[2].'">
		<input type="hidden" name="realRefresh" value="'.@$matches[3].'">
		<tr>
			<td align="right"><input type="checkbox" name="updateOrbiters" value="1" checked></td>
			<td>quick reload router & regenerate the UI at this resolution now?</td>
		</tr>
		<tr>
			<td colspan="2" align="center"><input type="submit" class="button" name="yesBtn" value="Yes"> <input type="submit" class="button" name="noBtn" value="No"></td>
		</tr>';
	}else{
		$out.='
		<tr>
			<td colspan="2" align="center"><input type="button" class="button" name="retry" value="Try again" onClick="self.location=\'index.php?section=setResolution&mdID='.$mdID.'\'"> </td>
		</tr>
		';
	}
	$out.='
	</table>
	</form>';
}

			
	}else {
		if(isset($_POST['yesBtn'])){
			$Answer = "Y"; // 'Y', 'N'
		
			$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
			if (!$canModifyInstallation){
				header("Location: index.php?section=setResolution&error=You are not authorised to change the installation.");
				exit(0);
			}

			$resolution=$_POST['realResolution'];
			$refresh=$_POST['realRefresh'];
			$newVS=$resolution.'/'.$refresh;
			
			$dbADO->Execute('UPDATE Device_DeviceData SET IK_DeviceData=? WHERE FK_Device=? AND FK_DeviceData=?',array($newVS,$mdID,$GLOBALS['VideoSettings']));
			if(@$_POST['updateOrbiters']==1){
				// update device data for onscreen orbiter or orbiter
				$orbiterArray=getOSDFromMD($mdID,$dbADO);
				@list($resX, $resY, $resType) = explode(' ', $resolution);	
				
				if(!is_null($orbiterArray['FK_DeviceTemplate'])){	
					
					$dbADO->Execute('UPDATE Device_DeviceData SET IK_DeviceData=? WHERE FK_Device=? AND FK_DeviceData=?',array($resX,$orbiterArray['PK_Device'],$GLOBALS['ScreenWidth']));
					$dbADO->Execute('UPDATE Device_DeviceData SET IK_DeviceData=? WHERE FK_Device=? AND FK_DeviceData=?',array($resY,$orbiterArray['PK_Device'],$GLOBALS['ScreenHeight']));
					
					$sizeFields=getFieldsAsArray('Size','PK_Size',$dbADO,'WHERE Width='.$resX,' AND Height='.$resY);

					if(isset($sizeFields['PK_Size'][0])){
						$dbADO->Execute('UPDATE Device_DeviceData SET IK_DeviceData=? WHERE FK_Device=? AND FK_DeviceData=?',array($sizeFields['PK_Size'][0],$orbiterArray['PK_Device'],$GLOBALS['Size']));
					}
				}

				// append orbiter ID to orbiter plugin status, to be regenerated
				$orbiterPlugIn=getInfraredPlugin($installationID,$dbADO);
				if(isset($orbiterArray['PK_Device'])){
					$dbADO->Execute('UPDATE Device SET Status=IF(Status IS NULL,?,concat(Status, ?)) WHERE PK_Device=?',array($orbiterArray['PK_Device'],','.$orbiterArray['PK_Device'],$orbiterPlugIn));
				}
				
				// do quick reload router
				$command='/usr/pluto/bin/MessageSend localhost 0 -1000 7 1';
				exec($command);
				
			}
			
			$msg='Resolution and refresh updated.';
		}else{
			$Answer="N";
		}

		exec("sudo -u root /usr/pluto/bin/LaunchRemoteCmd.sh '$ipAddress' '/usr/pluto/bin/Xres_config_end.sh $Answer'");

		//full regen
		$commandToSend='/usr/pluto/bin/MessageSend localhost -targetType template '.$orbiterArray['PK_Device'].' '.$GLOBALS['OrbiterPlugIn'].' 1 266 2 '.$orbiterArray['PK_Device'].' 21 "-r" 24 1';				
		exec($commandToSend);

		
		header("Location: index.php?section=setResolution&mdID=$mdID&msg=$msg");
	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Setup resolution and refresh');			
	$output->output();
}
?>
