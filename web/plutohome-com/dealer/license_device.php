<?
if(!isset($_SESSION['userIsLogged'])){
	header('Location: index.php?section=login');
	exit();
}
function license_device($output,$dbADO) {
	/* @var $dbADO ADOConnection */
	global $dbDealerServer, $dbDealerUser, $dbDealerPass, $dbDealerDatabase;
	global $dbManufacturerServer, $dbManufacturerUser, $dbManufacturerPass, $dbManufacturerDatabase;
	
	if ($_SESSION['userIsLogged']!="yes") {
		header("Location: index.php");
	}
	$dealerADO=dbConnection($dbDealerServer,$dbDealerUser,$dbDealerPass,$dbDealerDatabase);
	
	$FK_Users=(isset($_SESSION['userID']))?(int)$_SESSION['userID']:0;
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$manufacturerID	=(int)@$_REQUEST['mID'];
	if($manufacturerID==0){
		die('Invalid parameter.');
	}
	
if($action=='form'){
	

	
	$errMsg=(isset($_REQUEST['err']))?'<span class="err">'.stripslashes($_REQUEST['err']).'</span>':'';
	$msg=(isset($_REQUEST['msg']))?'<span class="confirm">'.stripslashes($_REQUEST['msg']).'</span>':'';
	$out = '
		<form action="index.php" method="POST" name="license_device">
			<input type="hidden" name="section" value="license_device">
			<input type="hidden" name="action" value="add">		
			<input type="hidden" name="mID" value="'.$manufacturerID.'">	
		
      		<table width="100%">
	      		<tr>
      				<td align="left" colspan="2" class="insidetable"><B>License device</B> </td>
				</tr>
				<tr>
					<td class="normaltext" align="center">&nbsp;'.@$msg.' '.@$errMsg.'</td>
      			</tr>	
				<tr>
					<td class="normaltext" align="center">
						<table cellpadding="3" cellspacing="0" border="0">
							<tr bgcolor="#DADDE4">
								<td colspan="2"><B>License device </B></td>
							</tr>
							<tr>
								<td>Installation</td>
								<td><input type="text" name="inst" value=""></td>
							</tr>
							<tr>
								<td>Device Template</td>
								<td><input type="text" name="dtID" value=""></td>
							</tr>	
							<tr>
								<td>Device</td>
								<td><input type="text" name="did" value=""></td>
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
  		<script>
		 	var frmvalidator = new formValidator("license_device");
			frmvalidator.addValidation("inst","req","Please type the installation number.");
			frmvalidator.addValidation("did","req","Please type the device number.");
			frmvalidator.addValidation("dtID","req","Please type the device template number.");
		</script>		
      	';
}else{
	
	$inst=(int)$_REQUEST['inst'];
	$did=(int)$_REQUEST['did'];
	
	$dtID=(int)@$_REQUEST['dtID'];
	$value=convertMac($_POST['value']);
	$type=(int)$_POST['type'];
	$isMacAddress=($type==3 || $type==4)?1:0;

	if($did==0 || $dtID==0 || $inst==0){
		header('Location: index.php?section=license_device&mID='.$manufacturerID.'&err=Device number, device template number or installation number is invalid.');
		exit();
	}
	$manufacturerADO=dbConnection($dbManufacturerServer,$dbManufacturerUser,$dbManufacturerPass,$dbManufacturerDatabase);
	$manufacturerADO->Execute('
		INSERT INTO LicensedDevice 
			(EK_DeviceTemplate,IsMacAddress,SerialOrMacAddress,EK_Device,EK_Installation,DateRegistered,EK_Manufacturer)
		VALUES
			(?,?,?,?,?,?,?)',
	array($dtID,$isMacAddress,$value,$did,$inst,date('Y-m-d H:i:s'),$manufacturerID));
	
	$msg='The device was licensed.';	
	
	
	header('Location: index.php?section=license_device&mID='.$manufacturerID.'&msg='.$msg);
	exit();
}


	$output->setNavigationMenu(array("License device"=>"index.php?section=license_device"));

	$output->setScriptCalendar('null');
	$output->setScriptTRColor('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME." :: License Device");
	$output->output();
}
?>