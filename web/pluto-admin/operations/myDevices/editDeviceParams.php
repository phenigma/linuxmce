<?
function editDeviceParams($output,$dbADO) {
/* @var $dbADO ADOConnection */
/* @var $rs ADORecordSet */

$out='';
$dbADO->debug=false;
$deviceID = (int)$_REQUEST['deviceID'];
$installationID = (int)@$_SESSION['installationID'];

	if (!isset($_SESSION['userLoggedIn']) || $_SESSION['userLoggedIn']!=true) {
		header("Location: index.php?section=login&last=editDeviceParams&deviceID=$deviceID");
		exit();
	}
	$query = "select 
				FK_DeviceTemplate,FK_Device_ControlledVia,Device.Description,IPaddress,MACaddress,IgnoreOnOff,NeedConfigure,
				DeviceTemplate.Description as MDL_description,FK_Room, Comments
					from Device 
						Inner JOIN DeviceTemplate on FK_DeviceTemplate = PK_DeviceTemplate
						where PK_Device = ?";
	$res = $dbADO->Execute($query,array($deviceID));
	
	
	if ($res) {
		$row = $res->FetchRow();
		$DeviceTemplate = $row['FK_DeviceTemplate'];	
		$controlledVia = $row['FK_Device_ControlledVia'];
		$description = stripslashes($row['Description']);
		$mdlDescription = stripslashes($row['MDL_description']);
		$ipAddress = stripslashes($row['IPaddress']);
		$macAddress = stripslashes($row['MACaddress']);
		$ignoreOnOff= $row['IgnoreOnOff'];
		$deviceNeedConfigure= $row['NeedConfigure'];
		$deviceRoom=$row['FK_Room'];
		$dtComments=$row['Comments'];
	}
	
	if ($DeviceTemplate==0) {
		header("Location: index.php?error=invalid_Device_id");
	}
	
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';

	if ($action == 'form') {
	$deviceDataFromMasterDevice = "
	SELECT 						
						DeviceTemplate_DeviceData.FK_DeviceData
						 FROM 
						DeviceTemplate_DeviceData
							WHERE (FK_DeviceTemplate='$DeviceTemplate')
	";	
	
	$resDatafromMasterDevice = $dbADO->Execute($deviceDataFromMasterDevice);
	$deviceDataToShow=array();
	$deviceDataToShow[]=0;
	
	if ($resDatafromMasterDevice) {
		while($row=$resDatafromMasterDevice->FetchRow())
			$deviceDataToShow[]=$row['FK_DeviceData'];
	}

	$deviceData="
		SELECT
			DeviceData.Description as DD_desc,
			DeviceData.PK_DeviceData as PK_DD,
			ParameterType.Description as PT_Desc,
			Device_DeviceData.IK_DeviceData as IK_DeviceData,
			AllowedToModify,DeviceTemplate_DeviceData.FK_DeviceTemplate AS DT_DD_Exists
		FROM Device_DeviceData 
			INNER JOIN DeviceData ON Device_DeviceData.FK_DeviceData=PK_DeviceData
			INNER JOIN ParameterType on FK_ParameterType = PK_ParameterType
			INNER JOIN Device ON FK_Device=PK_Device
			LEFT JOIN DeviceTemplate_DeviceData ON DeviceTemplate_DeviceData.FK_DeviceTemplate=Device.FK_DeviceTemplate AND DeviceTemplate_DeviceData.FK_DeviceData=Device_DeviceData.FK_DeviceData
		 WHERE FK_Device = $deviceID";
	$resDeviceData = $dbADO->_Execute($deviceData);
	$childsNo = getChildsNo($deviceID,$dbADO);
	$out.='
	<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
	</script>
	
	
		<div class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
		<div class="confirm">'.(isset($_GET['msg'])?strip_tags($_GET['msg']):'').'</div>
	
	
		<a href="index.php?section=addMyDevice&parentID='.$deviceID.'">Create '.($deviceID==0?' Top Level ':'').'Child Device</a> &nbsp; &nbsp; &nbsp;

	<a href="javascript: if (confirm(\'Are you sure you want to delete this device? '.($childsNo>0?'It has '.$childsNo.' child'.($childsNo>1?'s':''):'').'!\')) {document.location.href=\'index.php?section=deleteMyDevice&deviceID='.$deviceID.'&from=editDeviceParams\';}">Delete This Device</a>
	<form method="post" action="index.php" name="editDeviceParams">
	<fieldset>
	<legend>Device Info #'.$deviceID.'</legend>
	<table>
	<tr><td>Description</td><td><input type="text" name="DeviceDescription" value="'.$description.'" size="40"></td></tr>
	<tr>
		<td>Device Template</td><td><B>'.$mdlDescription.' #'.$DeviceTemplate.'</B> <input value="View" type="button" class="button" name="controlGoToMDL" onClick="windowOpen(\'index.php?section=editMasterDevice&model='.$DeviceTemplate.'&from=editDeviceParams\',\'width=1024,height=768,toolbars=true,scrollbars=1,resizable=1\');"></td>
	</tr>
	<tr>
		<td>Device Template Comments</td>
		<td>'.$dtComments.'</td>
	</tr>
	
				<tr>
					<td valign="top">This device is controlled via:</td>
					<td>
					';
					$whereClause='';
					if ($deviceID!=0) {
						$selectMasterDeviceForParent = 'SELECT FK_DeviceTemplate FROM Device WHERE PK_Device = ? AND FK_Installation=?';
						$resSelectMasterDeviceForParent = $dbADO->Execute($selectMasterDeviceForParent,array($deviceID,$installationID));
						 if ($resSelectMasterDeviceForParent) {
						 	$rowSelectMasterDeviceForParent = $resSelectMasterDeviceForParent->FetchRow();
						 	$DeviceTemplateForParent = $rowSelectMasterDeviceForParent['FK_DeviceTemplate']; 	
						 	
						 	$querySelectControlledViaCategory ='SELECT FK_DeviceCategory FROM DeviceTemplate_DeviceCategory_ControlledVia where FK_DeviceTemplate = ?';
							$resSelectControlledViaCategory = $dbADO->Execute($querySelectControlledViaCategory,array($DeviceTemplateForParent));
						
							$GLOBALS['childsDeviceCategoryArray'] = array();
								if ($resSelectControlledViaCategory) {
									while ($rowSelectControlledVia = $resSelectControlledViaCategory->FetchRow()) {
										$GLOBALS['childsDeviceCategoryArray'][]=$rowSelectControlledVia['FK_DeviceCategory'];
										getDeviceCategoryChildsArray($rowSelectControlledVia['FK_DeviceCategory'],$dbADO);			
									}
								}
							$controlledViaCategoryArray = cleanArray($GLOBALS['childsDeviceCategoryArray']);
							
							$querySelectControlledViaDeviceTemplate ='SELECT  FK_DeviceTemplate_ControlledVia FROM DeviceTemplate_DeviceTemplate_ControlledVia WHERE FK_DeviceTemplate = ?';
							$resSelectControlledViaDeviceTemplate= $dbADO->Execute($querySelectControlledViaDeviceTemplate,array($DeviceTemplateForParent));
						
							$childsDeviceTemplateArray = array();
								if ($resSelectControlledViaDeviceTemplate) {
									while ($rowSelectControlledVia = $resSelectControlledViaDeviceTemplate->FetchRow()) {
										$childsDeviceTemplateArray[]=$rowSelectControlledVia['FK_DeviceTemplate_ControlledVia'];				
									}
								}
							
							
							
							if (count($controlledViaCategoryArray)>0) {
								$whereClause.=' and (FK_DeviceCategory in ('.join(",",$controlledViaCategoryArray).')';
								if (count($childsDeviceTemplateArray)) {
									$whereClause.=' OR PK_DeviceTemplate in ('.join(",",$childsDeviceTemplateArray).')';
								}
								$whereClause.=')';
							} else {
								if (count($childsDeviceTemplateArray)) {
									$whereClause.=' AND PK_DeviceTemplate in ('.join(",",$childsDeviceTemplateArray).')';
								}
							}
							
						 }						 
					}
					$whereClause.=" AND PK_Device!='$deviceID'";
					$queryDeviceTemplate = "
						SELECT DISTINCT Device.Description,Device.PK_Device
							FROM Device 
						INNER JOIN DeviceTemplate ON FK_DeviceTemplate = PK_DeviceTemplate
							WHERE FK_Installation=? $whereClause order by Device.Description asc";
					$resDeviceTemplate = $dbADO->Execute($queryDeviceTemplate,$installationID);
					$DeviceTemplateTxt = '';
					if($resDeviceTemplate) {
						while ($row=$resDeviceTemplate->FetchRow()) {
							$DeviceTemplateTxt.='<option value="'.$row['PK_Device'].'" '.($row['PK_Device']==$controlledVia?' selected="selected" ':'').'>'.$row['Description'].'</option>';
						}
					}

					$out.='<select name="controlledVia">
					<option value="0"></option>'.$DeviceTemplateTxt.'</select>';
					
					$out.='		
					</td>
				</tr>
				<tr>
					<td>Room:</td>
					<td><select name="Room">
						<option value="0">Please select</option>';
					$selectRooms='SELECT * FROM Room WHERE FK_installation=?';
					$resRooms=$dbADO->Execute($selectRooms,$installationID);
					while($rowRooms=$resRooms->FetchRow()){
						$out.='<option value="'.$rowRooms['PK_Room'].'" '.(($rowRooms['PK_Room']==$deviceRoom)?'selected':'').'>'.$rowRooms['Description'].'</option>';
					}
					$out.='
					</select></td>
				</tr>
				<tr>
					<td>Entertain Areas:</td>
					<td>';
					$selectCheckedEA='SELECT * FROM Device_EntertainArea WHERE FK_Device=?';
					$resCheckedEA=$dbADO->Execute($selectCheckedEA,$deviceID);
					$checkedArray=array();
					while($rowCheckedEA=$resCheckedEA->FetchRow()){
						$checkedArray[]=$rowCheckedEA['FK_EntertainArea'];
					}

					$selectEntertainArea='
						SELECT 
							EntertainArea.*
						FROM EntertainArea
							INNER JOIN Room ON FK_Room=PK_Room
						WHERE FK_installation=?';
					$displayedEntAreas=array();
					$oldEntAreas=array();
					$resEntertainArea=$dbADO->Execute($selectEntertainArea,$installationID);
					while($rowEntertainArea=$resEntertainArea->FetchRow()){
						$oldEntAreas[]=(in_array($rowEntertainArea['PK_EntertainArea'],$checkedArray))?1:0;
						$displayedEntAreas[]=$rowEntertainArea['PK_EntertainArea'];
						$out.='<input type="checkbox" value="1" name="entArea_'.$rowEntertainArea['PK_EntertainArea'].'" '.((in_array($rowEntertainArea['PK_EntertainArea'],$checkedArray))?'checked':'').'> '.$rowEntertainArea['Description'].'<br>';
					}
					$out.='
					<input type="hidden" name="displayedEntAreas" value="'.join(',',$displayedEntAreas).'">
					<input type="hidden" name="oldEntAreas" value="'.join(',',$oldEntAreas).'">
					</td>
				</tr>					
				<tr>
					<td>IP Address:</td>
					<td><input name="ipAddress" value="'.$ipAddress.'"></td>
				</tr>
				<tr>
					<td>MAC Address:</td>
					<td><input name="macAddress" value="'.$macAddress.'"></td>
				</tr>
				<tr>
					<td>Ignore On/Off:</td>
					<td>On:<input type="radio" value="1" name="IgnoreOnOff" '.($ignoreOnOff==1?' checked="checked" ':'').'> &nbsp; 
						Off:<input type="radio" value="0" name="IgnoreOnOff" '.($ignoreOnOff==0?' checked="checked" ':'').'></td>
				</tr>
				<tr>
					<td><input type="checkbox" name="needConfigure" value="1" '.(($deviceNeedConfigure==1)?'checked':'').' onClick="javascript:document.editDeviceParams.submit();"></td>
					<td>Reconfigure device</td>
				</tr>
					
				<tr>
					<td colspan="2" align="center"><input type="submit" class="button" name="submitX" value="Save"  ></td>
				</tr>
	</table>
	</fieldset>
					<br />
	<fieldset>				
	<legend>Device Pipes Used</legend>
	<table>
		<tr>
			<td>
				<table>
					';
	
	$selectPipes = 'SELECT * FROM Pipe Order By Description ASC';
	$resSelectPipes = $dbADO->Execute($selectPipes);
	
	$selectPipesUsed = 'SELECT Device_Device_Pipe.*,D1.Description as Desc_From,D2.Description as Desc_To,D2.FK_DeviceTemplate AS DT_To
							 FROM Device_Device_Pipe 
								INNER JOIN Device D1 ON D1.PK_Device = FK_Device_From
								INNER JOIN Device D2 ON D2.PK_Device = FK_Device_To
						WHERE FK_Device_From = ?';
	$resSelectPipesUsed = $dbADO->Execute($selectPipesUsed,array($deviceID));
	
	if ($resSelectPipesUsed) {
		while ($rowSelectedPipesUsed = $resSelectPipesUsed->FetchRow()) {
			$selectInputs = '
				SELECT * 
				FROM Command 
				INNER JOIN DeviceTemplate_Input ON FK_Command=PK_Command
				WHERE FK_CommandCategory=22 AND FK_DeviceTemplate=?
				ORDER BY Description ASC';
			$resSelectInputs = $dbADO->Execute($selectInputs,$rowSelectedPipesUsed['DT_To']);
			
			$selectOutputs = '
				SELECT * FROM Command 
				INNER JOIN DeviceTemplate_Output ON FK_Command=PK_Command
				WHERE FK_CommandCategory=27 AND FK_DeviceTemplate=?
				Order By Description ASC';
			$resSelectOutputs = $dbADO->Execute($selectOutputs,$rowSelectedPipesUsed['DT_To']);
			
			$out.='<tr><td>'.$rowSelectedPipesUsed['Desc_To']." <input type='hidden' name='deviceTo_{$rowSelectedPipesUsed['FK_Device_To']}'> &nbsp;&nbsp;&nbsp;&nbsp;</td>";
			
			$selectInputsTxt='';
			while ($rowSelInputs = $resSelectInputs->FetchRow()) {
				$selectInputsTxt.= '<option '.($rowSelInputs['PK_Command']==$rowSelectedPipesUsed['FK_Command_Input']?" selected='selected' ":"").' value="'.$rowSelInputs['PK_Command'].'">'.$rowSelInputs['Description'].'</option>';
			}
			
			$out.='<td> Input on '.$rowSelectedPipesUsed['Desc_To'].' <select name="input_'.$rowSelectedPipesUsed['FK_Device_To'].'_'.$rowSelectedPipesUsed['FK_Pipe'].'"><option value="0">-please select-</option>'.$selectInputsTxt.'</select></td>';
			
			$selectOutputsTxt='';
			while ($rowSelOutputs = $resSelectOutputs->FetchRow()) {
				$selectOutputsTxt.= '<option '.($rowSelOutputs['PK_Command']==$rowSelectedPipesUsed['FK_Command_Output']?" selected='selected' ":"").' value="'.$rowSelOutputs['PK_Command'].'">'.$rowSelOutputs['Description'].'</option>';
			}
			
			$out.='<td> Output on '.$rowSelectedPipesUsed['Desc_From'].' <select name="output_'.$rowSelectedPipesUsed['FK_Device_To'].'_'.$rowSelectedPipesUsed['FK_Pipe'].'"><option value="0">-please select-</option>'.$selectOutputsTxt.'</select></td>';
			

			$resSelectPipes->MoveFirst();			
			$selectPipesTxt='';
			while ($rowSelPipes = $resSelectPipes->FetchRow()) {
				$selectPipesTxt.= ($rowSelPipes['PK_Pipe']==$rowSelectedPipesUsed['FK_Pipe'])?$rowSelPipes['Description']:"";
			}
			
			$out.='<td> Pipe '.$selectPipesTxt.'</td>';
			$out.='<td><input value="Delete" type="button" class="button" onClick="if (confirm(\'Are you sure you want to delete this pipe?\')) {windowOpen(\'index.php?section=deleteDevicePipeFromDevice&deviceFromID='.$rowSelectedPipesUsed['FK_Device_From'].'&deviceToID='.$rowSelectedPipesUsed['FK_Device_To'].'&pipe='.$rowSelectedPipesUsed['FK_Pipe'].'&from=editDeviceParams\',\'width=100,height=100,toolbars=true,scrollbars=1,resizable=1\');}"></td>';
			
			$out.='</tr>';
		}
	}
	
	$out.='
				</table>
			</td>
		</tr>
		<tr>
			<td>
				<select name="addDeviceForPiping">
					<option value="0">-please select-</option>
					';
			
				$querySelectDevice = "SELECT Description,PK_Device FROM Device Where FK_Installation = ? Order By Description ASC";
				$resSelectDevice = $dbADO->Execute($querySelectDevice,array($installationID));
				 if ($resSelectDevice) {
				 	while ($rowSelectDevice = $resSelectDevice->FetchRow()) {
				 		$out.='<option value="'.$rowSelectDevice['PK_Device'].'">'.$rowSelectDevice['Description'].'</option>'."\n";
				 	}
				 }
			$resSelectPipes->MoveFirst();			
			$selectPipesTxt='';
			while ($rowSelPipes = $resSelectPipes->FetchRow()) {
				$selectPipesTxt.= '<option value="'.$rowSelPipes['PK_Pipe'].'">'.$rowSelPipes['Description'].'</option>';
			}
				 
			$out.='
				</select>
				<select name="addPipe">
					<option value="0">-please select-</option>'.$selectPipesTxt.'</select>
			<input type="submit" class="button" name="submitX" value="Add"  >
			</td>
		</tr>
	    <tr><td><input type="submit" class="button" name="submitX" value="Save"  ></td></tr>
	</table>
	</fieldset>

	<br />
	<fieldset>				
	<legend>Member Of Group</legend>
	
	<table>				';

	$selectDeviceGroups = 'SELECT DeviceGroup.Description,Device.PK_Device,DeviceGroup.PK_DeviceGroup FROM 
		Device_DeviceGroup			
			INNER JOIN Device ON FK_Device = PK_Device
			INNER JOIN DeviceGroup ON FK_DeviceGroup = PK_DeviceGroup
		WHERE Device_DeviceGroup.FK_Device = ?
	';
	$resDevicesGroups = $dbADO->Execute($selectDeviceGroups,array($deviceID));
	
	$deviceGroups = array();
	
	$deviceGroups[]=0;
	if ($resDevicesGroups) {	
		if ($resDevicesGroups->RecordCount()==0){
			$out.='<tr><td colspan="2">No groups for device</a></td></tr>';
		} 
			
		while ($rowDevicesGroup = $resDevicesGroups->FetchRow()) {
			$out.="<tr>
					<td>".$rowDevicesGroup['Description']."</td>					
					<td>
						<input value='Delete' type='button' onClick=\"if (confirm('Are you sure you want to delete this device from the group?')) {windowOpen('index.php?section=deleteDeviceFromDeviceGroup&deviceID=$deviceID&deviceGroupID=".$rowDevicesGroup['PK_DeviceGroup']."&from=editDeviceParams','width=200,height=200,toolbars=true,scrollbars=1,resizable=1');}\">
					</td>
				  </tr>";
			$deviceGroups[]=$rowDevicesGroup['PK_DeviceGroup'];
		}
	}
					
	$selectRemainingGroups = "SELECT Description,PK_DeviceGroup from DeviceGroup where FK_Installation = ? and PK_DeviceGroup not in (".join(",",$deviceGroups).") order by Description ASC";
	$resRemainingGroups = $dbADO->Execute($selectRemainingGroups,array($installationID));
	
	$remainingGroups = '<option value="0">-please select-</option>';
	if ($resRemainingGroups) {
		while ($rowRemainingGroups = $resRemainingGroups->FetchRow()) {
			$remainingGroups.='<option value="'.$rowRemainingGroups['PK_DeviceGroup'].'">'.$rowRemainingGroups['Description'].'</option>';
		}
	}
	$out.='
	
	<tr><td><select name="addNewGroup">'.$remainingGroups.'</select></td><td><input type="submit" class="button"   name="submitX" value="Add"></td></tr>		
	<tr><td><a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=createDeviceGroup&deviceID='.$deviceID.'&from=editDeviceParams\',\'width=400,height=400,toolbars=true,scrollbars=1,resizable=1\');">Create new device group</a></td></tr>
	
		<tr><td colspan="2"><input type="submit" class="button" name="submitX" value="Save"  ></td></tr>
	</table>
	</fieldset>
	<fieldset>
	<br />
	<legend>Related Devices</legend>
					
	<table>
	';
	$selectRelatedDevices = 'SELECT Device_Device_Related.Value,FK_Device_Related,Device.Description,DeviceTemplate.Description as  DT_Desc FROM 
		Device_Device_Related 			
			INNER JOIN Device ON FK_Device_Related = PK_Device
			INNER JOIN DeviceTemplate ON FK_DeviceTemplate = PK_DeviceTemplate
		WHERE FK_Device = ?
	';
	$resRelatedDevices = $dbADO->Execute($selectRelatedDevices,array($deviceID));
	
	$deviceRelatedData = array();
	$deviceRelatedData[]=0;
	if ($resRelatedDevices) {	
		if ($resRelatedDevices->RecordCount()==0){
			$out.='<tr><td colspan="2">No related devices</a></td></tr>';
		} 
			
		while ($rowRelatedDevice = $resRelatedDevices->FetchRow()) {
			$out.="<tr>
					<td>".$rowRelatedDevice['Description']." ({$rowRelatedDevice['DT_Desc']}) #".$rowRelatedDevice['FK_Device_Related']."</td><td>
						<input type=\"text\" name=\"relatedDeviceValue_".$rowRelatedDevice['FK_Device_Related']."\" value=\"".stripslashes($rowRelatedDevice['Value'])."\">
					</td>
					<td>
						<input value='Delete' type='button' onClick=\"if (confirm('Are you sure you want to delete this device?')) {windowOpen('index.php?section=deleteDeviceRelatedFromDeviceParams&deviceID=$deviceID&relatedID=".$rowRelatedDevice['FK_Device_Related']."&from=editDeviceParams','width=200,height=200,toolbars=true,scrollbars=1,resizable=1');}\">
					</td>
				  </tr>";
			$deviceRelatedData[]=$rowRelatedDevice['FK_Device_Related'];
		}
	}
	
	$selectAddNewRelatedDevices = "SELECT Device.Description,PK_Device,DeviceTemplate.Description as DT_Desc FROM Device 
		INNER JOIN DeviceTemplate ON FK_DeviceTemplate = PK_DeviceTemplate
		WHERE FK_Installation = ? and PK_Device != ? 
		AND PK_Device NOT IN (".join(",",$deviceRelatedData).") ORDER BY Description ASC";
	
	$resAddNewRelatedDevices = $dbADO->Execute($selectAddNewRelatedDevices,array($installationID,$deviceID));
	$newDeviceRelated='<option value="0">-please select-</option>';
	if ($resAddNewRelatedDevices) {
		while ($rowAddNewRelatedDevices = $resAddNewRelatedDevices->FetchRow()) {
			$newDeviceRelated.='<option value="'.$rowAddNewRelatedDevices['PK_Device'].'">'.$rowAddNewRelatedDevices['Description'].'('.$rowAddNewRelatedDevices['DT_Desc'].')</option>';
		}
	}
	$out.='
		
		<tr><td><select name="addNewDeviceRelated">'.$newDeviceRelated.'</select></td><td><input type="submit" class="button" name="submitX" value="Add"  ></td></tr>
	
		<input type="hidden" name="selectedRelatedDevice" value="'.(join(",",$deviceRelatedData)).'">
		<tr><td colspan="2"><input type="submit" class="button" name="submitX" value="Save"  ></td></tr>
	</table>	
	</fieldset>
	
	<br />
	<fieldset>
		<legend>Device Data</legend>
	
		
			<input type="hidden" value="editDeviceParams" name="section">
			<input type="hidden" value="add" name="action">
			<input type="hidden" value="'.$deviceID.'" name="deviceID">			
	<table>
	';
	$deviceData = array();
	$deviceData[]=0;
	if ($resDeviceData) {	
		if ($resDeviceData->RecordCount()==0){
			$out.='<tr><td colspan="2">No device data</a></td></tr>';
		} 
			
		while ($rowDevicedata = $resDeviceData->FetchRow()) {
			$out.="
				<tr>
					<td>{$rowDevicedata['DD_desc']}({$rowDevicedata['PT_Desc']})</td>
					<td><input type=\"text\" name=\"deviceData_".$rowDevicedata['PK_DD']."\" value=\"".stripslashes($rowDevicedata['IK_DeviceData'])."\" ".(($rowDevicedata['AllowedToModify']==1 || $rowDevicedata['DT_DD_Exists']=='')?'':'disabled')."></td>
				</tr>";
			$deviceData[]=$rowDevicedata['PK_DD'];
		}
	}
	
		$validOrbiters=getValidOrbitersArray($installationID,$dbADO);
		$validComputers=getValidComputersArray($installationID,$dbADO);
	
	$out.=(($resDeviceData && $resDeviceData->RecordCount()>0)?'
		<input type="hidden" name="selectedData" value="'.(join(",",$deviceData)).'">
		<tr><td><input type="submit" class="button" name="submitX" value="Save"  ></td></tr>
	':'').'	
	</table>
	</fieldset>
	</form>
	<script>
		 	var frmvalidator = new formValidator("editDeviceParams");
 			frmvalidator.addValidation("DeviceDescription","req","Please enter a description");
			frmvalidator.addValidation("DeviceTemplate","dontselect=0","Please select a master device!");			
		</script>
	
	<br />';
	
	
	} else {
		
		//check if current user canModifyInstallation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$installationID,$dbADO);
	
	if ($canModifyInstallation) {
		
		$description = cleanString($_POST['DeviceDescription']);
		$ipAddress = cleanString($_POST['ipAddress']);
		$macAddress = cleanString($_POST['macAddress']);
		$ignoreOnOff = cleanInteger($_POST['IgnoreOnOff']);
		$controlledVia = (@$_POST['controlledVia']!='0')?cleanInteger(@$_POST['controlledVia']):NULL;
		$needConfigure = (isset($_POST['needConfigure']))?cleanInteger($_POST['needConfigure']):0;
		
		$addNewDeviceRelated = (int)$_POST['addNewDeviceRelated'];
		if ($addNewDeviceRelated!=0) {
			$queryInsertDeviceRelated = 'INSERT INTO Device_Device_Related 
			(FK_Device,FK_Device_Related)
		values(?,?)
			';
			$res=$dbADO->Execute($queryInsertDeviceRelated,array($deviceID,$addNewDeviceRelated));			
		}
		
		$addNewGroup = (int)$_POST['addNewGroup'];
		if ($addNewGroup!=0) {
			$queryInsertDeviceGroup = 'INSERT INTO Device_DeviceGroup
			(FK_Device,FK_DeviceGroup)
		values(?,?)
			';
			$res=$dbADO->Execute($queryInsertDeviceGroup,array($deviceID,$addNewGroup));			
		}
		
		
		$selectedRelatedDevices = $_POST['selectedRelatedDevice'];
		$selectedRelatedDevicesArray = explode(",",$selectedRelatedDevices);
		foreach ($selectedRelatedDevicesArray as $selectedRelatedDev) {
			$selectOldValueQuery = "SELECT Value From Device_Device_Related WHERE FK_Device = ? AND  FK_Device_Related = ?";
			$selectOldValueRes = $dbADO->Execute($selectOldValueQuery,array($deviceID,$selectedRelatedDev));
			$selectOldValueRow = array();
			 if ($selectOldValueRes) {
			 	$selectOldValueRow = $selectOldValueRes->FetchRow();			 	
			 }
			$valueRelatedDevice = cleanString(@$_POST["relatedDeviceValue_".$selectedRelatedDev]);
			if ($valueRelatedDevice!=$selectOldValueRow) {
				$updateQuery = 'UPDATE Device_Device_Related SET IK_DeviceData = ? WHERE FK_Device = ? AND FK_Device_Related = ?';
				$resUpdate = $dbADO->Execute($updateQuery,array($valueRelatedDevice,$deviceID,$selectedRelatedDev));
			}
		}

		// save the Device Data values
		$selectedDate = cleanString(@$_POST['selectedData']);
		$selectedDateArray = explode(",",$selectedDate);
		if (!is_array($selectedDateArray)) {$selectedDateArray=array();$selectedDateArray[]=0;}

		foreach ($selectedDateArray as $elem) {
			$value = cleanString(@$_POST['deviceData_'.$elem]);
			if ($value!='') {
				$checkIfExists = "select IK_DeviceData from Device_DeviceData where FK_Device = ? and FK_DeviceData = ?";
				$res = $dbADO->Execute($checkIfExists,array($deviceID,$elem));
				if ($res && $res->RecordCount()==1) {
					
					$query = "update Device_DeviceData set IK_DeviceData = ? where  FK_Device = ? and FK_DeviceData = ?";
					$rs=$dbADO->Execute($query,array($value,$deviceID,$elem));	
				} else {
					if ($elem!=0) {
						$query = "insert into Device_DeviceData  (IK_DeviceData,FK_Device,FK_DeviceData) values(?,?,?)";
						$rs=$dbADO->Execute($query,array($value,$deviceID,$elem));
						
					}
				}				
			}
		}		
		if (trim($description)!='') {		
			$selectOldValues = 'SELECT * FROM Device where PK_Device = ?';	
			$resOldValues = $dbADO->Execute($selectOldValues,array($deviceID));
				if ($resOldValues) {
					$rowOldValues = $resOldValues->FetchRow();
					$old_description = cleanString($rowOldValues['Description']);
					$old_DeviceTemplate = cleanInteger($rowOldValues['FK_DeviceTemplate']);
						//check if master device list ischanged. if yes, delete the data values
				}
			
			$room=(@$_POST['Room']!='0')?@$_POST['Room']:NULL;	
			$query = "UPDATE Device SET Description=?,IPaddress=?,MACaddress=?,IgnoreOnOff=?,FK_Device_ControlledVia=?,NeedConfigure=?,FK_Room=? WHERE PK_Device = ?";
			$dbADO->Execute($query,array($description,$ipAddress,$macAddress,$ignoreOnOff,$controlledVia,$needConfigure,$room,$deviceID));
			setDCERouterNeedConfigure($installationID,$dbADO);
			$EntAreasArray=explode(',',$_POST['displayedEntAreas']);
			$OldEntAreasArray=explode(',',$_POST['oldEntAreas']);
			
			foreach($EntAreasArray AS $key => $value){
				$entArea=(isset($_POST['entArea_'.$value]))?(int)@$_POST['entArea_'.$value]:0;
				if($entArea!=$OldEntAreasArray[$key]){
					if($entArea==1){
						addDeviceToEntertainArea($deviceID,$value,$dbADO);
					}else{
						$deleteDeviceEntertainArea='DELETE FROM Device_EntertainArea WHERE FK_Device=? AND FK_EntertainArea=?';
						$dbADO->Execute($deleteDeviceEntertainArea,array($deviceID,$value));
					}
				}
			}
		}
		
			
		//pipes
		$addDeviceForPiping = cleanInteger(@$_POST['addDeviceForPiping']);
		$addPipe = cleanInteger(@$_POST['addPipe']);
		
		if ($addDeviceForPiping!=0 && $addPipe!=0) {
			$checkDDP=$dbADO->Execute('SELECT * FROM Device_Device_Pipe WHERE FK_Device_From=? AND FK_Device_To=? AND FK_Pipe=?',array($deviceID,$addDeviceForPiping,$addPipe));
			if($checkDDP->RecordCount()==0){
				$insertDevicePipe  = "INSERT INTO Device_Device_Pipe (FK_Device_From,FK_Device_To,FK_Pipe) VALUES(?,?,?)";
				$res=$dbADO->Execute($insertDevicePipe,array($deviceID,$addDeviceForPiping,$addPipe));
				$msg='Device Pipe added.';
			}
			else
				$error='Device pipe already exist.';
		}
		
		$selectPipesUsed = 'SELECT Device_Device_Pipe.*,D1.Description as Desc_From,D2.Description as Desc_To
							 FROM Device_Device_Pipe 
								INNER JOIN Device D1 ON D1.PK_Device = FK_Device_From
								INNER JOIN Device D2 ON D2.PK_Device = FK_Device_To
						WHERE FK_Device_From = ?';
		$resSelectPipesUsed = $dbADO->Execute($selectPipesUsed,array($deviceID));
		
		while ($rowSelectedPipesUsed = $resSelectPipesUsed->FetchRow()) {	
			$inputName='input_'.$rowSelectedPipesUsed['FK_Device_To'].'_'.$rowSelectedPipesUsed['FK_Pipe'];
			$outputName='output_'.$rowSelectedPipesUsed['FK_Device_To'].'_'.$rowSelectedPipesUsed['FK_Pipe'];
			
			$input=((int)@$_POST[$inputName]!=0)?(int)@$_POST[$inputName]:NULL;
			$pipeOutput=((int)@$_POST[$outputName]!=0)?$_POST[$outputName]:NULL;
						
			$deviceTo=$rowSelectedPipesUsed['FK_Device_To'];
			$pipe=$rowSelectedPipesUsed['FK_Pipe'];
			$updateDevicePipe = 'UPDATE Device_Device_Pipe SET FK_Command_Input=?,FK_Command_Output=? WHERE FK_Device_From = ? AND FK_Device_To = ? AND FK_Pipe=?';
			$res=$dbADO->Execute($updateDevicePipe,array($input,$pipeOutput,$deviceID,$deviceTo,$pipe));
		}

		$out.='
		<script>
			self.location=\'index.php?section=editDeviceParams&deviceID='.$deviceID.((isset($error))?'&error='.$error:'&msg='.@$msg).'\';
			top.frames[\'treeframe\'].location=\'index.php?section=leftMenu\';
		</script>';
	} else {
		Header('Location: index.php?section=editDeviceParams&deviceID='.$deviceID.'&error=You are not allowed to modify installation.');
		exit();
	}
}


$parentsForMenu = getMyDeviceParents($deviceID,$dbADO);
$parentsForMenuArray = explode("**&&**",$parentsForMenu);
$parentsForMenuArray=array_reverse($parentsForMenuArray);
$navMenuString = array();

$navMenuString[]=('My Devices:!:index.php?section=myDevices');


foreach ($parentsForMenuArray as $parent) {
	$parentArray = explode("||&&||",$parent);
	if (count($parentArray)==2)
		$navMenuString[]=($parentArray[1].':!:index.php?section=editDeviceParams&deviceID='.$parentArray[0]);	
}


eval("\$c=array(\$navMenuString);");
	
	$output->setNavigationMenu($c);
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();  		
}

function getMyDeviceParents($childID,$dbADO) {
	//$dbADO->debug=true;
	$stringResult = '';
	
	if ($childID!=0) {
		$queryGP = "select FK_Device_ControlledVia,Description,PK_Device from Device where PK_Device = $childID order by Description Asc";
		$resGP = $dbADO->Execute($queryGP);
		
		if ($resGP) {
			while ($row=$resGP->FetchRow()) {				
					$stringResult.= $row['PK_Device'].'||&&||'.$row['Description'].'**&&**';
					$stringResult.= getMyDeviceParents($row['FK_Device_ControlledVia'],$dbADO);
			}
		}	
	}
	
	return $stringResult;
}

?>
