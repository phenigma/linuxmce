<?php
function mediaScenarios($output,$dbADO) {
	global $dbPlutoMainDatabase;	
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	/* @var $output Template */

//	$dbADO->debug=true;			
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	$arrayID=$GLOBALS['ArrayIDForMedia'];
	$templateWizard=$GLOBALS['MediaScenariosTemplate'];			// from table Template, PK_Template for Media Wizard
		
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

	$out='Each Media Scenario will be a button on the Orbiter.  To use Media from a Pluto Media Director, just check the box for the type of media you want.  To use Media from
	another source, like a VCR, DVD player, etc., use the "Add Media Scenario" section.';
	if($action=='form'){
		$devicesMediaType=getDevicesArrayFromCategory($GLOBALS['rootAVEquipment'],$dbADO);	// start with all A/V/ devices
		$out.=setLeftMenu($dbADO).'
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
				<input type="hidden" name="EntAreaDescription" value="'.@$_POST['newEntArea'].'">
				<input type="hidden" name="GoTo" value="">	
			<div align="center" class="confirm"><B>'.(isset($_GET['msg'])?strip_tags($_GET['msg'].'<br>'):'').'</B></div>
			<div align="center"><h2>Media scenarios</h2></div>
			<table>
				<tr>
					<td colspan="2"><B>Add Media Scenario</B><a name="addMS"></a></td>
				</tr>
				<tr>
					<td>Description</td>
					<td><input type="text" name="newDescription" value="'.@$_POST['newDescription'].'"></td>
				</tr>
				<tr>
					<td>Entertain Area</td>
					<td><select name="newEntArea" onChange="document.mediaScenarios.action.value=\'form\';document.mediaScenarios.GoTo.value=\'addMS\';document.mediaScenarios.submit()">
						<option value="0">please select</option>';
			
			while($rowEntAreas=$resEntAreas->FetchRow()){
				$out.='<option value="'.$rowEntAreas['PK_EntertainArea'].'" '.((@$_POST['newEntArea']==$rowEntAreas['PK_EntertainArea'])?'selected':'').'>'.$rowEntAreas['Description'].'</option>';
			}
			$out.='
					</select></td>
				</tr>';
			if($resEntAreas->RecordCount()==0){
				$out.='
				<tr>
					<td colspan="2" class="err">No entertain areas available. They will be created automatically when you next restart your Core, and you can add media scenarios then. If you want to manually configure your entertainment areas please do so on the <a href="index.php?section=rooms">Rooms</a> page. Refer to the help on that page for details.</td>
				</tr>';
			}
			
			if((int)@$_POST['newEntArea']!=0){
				$queryDevices='
					SELECT DISTINCT Device.*
						FROM Device_EntertainArea
					INNER JOIN Device ON FK_Device=PK_Device
					INNER JOIN DeviceTemplate ON Device.FK_DeviceTemplate=PK_DeviceTemplate
					INNER JOIN DeviceTemplate_MediaType ON DeviceTemplate_MediaType.FK_DeviceTemplate=Device.FK_DeviceTemplate
					WHERE FK_EntertainArea=?';
				$resDevices=$dbADO->Execute($queryDevices,array($_POST['newEntArea']));
				$deviceTemplatesArray=array();
				while($rowDevices=$resDevices->FetchRow()){
					if(!in_array($rowDevices['PK_Device'],$devicesMediaType)){
						$devicesMediaType[$rowDevices['PK_Device']]=$rowDevices['Description'];
						$deviceTemplatesArray[$rowDevices['PK_Device']]=$rowDevices['FK_DeviceTemplate'];
					}
				}
				$out.='
					<tr>
						<td>Device</td>
						<td><select name="MSDevice" onChange="document.mediaScenarios.action.value=\'form\';document.mediaScenarios.GoTo.value=\'addMS\';document.mediaScenarios.submit()" '.((substr($_POST['MSDevice'],-1)=='-')?'style="background:red;"':'').'>
							<option value="0" style="background:white;">Please select</option>';

				foreach ($devicesMediaType AS $deviceID=>$description){
					$out.='<option value="'.$deviceID.'-'.@$deviceTemplatesArray[$deviceID].'" '.((@$_POST['MSDevice']==$deviceID.'-'.@$deviceTemplatesArray[$deviceID])?'selected':'').' '.((!isset($deviceTemplatesArray[$deviceID]))?'style="background:red;"':'style="background:white;"').'>'.$description.'</option>';
				}
				$out.='
						</select>'.((count($devicesMediaType)!=count($deviceTemplatesArray))?' <font color=red>The devices highlighted doen\'t have specified a media type.</font>':'').'</td>
					</tr>';
			if($resDevices->RecordCount()==0){
				$out.='
				<tr>
					<td colspan="2" class="err">No media devices in selected entertain area. Please add media devices to this entertain area.</td>
				</tr>';
			}

				
				if(isset($_POST['MSDevice']) && $_POST['MSDevice']!='0' && substr($_POST['MSDevice'],-1)!='-'){
					$deviceArray=explode('-',$_POST['MSDevice']);
					$out.='
					<tr>
						<td>Type</td>
						<td><select name="newType" onChange="document.mediaScenarios.action.value=\'form\';document.mediaScenarios.GoTo.value=\'addMS\';document.mediaScenarios.submit()">
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
					if($resMediaTypes->RecordCount()==0){
						$out.='
							<tr>
								<td colspan="2" class="err">Selected device does not have any media type. Please select another device.</td>
							</tr>';
					}						
					
					if(isset($_POST['newType']) && $_POST['newType']!='0'){
						$remotesArray=array();
						$secondaryDescription=array();
						$queryRemotes='				
							SELECT 
							FK_DesignObj,DeviceTemplate_MediaType_DesignObj.Description,DesignObj.Description AS dobjDescription,FK_MediaType
							FROM DeviceTemplate_MediaType_DesignObj
							JOIN DesignObj ON FK_DesignObj=PK_DesignObj
							JOIN DeviceTemplate_MediaType ON FK_DeviceTemplate_MediaType=PK_DeviceTemplate_MediaType
							WHERE FK_DeviceTemplate_MediaType=?';
						$resRemotes=$dbADO->Execute($queryRemotes,(int)$_POST['newType']);
						while($rowRemotes=$resRemotes->FetchRow()){
							$remotesArray[$rowRemotes['FK_DesignObj']]=$rowRemotes['Description'];
							$secondaryDescription[$rowRemotes['FK_DesignObj']]=$rowRemotes['dobjDescription'];
							$selectedMediatype=$rowRemotes['FK_MediaType'];
						}
						
						if(isset($selectedMediatype)){
							$queryOtherRemotes='
								SELECT DesignObj.Description,PK_DesignObj
								FROM DesignObj
								INNER JOIN MediaType_DesignObj ON FK_DesignObj=PK_DesignObj
								WHERE FK_MediaType=?';
							$resOtherRemotes=$dbADO->Execute($queryOtherRemotes,$selectedMediatype);
							while($rowOtherRemotes=$resOtherRemotes->FetchRow()){
								$remotesArray[$rowOtherRemotes['PK_DesignObj']]=$rowOtherRemotes['Description'];
							}
						}						
						$out.='
						<tr>
							<td>Remote</td>
							<td><select name="newRemote" onChange="document.mediaScenarios.action.value=\'form\';document.mediaScenarios.GoTo.value=\'addMS\';document.mediaScenarios.submit()">
								<option value="0">Please select</option>
						';
						foreach ($remotesArray AS $key=>$value){
							$out.='<option value="'.$key.'" '.(($key==@$_POST['newRemote'])?'selected':'').'>'.(($value!='')?$value:$secondaryDescription[$key]).'</option>';
						}
						$out.='</select>
						</td>
					</tr>';
						if($resRemotes->RecordCount()==0){
							$out.='
							<tr>
								<td colspan="2" class="err">Selected type does not have a remote. Please select another type.</td>
							</tr>';
						}						
					}
					if(isset($_POST['newRemote']) && $_POST['newRemote']!='0'){
						$out.='
						<tr>
							<td colspan="2" align="center"><input type="submit" class="button" name="add" value="add"  ></td>
						</tr>
						';
					}
				}
			}
		$out.='
			</table>
		
			<table width="100%" border="0">';
		$resEntAreas->MoveFirst();
		while($rowEntAreas=$resEntAreas->FetchRow()){
			$checkBoxes='';
			foreach($GLOBALS['mediaOptionsArray'] AS $value){
				$selectOptions='
					SELECT PK_CommandGroup FROM CommandGroup
						INNER JOIN CommandGroup_EntertainArea on CommandGroup_EntertainArea.FK_CommandGroup=PK_CommandGroup
						INNER JOIN Template ON FK_Template=PK_Template
					WHERE Template.Description=? AND FK_EntertainArea=?';
				$resOptions=$dbADO->Execute($selectOptions,array('Media Wiz - '.$value,$rowEntAreas['PK_EntertainArea']));
				if($resOptions->RecordCount()>0){
					$rowCG=$resOptions->FetchRow();
					$optionLink='<a href="index.php?section=scenarioWizard&cgID='.$rowCG['PK_CommandGroup'].'&wizard=2&from=mediaScenarios">'.$value.'</a>';
				}else
					$optionLink=$value;
				$checkBoxes.='<input type="checkbox" name="checkbox" value="1" '.(($resOptions->RecordCount()>0)?'checked':'').' onClick="javascript:document.mediaScenarios.optionEntArea.value=\''.$rowEntAreas['PK_EntertainArea'].'\';document.mediaScenarios.actionType.value=\''.(($resOptions->RecordCount()>0)?'deleteOption':'addOption').'\';document.mediaScenarios.optionName.value=\''.$value.'\';document.mediaScenarios.EntAreaDescription.value=\''.$rowEntAreas['Description'].'\';document.mediaScenarios.submit()"> '.$optionLink.' ';
			}

			$out.='
			<tr bgcolor="#D1D9EA">
				<td colspan="7" align="left"><B>'.stripslashes($rowEntAreas['Description']).'</B></td>
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
				<td colspan="7" align="center"><input type="submit" class="button" name="saveChanges" value="Update descriptions"  ></td>
			</tr>
			';
		}
		$out.='
			<input type="hidden" name="displayedMediaScenarios" value="'.(join(',',$displayedMediaScenarios)).'">
				<tr>
					<td colspan="7">&nbsp;</td>
				</tr>
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
					setOrbitersNeedConfigure($installationID,$dbADO);
				}
			}else{
			
				if($action=='delete'){
					$delCG=$_POST['delCG'];
					deleteCommandGroup($delCG,$dbADO);
					setOrbitersNeedConfigure($installationID,$dbADO);
				}else{
					if(isset($_POST['add'])){
						$newDescription=cleanString($_POST['newDescription']);
						$deviceArray=explode('-',$_POST['MSDevice']);
						$newPKDevice=$deviceArray[0];
						$newFKDeviceTemplate=$deviceArray[1];
						$newRemote=$_POST['newRemote'];
						$newEntArea=$_POST['newEntArea'];
						$newType=$_POST['newType'];
						$EntAreaDescription=$_POST['EntAreaDescription'];
						
						$insertMediaScenario='INSERT INTO CommandGroup (FK_Array, FK_Installation, Description,FK_Template,Hint) VALUES (?,?,?,?,?)';
						$dbADO->Execute($insertMediaScenario,array($arrayID,$installationID,$newDescription,$templateWizard,$EntAreaDescription));
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
						setOrbitersNeedConfigure($installationID,$dbADO);
						$msg="New Media Scenario was added.";
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
							$EntAreaDescription=$_POST['EntAreaDescription'];
							
							
							// old code	who set entertain areas and scenarios
							/*
							$insertScenario='INSERT INTO CommandGroup (FK_Array, FK_Installation, Description,FK_Template,Hint) VALUES (?,?,?,?,?)';
							$dbADO->Execute($insertScenario,array($arrayID,$installationID,$optionName,$FK_Template,$EntAreaDescription));
							$insertID=$dbADO->Insert_ID();
							
							$insertDeviceCommandGroup='INSERT INTO CommandGroup_EntertainArea (FK_EntertainArea, FK_CommandGroup,Sort) VALUES (?,?,?)';
							$dbADO->Execute($insertDeviceCommandGroup,array($FK_EntertainArea,$insertID,$insertID));
							$CG_C_insertID=$dbADO->Insert_ID();
							$queryInsertCommandGroup_Command = "
								INSERT INTO CommandGroup_Command 
									(FK_CommandGroup,FK_Command,FK_Device) 
								VALUES(?,?,?)";								
							switch ($optionName){
								case 'TV':
									$queryMediaPlugin='SELECT * FROM Device WHERE FK_Installation=? AND FK_DeviceTemplate=?';
									$resMediaPlugin=$dbADO->Execute($queryMediaPlugin,array($installationID,$GLOBALS['rootMediaPlugin']));
									$rowMediaPlugin=$resMediaPlugin->FetchRow();
									$mediaPluginID=$rowMediaPlugin['PK_Device'];
									
									$dbADO->Execute($queryInsertCommandGroup_Command,array($insertID,$GLOBALS['commandMHPlayMedia'],$mediaPluginID));			
									$CG_C_insertID=$dbADO->Insert_ID();
									
									$insertCommandParam='INSERT INTO CommandGroup_Command_CommandParameter (FK_CommandGroup_Command,FK_CommandParameter,IK_CommandParameter) VALUES (?,?,?)';
									$dbADO->Execute($insertCommandParam,array($CG_C_insertID,$GLOBALS['commandParamPK_MediaType'],1));
								break;								
								case 'playlists':
									$dbADO->Execute($queryInsertCommandGroup_Command,array($insertID,$GLOBALS['commandGotoScreen'],$GLOBALS['localOrbiter']));	
									$CG_C_insertID=$dbADO->Insert_ID();
									$insertCommandParam='INSERT INTO CommandGroup_Command_CommandParameter (FK_CommandGroup_Command,FK_CommandParameter,IK_CommandParameter) VALUES (?,?,?)';
									$dbADO->Execute($insertCommandParam,array($CG_C_insertID,$GLOBALS['commandParamPK_DesignObj'],$GLOBALS['mnuMediaFileList2DesignObj']));
								break;
								case 'documents':
									$dbADO->Execute($queryInsertCommandGroup_Command,array($insertID,$GLOBALS['commandGotoScreen'],$GLOBALS['localOrbiter']));	
									$CG_C_insertID=$dbADO->Insert_ID();

									$insertCommandParam='INSERT INTO CommandGroup_Command_CommandParameter (FK_CommandGroup_Command,FK_CommandParameter,IK_CommandParameter) VALUES (?,?,?)';
									$dbADO->Execute($insertCommandParam,array($CG_C_insertID,$GLOBALS['commandParamID'],''));
									$dbADO->Execute($insertCommandParam,array($CG_C_insertID,$GLOBALS['commandParamPK_DesignObj'],$GLOBALS['mnuPicturesDocumentsList']));
									$dbADO->Execute($insertCommandParam,array($CG_C_insertID,$GLOBALS['commandParamDesignObjCurrentScreen'],''));
									$dbADO->Execute($insertCommandParam,array($CG_C_insertID,$GLOBALS['commandParamPK_Device'],''));
									$dbADO->Execute($insertCommandParam,array($CG_C_insertID,$GLOBALS['commandParamStoreVariables'],''));

									$dbADO->Execute($queryInsertCommandGroup_Command,array($insertID,$GLOBALS['commandSetVar'],$GLOBALS['localOrbiter']));	
									$CG_C_insertID=$dbADO->Insert_ID();
									$dbADO->Execute($insertCommandParam,array($CG_C_insertID,$GLOBALS['commandParameterVariableNumber'],2));
									$dbADO->Execute($insertCommandParam,array($CG_C_insertID,$GLOBALS['commandParameterValueToAsign'],$optionName));

								break;
								case 'pictures':
									$dbADO->Execute($queryInsertCommandGroup_Command,array($insertID,$GLOBALS['commandGotoScreen'],$GLOBALS['localOrbiter']));	
									$CG_C_insertID=$dbADO->Insert_ID();

									$insertCommandParam='INSERT INTO CommandGroup_Command_CommandParameter (FK_CommandGroup_Command,FK_CommandParameter,IK_CommandParameter) VALUES (?,?,?)';
									$dbADO->Execute($insertCommandParam,array($CG_C_insertID,$GLOBALS['commandParamID'],''));
									$dbADO->Execute($insertCommandParam,array($CG_C_insertID,$GLOBALS['commandParamPK_DesignObj'],$GLOBALS['mnuPicturesDocumentsList']));
									$dbADO->Execute($insertCommandParam,array($CG_C_insertID,$GLOBALS['commandParamDesignObjCurrentScreen'],''));
									$dbADO->Execute($insertCommandParam,array($CG_C_insertID,$GLOBALS['commandParamPK_Device'],''));
									$dbADO->Execute($insertCommandParam,array($CG_C_insertID,$GLOBALS['commandParamStoreVariables'],''));

									$dbADO->Execute($queryInsertCommandGroup_Command,array($insertID,$GLOBALS['commandSetVar'],$GLOBALS['localOrbiter']));	
									$CG_C_insertID=$dbADO->Insert_ID();
									$dbADO->Execute($insertCommandParam,array($CG_C_insertID,$GLOBALS['commandParameterVariableNumber'],2));
									$dbADO->Execute($insertCommandParam,array($CG_C_insertID,$GLOBALS['commandParameterValueToAsign'],$optionName));
									
									$dbADO->Execute($queryInsertCommandGroup_Command,array($insertID,$GLOBALS['commandSetVar'],$GLOBALS['localOrbiter']));	
									$CG_C_insertID=$dbADO->Insert_ID();
									$dbADO->Execute($insertCommandParam,array($CG_C_insertID,$GLOBALS['commandParameterVariableNumber'],14));
									$dbADO->Execute($insertCommandParam,array($CG_C_insertID,$GLOBALS['commandParameterValueToAsign'],7));
								break;								
								case 'Disc CD/DVD':
									$dbADO->Execute($queryInsertCommandGroup_Command,array($insertID,$GLOBALS['commandMHPlayMedia'],getMediaPluginID($installationID,$dbADO)));	
									$CG_C_insertID=$dbADO->Insert_ID();

									$dbADO->Execute('INSERT INTO CommandGroup_Command_CommandParameter (FK_CommandGroup_Command,FK_CommandParameter) SELECT '.$CG_C_insertID.',FK_CommandParameter FROM Command_CommandParameter WHERE FK_Command=?',$GLOBALS['commandMHPlayMedia']);

									$dbADO->Execute($queryInsertCommandGroup_Command,array($insertID,$GLOBALS['commandGotoScreen'],$GLOBALS['localOrbiter']));	
									$CG_C_insertID=$dbADO->Insert_ID();

									$insertCommandParam='INSERT INTO CommandGroup_Command_CommandParameter (FK_CommandGroup_Command,FK_CommandParameter,IK_CommandParameter) VALUES (?,?,?)';
									$dbADO->Execute($insertCommandParam,array($CG_C_insertID,$GLOBALS['commandParamID'],''));
									$dbADO->Execute($insertCommandParam,array($CG_C_insertID,$GLOBALS['commandParamPK_DesignObj'],$GLOBALS['mnuNoMediaInDrive']));
									$dbADO->Execute($insertCommandParam,array($CG_C_insertID,$GLOBALS['commandParamDesignObjCurrentScreen'],''));
									$dbADO->Execute($insertCommandParam,array($CG_C_insertID,$GLOBALS['commandParamPK_Device'],''));
									$dbADO->Execute($insertCommandParam,array($CG_C_insertID,$GLOBALS['commandParamStoreVariables'],''));

								break;								
								default:
									$dbADO->Execute($queryInsertCommandGroup_Command,array($insertID,$GLOBALS['commandSetVar'],$GLOBALS['localOrbiter']));			
									$CG_C_insertID=$dbADO->Insert_ID();
									
									$insertCommandParam='INSERT INTO CommandGroup_Command_CommandParameter (FK_CommandGroup_Command,FK_CommandParameter,IK_CommandParameter) VALUES (?,?,?)';
									$dbADO->Execute($insertCommandParam,array($CG_C_insertID,$GLOBALS['commandParameterVariableNumber'],2));
									$dbADO->Execute($insertCommandParam,array($CG_C_insertID,$GLOBALS['commandParameterValueToAsign'],$optionName));
									
									$dbADO->Execute($queryInsertCommandGroup_Command,array($insertID,$GLOBALS['commandGotoScreen'],$GLOBALS['localOrbiter']));	
									$CG_C_insertID=$dbADO->Insert_ID();
									
									$dbADO->Execute($insertCommandParam,array($CG_C_insertID,$GLOBALS['commandParamPK_DesignObj'],($optionName=='documents')?$GLOBALS['butViewDesignObj']:$GLOBALS['mnuMediaFileListDesignObj']));
									
									$dbADO->Execute($queryInsertCommandGroup_Command,array($insertID,$GLOBALS['commandSetVar'],$GLOBALS['localOrbiter']));			
									$CG_C_insertID=$dbADO->Insert_ID();
	
									switch ($optionName){
										case 'music':
											$parmValue=4;
										break;
										case 'movies':
											$parmValue=3;
										break;
										case 'videos':
											$parmValue=5;
										break;
										case 'pictures':
											$parmValue=7;
										break;
									}
									if(isset($parmValue)){
										$dbADO->Execute($insertCommandParam,array($CG_C_insertID,$GLOBALS['commandParameterVariableNumber'],14));
										$dbADO->Execute($insertCommandParam,array($CG_C_insertID,$GLOBALS['commandParameterValueToAsign'],$parmValue));
									}
								break;								
							}
							*/
							// Aaron program
							$command='/usr/pluto/bin/UpdateEntArea -h localhost -i '.$installationID.' -D '.$dbPlutoMainDatabase.' -e '.$FK_EntertainArea.' -t '.$FK_Template;
							exec($command);
								
							setOrbitersNeedConfigure($installationID,$dbADO);
							$msg="New Media Scenario was added.";
						}else{
							$selectOptions='
								SELECT PK_CommandGroup FROM CommandGroup
									INNER JOIN CommandGroup_EntertainArea on CommandGroup_EntertainArea.FK_CommandGroup=PK_CommandGroup
									INNER JOIN Template ON FK_Template=PK_Template
								WHERE Template.Description=? AND FK_EntertainArea=?';
							$resOptions=$dbADO->Execute($selectOptions,array('Media Wiz - '.$optionName,$FK_EntertainArea));
							$rowOption=$resOptions->FetchRow();
							deleteCommandGroup($rowOption['PK_CommandGroup'],$dbADO);
							setOrbitersNeedConfigure($installationID,$dbADO);
							$msg="Media Scenario was deleted.";
						}						
					}
				}
			}
		}
	 
		header("Location: index.php?section=mediaScenarios&msg=".@$msg);
	}
	if(@$_REQUEST['GoTo']=='addMS')
		$jumpTo=';self.location=\'#addMS\'';

	$output->setNavigationMenu(array("My Scenarios"=>'index.php?section=myScenarios',"Media Scenarios"=>'index.php?section=mediaScenarios'));	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Media Scenarios');			
	$output->output();
}
?>
