<?
function viewCameras($output,$dbADO) {
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
//	$dbADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];

	$deviceCategory=$GLOBALS['rootCameras'];

	$displayedDevices=array();
	if($action=='form'){
		getDeviceCategoryChildsArray($deviceCategory,$dbADO);
		$GLOBALS['childsDeviceCategoryArray']=cleanArray($GLOBALS['childsDeviceCategoryArray']);
		$GLOBALS['childsDeviceCategoryArray'][]=$deviceCategory;
		
		$queryDeviceTemplate='
			SELECT * FROM DeviceTemplate 
				WHERE FK_DeviceCategory IN ('.join(',',$GLOBALS['childsDeviceCategoryArray']).')
			ORDER BY Description ASC';
		$resDeviceTemplate=$dbADO->Execute($queryDeviceTemplate);
		$DTArray=array();
		while($rowDeviceCategory=$resDeviceTemplate->FetchRow()){
			$DTArray[$rowDeviceCategory['Description']]=$rowDeviceCategory['PK_DeviceTemplate'];
		}
		if(count($DTArray)==0)
			$DTArray[]=0;
		
	
		$out.='
		<script>
		function selAllCheckboxes()
		{
		   val=(document.viewCameras.setAll.checked)?true:false;
		   for (i = 0; i < viewCameras.elements.length; i++)
		   {
		     if (viewCameras.elements[i].type == "checkbox")
		     {
		         viewCameras.elements[i].checked = val;
		     }
		   }
		}
		</script>
		
	<div class="err" align="center">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
	<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
	<div align="center"><h3>View Cameras</h3></div>
	<form action="index.php" method="POST" name="viewCameras">
	<input type="hidden" name="section" value="viewCameras">
	<input type="hidden" name="action" value="update">		
<table align="center" cellpading="3" cellspacing="3" border="0" width="100%">
<tr>
	<td width="200">
	<table align="center" cellpading="3" cellspacing="3" border="0">';
	$queryDevice='
		SELECT 
			Device.Description AS DeviceName, DeviceTemplate.Description AS Template, PK_Device, Room.Description AS RoomName, PK_Room
		FROM 
			Room
			LEFT JOIN Device ON FK_Room=PK_Room AND FK_DeviceTemplate IN ('.join(',',$DTArray).')
			LEFT JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
		WHERE
			Room.FK_Installation=? 
		ORDER BY RoomName ASC, DeviceName ASC';	
	$resDevice=$dbADO->Execute($queryDevice,array($installationID));
	if($resDevice->RecordCount()==0){
		$out.='
			<tr>
				<td align="center">No devices</td>
			</tr>
			';
	}
	$initRoom=0;
	$devicesArray=array();
	while($rowD=$resDevice->FetchRow()){
		if($initRoom!=$rowD['PK_Room']){
			$out.='
			<tr>
				<td align="center" bgcolor="#D1D9EA"><B>'.$rowD['RoomName'].'</B></td>
			</tr>';
			$initRoom=$rowD['PK_Room'];
		}
		if($rowD['PK_Device']!='')
			$devicesArray[]=$rowD['PK_Device'];
		$out.='
			<tr>
				<td align="left">'.(($rowD['PK_Device']!='')?'<input type="checkbox" name="camera_'.$rowD['PK_Device'].'" '.((isset($_POST['camera_'.$rowD['PK_Device']]))?'checked':'').'> ':'').'<B>'.$rowD['DeviceName'].'</B></td>
			</tr>';
	}
	if(count($devicesArray)>0){
		$out.='
			<tr>
				<td><input type="checkbox" name="setAll" onClick="selAllCheckboxes()" '.((isset($_POST['setAll']))?'checked':'').'> <B>Check/Uncheck all</B></td>
			</tr>
			<tr>
				<td><input type="button" class="button" name="preview" onClick="document.viewCameras.action.value=\'form\';document.viewCameras.submit();" Value="Preview checked cameras"></td>
			</tr>';
	}
	$out.='
	</table>
	<input type="hidden" name="devicesArray" value="'.join(',',$devicesArray).'">
	</td>
	<td align="center">';
	if(!isset($_POST['devicesArray'])){
		$out.='No camera selected.';
	}else{
		// TODO: display cameras screenshoots
		$out.='Selected cameras: '.$_POST['devicesArray'];
	}
	
	$out.='</td>
</tr>
</table>
	</form>';
	}else{
		// process area
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=viewCameras&error=You are not authorised to change the installation.");
			exit(0);
		}
		
	}
	
	
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: View Cameras');
	$output->output();
}
?>
