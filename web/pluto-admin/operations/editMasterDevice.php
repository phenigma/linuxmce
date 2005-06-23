<?
function editMasterDevice($output,$dbADO) {
	//$dbADO->debug=true;
$out='';
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	/* @var $output Template */
	

//get _POST	
	$deviceID = (int)$_REQUEST['model'];
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$lastAction = isset($_REQUEST['lastAction'])?cleanString($_REQUEST['lastAction']):'';
	$dtRelatedValues=array('1'=>'Sister device', '2'=>'Install on core', '3'=>'Plugin for the router');
	
//the form	
	if ($action=='form') {
		$queryGetMasterDeviceDetails = "
			SELECT DeviceTemplate.*,FK_DeviceTemplate
			FROM  DeviceTemplate 				
			LEFT JOIN DeviceTemplate_AV ON FK_DeviceTemplate=PK_DeviceTemplate
			WHERE PK_DeviceTemplate=?";
		$rs = $dbADO->Execute($queryGetMasterDeviceDetails,array($deviceID));	
		
		$row = $rs->FetchRow();
		$description = stripslashes($row['Description']);
		$deviceCategID = $row['FK_DeviceCategory'];
		$manufacturerID = $row['FK_Manufacturer'];
		$manufacturerURL = $row['ManufacturerURL'];
		$internalURLsufix = $row['InternalURLSuffix'];
		$implementsDCE = (int)$row['ImplementsDCE'];
		$commandLine = stripslashes($row['CommandLine']);
		$userID = $_SESSION['userID'];
		$quarantine = $row['FK_StabilityStatus'];
		$package = $row['FK_Package'];
		$isPlugIn = $row['IsPlugIn'];
		$isEmbedded = $row['IsEmbedded'];
		$inheritsMacFromPC = $row['InheritsMacFromPC'];
		$isIPBased=$row['IsIPBased'];
		$ConfigureScript=$row['ConfigureScript'];
		$comments=$row['Comments'];
		$isAVDevice = is_null($row['FK_DeviceTemplate'])?0:1;	
		$rs->Close();
		
		$out='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		</script>
		<div class="err">'.@$_GET['error'].'</div>
		<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
		<form action="index.php" method="post" name="editMasterDevice">
			<input type="hidden" name="section" value="editMasterDevice">
			<input type="hidden" name="action" value="modify">
			<input type="hidden" name="lastAction" value="">
			<input type="hidden" name="model" value="'.$deviceID.'">
			<input type="hidden" name="toDel" value="">
			<table>
				<tr>
					<td>Description:</td><td><input type="text" name="description" value="'.$description.'" size="40"> #'.$deviceID.'</td>
				</tr>
				<tr>
					<td>
						<input type="checkbox" name="ImplementsDCE" value="1" '.($implementsDCE==1?" checked ":"").'  onClick="javascript:this.form.submit();">Implements DCE
					</td>
					<td>
						Command line <input type="text" name="CommandLine" value="'.$commandLine.'">
					</td>
				</tr>
				<tr>
					<td>Category:</td><td>
						<select name="MasterDeviceCategory">
						<option value="0">-please select-</option>
		';
		$GLOBALS['categoriesArray']=array();
		$queryMasterDeviceCategories_parents = 'select PK_DeviceCategory,Description from DeviceCategory where FK_Devicecategory_Parent is null order by Description asc';
		$rs = $dbADO->_Execute($queryMasterDeviceCategories_parents);
							while ($row = $rs->FetchRow()) {
								$GLOBALS['categoriesArray'][$row['PK_DeviceCategory']]=$row['Description'];
								$out.='<option '.($row['PK_DeviceCategory']==$deviceCategID?' selected ': ' ').' value="'.$row['PK_DeviceCategory'].'">'.stripslashes($row['Description']).' #'.$row['PK_DeviceCategory'].'</option>';
								$out.=getDeviceCategoryChildsOptions($row['PK_DeviceCategory'],stripslashes($row['Description']),$deviceCategID,'',$dbADO);
							}
		$out.='			</select>
					</td>
				</tr>
			<tr>
				<td>Manufacturer:</td><td>
					<select name="Manufacturer">
						<option value="0">-please select-</option>
		';
				$manufacturersArray=array();
		$queryManufacturers = 'select Description, PK_Manufacturer from Manufacturer order by Description asc';
		$rs = $dbADO->_Execute($queryManufacturers);
							while ($row = $rs->FetchRow()) {								
								$manufacturersArray[$row['PK_Manufacturer']]=$row['Description'];
								$out.='<option '.($row['PK_Manufacturer']==$manufacturerID?' selected ': ' ').' value="'.$row['PK_Manufacturer'].'">'.stripslashes($row['Description']).'</option>';
							}
		$out.='		</select> <a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=addManufacturer&from=editMasterDevice\',\'width=300,height=200,toolbars=true\');">Create Manufacturer</a>
				</td>
			</tr>
			<tr>
				<td>Manufacturer URL:</td>
				<td><input type="text" name="manufacturerURL" value="'.$manufacturerURL.'"> Internal URL sufix: <input type="text" name="internalURLsufix" value="'.$internalURLsufix.'"></td>
			</tr>
			<tr>
				<td valign="top">Design Objects to use as remotes:</td>
				<td>
		';
		$selectObjects = 'select PK_DesignObj,Description from DesignObj inner join DeviceTemplate_DesignObj on FK_DesignObj = PK_DesignObj where FK_DeviceTemplate = ? order by Description asc';
		$rs = $dbADO->Execute($selectObjects,array($deviceID));
			while ($row = $rs->FetchRow()) {
				$out.=stripslashes($row['Description']) . "&nbsp; &nbsp; <a href=\"javascript:void(0);\" onClick=\"windowOpen('index.php?section=deleteObjectFromMasterDevice&from=editMasterDevice&deviceID=$deviceID&objID={$row['PK_DesignObj']}','status=0,resizable=1,width=300,height=200,toolbars=true');\">Delete</a> <br />";
			}
			if ($rs->RecordCount()===0) {
				$out.="No objects";
			}
		$out.="&nbsp; &nbsp; <br /><a href=\"javascript:void(0);\" onClick=\"windowOpen('index.php?section=addObjectToMasterDevice&from=editMasterDevice&deviceID={$deviceID}','status=0,resizable=1,width=300,height=200,toolbars=true');\">Add a new object to device</a>";
		
		$out.='		</td>
				</tr>
				<tr>
					<td valign="top">This device is controlled via:</td>
					<td>
					';
					$selectControlled = 'select MDL1.Description as d1,MDL1.PK_DeviceTemplate as pk1,
												MDL2.Description as d2,MDL2.PK_DeviceTemplate as pk2,
												DC.Description as d3,
												MDL_CV.RerouteMessagesToParent,MDL_CV.AutoCreateChildren,
												MDL_CV.PK_DeviceTemplate_DeviceTemplate_ControlledVia as pk
								FROM DeviceTemplate_DeviceTemplate_ControlledVia MDL_CV
								INNER JOIN  DeviceTemplate MDL1 on FK_DeviceTemplate  = MDL1.PK_DeviceTemplate 
								INNER JOIN DeviceTemplate MDL2 on FK_DeviceTemplate_ControlledVia  = MDL2.PK_DeviceTemplate
								LEFT JOIN DeviceCategory DC on MDL2.FK_DeviceCategory = DC.PK_DeviceCategory								
								WHERE MDL_CV.FK_DeviceTemplate = ? order by DC.Description asc';
					$rs = $dbADO->Execute($selectControlled,array($deviceID));
						while ($row = $rs->FetchRow()) {
							$out.='Device:'.stripslashes($row['d2']) .'&nbsp; Category: '. stripslashes($row['d3']) . "&nbsp; &nbsp; 
							<a href=\"javascript:void(0);\" onClick=\"windowOpen('index.php?section=editControlledViaToMasterDevice&from=editMasterDevice&deviceID=$deviceID&objID={$row['pk']}','status=0,resizable=1,width=800,height=600');\">Edit</a>
							&nbsp; &nbsp; <a href=\"javascript:void(0);\" onClick=\"windowOpen('index.php?section=deleteControlledViaFromMasterDevice&from=editMasterDevice&objID={$row['pk']}','status=0,resizable=1,width=10,height=10,toolbars=true');\">Delete</a>
							<br />";
						}
						if ($rs->RecordCount()===0) {
							$out.="No records";
						}
					$out.="&nbsp; &nbsp; <br /><a href=\"javascript:void(0);\" onClick=\"windowOpen('index.php?section=addControlledViaToMasterDevice&from=editMasterDevice&deviceID={$deviceID}','status=0,resizable=1,width=500,height=250,toolbars=true');\">Add a new controlled via device</a>";
					
					$out.='		
					</td>
				</tr>
				<tr>
					<td valign="top">This device is controlled via category:</td>
					<td>
					';
					$selectControlled = 'select MDL1.Description as d1,MDL1.PK_DeviceTemplate as pk1,												
												DC.Description as d3,
												MDL_CV.RerouteMessagesToParent,MDL_CV.AutoCreateChildren,
												MDL_CV.PK_DeviceTemplate_DeviceCategory_ControlledVia as pk
								FROM DeviceTemplate_DeviceCategory_ControlledVia MDL_CV
								INNER JOIN  DeviceTemplate MDL1 on FK_DeviceTemplate  = MDL1.PK_DeviceTemplate 								
								INNER JOIN DeviceCategory DC on MDL_CV.FK_DeviceCategory = DC.PK_DeviceCategory
								WHERE MDL_CV.FK_DeviceTemplate = ? order by DC.Description asc';
					$rs = $dbADO->Execute($selectControlled,array($deviceID));
						while ($row = $rs->FetchRow()) {
							$out.='Category: '. stripslashes($row['d3']) . "&nbsp; &nbsp; 
							<a href=\"javascript:void(0);\" onClick=\"windowOpen('index.php?section=editControlledViaCategoryToMasterDevice&from=editMasterDevice&deviceID=$deviceID&objID={$row['pk']}','status=0,resizable=1,width=800,height=500');\">Edit</a>
							&nbsp; &nbsp; <a href=\"javascript:void(0);\" onClick=\"windowOpen('index.php?section=deleteControlledViaCategoryFromMasterDevice&from=editMasterDevice&objID={$row['pk']}','status=0,resizable=1,width=700,height=800,toolbars=yes,scrollbars=1');\">Delete</a>
							<br />";
						}
						if ($rs->RecordCount()===0) {
							$out.="No records";
						}
					$out.="&nbsp; &nbsp; <br /><a href=\"javascript:void(0);\" onClick=\"windowOpen('index.php?section=addControlledViaCategoryToMasterDevice&from=editMasterDevice&deviceID={$deviceID}','status=0,resizable=1,width=800,height=600,toolbars=true');\">Add a new controlled via category device</a>";
					
					$out.='		
					</td>
				</tr>
				<tr>
					<td valign="top">Packages:</td><input type="hidden" name="packagesCtrl" value="0">
					<td><select name="package">
						<option value="0">-Please select-</option>
					';
				$querySelectPackages = "SELECT * FROM Package ORDER BY Description ASC";
				$rs = $dbADO->Execute($querySelectPackages);
				if($rs->RecordCount()==0)
					$out.='No records.<br>';
				while ($row = $rs->FetchRow()) {
					$out.='<option value="'.$row['PK_Package'].'" '.(($row['PK_Package']==$package)?'selected':'').'>'.$row['Description'].'</option>';
				}
				$rs->Close();
				$out.='</select>&nbsp;<input type="submit" class="button" name="submitX" value="Add">';
				if($package!=0)
					$out.="  <a href=\"javascript:void(0);\" onClick=\"windowOpen('index.php?section=addPackageToMasterDevice&from=editMasterDevice&deviceID={$deviceID}&PK_Package={$package}','status=0,resizable=1,width=700,height=700,toolbars=true,scrollbars=1,fullscreen=yes');\">Edit package</a>";
				$out.="&nbsp; <a href=\"javascript:void(0);\" onClick=\"windowOpen('index.php?section=addPackageToMasterDevice&from=editMasterDevice&deviceID={$deviceID}','status=0,resizable=1,width=700,height=850,toolbars=true,scrollbars=1');\">Create new package</a>";
				$out.='	
					</td>
				</tr>
				<tr>
					<td valign="top"><a name="isAVDevice_link"></a>Audio/Video Device</td>
					<td>
						<input type="hidden" value="'.$isAVDevice.'" name="old_isAVDevice"><input type="checkbox" name="isAVDevice" '.($isAVDevice==1?" checked='checked' ":"").' value="1" onClick="javascript:this.form.submit();"><input type="button" class="button" name="isAV"  '.($isAVDevice!=1?'value="Is NOT Audio/Video" disabled="disabled" ':" value=\"Edit Audio/Video Properties\" onClick=\"windowOpen('index.php?section=editAVDevice&from=editMasterDevice&deviceID={$deviceID}','status=0,resizable=1,width=800,height=600,toolbars=true,scrollbars=1,resizable=1');\"").'>
					</td>
				</tr>
				<tr>
					<td valign="top">Is PlugIn</td>
					<td><input type="checkbox" name="isPlugIn" value="1" '.(($isPlugIn==1)?'checked':'').' onClick=\'javascript:this.form.submit();\'>
					<input type="hidden" name="oldIsPlugIn" value="'.(($isPlugIn==1)?'1':'0').'"></td>
				</tr>					
				<tr>
					<td valign="top">Is Embedded</td>
					<td><input type="checkbox" name="isEmbedded" value="1" '.(($isEmbedded==1)?'checked':'').' onClick=\'javascript:this.form.submit();\'>
					<input type="hidden" name="oldIsEmbedded" value="'.(($isEmbedded==1)?'1':'0').'"></td>
				</tr>					
				<tr>
					<td valign="top">Inherits MAC From PC</td>
					<td><input type="checkbox" name="inheritsMacFromPC" value="1" '.(($inheritsMacFromPC==1)?'checked':'').' onClick=\'javascript:this.form.submit();\'>
					<input type="hidden" name="oldInheritsMacFromPC" value="'.(($inheritsMacFromPC==1)?'1':'0').'"></td>
				</tr>					
				<tr>
					<td valign="top">Is IP Based</td>
					<td><input type="checkbox" name="isIPBased" value="1" '.(($isIPBased==1)?'checked':'').' onClick=\'javascript:this.form.submit();\'>
					<input type="hidden" name="oldIsIPBased" value="'.(($isIPBased==1)?'1':'0').'"></td>
				</tr>	
				<tr>
					<td valign="top">Configuration script</td>
					<td><input type="text" name="ConfigureScript" value="'.$ConfigureScript.'">
				</tr>	
				
				<tr>
					<td valign="top">Comments</td>
					<td><textarea name="comments" rows="2" style="width:500;">'.$comments.'</textarea></td>
				</tr>	
				<tr>
					<td valign="top">Contact</td>
					<td>';
					$selectUserDetails = "select * from Users where PK_Users = ?";
					$res = $dbADO->Execute($selectUserDetails,array($userID));
					if ($res) {
						$row=$res->FetchRow();
						$fName = stripslashes($row['FirstName']);
						$lName = stripslashes($row['LastName']);
						$mail = stripslashes(@$row['ForwardEmail']);
						$mail=str_replace(array(".","@"),array(" dot ", " at "),$mail); 
					}
					$out.='
					<a href="#">'.$fName.' '.$lName.' '.$mail.'</a>
					</td>
				</tr>
		
				
					
				<tr>
					<td valign="top" colspan="2"><a name="deviceData_link"></a>				
						<fieldset>
		            		<legend>Device Data</legend>
								<table>';
					
				$deviceData="SELECT DeviceTemplate_DeviceData.*,
							DeviceData.Description as DD_desc,PK_DeviceData,
							ParameterType.Description as PT_Desc FROM 
							DeviceTemplate_DeviceData 
								INNER JOIN DeviceData on FK_DeviceData = PK_DeviceData
								INNER JOIN ParameterType on FK_ParameterType = PK_ParameterType
							WHERE (FK_DeviceTemplate='$deviceID')
							";
				
				$resDeviceData = $dbADO->_Execute($deviceData);
				$usedParams = array();
				$usedParams[] = 0;
				$deviceDataFormValidation='';
					if ($resDeviceData) {
						if ($resDeviceData->RecordCount()>0) {
							$out.='<tr>									
										<td colspan="2">
											<table border="1">
												<tr><td>Current Data</td><td>Comments</td><td>Default Value</td><td>Required</td><td>Allowed to modify</td><td>User Master Device List Defaults</td><td>Set by device</td><td>Operation</td></tr>
											';
						}
						while ($row=$resDeviceData->FetchRow()) {
							$out.="
												<tr ".((trim($row['Description'])=='')?(' bgColor = "lightgreen"'):('')).">
													<td>#{$row['FK_DeviceData']} {$row['DD_desc']}({$row['PT_Desc']}) 
													<a href=\"javascript:void(0);\" onClick=\"windowOpen('index.php?section=editDeviceData&from=editMasterDevice&deviceID=$deviceID&deviceDataID={$row['PK_DeviceData']}','status=0,resizable=1,width=500,height=250,toolbars=true');\">Edit</a></td>
													<td><input type='text' value='{$row['Description']}' name='Data_Description_{$row['FK_DeviceData']}'></td>
													<td><textarea name='Data_DefaultValue_{$row['FK_DeviceData']}' rows='1'>{$row['IK_DeviceData']}</textarea></td>
													<td><input type='checkbox' value='1' name='Data_Required_{$row['FK_DeviceData']}' ".($row['Required']==1?' checked="checked" ':'')."></td>
													<td><input type='checkbox' value='1' name='Data_AllowedToModify_{$row['FK_DeviceData']}' ".($row['AllowedToModify']==1?' checked="checked" ':'')."></td>
													<td><input type='checkbox' value='1' name='Data_UseDeviceTemplateDefault_{$row['FK_DeviceData']}' ".($row['UseDeviceTemplateDefault']==1?' checked="checked" ':'')."></td>
													<td><input type='checkbox' value='1' name='Data_SetByDevice_{$row['FK_DeviceData']}' ".($row['SetByDevice']==1?' checked="checked" ':'')."></td>
													<td><a href=\"javascript:void(0);\" onClick=\"windowOpen('index.php?section=deleteParameterFromMasterDevice&from=editMasterDevice&deviceData={$row['FK_DeviceData']}&deviceID={$deviceID}','status=1,resizable=1,width=500,height=250,toolbars=true');\">Delete</a></td>
												</tr>							
							";	
							$usedParams[]=$row['FK_DeviceData'];
							$deviceDataFormValidation.= '
								frmvalidator.addValidation("Data_Description_'.$row['FK_DeviceData'].'","req","Please enter a description!");
								//frmvalidator.addValidation("Data_DefaultValue_'.$row['FK_DeviceData'].'","req","Please enter a default value!");
							';
						}
						if ($resDeviceData->RecordCount()>0) {
							$out.='	
											 <input type=\'hidden\' name="usedData" value="'.(join(",",$usedParams)).'">
											</table>
										</td>
								   </tr>	
							';
						} else {
							$out.='<tr><td colspan="2">No data for this device!</td></tr>';
						}
						
					}
					
				$resDeviceData->close();
				
				$remainingData='<option value="0">-please select-</option>';
				$querySelectRemainingParameterForDevice = "
					select  * from DeviceData
					where PK_DeviceData NOT IN (".join(",",$usedParams).")
					order by Description asc";
				$rs = $dbADO->_Execute($querySelectRemainingParameterForDevice);
				while ($row = $rs->FetchRow()) {
						$remainingData.='<option value="'.$row['PK_DeviceData'].'">'.$row['Description'].'</option>';
				}
				$rs->Close();
				
				$out.='<tr>
					
				</tr>
				
				<tr>
					<td colspan="2"><a name="deviceData_link"></a>Add a new parameter: <select name="newDeviceData">'.$remainingData.'</select>&nbsp;&nbsp;
					<input type="submit" class="button" name="submitX" value="Add" '.(@(int)$_SESSION['userID']!=$userID?' mdisabled="mdisabled" ':'').'>
				<br /> If the parameter you wish to add is not in the list, <a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=addParameter&from=editMasterDevice&deviceID='.$deviceID.'\',\'status=0,resizable=1,width=500,height=250,toolbars=true\');">click here to create a new parameter</a>
					</td>
				</tr>
				</table>
					</fieldset>
				</td>
				</tr>	
			
				<tr>
					<td valign="top" colspan="2"><hr /></td>
				</tr>
				<tr>
					<td valign="top" colspan="2"><a name="commands_link"></a>				
						<fieldset>
		            		<legend>Commands</legend>
								<table>				
				';
				
				$querySelCheckedCommandGroups = 'select FK_DeviceCommandGroup from DeviceTemplate_DeviceCommandGroup where  FK_DeviceTemplate = ?';
				$resSelCheckedCommandGroups = $dbADO->Execute($querySelCheckedCommandGroups,$deviceID);
				$selCheckedCommandsGroups = array();
				if ($resSelCheckedCommandGroups) {
					while ($rowSelCheckedCommandGroups = $resSelCheckedCommandGroups->FetchRow()) {
						$selCheckedCommandsGroups[]=$rowSelCheckedCommandGroups['FK_DeviceCommandGroup'];
					}
				}
				
				if (!is_array($selCheckedCommandsGroups)) {
					$selCheckedCommandsGroups=array();
					$selCheckedCommandsGroups[]=0;
				}
				if (count($selCheckedCommandsGroups)==0) {
					$selCheckedCommandsGroups[]=0;
				}
				
				$query = "select  ".$dbADO->IfNull('FK_DeviceCategory_Parent','0')." as a from DeviceCategory 				
							WHERE 
								PK_DeviceCategory = ?
				";
				$deviceParent=0;
				$res = $dbADO->Execute($query,array($deviceCategID));
				if ($res) {
					while ($row = $res->FetchRow()) {
						$deviceParent=$row['a'];												
					}
				}
				$deviceParent=(int)$deviceParent;
				$deviceCG = array();
				$query = "select DeviceCommandGroup.* FROM
							 DeviceCommandGroup
								INNER JOIN DeviceCategory on FK_DeviceCategory = PK_DeviceCategory
							WHERE 
								FK_DeviceCategory in ($deviceParent,$deviceCategID) or
								PK_DeviceCommandGroup in (".join(",",$selCheckedCommandsGroups).")
							ORDER BY DeviceCommandGroup.Description Asc
				";
				$resCommands = $dbADO->_Execute($query);
				
				$commandsDisplayed = array();
				if ($resCommands) {
					while ($row = $resCommands->FetchRow()) {
						$out.='<tr><td><input '.(in_array($row['PK_DeviceCommandGroup'],$selCheckedCommandsGroups)?" checked='checked' ": '').' type="checkbox" name="DeviceCommandGroup_'.$row['PK_DeviceCommandGroup'].'" value="1">'.stripslashes($row['Description']).'</td><td> <a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=editCommandGroupFromMasterDevice&from=editMasterDevice&deviceID='.$deviceID.'&commandGroupID='.$row['PK_DeviceCommandGroup'].'\',\'width=800,height=600,toolbars=true,resizable=1,scrollbars=yes\');">Edit Commands</a></td></tr>';
						$commandsDisplayed[]=$row['PK_DeviceCommandGroup'];
					}					
				}
				if (count($commandsDisplayed)==0) {
					$commandsDisplayed[]=0;
				}
				$out.='
						<input type="hidden" name="DeviceCommandGroupDisplayed" value="'.(join(",",$commandsDisplayed)).'">
						
						<tr><td><a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=addCommandGroup&from=editMasterDevice\',\'width=550,height=300,toolbars=true,scrollbars=1,resizable=1\');">Create Command Group</a>&nbsp;&nbsp;&nbsp;&nbsp;
						Manually add command group 
						<select name="addNewCommandGroupToMasterDevice">
							';
						$query = "select PK_DeviceCommandGroup, Description FROM DeviceCommandGroup where PK_DeviceCommandGroup NOT IN (".join(",",$commandsDisplayed).") Order By Description ASC";
						$res = $dbADO->Execute($query);
						$out.= '<option value="0">-please select-</option>';
						if ($res) {
							while ($row=$res->FetchRow()) {
								$out.='<option value="'.$row['PK_DeviceCommandGroup'].'">'.$row['Description'].'</option>';	
							}							
						}
						$res->Close();
				$out.='	</select> <input type="submit" class="button" name="submitX" value="Add" '.(@(int)$_SESSION['userID']!=$userID?' mdisabled="mdisabled" ':'').'></td></tr>
					</table>
					</fieldset>
				</td>
				</tr>

				<tr>
					<td valign="top" colspan="2"><hr /></td>
				</tr>
				<tr>
					<td valign="top" colspan="2"><a name="infrared_link"></a>				
						<fieldset>
		            		<legend>Infrared Groups</legend>
								<table>				
				';

				$queryIGNonDelete='
					SELECT PK_InfraredGroup,FK_DeviceCategory,InfraredGroup.Description 
					FROM InfraredGroup 
					WHERE FK_Manufacturer=? AND FK_DeviceCategory=?
					ORDER BY Description ASC';
				$resIGNonDelete=$dbADO->Execute($queryIGNonDelete,array($manufacturerID,$deviceCategID));
				
				$queryCheckedIG = "SELECT InfraredGroup.* FROM InfraredGroup
								INNER JOIN DeviceTemplate_InfraredGroup on FK_InfraredGroup = PK_InfraredGroup
							WHERE 
								FK_DeviceTemplate='$deviceID'
								ORDER BY InfraredGroup.Description Asc
				";
				$resCheckedIG = $dbADO->_Execute($queryCheckedIG);
				
				$infraredGroupsDisplayed = array();
				if ($resCheckedIG) {
					$checkedIGArray=array();
					while ($row = $resCheckedIG->FetchRow()) {
						$checkedIGArray[]=$row['PK_InfraredGroup'];
					}
					
					$categIGArray=array();
					while ($rowCategIG = $resIGNonDelete->FetchRow()) {
						$categIGArray[]=$rowCategIG['PK_InfraredGroup'];
						$out.='<tr>
								<td>
									<input type="checkbox" name="InfraredGroup_'.$rowCategIG['PK_InfraredGroup'].'" value="1" '.((in_array($rowCategIG['PK_InfraredGroup'],$checkedIGArray))?'checked':'').' onClick="document.editMasterDevice.submit();">'.stripslashes($rowCategIG['Description']).' #'.$rowCategIG['PK_InfraredGroup'].' (by category and manufacturer)
								</td>
								<td>
									<a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=irCodes&from=editMasterDevice&dtID='.$deviceID.'&infraredGroupID='.$rowCategIG['PK_InfraredGroup'].'\',\'width=800,height=600,toolbars=true,resizable=1,scrollbars=yes\');">Edit IR/GSD Commands</a>								
								</td>
							  </tr>';
						$infraredGroupsDisplayed[]=$rowCategIG['PK_InfraredGroup'];
					}					
					
					
					$resCheckedIG->MoveFirst();
					while ($row = $resCheckedIG->FetchRow()) {
						if(!in_array($row['PK_InfraredGroup'],$categIGArray)){
							$out.='<tr>
									<td>
										<input type="checkbox" name="InfraredGroup_'.$row['PK_InfraredGroup'].'" value="1" checked onClick="document.editMasterDevice.submit();">'.stripslashes($row['Description']).' #'.$row['PK_InfraredGroup'].'
									</td>
									<td>
										<a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=editInfraredGroupFromMasterDevice&from=editMasterDevice&deviceID='.$deviceID.'&infraredGroupID='.$row['PK_InfraredGroup'].'\',\'width=800,height=600,toolbars=true,resizable=1,scrollbars=yes\');">Edit IR/GSD Commands</a>								
									</td>
								  </tr>';
							$infraredGroupsDisplayed[]=$row['PK_InfraredGroup'];
						}
					}					
				}

				$out.='
						<input type="hidden" name="InfraredGroupDisplayed" value="'.(join(",",$infraredGroupsDisplayed)).'">					
						<tr>
							<td>'.(isset($_GET['irerror'])?'<b>'.$_GET['irerror'].'</b><br>':'').'
								<a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=addInfraredGroup&from=editMasterDevice\',\'width=550,height=300,toolbars=true\');">Create Infrared Group</a>&nbsp;&nbsp;&nbsp;&nbsp;
								Manually add infrared group 
								<input type="text" name="addNewInfraredGroupToMasterDevice" value=""> <input type="submit" class="button" name="submitX" value="Add" '.(@(int)$_SESSION['userID']!=$userID?' mdisabled="mdisabled" ':'').'>
							</td>
						</tr>
					</table>
					</fieldset>
				</td>
				</tr>';
				
				
		
		$out.='		
				<tr>
					<td valign="top" colspan="2"><hr /></td>
				</tr>
				<tr>
						<td valign="top" colspan="2"><a name="eventsList_link"></a>
				
				<fieldset>
            		<legend>Events</legend>
						<table>
					';
					$queryEventsSelected = 'SELECT EL.Description as EL_Desc,MDL_EL.Description as MDL_EL_Desc,PK_Event
						FROM DeviceTemplate_Event MDL_EL
							INNER JOIN DeviceTemplate MDL on FK_DeviceTemplate = PK_DeviceTemplate
							INNER JOIN Event EL on PK_Event = MDL_EL.FK_Event 						
							WHERE 
							FK_DeviceTemplate = ?
						Order By EL_Desc asc
					';
					//INNER JOIN Event_EventParameter EL_EP  on EL_EP.FK_Event = EL.PK_Event
					//INNER JOIN EventParameter EP  on EP.PK_EventParameter = EL_EP.FK_EventParameter
					//INNER JOIN ParameterType PT  on PT.PK_ParameterType = EP.FK_ParameterType					
					
					$resEventsSelected = $dbADO->Execute($queryEventsSelected,array($deviceID));
					$eventsSelectedArray=array();
					$eventsSelectedArray[]=0;
					if ($resEventsSelected) {
						while ($rowEventsSelected = $resEventsSelected->FetchRow()) {
							$out.="<tr><td>".$rowEventsSelected['EL_Desc'].'
							<a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=editEvent&from=editMasterDevice&EventID='.$rowEventsSelected['PK_Event'].'&deviceID='.$deviceID.'\',\'status=0,resizable=1,width=500,height=250,toolbars=true,scrollbars=1\');">Edit</a>
							<br />#'.$rowEventsSelected['PK_Event'].
							"</td><td><textarea cols='40' rows='5' name='EventDesc_{$rowEventsSelected['PK_Event']}'>".$rowEventsSelected['MDL_EL_Desc']."</textarea></td><td>".'
							 
							&nbsp; &nbsp; <a href="javascript:void(0);" onClick="if (confirm(\'Ar you sure you want to delete this event?\')) windowOpen(\'index.php?section=deleteEventFromMasterDevice&from=editMasterDevice&Event='.$rowEventsSelected['PK_Event'].'&deviceID='.$deviceID.'\',\'status=0,resizable=1,width=500,height=250,toolbars=true\');">Delete</a></td></tr>							
							';
							$eventsSelectedArray[]=$rowEventsSelected['PK_Event'];
						}
					}
					$resEventsSelected->Close();
					
					$queryEventsList = 'SELECT Description,PK_Event
							FROM Event
							WHERE PK_Event NOT IN ('.join(",",$eventsSelectedArray).')
							ORDER BY Description ASC
					';
					
					$resEventsList = $dbADO->_Execute($queryEventsList);
					if ($resEventsList) {					
						if ($resEventsList->RecordCount()>0) {
							$eventsTxt = '<option value=\'0\'>-please select-</option>';
						}else{
							$eventsTxt = '<option value="0">No Events Lists available</option>';
						}
						while ($rowEventsList = $resEventsList->FetchRow()) {
							$eventsTxt.= '<option value="'.$rowEventsList['PK_Event'].'">'.$rowEventsList['Description'].'</option>';
						}
					}
					$out.='
							
							<tr>
									<td>Add new event: <select name="newEventToMasterDevice">'.$eventsTxt.'</select></td><td><input type="submit" class="button" name="submitX" value="Add" '.(@(int)$_SESSION['userID']!=$userID?' mdisabled="mdisabled" ':'').'>
										<input type="hidden" name="eventsListDisplayed" value="'.join(",",$eventsSelectedArray).'">
									</td>
							</tr>
							<tr>
								<td colspan="2">
									<a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=createEvent&from=editMasterDevice&deviceID='.$deviceID.'\',\'status=1,resizable=1,width=500,height=250,toolbars=true\');">Create an Event</a>
								</td>
							</tr>
						</table>
					</fieldset>
					</td>
				</tr>
				<tr>
						<td valign="top" colspan="2"><a name="plugAndPlay"></a>
				
				<fieldset>
            		<legend>Plug & Play</legend>
						<table>';
					$dhcpArray=array();
					$resDHCP=$dbADO->Execute('SELECT * FROM DHCPDevice WHERE FK_DeviceTemplate=?',$deviceID);
					$out.='
							<tr>
								<td colspan="3">'.(($resDHCP->RecordCount()==0)?'No records.':'Range of MAC').'</td>
							</tr>
						';
					while($rowDHCP=$resDHCP->FetchRow()){
						$dhcpArray[]=$rowDHCP['PK_DHCPDevice'];
						$out.='
							<tr>
								<td>From: <input type="text" name="mac_from_'.$rowDHCP['PK_DHCPDevice'].'" value="'.$rowDHCP['Mac_Range_Low'].'"></td>
								<td>To: <input type="text" name="mac_to_'.$rowDHCP['PK_DHCPDevice'].'" value="'.$rowDHCP['Mac_Range_High'].'"></td>
								<td>Manufacturer: '.pulldownFromArray($manufacturersArray,'manufacturerPnp_'.$rowDHCP['PK_DHCPDevice'],$rowDHCP['FK_Manufacturer']).'</td>
							</tr>
							<tr>
								<td align="right">Category: </td>
								<td colspan="2">'.pulldownFromArray($GLOBALS['categoriesArray'],'categoryPnp_'.$rowDHCP['PK_DHCPDevice'],$rowDHCP['FK_DeviceCategory']).'</td>
							</tr>
							<tr>
								<td align="right">Comment: </td>
								<td colspan="2"><textarea name="commentPnp_'.$rowDHCP['PK_DHCPDevice'].'" rows="2" cols="50">'.$rowDHCP['Description'].'</textarea> <input type="button" class="button" name="editDHCP" value="Edit" onClick="windowOpen(\'index.php?section=editDHCP&dhcpID='.$rowDHCP['PK_DHCPDevice'].'\',\'width=500,height=400,toolbars=true,resizable=1\');"> <input type="button" class="button" name="delDHCP" value="Delete" onClick="document.editMasterDevice.toDel.value=\''.$rowDHCP['PK_DHCPDevice'].'\';document.editMasterDevice.submit();"></td>
							</tr>';
					}
					$out.='
							<tr>
								<td colspan="3"><hr></td>
							</tr>
							<tr bgcolor="#EEEEEE">
								<td>From: <input type="text" name="mac_from" value=""></td>
								<td>To: <input type="text" name="mac_to" value=""></td>
								<td>Manufacturer: '.pulldownFromArray($manufacturersArray,'manufacturerPnp','').'</td>
							</tr>
							<tr bgcolor="#EEEEEE">
								<td align="right">Category: </td>
								<td colspan="2">'.pulldownFromArray($GLOBALS['categoriesArray'],'categoryPnp','').'</td>
							</tr>
							<tr bgcolor="#EEEEEE">
								<td align="right">Comment: </td>
								<td colspan="2"><textarea name="commentPnp" rows="2" cols="50"></textarea> <input type="submit" class="button" name="addDHCP" value="Add"></td>
							</tr>
							<input type="hidden" name="dhcpArray" value="'.((join(',',$dhcpArray))).'">	
						</table>
					</fieldset>
					</td>
				</tr>';
			
			$resRel=$dbADO->Execute('
				SELECT DeviceTemplate_DeviceTemplate_Related.*, D1.Description
				FROM DeviceTemplate_DeviceTemplate_Related 
				INNER JOIN DeviceTemplate D1 ON FK_DeviceTemplate_Related=PK_DeviceTemplate
				WHERE FK_DeviceTemplate=?',$deviceID);				
			$dtRelatedRows='';
			while($rowRel=$resRel->FetchRow()){
				$dtRelatedRows.='
					<tr>
						<td>&nbsp;</td>
						<td bgcolor="#EEEEEE">'.$rowRel['Description'].'</td>
						<td bgcolor="#EEEEEE">'.$dtRelatedValues[$rowRel['Value']].'</td>
						<td bgcolor="#EEEEEE"><a href="javascript:if(confirm(\'Are you sure you want to delete this relation?\')){document.editMasterDevice.delRelated.value='.$rowRel['FK_DeviceTemplate_Related'].';document.editMasterDevice.submit();}">Delete</a></td>
					</tr>';
			}
			$out.='		
				<tr>
						<td valign="top" colspan="2"><a name="dtRelated"></a>
				
				<fieldset>
            		<legend>Device Template Related</legend>
						<input type="hidden" name="delRelated" value="">
						<span class="err">'.@$_REQUEST['errRelated'].'</span>
						<table>'.$dtRelatedRows.'
							<tr>
								<td>Add device template related</td>
								<td><input type="text" name="addRelated" value=""></td>
								<td>'.pulldownFromArray($dtRelatedValues,'valueRelated','','','key','').'</td>
								<td><input type="submit" class="button" name="add" value="Add"></td>
							</tr>
						</table>
					</fieldset>
					</td>
				</tr>';
					
			$queryDT_DO='
				SELECT DesignObj.Description, PK_DesignObj 
				FROM DeviceTemplate_DesignObj 
				INNER JOIN DesignObj ON FK_DesignObj=PK_DesignObj
				WHERE FK_DeviceTemplate=? ORDER BY Description ASC';
			$resDT_DO=$dbADO->Execute($queryDT_DO,$deviceID);
			$linksArray=array();
			while($rowDT_DO=$resDT_DO->FetchRow()){
				$linksArray[]='<a href="index.php?section=editMasterDevice&model='.$deviceID.'&action=removescreen&doID='.$rowDT_DO['PK_DesignObj'].'" title="Click to remove screen">'.$rowDT_DO['Description'].'</a>';
			}
			$out.='		
				<tr>
					<td valign="top" colspan="2">This device requries the following screens: '.join(', ',$linksArray).'</td>
				</tr>
				<tr>
					<td valign="top" colspan="2"><input type="text" name="newScreen" value=""> <input type="submit" class="button" name="addScreen" value="Add screen"></td>
				</tr>
				<tr>
					<td valign="top" colspan="2"><hr /></td>
				</tr>
					';
					
				$querySelectCustomPages = '
					SELECT PageSetup.* 
						FROM DeviceTemplate_PageSetup 
							INNER JOIN DeviceTemplate ON PK_DeviceTemplate = FK_DeviceTemplate
							INNER JOIN PageSetup ON PK_PageSetup = FK_PageSetup
						WHERE FK_DeviceTemplate = ?
					';
				$resSelectCustomPages = $dbADO->Execute($querySelectCustomPages,array($deviceID));
				
				if ($resSelectCustomPages->RecordCount()) {
					$out.='<tr>
						<td valign="top" colspan="2">
				
				<fieldset>
            		<legend>Config Pages</legend>
						<table>
					';
					
					while ($rowCustomPage = $resSelectCustomPages->FetchRow()) {
						$URL = str_replace("::ID::",$deviceID,$rowCustomPage['pageURL']);
						$out.='
						<tr>
							<td>
								<a href="javascript:void(0);" onClick="windowOpen(\''.$URL.'\',\'status=1,resizable=1,width=800,height=600,toolbars=true\');">'.$rowCustomPage['Description'].'</a>
							</td>
						</tr>';
					}
					$out.='
						</table>
					</fieldset>
					</td>
				</tr>';
					
				}
				
				$out.='
				<tr>
					<td valign="top" colspan="2"><input type="submit" class="button" name="submitX" value="Save" '.(@(int)$_SESSION['userID']!=$userID?' mdisabled="mdisabled" ':'').'></td>
				</tr>
					
			</table>
		</form>
		<script>
		 	var frmvalidator = new formValidator("editMasterDevice");
 			frmvalidator.addValidation("description","req","Please enter a description");
			frmvalidator.addValidation("MasterDeviceCategory","dontselect=0","Please select a category");
			frmvalidator.addValidation("Manufacturer","dontselect=0","Please select a manufacturer");			
			'.$deviceDataFormValidation.'
		</script>
		';
	} else {
		if(isset($_REQUEST['doID'])){
			$doID=(int)$_REQUEST['doID'];
			$dbADO->Execute('DELETE FROM DeviceTemplate_DesignObj WHERE FK_DeviceTemplate=? AND FK_DesignObj=?',array($deviceID,$doID));
			header("Location: index.php?section=editMasterDevice&model=$deviceID&msg=Screen removed from device template.");
			exit();			
		}
		
		$description=cleanString($_POST['description'],30);
		
		$commandLine = cleanString($_POST['CommandLine'],100);
		$ImplementsDCE = @cleanInteger($_POST['ImplementsDCE']);
		$category = cleanInteger($_POST['MasterDeviceCategory']);
		$manufacturer = cleanInteger($_POST['Manufacturer']);
		$package = (@$_POST['package']!='0')?cleanInteger(@$_POST['package']):NULL;
		$isPlugIn = cleanInteger(@$_POST['isPlugIn']);
		$oldIsPlugIn = cleanInteger(@$_POST['oldIsPlugIn']);
		$isEmbedded = cleanInteger(@$_POST['isEmbedded']);
		$oldIsEmbedded = cleanInteger(@$_POST['oldIsEmbedded']);
		$inheritsMacFromPC = cleanInteger(@$_POST['inheritsMacFromPC']);
		$oldInheritsMacFromPC= cleanInteger(@$_POST['oldInheritsMacFromPC']);
		$isIPBased= cleanInteger(@$_POST['isIPBased']);
		$ConfigureScript=$_POST['ConfigureScript'];
		$oldIsIPBased= cleanInteger(@$_POST['oldIsIPBased']);
		$comments=cleanString($_POST['comments']);
		$newScreen=(int)$_POST['newScreen'];
		$newMacFrom=(int)$_POST['mac_from'];
		$newMacTo=(int)$_POST['mac_to'];
		$newManufacturer=((int)$_POST['manufacturerPnp']>0)?(int)$_POST['manufacturerPnp']:NULL;
		$manufacturerURL=($_POST['manufacturerURL']!='')?$_POST['manufacturerURL']:NULL;
		$internalURLsufix=($_POST['internalURLsufix']!='')?$_POST['internalURLsufix']:NULL;
		$newCategory=((int)$_POST['categoryPnp']>0)?(int)$_POST['categoryPnp']:NULL;
		$newComment=$_POST['commentPnp'];
		$dhcpArray=explode(',',$_POST['dhcpArray']);
		$isAVDevice = cleanInteger(@$_POST['isAVDevice']);
		$old_isAVDevice = cleanInteger(@$_POST['old_isAVDevice']);

		
		if($newMacFrom!='' && $newMacTo!=''){
			$dbADO->Execute('INSERT INTO DHCPDevice (FK_DeviceTemplate, Mac_Range_Low, Mac_Range_High,FK_Manufacturer,FK_DeviceCategory,Description) VALUES (?,?,?,?,?,?)',array($deviceID,$newMacFrom,$newMacTo,$newManufacturer,$newCategory,$newComment));
			$locationGoTo='plugAndPlay';
		}
		
		foreach ($dhcpArray AS $dhcpID){
			$macFrom=@$_POST['mac_from_'.$dhcpID];
			$macTo=@$_POST['mac_to_'.$dhcpID];
			$newManufacturer=((int)@$_POST['manufacturerPnp_'.$dhcpID]>0)?(int)$_POST['manufacturerPnp_'.$dhcpID]:NULL;
			$newCategory=((int)@$_POST['categoryPnp_'.$dhcpID]>0)?(int)$_POST['categoryPnp_'.$dhcpID]:NULL;
			$newComment=@$_POST['commentPnp_'.$dhcpID];
			$dbADO->Execute('UPDATE DHCPDevice SET Mac_Range_Low=?, Mac_Range_High=?,FK_Manufacturer=?,FK_DeviceCategory=?,Description=? WHERE PK_DHCPDevice=?',array($macFrom, $macTo,$newManufacturer,$newCategory,$newComment,$dhcpID));
		}
		
		if($_REQUEST['toDel']!=''){
			$dhcpID=(int)$_REQUEST['toDel'];
			if($dhcpID!=0){
				$dbADO->Execute('DELETE FROM DHCPDevice WHERE PK_DHCPDevice=?',$dhcpID);
			}
		}
		
		if($newScreen!=0){
			$newScreenExist=$dbADO->Execute('SELECT * FROM DesignObj WHERE PK_DesignObj=?',$newScreen);
			if($newScreenExist->RecordCount()>0){
				$screenDT=$dbADO->Execute('SELECT * FROM DeviceTemplate_DesignObj WHERE FK_DeviceTemplate=? AND FK_DesignObj=?',array($deviceID,$newScreen));
				if($screenDT->RecordCount()>0){
					header("Location: index.php?section=editMasterDevice&model=$deviceID&error=Screen already exist!");
					exit();
				}else{
					$dbADO->Execute('INSERT INTO DeviceTemplate_DesignObj (FK_DeviceTemplate,FK_DesignObj) VALUES (?,?)',array($deviceID,$newScreen));
				}
			}else{
				header("Location: index.php?section=editMasterDevice&model=$deviceID&error=Screen does not exist!");
				exit();			
			}
		}
		
		$locationGoTo=''; 

		
		$usedParams = @cleanString($_POST['usedData']);
		
		$usedParamsArray = explode(",",$usedParams);
		foreach ($usedParamsArray as $param) {
			$param=(int)$param;
			$descriptionParam = cleanString(@$_POST['Data_Description_'.$param]);
			$defaultValueParam = cleanString(@$_POST['Data_DefaultValue_'.$param]);
			$requiredParam = cleanInteger(@$_POST['Data_Required_'.$param]);
			$allowedToModifyParam = cleanInteger(@$_POST['Data_AllowedToModify_'.$param]);
			$useDeviceTemplateDefaultParam = cleanInteger(@$_POST['Data_UseDeviceTemplateDefault_'.$param]);
			$setByDeviceParam = cleanInteger(@$_POST['Data_SetByDevice_'.$param]);
			if ($descriptionParam!='') {
				
				$getOldValues = "select IK_DeviceData,Description,Required,
										AllowedToModify,UseDeviceTemplateDefault,SetByDevice
								 FROM DeviceTemplate_DeviceData WHERE FK_DeviceData = $param AND FK_DeviceTemplate = $deviceID";
				$resOldValuesData = $dbADO->Execute($getOldValues);
				if ($resOldValuesData && $resOldValuesData->RecordCount()==1) {
					$rowOldValuesData = $resOldValuesData->FetchRow();
					if ($descriptionParam!=$rowOldValuesData['Description']) {
						$locationGoTo = "Data_Description_{$param}";
					}
					if ($defaultValueParam!=$rowOldValuesData['IK_DeviceData']) {
						$locationGoTo = "Data_DefaultValue_{$param}";
					}
					if ($requiredParam!=$rowOldValuesData['Required']) {
						$locationGoTo = "Data_Required_{$param}";
					}
					if ($allowedToModifyParam!=$rowOldValuesData['AllowedToModify']) {
						$locationGoTo = "Data_AllowedToModify_{$param}";
					}
					if ($useDeviceTemplateDefaultParam!=$rowOldValuesData['UseDeviceTemplateDefault']) {
						$locationGoTo = "Data_UseDeviceTemplateDefault_{$param}";
					}
					if ($setByDeviceParam!=$rowOldValuesData['SetByDevice']) {
						$locationGoTo = "Data_SetByDevice_{$param}";
					}

				}
				$query = "update DeviceTemplate_DeviceData
							SET
						  IK_DeviceData = ?,
						  Description = ?,
						  Required = ?,
						  AllowedToModify = ?,
						  UseDeviceTemplateDefault = ?,
						  SetByDevice = ?
							WHERE FK_DeviceData = ? AND FK_DeviceTemplate = ?
						";			
				$dbADO->Execute($query,array($defaultValueParam,$descriptionParam,$requiredParam,
											$allowedToModifyParam,$useDeviceTemplateDefaultParam,
											$setByDeviceParam,$param,$deviceID));
			}
		}
		//commands process
		$usedCommands = cleanString($_POST['DeviceCommandGroupDisplayed']);		
		$usedCommandsArray = explode(",",$usedCommands);
		
		
		foreach ($usedCommandsArray as $param) {
			$queryForCommandGroup='';
			$param=(int)$param;
			$deviceCommandGroup = cleanInteger(@$_POST['DeviceCommandGroup_'.$param]);
				
				$getOldValues = "select FK_DeviceTemplate  
								 FROM DeviceTemplate_DeviceCommandGroup WHERE FK_DeviceTemplate = $deviceID and FK_DeviceCommandGroup = $param";
				
				$resOldValuesData = $dbADO->Execute($getOldValues);
				
				if ($resOldValuesData && $resOldValuesData->RecordCount()==1) {					
					if ($deviceCommandGroup==0) {
						$queryForCommandGroup = "delete from DeviceTemplate_DeviceCommandGroup where FK_DeviceTemplate = $deviceID and FK_DeviceCommandGroup = $param";
						$dbADO->Execute($queryForCommandGroup);
						$locationGoTo = "#commands_link";
						//$locationGoTo = "DeviceCommandGroup_".$param;
					} else {
						//nothing: is in database, and it comes selected
					}
				} else {
					//insert record in database
						if ($deviceCommandGroup==1) {
								$queryForCommandGroup = "insert into DeviceTemplate_DeviceCommandGroup(FK_DeviceCommandGroup,FK_DeviceTemplate) values(?,?)";
								$dbADO->Execute($queryForCommandGroup,array($param,$deviceID));
								//$locationGoTo = "DeviceCommandGroup_".$param;
								$locationGoTo = "#commands_link";								
						}
				}				
		}

		//new command group added: addNewCommandGroupToMasterDevice
		$addNewCommandGroupToMasterDevice = (int)@$_POST['addNewCommandGroupToMasterDevice'];
		if ($addNewCommandGroupToMasterDevice!=0) {
			$query = "insert into DeviceTemplate_DeviceCommandGroup (FK_DeviceTemplate , FK_DeviceCommandGroup)
					values ($deviceID,$addNewCommandGroupToMasterDevice)";
			$dbADO->Execute($query);
		}

		// Infrared Group process
		$usedInfrared = cleanString($_POST['InfraredGroupDisplayed']);		
		$usedInfraredArray = explode(",",$usedInfrared);
		
		
		foreach ($usedInfraredArray as $param) {
			$queryForInfraredGroup='';
			$param=(int)$param;
			$InfraredGroup = cleanInteger(@$_POST['InfraredGroup_'.$param]);
				//$dbADO->debug=true;
				$getOldValues = "select FK_DeviceTemplate  
								 FROM DeviceTemplate_InfraredGroup WHERE FK_DeviceTemplate = $deviceID and FK_InfraredGroup = $param";
				
				$resOldValuesData = $dbADO->Execute($getOldValues);
				
				if ($resOldValuesData && $resOldValuesData->RecordCount()==1) {					
					if ($InfraredGroup==0) {
						$queryForInfraredGroup = "DELETE FROM DeviceTemplate_InfraredGroup where FK_DeviceTemplate = $deviceID and FK_InfraredGroup = $param";
						$dbADO->Execute($queryForInfraredGroup);
						$locationGoTo = "#infrared_link";
					} else {
						//nothing: is in database, and it comes selected
					}
				} else {
					//insert record in database
						if ($InfraredGroup==1) {
								$queryForInfraredGroup = "insert into DeviceTemplate_InfraredGroup(FK_InfraredGroup,FK_DeviceTemplate) values(?,?)";
								$dbADO->Execute($queryForInfraredGroup,array($param,$deviceID));
								$locationGoTo = "#infrared_link";								
						}
				}				
		//$dbADO->debug=false;
				
		}

		//new infrared group added: addNewInfraredGroupToMasterDevice
		$addNewInfraredGroupToMasterDevice = (int)@$_POST['addNewInfraredGroupToMasterDevice'];
		$query='SELECT * FROM DeviceTemplate_InfraredGroup WHERE FK_DeviceTemplate=? AND FK_InfraredGroup=?';
		$resInfraredGroupDeviceTemplate=$dbADO->Execute($query,array($deviceID,$addNewInfraredGroupToMasterDevice));
		if($resInfraredGroupDeviceTemplate->RecordCount()!=0){
			header("Location: index.php?section=editMasterDevice&model=$deviceID&irerror=Infrared Group already exists!");
			exit();
		}
		if ($addNewInfraredGroupToMasterDevice!=0) {
			$query = "insert into DeviceTemplate_InfraredGroup (FK_DeviceTemplate , FK_InfraredGroup)
					values ($deviceID,$addNewInfraredGroupToMasterDevice)";
			$dbADO->Execute($query);
		}

		
		//newEventToMasterDevice
		$newEventToMasterDevice = (int)$_POST['newEventToMasterDevice'];
		if ($newEventToMasterDevice!=0) {
			$query = "insert into DeviceTemplate_Event( FK_DeviceTemplate , FK_Event ) values(?,?)";
			$dbADO->Execute($query,array($deviceID,$newEventToMasterDevice));
			$locationGoTo = "EventDesc_{$newEventToMasterDevice}";
		}
		
		//EventDisplayed
		$usedEvents = cleanString($_POST['eventsListDisplayed']);
		$usedEventsArray = explode(",",$usedEvents);
		 foreach ($usedEventsArray as $elem) {
		 	$desc = cleanString(@$_POST['EventDesc_'.$elem]);
		 	$selectOldValue = 'select Description from DeviceTemplate_Event where FK_DeviceTemplate = ? and FK_Event = ?';
		 	$resOldValue = $dbADO->Execute($selectOldValue,array($deviceID,$elem));
		 	$oldDescForEvent='';
		 		if ($resOldValue) {
		 			$row=$resOldValue->FetchRow();
		 			$oldDescForEvent = $row['Description'];
		 		}
		 	if ($desc!='' && $desc!=$oldDescForEvent) {		 		
		 		$query = 'update DeviceTemplate_Event set Description = ? where FK_DeviceTemplate = ? and FK_Event = ?';
		 		$dbADO->Execute($query,array($desc,$deviceID,$elem));
		 		$locationGoTo = "EventDesc_".$elem;
		 	}
		 }
		
		$newDeviceData = (int)$_POST['newDeviceData'];
		if ($newDeviceData!=0 && $deviceID!=0) {
			$query = "insert into DeviceTemplate_DeviceData( FK_DeviceTemplate , FK_DeviceData) values(?,?)";
			$dbADO->Execute($query,array($deviceID,$newDeviceData));
			
			$dbADO->Execute('INSERT IGNORE INTO Device_DeviceData (FK_DeviceData, FK_Device) SELECT '.$newDeviceData.',PK_Device FROM Device WHERE FK_DeviceTemplate=?',$deviceID);
			$locationGoTo = "Data_Description_{$newDeviceData}";

		}

		if ($isAVDevice!=$old_isAVDevice) {			
			if($isAVDevice==1){
				$insertRecord = "INSERT INTO DeviceTemplate_AV (FK_DeviceTemplate,UsesIR) VALUES(?,1)";
				$resInsertRecord = $dbADO->Execute($insertRecord,array($deviceID));
				$controlSQL='INSERT INTO DeviceTemplate_DeviceCategory_ControlledVia (FK_DeviceTemplate,FK_DeviceCategory) VALUES (?,?)';
				$dbADO->Execute($controlSQL,array($deviceID,$GLOBALS['specialized']));
				$dbADO->Execute($controlSQL,array($deviceID,$GLOBALS['InfraredInterface']));
			}else{
				$dbADO->Execute('DELETE FROM DeviceTemplate_AV WHERE FK_DeviceTemplate=?',$deviceID);
				$dbADO->Execute('DELETE FROM DeviceTemplate_DeviceCategory_ControlledVia WHERE FK_DeviceTemplate=?',$deviceID);
			}
		}

		
		if ($deviceID!=0 && $description!='' && $category!=0 && $manufacturer!=0) {
			$updateQuery = "UPDATE DeviceTemplate SET 
							Description = ?, 
							ImplementsDCE = ?,							
							CommandLine = ?, 
							FK_DeviceCategory = ?,FK_Manufacturer  = ?,
							FK_Package=?,
							IsPlugIn =?,
							IsEmbedded=?,
							InheritsMacFromPC=?,
							IsIPBased=?,
							ConfigureScript=?,
							Comments=?,
							ManufacturerURL=?,
							InternalURLSuffix=?
							WHERE PK_DeviceTemplate = ?";

			$dbADO->Execute($updateQuery,array($description,$ImplementsDCE,$commandLine,$category,$manufacturer,$package,$isPlugIn,$isEmbedded,$inheritsMacFromPC,$isIPBased,$ConfigureScript,$comments,$manufacturerURL,$internalURLsufix,$deviceID));

			if($isPlugIn==1 && $oldIsPlugIn==0){
				$insertControlledVia = '
					INSERT INTO DeviceTemplate_DeviceTemplate_ControlledVia
						(FK_DeviceTemplate, FK_DeviceTemplate_ControlledVia) 
					VALUES(?,?)';
				$query = $dbADO->Execute($insertControlledVia,array($deviceID,$GLOBALS['rootDCERouter']));
			
			}	
			$addRelated=(int)$_POST['addRelated'];
			$valueRelated=(int)$_POST['valueRelated'];
			if($addRelated!=0){
				$dtRelatedArray=getFieldsAsArray('DeviceTemplate','PK_DeviceTemplate',$dbADO,'WHERE PK_DeviceTemplate='.$addRelated);
				if(count($dtRelatedArray)==0){
					header("Location: index.php?section=editMasterDevice&model=$deviceID&errRelated=Device Template no $addRelated does not exist!#dtRelated");
					exit();
				}else{
					$alreadyExist=getFieldsAsArray('DeviceTemplate_DeviceTemplate_Related','FK_DeviceTemplate_Related',$dbADO,'WHERE FK_DeviceTemplate='.$deviceID.' AND FK_DeviceTemplate_Related='.$addRelated);
					if(count($alreadyExist)>0){
						header("Location: index.php?section=editMasterDevice&model=$deviceID&errRelated=Device Template is already related with $addRelated!#dtRelated");
						exit();
					}else{
						$dbADO->Execute('INSERT INTO DeviceTemplate_DeviceTemplate_Related (FK_DeviceTemplate,FK_DeviceTemplate_Related,Value) VALUES (?,?,?)',array($deviceID,$addRelated,$valueRelated));
						$locationGoTo='#dtRelated';
					}
				}
			}
			
			if((int)$_POST['delRelated']>0){
				$dbADO->Execute('DELETE FROM DeviceTemplate_DeviceTemplate_Related WHERE FK_DeviceTemplate=? AND FK_DeviceTemplate_Related=?',array($deviceID,(int)$_POST['delRelated']));
				$locationGoTo='#dtRelated';
			}
			
			if (strstr($locationGoTo,"#")) {
				header("Location: index.php?section=editMasterDevice&model=$deviceID&msg=Saved!".$locationGoTo);
			} else {
				header("Location: index.php?section=editMasterDevice&model=$deviceID&msg=Saved!&lastAction=".$locationGoTo);
			}
		} else {
			header("Location: index.php?section=editMasterDevice&model=$deviceID&msg=Missed parrameters!");
		}
		
		
	}
	$onLoad='';
	if ($lastAction!=''?$onLoad.="if (document.forms.editMasterDevice.{$lastAction}) {document.forms.editMasterDevice.{$lastAction}.focus();} ":$onLoad.="")
	if (strlen($onLoad)>2) {
		$output->setScriptInBody("onLoad=\"javascript:eval('$onLoad');\"");
	}
	
	$output->setNavigationMenu(array("Device Templates"=>'index.php?section=deviceTemplates',$description=>'index.php?section=editMasterDevice&model='.$deviceID));
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Edit Device Template');			
	$output->output();
	
}
?>
