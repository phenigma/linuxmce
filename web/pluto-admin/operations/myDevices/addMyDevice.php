<?
//se iau intai cele selectate, din baza de date si apoi se verifica array-ul din DeviceTemplate_DeviceParameter!
function addMyDevice($output,$dbADO) {
	global $dbPlutoMainDatabase;
/* @var $dbADO ADOConnection */
/* @var $rs ADORecordSet */
$userID = (int)@$_SESSION['userID'];
$out='';
$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
$parentID = isset($_REQUEST['parentID'])?cleanInteger($_REQUEST['parentID']):0;
$installationID=(int)$_SESSION['installationID'];

//$dbADO->debug=true;
//grab the 'controlled via'
$whereClause='';

if ($parentID!=0) {
	$selectMasterDeviceForParent = '
		SELECT Device.FK_DeviceTemplate,FK_DeviceCategory,FK_DeviceCategory_Parent 
		FROM Device 
			INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
			INNER JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory
		WHERE PK_Device = ?';
	$resSelectMasterDeviceForParent = $dbADO->Execute($selectMasterDeviceForParent,array($parentID));
	if ($resSelectMasterDeviceForParent) {
		$rowSelectMasterDeviceForParent = $resSelectMasterDeviceForParent->FetchRow();
		$DeviceTemplateForParent = $rowSelectMasterDeviceForParent['FK_DeviceTemplate'];
		$DeviceCategoryForParent = $rowSelectMasterDeviceForParent['FK_DeviceCategory'];
		$ParentDeviceCategoryForParent = $rowSelectMasterDeviceForParent['FK_DeviceCategory_Parent'];

		$queryDTControlledByCategory ='SELECT FK_DeviceTemplate FROM DeviceTemplate_DeviceCategory_ControlledVia WHERE FK_DeviceCategory = ? OR FK_DeviceCategory = ?';
		$resDTControlledByCategory = $dbADO->Execute($queryDTControlledByCategory,array($DeviceCategoryForParent,$ParentDeviceCategoryForParent));

		$childsDeviceTemplateArray = array();
		if ($resDTControlledByCategory) {
			while ($rowDTControlledByCategory= $resDTControlledByCategory->FetchRow()) {
				$childsDeviceTemplateArray[]=$rowDTControlledByCategory['FK_DeviceTemplate'];
			}
		}

		$querySelectControlledViaDeviceTemplate ='SELECT FK_DeviceTemplate FROM DeviceTemplate_DeviceTemplate_ControlledVia WHERE FK_DeviceTemplate_ControlledVia = ?';
		$resSelectControlledViaDeviceTemplate= $dbADO->Execute($querySelectControlledViaDeviceTemplate,array($DeviceTemplateForParent));

		
		if ($resSelectControlledViaDeviceTemplate) {
			while ($rowSelectControlledVia = $resSelectControlledViaDeviceTemplate->FetchRow()) {
				$childsDeviceTemplateArray[]=$rowSelectControlledVia['FK_DeviceTemplate'];
			}
		}

		if (count($childsDeviceTemplateArray)) {
			$whereClause.=' AND PK_DeviceTemplate in ('.join(",",$childsDeviceTemplateArray).')';
		}
	}
}


$queryDeviceTemplate = "select PK_DeviceTemplate ,Description from DeviceTemplate where 1=1 $whereClause order by Description asc";
$resDeviceTemplate = $dbADO->_Execute($queryDeviceTemplate);
$DeviceTemplate = '<option value="0">-please select-</option>';
if($resDeviceTemplate) {
	while ($row=$resDeviceTemplate->FetchRow()) {
		$DeviceTemplate.='<option value="'.$row['PK_DeviceTemplate'].'">'.$row['Description'].'</option>';
	}
}

if ($action == 'form') {
	$out.='
	<div class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
	<form action="index.php" method="POST" name="addMyDevice">
	<input type="hidden" name="section" value="addMyDevice">
	<input type="hidden" name="action" value="add">	
	<input type="hidden" name="parentID" value="'.$parentID.'">
		<table>			
				<tr>
					<td>Description:</td>
					<td><input type="text" size="15" name="Description" value=""></td>
				</tr>				
				<tr>
					<td>Device Template:</td>
					<td><select name="masterDevice">'.$DeviceTemplate.'</select></td>
				</tr>
				<tr>
					<td>IP Address:</td>
					<td><textarea name="IPaddress"></textarea></td>
				</tr>
				<tr>
					<td>MACaddress:</td>
					<td><textarea name="MACaddress"></textarea></td>
				</tr>
				<tr>
					<td>Ignore On/Off:</td>
					<td>On:<input type="radio" value="1" name="IgnoreOnOff"> &nbsp; Off:<input type="radio" value="0" name="IgnoreOnOff" checked="checked"></td>
				</tr>
				<tr>
					<td colspan="2" align="center"><input type="submit" class="button" name="submitX" value="Save"  ></td>
				</tr>
			</table>
		</form>
		<script>
		 	var frmvalidator = new formValidator("addMyDevice");
 			frmvalidator.addValidation("Description","req","Please enter a device description");			
	 		frmvalidator.addValidation("masterDevice","dontselect=0","Please select a Device Template!");			
		</script>
	
	</form>
	';
} else {
	// check if the user has the right to modify installation
	$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
	if (!$canModifyInstallation){
		header("Location: index.php?section=addMyDevice&parentID=$parentID&error=You are not authorised to change the installation.");
		exit(0);
	}
		
	$descriptionMyDevice = cleanString($_POST['Description']);
	$IPaddressMyDevice = cleanString($_POST['IPaddress']);
	$MACaddressMyDevice = cleanString($_POST['MACaddress']);
	$ignoreOnOff = cleanInteger(@$_POST['IgnoreOnOff']);
	$masterDeviceID = cleanInteger($_POST['masterDevice']);
	

	if ($masterDeviceID!=0 && $descriptionMyDevice!='') {
		echo '/usr/pluto/bin/CreateDevice -h localhost -D '.$dbPlutoMainDatabase.' -d '.$masterDeviceID.' -i '.$installationID.(($IPaddressMyDevice!='')?' -I '.$IPaddressMyDevice:'').(($MACaddressMyDevice!='')?' -M '.$MACaddressMyDevice:'').(($parentID!=0)?' -C '.$parentID:'');
		$insertID=exec('/usr/pluto/bin/CreateDevice -h localhost -D '.$dbPlutoMainDatabase.' -d '.$masterDeviceID.' -i '.$installationID.(($IPaddressMyDevice!='')?' -I '.$IPaddressMyDevice:'').(($MACaddressMyDevice!='')?' -M '.$MACaddressMyDevice:'').(($parentID!=0)?' -C '.$parentID:''));
		$dbADO->Execute('UPDATE Device SET Description=?, IgnoreOnOff=? WHERE PK_Device=?',array($descriptionMyDevice,$ignoreOnOff,$insertID));
		setDCERouterNeedConfigure($_SESSION['installationID'],$dbADO);
		exit();
		$out.="<script>
			top.frames['treeframe'].location='index.php?section=leftMenu';
			self.location.href=\"index.php?section=editDeviceParams&deviceID=$insertID&parentID=$parentID\";
		</script>";
		
	} else {
		header("Location: index.php?section=addMyDevice&parentID=$parentID&error=not all MACaddress");
		exit(0);
	}
	
}

$output->setScriptCalendar('null');

$output->setBody($out);
$output->setTitle(APPLICATION_NAME);			
$output->output();  	
}
