<?php
function mediaScenarios($output,$dbADO) {
	
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	/* @var $output Template */

//	$dbADO->debug=true;			
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	$arrayID=$GLOBALS['ArrayIDForMedia'];
	$templateWizard=$GLOBALS['MediaScenariosTemplate'];			// from table Template, PK_Template for Media Wizard
	
	$out='';
	if($action=='form'){
		$out.='
			<script>
				function windowOpen(locationA,attributes) {
					window.open(locationA,\'\',attributes);
				}
			</script>
			<form action="index.php" method="POST" name="mediaScenarios">
				<input type="hidden" name="section" value="mediaScenarios">
				<input type="hidden" name="action" value="add">	
				<input type="hidden" name="delCG" value="">	
				<input type="hidden" name="optionEntArea" value="">	
				<input type="hidden" name="optionName" value="">	
				<input type="hidden" name="actionType" value="">	
			<div align="center"><h2>Media scenarios</h2></div>
			<table width="100%" border="0">';
		$queryEntAreas='
			SELECT 
				EntertainArea.*,
				Room.Description AS RoomName 
			FROM EntertainArea
				INNER JOIN Room on FK_Room=PK_Room
			WHERE FK_Installation=?
			ORDER BY RoomName ASC, Description ASC';
		$resEntAreas=$dbADO->Execute($queryEntAreas,$installationID);
		$displayedMediaScenarios=array();
		while($rowEntAreas=$resEntAreas->FetchRow()){
			$checkBoxes='';
			foreach($GLOBALS['mediaOptionsArray'] AS $value){
				$selectOptions='
					SELECT PK_CommandGroup FROM CommandGroup
						INNER JOIN CommandGroup_EntertainArea on CommandGroup_EntertainArea.FK_CommandGroup=PK_CommandGroup
						INNER JOIN Template ON FK_Template=PK_Template
					WHERE Template.Description=? AND FK_EntertainArea=?';
				$resOptions=$dbADO->Execute($selectOptions,array('Media Wiz - '.$value,$rowEntAreas['PK_EntertainArea']));
				
				$checkBoxes.='<input type="checkbox" name="checkbox" value="1" '.(($resOptions->RecordCount()>0)?'checked':'').' onChange="javascript:document.mediaScenarios.optionEntArea.value=\''.$rowEntAreas['PK_EntertainArea'].'\';document.mediaScenarios.actionType.value=\''.(($resOptions->RecordCount()>0)?'deleteOption':'addOption').'\';document.mediaScenarios.optionName.value=\''.$value.'\';document.mediaScenarios.submit()"> '.$value.'<br>';
			}

			$out.='
			<tr bgcolor="#D1D9EA">
				<td colspan="7" align="left"><B>'.$rowEntAreas['Description'].'</B></td>
			</tr>
			<tr>
				<td colspan="7">'.$checkBoxes.'</td>
			</tr>			
			';
			$selectMediaScenarios='
				SELECT CommandGroup.Description, PK_CommandGroup
				FROM CommandGroup
					JOIN CommandGroup_EntertainArea ON CommandGroup_EntertainArea.FK_CommandGroup=PK_CommandGroup
				WHERE CommandGroup.FK_Array=? AND CommandGroup_EntertainArea.FK_EntertainArea=? AND CommandGroup.FK_Installation=? AND FK_Template=?';
			$resMediaScenarios=$dbADO->Execute($selectMediaScenarios,array($arrayID,$rowEntAreas['PK_EntertainArea'],$installationID,$templateWizard));
			if($resMediaScenarios->RecordCount()==0){
				$out.='
				<tr>
					 <td align="center" colspan="7">No Media Scenarios</td>
				</tr>';
			}
			
			while($rowMediaScenarios=$resMediaScenarios->FetchRow()){
				$displayedMediaScenarios[]=$rowMediaScenarios['PK_CommandGroup'];
				$query = "
					SELECT * FROM CommandGroup_Command_CommandParameter 
						INNER JOIN CommandGroup_Command ON FK_CommandGroup_Command = PK_CommandGroup_Command
					WHERE FK_Command = ? AND FK_CommandGroup=?";
				$resParams=$dbADO->Execute($query,array($GLOBALS['MediaScenariosCommand'],$rowMediaScenarios['PK_CommandGroup']));
				$paramsArray=array();
				if($resParams->RecordCount()!=0){
					
				$out.='
				<tr>
					<td align="left"><B>Description</B></td>
					<td align="center"><B>Type</B></td>
					<td align="center"><B>Device</B></td>
					<td align="center"><B>Remote</B></td>
					<td align="center"><B>Device template</B></td>
					<td align="center"><B>Filename</B></td>
					<td>&nbsp;</td>
				</tr>';
					while($rowParams=$resParams->FetchRow()){
						$paramsArray[$rowParams['FK_CommandParameter']]=$rowParams['IK_CommandParameter'];
					}
				
					$getMediaType='SELECT * FROM MediaType WHERE PK_MediaType=?';
					$resMediaType=$dbADO->Execute($getMediaType,$paramsArray[$GLOBALS['commandParamPK_MediaType']]);
					$rowMediaType=$resMediaType->FetchRow();
					$mediaType=$rowMediaType['Description'];
					
					$getDevice='SELECT * FROM Device WHERE PK_Device=?';
					$resDevice=$dbADO->Execute($getDevice,$paramsArray[$GLOBALS['commandParamPK_Device']]);
					$rowDevice=$resDevice->FetchRow();
					$deviceName=$rowDevice['Description'];
	
					$getRemote='SELECT * FROM DesignObj WHERE PK_DesignObj=?';
					$resRemote=$dbADO->Execute($getRemote,$paramsArray[$GLOBALS['commandParamPK_DesignObj']]);
					$rowRemote=$resRemote->FetchRow();
					$remoteName=$rowRemote['Description'];
	
					$getDeviceTemplate='SELECT * FROM DeviceTemplate WHERE PK_DeviceTemplate=?';
					$resDeviceTemplate=$dbADO->Execute($getDeviceTemplate,$paramsArray[$GLOBALS['commandParamPK_DeviceTemplate']]);
					$rowDeviceTemplate=$resDeviceTemplate->FetchRow();
					$deviceTemplateName=$rowDevice['Description'];
				}
								
				$out.='
				<tr>
					<td><input type="text" name="description_'.$rowMediaScenarios['PK_CommandGroup'].'" value="'.$rowMediaScenarios['Description'].'"></td>
					<td align="center">'.@$mediaType.' #'.@$paramsArray[$GLOBALS['commandParamPK_MediaType']].'</td>
					<td align="center">'.@$deviceName.' #'.@$paramsArray[$GLOBALS['commandParamPK_Device']].'</td>
					<td align="center">'.@$remoteName.' #'.@$paramsArray[$GLOBALS['commandParamPK_DesignObj']].'</td>
					<td align="center">'.@$deviceTemplateName.' #'.@$paramsArray[$GLOBALS['commandParamPK_DeviceTemplate']].'</td>
					<td align="center">'.@$paramsArray[$GLOBALS['commandParamFilename']].'</td>
					<td><a href="index.php?section=editCommandGroup&cgID='.$rowMediaScenarios['PK_CommandGroup'].'">Edit</a> <a href="#" onClick="javascript:if(confirm(\'Are you sure you want to delete the scenario?\')){document.mediaScenarios.action.value=\'delete\';document.mediaScenarios.delCG.value=\''.$rowMediaScenarios['PK_CommandGroup'].'\';document.mediaScenarios.submit()}">Delete</a></td>
				</tr>
				';
			}
		}
		
		if(count($displayedMediaScenarios)>0){
			$out.='
			<tr>
				<td colspan="7" align="center"><input type="submit" name="saveChanges" value="Update descriptions"></td>
			</tr>
			';
		}
		$out.='
			<input type="hidden" name="displayedMediaScenarios" value="'.(join(',',$displayedMediaScenarios)).'">
				<tr>
					<td colspan="7">&nbsp;</td>
				</tr>
			</table>	
			<table>
				<tr>
					<td colspan="2"><B>Add Media Scenario</B></td>
				</tr>
				<tr>
					<td>Description</td>
					<td><input type="text" name="newDescription" value="'.@$_POST['newDescription'].'"></td>
				</tr>
				<tr>
					<td>Entertain Area</td>
					<td><select name="newEntArea" onChange="document.mediaScenarios.action.value=\'form\';document.mediaScenarios.submit()">
						<option value="0">please select</option>';
			$resEntAreas->MoveFirst();
			while($rowEntAreas=$resEntAreas->FetchRow()){
				$out.='<option value="'.$rowEntAreas['PK_EntertainArea'].'" '.((@$_POST['newEntArea']==$rowEntAreas['PK_EntertainArea'])?'selected':'').'>'.$rowEntAreas['Description'].'</option>';
			}
			$out.='
					</select></td>
				</tr>';
			if((int)@$_POST['newEntArea']!=0){
				$queryDevices='
					SELECT DISTINCT PK_Device, Device.Description, Device.FK_DeviceTemplate
						FROM Device_EntertainArea
					INNER JOIN Device ON FK_Device=PK_Device
					INNER JOIN DeviceTemplate ON Device.FK_DeviceTemplate=PK_DeviceTemplate
					INNER JOIN DeviceTemplate_MediaType ON DeviceTemplate_MediaType.FK_DeviceTemplate=Device.FK_DeviceTemplate
					WHERE FK_EntertainArea=?';
				$out.='
					<tr>
						<td>Device</td>
						<td><select name="MSDevice" onChange="document.mediaScenarios.action.value=\'form\';document.mediaScenarios.submit()">
							<option value="0">Please select</option>';
				$resDevices=$dbADO->Execute($queryDevices,array($_POST['newEntArea']));
				while($rowDevices=$resDevices->FetchRow()){
					$out.='<option value="'.$rowDevices['PK_Device'].'-'.$rowDevices['FK_DeviceTemplate'].'" '.((@$_POST['MSDevice']==$rowDevices['PK_Device'].'-'.$rowDevices['FK_DeviceTemplate'])?'selected':'').'>'.$rowDevices['Description'].'</option>';
				}
				$out.='
						</select></td>
					</tr>';

				if(isset($_POST['MSDevice']) && $_POST['MSDevice']!='0'){
					$deviceArray=explode('-',$_POST['MSDevice']);
					$out.='
					<tr>
						<td>Type</td>
						<td><select name="newType" onChange="document.mediaScenarios.action.value=\'form\';document.mediaScenarios.submit()">
							<option value="0">Please select</option>';
					$getMediaTypes='
						SELECT * FROM MediaType 
							INNER JOIN DeviceTemplate_MediaType ON FK_MediaType=PK_MediaType
						WHERE FK_DeviceTemplate=? ORDER BY Description ASC';
					$resMediaTypes=$dbADO->Execute($getMediaTypes,$deviceArray[1]);
					while($rowMediaTypes=$resMediaTypes->FetchRow()){
						$out.='<option value="'.$rowMediaTypes['PK_DeviceTemplate_MediaType'].'" '.((@$_POST['newType']==$rowMediaTypes['PK_DeviceTemplate_MediaType'])?'selected':'').'>'.$rowMediaTypes['Description'].'</option>';
					}
					$out.='
						</select>
						</td>
					</tr>';
					
					if(isset($_POST['newType']) && $_POST['newType']!='0'){
						$queryRemotes='				
							SELECT 
								FK_DesignObj,DeviceTemplate_MediaType_DesignObj.Description,DesignObj.Description AS dobjDescription
							FROM DeviceTemplate_MediaType_DesignObj
								JOIN DesignObj ON FK_DesignObj=PK_DesignObj
							WHERE FK_DeviceTemplate_MediaType=?';
						$resRemotes=$dbADO->Execute($queryRemotes,$_POST['newType']);

						$out.='
						<tr>
							<td>Remote</td>
							<td><select name="newRemote" onChange="document.mediaScenarios.action.value=\'form\';document.mediaScenarios.submit()">
								<option value="0">Please select</option>
						';
						while($rowRemotes=$resRemotes->FetchRow()){
							$out.='<option value="'.$rowRemotes['FK_DesignObj'].'" '.(($rowRemotes['FK_DesignObj']==@$_POST['newRemote'])?'selected':'').'>'.(($rowRemotes['Description']!='')?$rowRemotes['Description']:$rowRemotes['dobjDescription']).'</option>';
						}
						$out.='</select>
						</td>
					</tr>';
					}
					if(isset($_POST['newRemote']) && $_POST['newRemote']!='0'){
						$out.='
						<tr>
							<td colspan="2" align="center"><input type="submit" name="add" value="add"></td>
						</tr>
						';
					}
				}
			}
		$out.='
			</table>
			</form>
			<script>
		 		var frmvalidator = new formValidator("mediaScenarios");
 				//frmvalidator.addValidation("newDescription","req","Please enter a description");			
			</script>
		';
	} else {
		//check if current user canModifyInstallation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$installationID,$dbADO);
	
		if ($canModifyInstallation) {
			if(isset($_POST['saveChanges'])){
				$displayedMediaScenariosArray=explode(',',$_POST['displayedMediaScenarios']);
				foreach($displayedMediaScenariosArray AS $value){
					$newMSDescription=$_POST['description_'.$value];
					$updateMediaScenario='UPDATE CommandGroup SET Description=? WHERE PK_CommandGroup=?';
					$dbADO->Execute($updateMediaScenario,array($newMSDescription,$value));
				}
			}else{
			
				if($action=='delete'){
					$delCG=$_POST['delCG'];
					deleteCommandGroup($delCG,$dbADO);
					
				}else{
					if(isset($_POST['add'])){
						$newDescription=cleanString($_POST['newDescription']);
						$deviceArray=explode('-',$_POST['MSDevice']);
						$newPKDevice=$deviceArray[0];
						$newFKDeviceTemplate=$deviceArray[1];
						$newRemote=$_POST['newRemote'];
						$newEntArea=$_POST['newEntArea'];
						$newType=$_POST['newType'];
		
						
						$insertMediaScenario='INSERT INTO CommandGroup (FK_Array, FK_Installation, Description,FK_DesignObj,FK_Template) VALUES (?,?,?,?,?)';
						$dbADO->Execute($insertMediaScenario,array($arrayID,$installationID,$newDescription,@$_POST['newRemote'],$templateWizard));
						$insertID=$dbADO->Insert_ID();
						
						$insertDeviceCommandGroup='INSERT INTO CommandGroup_EntertainArea (FK_EntertainArea, FK_CommandGroup,Sort) VALUES (?,?,?)';
						$dbADO->Execute($insertDeviceCommandGroup,array(@$_POST['newEntArea'],$insertID,$insertID));
						
						$queryMediaPlugin='SELECT * FROM Device WHERE FK_DeviceTemplate=? AND FK_Installation=?';
						$res=$dbADO->Execute($queryMediaPlugin,array($GLOBALS['rootMediaPlugin'],$installationID));
						if($res->RecordCount()!=0){
							$row=$res->FetchRow();
							$mediaPluginID=$row['PK_Device'];
							
							$queryInsertCommandGroup_Command = "INSERT INTO CommandGroup_Command (FK_CommandGroup,FK_Command,FK_Device) values(?,?,?)";			
							$insertRs = $dbADO->Execute($queryInsertCommandGroup_Command,array($insertID,$GLOBALS['MediaScenariosCommand'],$mediaPluginID));			
							$CG_C_insertID=$dbADO->Insert_ID();
							
							$insertCommandParam='INSERT INTO CommandGroup_Command_CommandParameter (FK_CommandGroup_Command,FK_CommandParameter,IK_CommandParameter) VALUES (?,?,?)';
			
							$dbADO->Execute($insertCommandParam,array($CG_C_insertID,$GLOBALS['commandParamPK_Device'],$newPKDevice));
							$dbADO->Execute($insertCommandParam,array($CG_C_insertID,$GLOBALS['commandParamPK_DesignObj'],$newRemote));
							$dbADO->Execute($insertCommandParam,array($CG_C_insertID,$GLOBALS['commandParamFilename'],''));
							$dbADO->Execute($insertCommandParam,array($CG_C_insertID,$GLOBALS['commandParamPK_MediaType'],$newType));
							$dbADO->Execute($insertCommandParam,array($CG_C_insertID,$GLOBALS['commandParamPK_DeviceTemplate'],$newFKDeviceTemplate));
							$dbADO->Execute($insertCommandParam,array($CG_C_insertID,$GLOBALS['commandParamPK_EntertainArea'],$newEntArea));
						}
					}
					
					if(isset($_POST['optionEntArea']) && $_POST['optionEntArea']!=''){
						$FK_EntertainArea=$_POST['optionEntArea'];
						$optionName=$_POST['optionName'];
						$optionValue=$_POST['actionType'];
						if($optionValue=='addOption'){					
							$getTemplateID='SELECT * FROM Template WHERE Description=?';
							$resTemplateID=$dbADO->Execute($getTemplateID,'Media Wiz - '.$optionName);
							$rowTemplateID=$resTemplateID->FetchRow();
							$FK_Template=$rowTemplateID['PK_Template'];
												
							$insertScenario='INSERT INTO CommandGroup (FK_Array, FK_Installation, Description,FK_Template) VALUES (?,?,?,?)';
							$dbADO->Execute($insertScenario,array($arrayID,$installationID,$optionName,$FK_Template));
							$insertID=$dbADO->Insert_ID();
							
							$insertDeviceCommandGroup='INSERT INTO CommandGroup_EntertainArea (FK_EntertainArea, FK_CommandGroup,Sort) VALUES (?,?,?)';
							$dbADO->Execute($insertDeviceCommandGroup,array($FK_EntertainArea,$insertID,$insertID));
							$CG_C_insertID=$dbADO->Insert_ID();
							
							$queryInsertCommandGroup_Command = "INSERT INTO CommandGroup_Command (FK_CommandGroup,FK_Command,FK_Device) values(?,?,?)";			
							$dbADO->Execute($queryInsertCommandGroup_Command,array($insertID,$GLOBALS['commandSetVar'],-300));			
							$dbADO->Execute($queryInsertCommandGroup_Command,array($insertID,$GLOBALS['commandGotoScreen'],-300));			
						}else{
							$selectOptions='
								SELECT PK_CommandGroup FROM CommandGroup
									INNER JOIN CommandGroup_EntertainArea on CommandGroup_EntertainArea.FK_CommandGroup=PK_CommandGroup
									INNER JOIN Template ON FK_Template=PK_Template
								WHERE Template.Description=? AND FK_EntertainArea=?';
							$resOptions=$dbADO->Execute($selectOptions,array('Media Wiz - '.$optionName,$FK_EntertainArea));
							$rowOption=$resOptions->FetchRow();
							deleteCommandGroup($rowOption['PK_CommandGroup'],$dbADO);
						}						
					}
				}
			}
		}
	 
		header("Location: index.php?section=mediaScenarios");
	}
	
	$output->setNavigationMenu(array("My Scenarios"=>'index.php?section=myScenarios',"Media Scenarios"=>'index.php?section=mediaScenarios'));	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();
}
?>