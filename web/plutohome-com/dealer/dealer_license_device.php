<?
if(!isset($_SESSION['userIsLogged'])){
	header('Location: index.php?section=login');
	exit();
}
function dealer_license_device($output,$dbADO) {
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
	$did=(int)$_REQUEST['did'];
	
if($action=='form'){
	

	
	$errMsg=(isset($_REQUEST['err']))?'<span class="err">'.stripslashes($_REQUEST['err']).'</span>':'';
	$msg=(isset($_REQUEST['msg']))?'<span class="confirm">'.stripslashes($_REQUEST['msg']).'</span>':'';
	$out = '
		<form action="index.php" method="POST" name="dealer_license_device">
			<input type="hidden" name="section" value="dealer_license_device">
			<input type="hidden" name="action" value="add">		
			<input type="hidden" name="inst" value="'.$inst.'">	
			<input type="hidden" name="did" value="'.$did.'">	
	
      		<table width="100%">
	      		<tr>
      				<td align="left" colspan="2" class="insidetable"><B>License device #'.$did.'</B> </td>
				</tr>
				<tr>
					<td class="normaltext" align="left">&nbsp;<a href="index.php?section=dealer_installation&inst='.$inst.'">&lt;&lt; Back</a></td>
      			</tr>	
				<tr>
					<td class="normaltext" align="center">&nbsp;'.@$msg.' '.@$errMsg.'</td>
      			</tr>	
				<tr>
					<td class="normaltext" align="center">
						<table cellpadding="3" cellspacing="0" border="0">
							<tr bgcolor="#DADDE4">
								<td colspan="2"><B>License device # '.$did.'</B></td>
							</tr>
							<tr>
								<td>Single serial number</td>
								<td><input type="radio" name="type" value="1" checked></td>
							</tr>
							<tr>
								<td>Range of serial numbers</td>
								<td><input type="radio" name="type" value="2"></td>
							</tr>
							<tr>
								<td>Single mac address</td>
								<td><input type="radio" name="type" value="3"></td>
							</tr>
							<tr>
								<td>Range of mac addresses</td>
								<td><input type="radio" name="type" value="4"></td>
							</tr>
							<tr>
								<td>Value</td>
								<td><input type="text" name="value" value=""></td>
							</tr>	
							<tr>
								<td>&nbsp;</td>
								<td><input type="submit" name="add" value="Add license"></td>
							</tr>		
					</table>
					</td>
      			</tr>	
			</table>
		</form>
      	';
}else{
	
	
	$value=convertMac($_POST['value']);
	$type=(int)$_POST['type'];
	$isMacAddress=($type==3 || $type==4)?1:0;
	$device=getFieldsAsArray('Installation_Device','EK_DeviceTemplate',$dealerADO,'WHERE EK_Installation='.$inst.' AND EK_Device='.$did);
	$manufacturerADO=dbConnection($dbManufacturerServer,$dbManufacturerUser,$dbManufacturerPass,$dbManufacturerDatabase);
	$manufacturerADO->Execute('
		INSERT INTO LicensedDevice 
			(EK_DeviceTemplate,IsMacAddress,SerialOrMacAddress,EK_Device,EK_Installation,DateRegistered)
		VALUES
			(?,?,?,?,?,?)',
	array($device['EK_DeviceTemplate'][0],$isMacAddress,$value,$did,$inst,date('Y-m-d H:i:s')));
	$licenseID=$manufacturerADO->Insert_ID();
	$dealerADO->Execute('UPDATE Installation_Device SET EK_LicensedDevice=? WHERE EK_Installation=? AND EK_Device=?',array($licenseID,$inst,$did));
	
	$msg='The device was licensed.';	
	
	
	header('Location: index.php?section=dealer_installation&inst='.$inst.'&msg='.$msg);
	exit();
}


	$output->setNavigationMenu(array("License device"=>"index.php?section=dealer_license_device"));

	$output->setScriptCalendar('null');
	$output->setScriptTRColor('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME." :: License Device");
	$output->output();
}
?>