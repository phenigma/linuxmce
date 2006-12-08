<?
function raid($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/raid.lang.php');

	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	//$dbADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
	
	if ($action == 'form') {
		$out.='
	<div class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
	<div class="confirm" align="center"><B>'.@$_GET['msg'].'</B></div>
	<form action="index.php" method="POST" name="raid">
	<input type="hidden" name="section" value="raid">
	<input type="hidden" name="action" value="update">	

	<table>
		<tr class="tablehead">
			<td align="center">'.$TEXT_ID_CONST.'</td>
			<td align="center">'.$TEXT_RAID_CONST.'</td>
			<td align="center">'.$TEXT_RAID_TYPE_CONST.'</td>
			<td align="center">'.$TEXT_NO_OF_DRIVES_CONST.'</td>
			<td align="center">'.$TEXT_RAID_STATUS_CONST.'</td>
			<td align="center">'.$TEXT_ACTION_CONST.'</td>
		</tr>';
		$pos=0;
		$raidCategories=getDescendantsForCategory($GLOBALS['RaidCategory'],$dbADO);
		$raidTemplates=getAssocArray('DeviceTemplate','PK_DeviceTemplate','Description',$dbADO,'WHERE FK_DeviceCategory IN ('.join(',',$raidCategories).')');
		
		$data=getFieldsAsArray('Device','PK_Device,Device.Description AS Description,DeviceTemplate.Description AS Type,DDD1.IK_DeviceData AS Status,DDD2.IK_DeviceData AS NoOFDisks,DDD3.IK_Devicedata AS RAIDStatus',$dbADO,'INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate LEFT JOIN Device_DeviceData DDD1 ON DDD1.FK_Device=PK_Device AND DDD1.FK_DeviceData='.$GLOBALS['State'].' LEFT JOIN Device_DeviceData DDD2 ON DDD2.FK_Device=PK_Device AND DDD2.FK_DeviceData='.$GLOBALS['NoofDisks'].' LEFT JOIN Device_DeviceData DDD3 on DDD3.FK_Device=PK_Device AND DDD3.FK_DeviceData='.$GLOBALS['RAIDStatus'].' WHERE FK_DeviceCategory IN ('.join(',',$raidCategories).')');
		if(count($data)==0){
			$out.='
			<tr class="alternate_back">
				<td colspan="6">'.$TEXT_NO_RECORDS_CONST.'</td>
			</tr>';
		}

		$raidDevices=array();
		for ($i=0;$i<count(@$data['PK_Device']);$i++){
			$raidDevices[]=$data['PK_Device'][$i];
			$bgColor=($data['RAIDStatus'][$i]==0)?'bgcolor="lightgreen"':'class="alternate_back"';
			$out.='
			<tr '.$bgColor.'>
				<td>'.$data['PK_Device'][$i].'</td>
				<td>'.$data['Description'][$i].'</td>
				<td>'.$data['Type'][$i].'</td>
				<td align="center">'.$data['NoOFDisks'][$i].'</td>
				<td align="center">'.$data['Status'][$i].'</td>
				<td>
				<input type="button" class="button_fixed" name="drives_'.$data['PK_Device'][$i].'" value="'.$TEXT_DRIVES_CONST.'"  onClick="self.location=\'index.php?section=raidDrives&deviceID='.$data['PK_Device'][$i].'\';"><br>
				<input type="button" class="button_fixed" name="edit_'.$data['PK_Device'][$i].'" value="'.$TEXT_ADVANCED_CONST.'"  onClick="self.location=\'index.php?section=editDeviceParams&deviceID='.$data['PK_Device'][$i].'\';"><br>
				<input type="submit" class="button_fixed" name="delete_'.$data['PK_Device'][$i].'" value="'.$TEXT_DELETE_CONST.'"  onClick="if(!confirm(\'Are you sure you want to delete this device?\'))return false;">
				
				</td>
			</tr>';
		}
		
		$out.='
			<tr>
				<td align="center" colspan="6"><B>'.$TEXT_ADD_RAID_CONST.'</B> <input type="text" name="description" value=""> '.$TEXT_TYPE_CONST.' '.pulldownFromArray($raidTemplates,'template',0).' <input type="submit" class="button" name="add" value="'.$TEXT_ADD_CONST.'"></td>
			</tr>		
	</table>
	<input type="hidden" name="raidDevices" value="'.join(',',$raidDevices).'">
	<br><em>'.$TEXT_GREEN_LABEL_INFO_CONST.'</em>
	</form>
	';
	} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=raid&error=$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST");
			exit(0);
		}

		if(isset($_POST['add']) && (int)@$_POST['template']!=0){
			$description=cleanString($_POST['description']);
			$raidID=createDevice((int)@$_POST['template'],$installationID,NULL,NULL,$dbADO);
			$dbADO->Execute('UPDATE Device SET Description=? WHERE PK_Device=?',array($description,$raidID));
		}
		
		$raidDevices=explode(',',$_POST['raidDevices']);
		foreach ($raidDevices AS $device){
			if(isset($_POST['delete_'.$device])){
				$cmd='sudo -u root /usr/pluto/bin/delete_raid.sh '.$device;
				exec_batch_command($cmd);
				
				deleteDevice($device,$dbADO);
			}
		}
		
		header("Location: index.php?section=raid&msg=".urlencode(@$msg));
		exit();
	}

	$output->setMenuTitle($TEXT_ADVANCED_CONST.' |');
	$output->setPageTitle($TEXT_RAID_CONST);

	$output->setScriptCalendar('null');
	$output->setNavigationMenu(array($TEXT_RAID_CONST=>'index.php?section=raid'));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}

?>
