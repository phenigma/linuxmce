<?
function core($output,$dbADO) {
/* @var $dbADO ADOConnection */
/* @var $rs ADORecordSet */

	$out='';
	$dbADO->debug=false;
	$installationID = (int)@$_SESSION['installationID'];
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';

	$queryDevice='
		SELECT Device.*,Room.Description AS RoomName 
		FROM Device
		INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
		LEFT JOIN Room ON FK_Room=PK_Room
		WHERE FK_DeviceTemplate=? AND Device.FK_Installation=?
		';
	$resDevice=$dbADO->Execute($queryDevice,array($GLOBALS['rootCoreID'],$installationID));
	if($resDevice->RecordCount()!=0){
		$rowDevice=$resDevice->FetchRow();
		$roomName=$rowDevice['RoomName'];
		$coreID=$rowDevice['PK_Device'];
		$coreRoom=$rowDevice['FK_Room'];
		$_SESSION['CoreID']=$coreID;
	}	

	$resDDD=$dbADO->Execute('SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device=? AND FK_DeviceData=?',array($coreID,$GLOBALS['rootPK_Distro']));
	$rowDDD=$resDDD->FetchRow();
	$coreDistro=$rowDDD['IK_DeviceData'];
		
	$resOS=$dbADO->Execute('SELECT FK_OperatingSystem FROM Distro WHERE PK_Distro=?',$coreDistro);
	$rowOS=$resOS->FetchRow();
	$CoreOS=$rowOS['FK_OperatingSystem'];

	$resDCERouter=$dbADO->Execute('SELECT * FROM Device WHERE FK_DeviceTemplate=? AND FK_Device_ControlledVia=?',array($GLOBALS['rootDCERouter'],$coreID));
	if($resDCERouter->RecordCount()!=0){
		$rowDCERouter=$resDCERouter->FetchRow();
		$coreDCERouterID=$rowDCERouter['PK_Device'];
		$_SESSION['CoreDCERouter']=$coreDCERouterID;
	}


	if ($action == 'form') {
	$out.=setLeftMenu($dbADO).'
	<script>
	function windowOpen(locationA,attributes) {
		window.open(locationA,\'\',attributes);
	}
	</script>
	
	<div class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
	<div class="confirm"><B>'.(isset($_GET['msg'])?strip_tags($_GET['msg']):'').'</B></div>
	<h3>General Info page for the core</h3>
		
<form method="post" action="index.php" name="core">
	<input type="hidden" name="section" value="core">
	<input type="hidden" name="action" value="update">
<table>
	<tr>
		<td>'.getInstallWizardDeviceTemplates(5,$dbADO,$coreDCERouterID,$coreDistro,$CoreOS).'</td>
	</tr>	
	<tr>
		<td align="center"><input type="submit" class="button" name="update" value="Update"></td>
	</tr>	
</table>
</form>';
	
	} else {
		
		//check if current user canModifyInstallation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$installationID,$dbADO);
		
		if (!$canModifyInstallation) {
			Header('Location: index.php?section=core&error=You are not allowed to modify installation.');
			exit();
		}
		
		// install options - delete or insert devices
		$installOptionsArray=explode(',',$_POST['displayedTemplatesRequired_'.$coreDCERouterID]);
		foreach($installOptionsArray AS $elem){
			$optionalDevice=(isset($_POST['device_'.$coreDCERouterID.'_requiredTemplate_'.$elem]))?$_POST['device_'.$coreDCERouterID.'_requiredTemplate_'.$elem]:0;
			$oldDevice=$_POST['oldDevice_'.$coreDCERouterID.'_requiredTemplate_'.$elem];
			if($optionalDevice!=0){
				$OptionalDeviceName=cleanString(@$_POST['templateName_'.$elem]);
				$queryIsPlugin='SELECT IsPlugIn FROM DeviceTemplate WHERE PK_DeviceTemplate=? AND IsPlugIn=1';
				$resIsPlugin=$dbADO->Execute($queryIsPlugin,$elem);
				$parentDevice=($resIsPlugin->RecordCount()>0)?$coreDCERouterID:$coreID;

				$insertDevice='
					INSERT INTO Device 
						(Description, FK_DeviceTemplate, FK_Installation, FK_Device_ControlledVia,FK_Room) 
					VALUES (?,?,?,?,?)';
				if($oldDevice==''){
					$dbADO->Execute($insertDevice,array($OptionalDeviceName,$elem,$installationID,$parentDevice,$coreRoom));
					$childDeviceId=$dbADO->Insert_ID();
				}
			}else{
				$dbADO->Execute("DELETE FROM Device WHERE PK_Device='".$oldDevice."'");
			}
		}

		Header('Location: index.php?section=core&msg=Core optional device updated.');
	}
	
	$output->setNavigationMenu(array("My Devices"=>'index.php?section=myDevices',"Core"=>"index.php?section=core"));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();  		
}
?>
