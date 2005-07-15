<?
if(!isset($_SESSION['userIsLogged'])){
	header('Location: index.php?section=login');
	exit();
}
function dealer_installation($output,$dbADO) {
	/* @var $dbADO ADOConnection */
	global $dbDealerServer, $dbDealerUser, $dbDealerPass, $dbDealerDatabase;
	global $dbManufacturerServer, $dbManufacturerUser, $dbManufacturerPass, $dbManufacturerDatabase;
	
	if ($_SESSION['userIsLogged']!="yes") {
		header("Location: index.php");
	}
	$dealerADO=dbConnection($dbDealerServer,$dbDealerUser,$dbDealerPass,$dbDealerDatabase);
	
	$FK_Users=(isset($_SESSION['userID']))?(int)$_SESSION['userID']:0;
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$inst=(int)$_REQUEST['inst'];
	$dtLabel=getAssocArray('DeviceTemplate','PK_DeviceTemplate','Description',$dbADO);
	
if($action=='form'){
	
	$deviceLabel=getAssocArray('Device','PK_Device','Description',$dbADO,'WHERE FK_Installation='.$inst);
	$roleArray=getAssocArray('DeviceRole','PK_DeviceRole','Description',$dealerADO);
	$licenseType=getAssocArray('LicenseType','PK_LicenseType','Description',$dealerADO);
	
	$devicesArray=getFieldsAsArray('Installation_Device','EK_Device,EK_DeviceTemplate,EK_LicensedDevice,FK_DeviceRole,FK_LicenseType,LocalDevice',$dealerADO,'WHERE EK_Installation='.$inst,'ORDER BY EK_DeviceTemplate ASC');

	$devicesTable='<table cellpadding="3" cellspacing="0" border="0">
			<tr bgcolor="#DADDE4">
				<td align="center"><B># Device</B></td>
				<td align="center" colspan="2"><B>Device Template</B></td>
				<td align="center"><B>Device Role</B></td>
				<td align="center"><B>License Type</B></td>
				<td align="center"><B>Licensed Device</B></td>
				<td align="center"><B>Action</B></td>
			</tr>';
	for ($i=0;$i<count(@$devicesArray['EK_Device']);$i++){
		$color=($i%2!=0)?'#F0F3F8':'#FFFFFF';
		$devicesTable.='
			<tr bgcolor="'.$color.'">
				<td align="center">'.$devicesArray['EK_Device'][$i].(($devicesArray['LocalDevice'][$i]==1)?'*':'').'</td>
				<td align="center">'.@$dtLabel[$devicesArray['EK_DeviceTemplate'][$i]].'</td>
				<td align="center">#'.$devicesArray['EK_DeviceTemplate'][$i].'</td>
				<td align="center">'.@$roleArray[$devicesArray['FK_DeviceRole'][$i]].'</td>
				<td align="center">'.@$licenseType[$devicesArray['FK_LicenseType'][$i]].'</td>
				<td align="center">'.((!is_null($devicesArray['EK_LicensedDevice'][$i]))?$devicesArray['EK_LicensedDevice'][$i]:'<a href="index.php?section=dealer_license_device&inst='.$inst.'&did='.$devicesArray['EK_Device'][$i].'">Add License</a>').'</td>
				<td align="center"><input type="submit" name="del_" value="Delete"></td>
			</tr>
		';
	}
	$devicesTable.='
		<tr>
			<td align="left" colspan="7">* <em>Local device</em></td>
		</tr>		
		<tr>
			<td align="left" colspan="7">&nbsp;</td>
		</tr>	
		<tr bgcolor="#DADDE4">
			<td align="center" colspan="7"><B>Add local device</B></td>
		</tr>		
			<tr>
				<td align="right">Device ID </td>
				<td colspan="6"><input type="text" name="device" value=""></td>
			</tr>
			<tr>
				<td align="right">Device template ID</td>
				<td colspan="6"><input type="text" name="dt" value=""></td>
			</tr>
			<tr>
				<td align="right">Device Role</td>
				<td colspan="6">'.htmlPulldown($roleArray,'role',0).'</td>
			</tr>
			<tr>
				<td align="right">License type</td>
				<td colspan="6">'.htmlPulldown($licenseType,'type',0).'</td>
			</tr>
			<tr>
				<td align="right">Comments</td>
				<td colspan="6"><textarea name="comments"></textarea></td>
			</tr>
			<tr>
				<td align="right">&nbsp;</td>
				<td colspan="6"><input type="submit" name="add" value="Add device"></td>
			</tr>	
	</table>';
	
	$errMsg=(isset($_REQUEST['err']))?'<span class="err">'.stripslashes($_REQUEST['err']).'</span>':'';
	$msg=(isset($_REQUEST['msg']))?'<span class="confirm">'.stripslashes($_REQUEST['msg']).'</span>':'';
	$out = '
		<form action="index.php" method="POST" name="dealer_installation">
			<input type="hidden" name="section" value="dealer_installation">
			<input type="hidden" name="action" value="add">		
			<input type="hidden" name="inst" value="'.$inst.'">	
	
      		<table width="100%">
	      		<tr>
      				<td align="left" colspan="2" class="insidetable"><B>Dealer Installation #'.$inst.'</B> </td>
				</tr>
				<tr>
					<td class="normaltext" align="left">&nbsp;<a href="index.php?section=dealer_area">&lt;&lt; Back</a></td>
      			</tr>	
				<tr>
					<td class="normaltext" align="center">&nbsp;'.@$msg.' '.@$errMsg.'</td>
      			</tr>	
				<tr>
					<td class="normaltext" align="center">&nbsp;'.$devicesTable.'</td>
      			</tr>	
			</table>
		</form>
  		<script>
		 	var frmvalidator = new formValidator("dealer_installation");
			frmvalidator.addValidation("device","req","Please type the device ID.");
			frmvalidator.addValidation("dt","req","Please type the device template ID.");
			frmvalidator.addValidation("role","dontselect=0","Please select the device role.");
			frmvalidator.addValidation("type","dontselect=0","Please select the license type.");
		</script>	
      	';
}else{
	
	$device=(int)$_POST['device'];
	$dt=(int)$_POST['dt'];
	$role=((int)$_POST['role']!=0)?(int)$_POST['role']:NULL;
	$type=((int)$_POST['type']!=0)?(int)$_POST['type']:NULL;
	$comments=stripslashes($_POST['comments']);
	if(!in_array($dt,array_keys($dtLabel))){
		header('Location: index.php?section=dealer_installation&inst='.$inst.'&err=The device template '.$dt.' does not exist.');
		exit();
	}
	if($device>0 && $dt>0){
		$existDevice=getFieldsAsArray('Installation_Device','EK_Device',$dealerADO,'WHERE EK_Device='.$device.' AND EK_Installation='.$inst);
		if(isset($existDevice['EK_Device'][0])){
			header('Location: index.php?section=dealer_installation&inst='.$inst.'&err=The device '.$device.' is already in database.');
			exit();
		}
		
		$manufacturerADO=dbConnection($dbManufacturerServer,$dbManufacturerUser,$dbManufacturerPass,$dbManufacturerDatabase);
		$existLicense=getFieldsAsArray('LicensedDevice','PK_LicensedDevice',$manufacturerADO,'WHERE EK_DeviceTemplate='.$dt.' AND EK_Device='.$device.' AND EK_Installation='.$inst);
		$license=(isset($existLicense['PK_LicensedDevice'][0]))?$existLicense['PK_LicensedDevice'][0]:NULL;
		
		$dealerADO->Execute('INSERT INTO Installation_Device (EK_Installation,EK_Device,EK_DeviceTemplate,LocalDevice,EK_LicensedDevice,FK_DeviceRole,FK_LicenseType,Comments) VALUES (?,?,?,1,?,?,?,?)',array($inst,$device,$dt,$license,$role,$type,$comments));		
		$msg='The device was added to database.';
	}else{
		header('Location: index.php?section=dealer_installation&inst='.$inst.'&err=Please type device and device template');
		exit();
	}
		
	
	
	header('Location: index.php?section=dealer_installation&inst='.$inst.'&msg='.$msg);
	exit();
}


	$output->setNavigationMenu(array("dealer_installation"=>"index.php?section=dealer_installation"));

	$output->setScriptCalendar('null');
	$output->setScriptTRColor('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME." :: Dealer Installation");
	$output->output();
}
?>