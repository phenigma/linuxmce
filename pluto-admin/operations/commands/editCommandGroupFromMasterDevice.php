<?php
function editCommandGroupFromMasterDevice($output,$dbADO) {
	//$dbADO->debug=true;
	$out='';
	$lastAction = isset($_REQUEST['lastAction'])?cleanString($_REQUEST['lastAction']):'';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	
	
	
	if ($action=='form') {
		
		$commandGroupID = (int)$_REQUEST['commandGroupID'];
		$deviceID = (int)$_REQUEST['deviceID'];
	
		$queryGetValues = "select * from DeviceCommandGroup where PK_DeviceCommandGroup = ?";
		$rs = $dbADO->Execute($queryGetValues,array($commandGroupID));
		if ($rs->RecordCount()==1) {
			$row=$rs->FetchRow();
			$deviceCategoryID = (int)$row['FK_DeviceCategory'];
			$commandGroupName = $row['Description'];
			$rs->Close();
		}

		$out.='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		</script>
		<form action="index.php" method="post" name="editCommandGroupFromMasterDevice">
		<input type="hidden" name="section" value="editCommandGroupFromMasterDevice">
		<input type="hidden" name="action" value="add">		
		<input type="hidden" name="lastAction" value="">
		<input type="hidden" name="deviceID" value="'.$deviceID.'">
		<input type="hidden" name="commandGroupID" value="'.$commandGroupID.'">
		<input type="hidden" name="from" value="'.$from.'">
			<table>			
				<tr>
					<td>Device Category:</td>
					<td>
						<select name="deviceCategory">
						<option value="0">-please select-</option>
						';
		
						$queryMasterDeviceCategories_parents = 'select PK_DeviceCategory,Description from DeviceCategory where FK_DeviceCategory_Parent is null order by Description asc';
						$rs = $dbADO->_Execute($queryMasterDeviceCategories_parents);
							while ($row = $rs->FetchRow()) {
								$out.='<option '.($row['PK_DeviceCategory']==$deviceCategoryID?' selected ': ' ').' value="'.$row['PK_DeviceCategory'].'">'.$row['Description'].'</option>';
								$out.=getDeviceCategoryChildsOptions($row['PK_DeviceCategory'],$row['Description'],$deviceCategoryID,'',$dbADO);
							}
						$rs->Close();
					$out.='
						</select>
					</td>
				</tr>
				<tr>
					<td>Description:</td>
					<td>
						<input name="Description" value="'.stripslashes($commandGroupName).'" type="text" size="20">						
					</td>
				</tr>		
				<tr>
					<td colspan="2">		
						<fieldset>
							<legend>Commands:</legend>							
							<table>
							';
							$query = "SELECT DeviceCommandGroup_Command.*, Command.Description as C_Description						
											FROM DeviceCommandGroup_Command 
												INNER JOIN Command on FK_Command = PK_Command
									  WHERE FK_DeviceCommandGroup = $commandGroupID
									  order by Command.Description asc";
							$res = $dbADO->Execute($query);
							$commandsDisplayed = array();
							$commandsDisplayed[]=0;
								if ($res) {
									while ($row=$res->FetchRow()) {
										$out.="<tr ".(strlen(trim($row['Description']))==0?" bgColor='green' ":"")."><td>#{$row['C_Description']}<br />#{$row['FK_Command']}</td><td><textarea cols=\"40\" rows=\"5\" name=\"CommandDescription_{$row['FK_Command']}\">".stripslashes($row['Description']).'</textarea></td><td><a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=editCommand&from=editCommandGroupFromMasterDevice&commandID='.$row['FK_Command'].'\',\'width=400,height=300,toolbars=true,resizable=1,scrollbars=1\');">Edit</a> ';
										$out.='<a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=deleteCommandFromMasterDevice&from=editCommandGroupFromMasterDevice&commandGroupID='.$commandGroupID.'&commandID='.$row['FK_Command'].'\',\'width=100,height=100,toolbars=true,resizable=1,scrollbars=1\');">Delete</a>';
										$out.='</td></tr>';
										$commandsDisplayed[]=$row['FK_Command'];
									}
								}
							$queryGetCommands = "select PK_Command,Description from Command where PK_Command NOT IN (".join(",",$commandsDisplayed).") order by Description asc";
							$otherCommands = '<option value="0">-please select-</option>';
							$res = $dbADO->Execute($queryGetCommands);
							if ($res->RecordCount()==0) {
								$otherCommands = '<option value="0">No commands left</option>';
							}
								if ($res) {
									while ($row=$res->FetchRow()) {
										$otherCommands.="<option value='{$row['PK_Command']}'>".stripslashes($row['Description']).'</option>';			
									}
								}
							
					$out.='
							</table>
						<br />Add this command to group: <select name="addNewCommandToDeviceCommandGroup">'.$otherCommands.'</select><input type="submit" class="button" name="submitX" value="Add">';
						$out.='<br /><a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=addCommand&from=editCommandGroupFromMasterDevice\',\'width=400,height=300,toolbars=true,resizable=1,scrollbars=1\');">Create new command</a>';
						$out.='<input type="hidden" name="displayedCommands" value="'.(join(",",$commandsDisplayed)).'">
						</fieldset>
					</td>
				</tr>
				<tr>
					<td colspan="2" align="center"><input type="submit" class="button" name="submitX" value="Save"></td>
				</tr>
			</table>
		</form>
		<script>
		 	var frmvalidator = new formValidator("editCommandGroupFromMasterDevice");
 			frmvalidator.addValidation("deviceCategory","dontselect=0","Please select a category"); 			
			frmvalidator.addValidation("Description","req","Please enter a description");
		</script>
		';
		
	} else {
		$lastAction='';
		$deviceCategory = isset($_POST['deviceCategory'])?cleanString($_POST['deviceCategory']):'0';
		$deviceID = isset($_POST['deviceID'])?cleanString($_POST['deviceID']):'0';				
		$description = cleanString($_POST['Description'],25);
		$commandGroupID = cleanInteger($_POST['commandGroupID']);
		
		$displayedCommands = explode(",",@$_POST['displayedCommands']);
		if (!is_array($displayedCommands)) {$displayedCommands=array();}
		foreach ($displayedCommands as $elem) {
			$elem = (int)$elem;
			$commandAssocDescription = cleanString(@$_POST['CommandDescription_'.$elem]);
			if ($commandAssocDescription!='') {
				//get old value
				$query = "select Description from DeviceCommandGroup_Command where FK_DeviceCommandGroup = ? and FK_Command = ?";
				$resD = $dbADO->Execute($query,array($commandGroupID,$elem));
				if ($resD && $resD->RecordCount()==1) {
					$rowD = $resD->FetchRow();
					if ($commandAssocDescription!=$rowD['Description']) {
						$lastAction='CommandDescription_'.$elem;
					}
				}
				$query = "update DeviceCommandGroup_Command set Description = ? where FK_DeviceCommandGroup = ? and FK_Command = ?";
				$dbADO->Execute($query,array(stripslashes($commandAssocDescription),$commandGroupID,$elem));
			}
		}
		
		$addNewCommandToThisCommandGroup = (int)$_POST['addNewCommandToDeviceCommandGroup'];
		if ($addNewCommandToThisCommandGroup!=0) {
			$query = "insert into DeviceCommandGroup_Command(FK_DeviceCommandGroup,FK_Command) values(?,?)";
			$dbADO->Execute($query,array($commandGroupID,$addNewCommandToThisCommandGroup));			
			$lastAction='CommandDescription_'.$addNewCommandToThisCommandGroup;
		}
		
		if ((int)$deviceCategory!=0 && (int)$deviceID!=0 && $description!='') {
			$insertObjToDevice = 'UPDATE DeviceCommandGroup 
					SET FK_DeviceCategory=?,
						Description=?
					WHERE   PK_DeviceCommandGroup = ? 
					';			
			$query = $dbADO->Execute($insertObjToDevice,array($deviceCategory,stripslashes($description),$commandGroupID));
			$out.="";
			header("Location: index.php?section=editCommandGroupFromMasterDevice&from=$from&deviceID=$deviceID&commandGroupID=$commandGroupID&lastAction=$lastAction");
		}		
	}
	
	$onLoad='';
	
	if ($lastAction!=''?$onLoad.="if (document.forms.editCommandGroupFromMasterDevice.{$lastAction}) {document.forms.editCommandGroupFromMasterDevice.{$lastAction}.focus();} ":$onLoad.="")
	if (strlen($onLoad)>2) {
		$output->setScriptInBody("onLoad=\"javascript:eval('$onLoad');\"");
	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Edit Command Group');			
	$output->output();
}
?>
