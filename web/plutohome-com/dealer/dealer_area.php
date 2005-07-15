<?
if(!isset($_SESSION['userIsLogged'])){
	header('Location: index.php?section=login');
	exit();
}
function dealer_area($output,$dbADO) {
	/* @var $dbADO ADOConnection */
	global $dbDealerServer, $dbDealerUser, $dbDealerPass, $dbDealerDatabase;
	global $dbManufacturerServer, $dbManufacturerUser, $dbManufacturerPass, $dbManufacturerDatabase;
	
	if ($_SESSION['userIsLogged']!="yes") {
		header("Location: index.php");
	}
	$dealerADO=dbConnection($dbDealerServer,$dbDealerUser,$dbDealerPass,$dbDealerDatabase);
	
	$FK_Users=(isset($_SESSION['userID']))?(int)$_SESSION['userID']:0;
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';

	$userDealer=getFieldsAsArray('Dealer_Users','Validated,CanEditProfile,FK_Dealer',$dealerADO,' WHERE EK_Users='.$FK_Users);
	$dealerID=(int)@$userDealer['FK_Dealer'][0];

	if($dealerID==0 || @$userDealer['Validated'][0]!=1){
		header('Location: index.php?section=dealer_signin');
		exit();
	}else{
		$dealerInstallations=getAssocArray('Dealer_Installation','EK_Installation','Description',$dealerADO,'WHERE FK_Dealer='.$dealerID,'ORDER BY Description ASC');
		$userInstallations=getAssocArray('Installation','PK_Installation','Description',$dbADO,'INNER JOIN Installation_Users ON FK_Installation=PK_Installation WHERE FK_Users='.$FK_Users,'ORDER BY Description ASC');
	}	
	
if($action=='form'){
	$dInstallations=getFieldsAsArray('Dealer_Installation','EK_Installation,Description,IsActive,EK_Installation_Replaces',$dealerADO,'where FK_Dealer='.$dealerID);
	$instTable='
		<table align="center" width="500">
			<tr bgcolor="#F0F3F8">
				<td width="20"><B>#</B></td>
				<td width="150"><B>Description</B></td>
				<td align="center"><B>Replaced by</B></td>
				<td align="center"><B>Actions</B></td>
			</tr>';
	$inactiveInst='
	<table align="center" width="500">
		<tr bgcolor="#F0F3F8">
				<td width="20"><B>#</B></td>
				<td width="150"><B>Description</B></td>
				<td align="center"><B>Replaced by</B></td>
				<td align="center"><B>Actions</B></td>
			</tr>';
	$active=0;
	$inactive=0;
	for ($i=0;$i<count(@$dInstallations['EK_Installation']);$i++){
		$remainingInst=array_diff($userInstallations,array($dInstallations['EK_Installation'][$i]=>$dInstallations['Description'][$i]));
		$row='
			<tr>
				<td width="20">'.$dInstallations['EK_Installation'][$i].'</td>
				<td width="150"><a href="index.php?section=dealer_installation&inst='.$dInstallations['EK_Installation'][$i].'">'.$dInstallations['Description'][$i].'</a></td>
				<td>'.htmlPulldown($remainingInst,'replacedBy_'.$dInstallations['EK_Installation'][$i],$dInstallations['EK_Installation_Replaces'][$i],'Please select').'</td>
				<td align="center"><input type="submit" name="changeState_'.$dInstallations['EK_Installation'][$i].'" value="'.(($dInstallations['IsActive'][$i]==1)?'Set Inactive':'Set Active').'"> <input type="submit" name="delete_'.$dInstallations['EK_Installation'][$i].'" value="Delete" onClick="if(!confirm(\'Are you sure you want to delete installation?\'))return false;"></td>
			</tr>';
		if($dInstallations['IsActive'][$i]==1){
			$active++;
			$instTable.=$row;
		}
		else{
			$inactive++;
			$inactiveInst.=$row;
		}
	}
	
	$instTable.=($active==0)?'<tr><td align="center" colspan="4">No active installations.</td></tr></table>':'</table>';
	$inactiveInst.=($inactive==0)?'<tr><td align="center" colspan="4">No inactive installations.</td></tr></table>':'</table>';
	$updateBtn=($active!=0 || $inactive!=0)?'<input type="submit" name="update" value="Update">':'&nbsp;';
	
	$errMsg=(isset($_REQUEST['err']))?'<span class="err">'.stripslashes($_REQUEST['err']).'</span>':'';
	$msg=(isset($_REQUEST['msg']))?'<span class="confirm">'.stripslashes($_REQUEST['msg']).'</span>':'';
	$out = '
		<form action="index.php" method="POST" name="dealer_area">
			<input type="hidden" name="section" value="dealer_area">
			<input type="hidden" name="action" value="add">		
	
      		<table width="100%">
	      		<tr>
      				<td align="left" colspan="2" class="insidetable"><B>Dealer area</B> </td>
				</tr>
				<tr>
					<td class="normaltext" align="right">&nbsp;'.(($userDealer['CanEditProfile'][0]==1)?'<a href="index.php?section=edit_dealer_profile">Edit my profile</a>':'').'</td>
      			</tr>	
				<tr>
					<td class="normaltext" align="center">&nbsp;'.@$msg.@$errMsg.'</td>
      			</tr>	
      			<tr bgcolor="#DADDE4">
      				<td align="center" width="33%" class="normaltext"><b>Active installations</b></td>
      			</tr>
      			<tr>
      				<td width="33%" class="normaltext">'.$instTable.'</td>
      			</tr>
      			<tr>
      				<td width="33%" class="normaltext" align="center"></td>
      			</tr>	
      			<tr bgcolor="#DADDE4">
      				<td align="center" width="33%" class="normaltext"><b>Inactive installations</b></td>
      			</tr>
      			<tr>
      				<td width="33%" class="normaltext" align="center">'.$inactiveInst.'</td>
      			</tr>	
      			<tr>
      				<td width="33%" class="normaltext" align="center">'.$updateBtn.'</td>
      			</tr>	
      			<tr>
      				<td width="33%" class="normaltext" align="center">&nbsp;</td>
      			</tr>	
				<tr bgcolor="#DADDE4">
      				<td align="center" width="33%" class="normaltext"><b>Add installation</b></td>
      			</tr>	
      			<tr>
      				<td width="33%" class="normaltext" align="center">Installation: '.htmlPulldown(@array_diff($userInstallations,$dealerInstallations),'installation','None','Please select').' <input type="submit" name="addInstallation" value="Add installation"></td>
      			</tr>	
			</table>
		</form>
      	';
}else{
	
	$msg='Installations updated.';
	foreach(array_keys($dealerInstallations) AS $dID){
		$replacedBy=((int)$_POST['replacedBy_'.$dID]>0)?(int)$_POST['replacedBy_'.$dID]:NULL;
		$dealerADO->Execute('UPDATE Dealer_Installation SET EK_Installation_Replaces=? WHERE FK_Dealer=? AND EK_Installation=?',array($replacedBy,$dealerID,$dID));
		
		if(isset($_POST['changeState_'.$dID])){
			$dealerADO->Execute('UPDATE Dealer_Installation SET IsActive =(NOT IsActive) WHERE FK_Dealer=? AND EK_Installation=?',array($dealerID,$dID));
		}
		
		if(isset($_POST['delete_'.$dID])){
			$dealerADO->Execute('DELETE Installation_Device FROM Installation_Device INNER JOIN Dealer_Installation ON Dealer_Installation.EK_Installation=Installation_Device.EK_Installation WHERE FK_Dealer=? AND Dealer_Installation.EK_Installation=?',array($dealerID,$dID));
			$dealerADO->Execute('DELETE FROM Dealer_Installation WHERE FK_Dealer=? AND EK_Installation=?',array($dealerID,$dID));
			$msg='The installation was deleted';
		}
	}
	
	
	if((int)$_POST['installation']!=0){
		$installationToAdd=(int)$_POST['installation'];
		$dealerADO->Execute('
			INSERT INTO Dealer_Installation 
				(FK_Dealer,EK_Installation,Description,IsActive) 
			SELECT FK_Dealer,'.$installationToAdd.',\''.$userInstallations[$installationToAdd].'\',1 
			FROM Dealer_Users WHERE EK_Users='.$FK_Users);

		$manufacturerADO=dbConnection($dbManufacturerServer,$dbManufacturerUser,$dbManufacturerPass,$dbManufacturerDatabase);
		
		$devices=getAssocArray('Device','PK_Device','FK_DeviceTemplate',$dbADO,'INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE FK_Installation='.$installationToAdd.' AND FK_Licensing IS NOT NULL');
		foreach ($devices AS $device=>$dt){
			$deviceLicensed=getAssocArray('LicensedDevice',0,'PK_LicensedDevice',$manufacturerADO,'WHERE EK_Installation='.$installationToAdd.' AND EK_Device='.$device.' AND EK_DeviceTemplate='.$dt);
			$license=(count($deviceLicensed)>0)?$deviceLicensed[0]:NULL;
			$dealerADO->Execute('INSERT INTO Installation_Device (EK_Installation,EK_Device,EK_DeviceTemplate,EK_LicensedDevice) VALUES (?,?,?,?)',array($installationToAdd,$device,$dt,$license));
		}
		header('Location: index.php?section=dealer_area&msg=Installation added.');
		exit();
	}
	
	header('Location: index.php?section=dealer_area&msg='.$msg);
	exit();
}


	$output->setNavigationMenu(array("My Pluto"=>"index.php?section=myPluto","Dealer Area"=>"index.php?section=dealer_area"));

	$output->setScriptCalendar('null');
	$output->setScriptTRColor('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME." :: Dealer area");
	$output->output();
}
?>