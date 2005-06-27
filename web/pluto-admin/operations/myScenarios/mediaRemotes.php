<?php
function mediaRemotes($output,$dbADO) {
	//$dbADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
	
	if ($action=='form') {

		$out.='

		<div class="err">'.@$_GET['error'].'</div>
		<div class="confirm" align="center"><B>'.strip_tags(@$_GET['msg']).'</B></div>
		
		<form action="index.php" method="post" name="mediaRemotes">
		<input type="hidden" name="section" value="mediaRemotes">
		<input type="hidden" name="action" value="add">
		<input type="hidden" name="drc" value="">
		
<span style="padding-left:10px; padding-right:10px;">
<h3>Choose preferred remotes</h3>		
Use this screen to specify what remote controls you want to use for media scenarios.  If you do not specify any preferences, Pluto will pick the most logical choice automatically.  If you do specify preferences, Pluto will use your choice instead.
</span>
<br><br>';
$orbitersArray=getDevicesArrayFromCategory($GLOBALS['rootOrbiterID'],$dbADO);
$avDevicesArray=getDevicesArrayFromCategory($GLOBALS['rootAVEquipment'],$dbADO);
$remotesArray=array();
if((int)@$_POST['mediaType']!=0){
	$mtArray=getFieldsAsArray('MediaType_DesignObj','FK_DesignObj,DesignObj.Description',$dbADO,'INNER JOIN DesignObj ON FK_DesignObj=PK_DesignObj WHERE FK_MediaType='.(int)@$_POST['mediaType']);
	for($i=0;$i<count($mtArray['FK_DesignObj']);$i++){
		$remotesArray[$mtArray['FK_DesignObj'][$i].':mt_do']=$mtArray['Description'][$i];
	}
}


if((int)@$_POST['device']!=0){
	$dt_mtArray=getFieldsAsArray('DeviceTemplate_MediaType','DesignObj.PK_DesignObj,DesignObj.Description',$dbADO,'
		INNER JOIN Device ON Device.FK_DeviceTemplate=DeviceTemplate_MediaType.FK_DeviceTemplate
		INNER JOIN DeviceTemplate_MediaType_DesignObj ON FK_DeviceTemplate_MediaType=PK_DeviceTemplate_MediaType
		INNER JOIN DesignObj ON DeviceTemplate_MediaType_DesignObj.FK_DesignObj=PK_DesignObj
		WHERE PK_Device='.(int)@$_POST['device']);
	for($i=0;$i<count(@$dt_mtArray['PK_DesignObj']);$i++){
		$remotesArray[$dt_mtArray['PK_DesignObj'][$i].':dt_mt_do']=$dt_mtArray['Description'][$i];
	}
}

$remotesTable='<table>';
$res=$dbADO->Execute('
	SELECT PK_RemoteControl,mt.Description AS mt,dt_mt.Description AS dt_mt, FK_MediaType_DesignObj,FK_DeviceTemplate_MediaType_DesignObj
	FROM RemoteControl
	LEFT JOIN DesignObj mt ON FK_MediaType_DesignObj=mt.PK_DesignObj
	LEFT JOIN DesignObj dt_mt ON FK_DeviceTemplate_MediaType_DesignObj=dt_mt.PK_DesignObj');
while($row=$res->FetchRow()){
	$remotesTable.='
		<tr>
			<td>'.(!is_null($row['FK_MediaType_DesignObj'])?$row['mt']:$row['dt_mt']).'</td>
			<td><a href="javascript:if(confirm(\'Are you sure you want to remove this remote?\')){document.mediaRemotes.drc.value='.$row['PK_RemoteControl'].';document.mediaRemotes.submit();}">Delete</a></td>
		</tr>
	';
}
$remotesTable.='</table>';


$out.='
<table>
	<tr>
		<td><B>Existing remotes</B></td>
		<td></td>
	</tr>
	<tr>
		<td>&nbsp;</td>
		<td>'.$remotesTable.'</td>
	</tr>
	<tr>
		<td><B>For this type of media</B></td>
		<td>'.generatePullDown('mediaType','MediaType','PK_MediaType','Description',(int)@$_POST['mediaType'],$dbADO,'','onChange="document.mediaRemotes.action.value=\'form\';document.mediaRemotes.submit();"').'</td>
	</tr>
	<tr>
		<td><B>Being played on this orbiter</B></td>
		<td>'.pulldownFromArray($orbitersArray,'orbiter',(int)@$_POST['orbiter'],'onChange="document.mediaRemotes.action.value=\'form\';document.mediaRemotes.submit();"','key','- Any orbiter -').'</td>
	</tr>
	<tr>
		<td><B>With this device as the source</B></td>
		<td>'.pulldownFromArray($avDevicesArray,'device',(int)@$_POST['device'],'onChange="document.mediaRemotes.action.value=\'form\';document.mediaRemotes.submit();"','key','- Any device -').'</td>
	</tr>		
	<tr>
		<td><B>Use this remote</B></td>
		<td>'.pulldownFromArray($remotesArray,'remote',0,'','key','- Please select -').'</td>
	</tr>
	<tr>
		<td colspan="2" align="center"><input type="submit" class="button" name="addPreference" value="Add Preference"></td>
	</tr>
</table>		
		</form>
		';
		
	} else {
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=mediaRemotes&error=You are not authorised to change the installation.");
			exit(0);
		}

		$mediaType=(int)$_POST['mediaType'];
		$orbiter=((int)$_POST['orbiter']>0)?(int)$_POST['orbiter']:NULL;
		$device=(int)$_POST['device'];
		$remote=$_POST['remote'];
		
		if($remote!='0'){
			$parts=explode(':',$remote);
			$remoteID=@$parts[0];
			if($remoteID!=''){
				if($parts[1]=='dt_mt_do')
					$dbADO->Execute('INSERT INTO RemoteControl (FK_Orbiter,FK_DeviceTemplate_MediaType_DesignObj) VALUES (?,?)',array($orbiter,$remoteID));
				else
					$dbADO->Execute('INSERT INTO RemoteControl (FK_Orbiter,FK_MediaType_DesignObj) VALUES (?,?)',array($orbiter,$remoteID));
				
				header('Location: index.php?section=mediaRemotes&msg=Remote added.');	
				exit();
			}
		}else{
			if((int)@$_POST['drc']>0){
				$dbADO->Execute('DELETE FROM RemoteControl WHERE PK_RemoteControl=?',(int)$_POST['drc']);
				
				header('Location: index.php?section=mediaRemotes&msg=Remote deleted.');	
				exit();
			}
			
			header('Location: index.php?section=mediaRemotes&error=No remote selected.');
			exit();
		}
	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Choose preferred remotes');			
	$output->output();
}
?>
