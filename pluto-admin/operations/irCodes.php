<?
function irCodes($output,$dbADO) {
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$dbADO->debug=false;

	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	
	$deviceID = (int)$_REQUEST['deviceID'];
	$userID = $_SESSION['userID'];
	
	if ($deviceID==0) {
		header("Location: index.php?section=userHome");
	}
	
	$infraredGroupID=@$_REQUEST['infraredGroupID'];	

	if ($action=='form') {
	$out.='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		</script>	
		<form action="index.php" method="POST" name="irCodes">
			<input type="hidden" name="section" value="irCodes">
			<input type="hidden" name="action" value="update">
			<input type="hidden" name="deviceID" value="'.$deviceID.'">';
	$selectDTData='
		SELECT DeviceTemplate.Description AS Template, DeviceCategory.Description AS Category,Manufacturer.Description AS Manufacturer 
		FROM DeviceTemplate
		INNER JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory
		INNER JOIN Manufacturer ON FK_Manufacturer=PK_Manufacturer
		WHERE PK_DeviceTemplate=?';
	$resDTData=$dbADO->Execute($selectDTData,$deviceID);
	if($resDTData->RecordCount()==0){
		header("Location: index.php?section=userHome");
		exit();
	}
	$rowDTData=$resDTData->FetchRow();
	$out.='
		<table>
			<tr>
				<td colspan="2">WARNING: the changes will afect all devices created from <B>'.$rowDTData['Template'].'</B> template.</td>
			</tr>
			<tr>
				<td width="50%" valign="top">Here are the groups of infrared codes for device template <B>'.$rowDTData['Template'].'</B>, category <B>'.$rowDTData['Category'].'</B> and manufacturer <B>'.$rowDTData['Manufacturer'].'</B>.<br><br>
				<table>				
				';

				
				$query = "SELECT InfraredGroup.* FROM InfraredGroup
								INNER JOIN DeviceTemplate_InfraredGroup on FK_InfraredGroup = PK_InfraredGroup
							WHERE 
								FK_DeviceTemplate='$deviceID'
								ORDER BY InfraredGroup.Description Asc
				";
				$resInfraredGroups = $dbADO->_Execute($query);
				
				$infraredGroupsDisplayed = array();
				if ($resInfraredGroups) {
					while ($row = $resInfraredGroups->FetchRow()) {
						$out.='<tr>
								<td>
									<input type="checkbox" name="InfraredGroup_'.$row['PK_InfraredGroup'].'" value="1" '.((isset($_POST['InfraredGroup_'.$row['PK_InfraredGroup']]))?'checked':'').' onClick="document.irCodes.action.value=\'form\';document.irCodes.submit();">'.stripslashes($row['Description']).' #'.$row['PK_InfraredGroup'].'
								</td>
								<td>
									<a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=editInfraredGroupFromMasterDevice&from=editMasterDevice&deviceID='.$deviceID.'&infraredGroupID='.$row['PK_InfraredGroup'].'\',\'width=800,height=600,toolbars=true,resizable=1,scrollbars=yes\');">Edit Infrared Commands</a>								
								</td>
							  </tr>';
						$infraredGroupsDisplayed[]=$row['PK_InfraredGroup'];
					}					
				}

				$out.='
						<input type="hidden" name="InfraredGroupDisplayed" value="'.(join(",",$infraredGroupsDisplayed)).'">					
						<tr>
							<td>'.(isset($_GET['irerror'])?'<b>'.$_GET['irerror'].'</b><br>':'').'
								<a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=addInfraredGroup&from=irCodes\',\'width=550,height=300,toolbars=true\');">Create Infrared Group</a>
							</td>
						</tr>
					</table>
				</td>
				<td>Edit codes for: <select name="infraredGroupID" onChange="document.irCodes.action.value=\'form\';document.irCodes.submit();"">
					<option value="Please select">';
				$resInfraredGroups->MoveFirst();
				while ($row = $resInfraredGroups->FetchRow()) {
					$out.='<option value="'.$row['PK_InfraredGroup'].'" '.(($row['PK_InfraredGroup']==$infraredGroupID)?'selected':'').'>'.$row['Description'].'</option>';
				}
				$out.='</select><br>';
				
				if(isset($infraredGroupID) && $infraredGroupID!=0){
					$out.='
					<fieldset>
						<legend>Commands:</legend>							
							<table border="0">
								';
					$queryC = "
						SELECT Input.PK_Command As PK_Command_Input,Output.PK_Command AS PK_Command_Output,DSPMode.PK_Command AS PK_Command_DSPMode,InfraredGroup_Command.*, Command.Description as C_Description 
						FROM InfraredGroup_Command 
						INNER JOIN Command on InfraredGroup_Command.FK_Command = PK_Command 
						LEFT JOIN Command AS Input ON Input.PK_Command=Command.PK_Command AND Input.FK_CommandCategory=22
						LEFT JOIN Command AS Output ON Output.PK_Command=Command.PK_Command AND Output.FK_CommandCategory=27
						LEFT JOIN Command AS DSPMode ON DSPMode.PK_Command=Command.PK_Command AND DSPMode.FK_CommandCategory=21
						WHERE FK_InfraredGroup = ? 
						ORDER BY Command.Description asc
					";
								
					$res = $dbADO->Execute($queryC,$infraredGroupID);
					$commandsDisplayed = array();
					$commandsDisplayed[]=0;
					$simpleCommands=array();
					$inputCommands=array();
					$outputCommands=array();
					$dspModeCommands=array();
					if ($res) {
						while ($row=$res->FetchRow()) {
							if(is_null($row['PK_Command_Input']) && is_null($row['PK_Command_Output']) && is_null($row['PK_Command_DSPMode']))
								$simpleCommands[$row['FK_Command']]=$row['Description'];
							if(!is_null($row['PK_Command_Input']) && $row['PK_Command_Input']!='')
								$inputCommands[$row['FK_Command']]=$row['Description'];
							if(!is_null($row['PK_Command_Output']) && $row['PK_Command_Output']!='')
								$outputCommands[]=$row['FK_Command'];
							if(!is_null($row['PK_Command_DSPMode']) && $row['PK_Command_DSPMode']!='')
								$dspModeCommands[$row['FK_Command']]=$row['Description'];
							$commandsDisplayed[]=$row['FK_Command'];
						}
						
						$normalCommandsTxt='<tr><td><B>Normal Commands</B></td></tr>';
						foreach($simpleCommands AS $commandID=>$commandDescription){
							$normalCommandsTxt.="<tr ".(strlen(trim($commandDescription))==0?" bgColor='lightgreen' ":"")."><td>#{$commandDescription}<br />#{$commandID}</td><td><textarea cols=\"40\" rows=\"5\" name=\"CommandDescription_{$commandID}\">".stripslashes($commandDescription).'</textarea></td><td> ';
							$normalCommandsTxt.='<a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=deleteInfraredCommandFromMasterDevice&from=irCodes&infraredGroupID='.$infraredGroupID.'&commandID='.$commandID.'\',\'width=100,height=100,toolbars=true,resizable=1,scrollbars=1\');">Delete</a>';
							$normalCommandsTxt.='</td></tr>';
						}

						$inputCommandsTxt='<tr><td><B>Intput Commands</B></td></tr>';
						foreach($inputCommands AS $commandID=>$commandDescription){
							$inputCommandsTxt.="<tr ".(strlen(trim($commandDescription))==0?" bgColor='lightgreen' ":"")."><td>#{$commandDescription}<br />#{$commandID}</td><td><textarea cols=\"40\" rows=\"5\" name=\"CommandDescription_{$commandID}\">".stripslashes($commandDescription).'</textarea></td><td> ';
							$inputCommandsTxt.='<a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=deleteInfraredCommandFromMasterDevice&from=irCodes&infraredGroupID='.$infraredGroupID.'&commandID='.$commandID.'\',\'width=100,height=100,toolbars=true,resizable=1,scrollbars=1\');">Delete</a>';
							$inputCommandsTxt.='</td></tr>';
						}

						
						$outputCommandsTxt='<tr><td><B>Output Commands</B></td></tr>';
						foreach($outputCommands AS $commandID=>$commandDescription){
							$outputCommandsTxt.="<tr ".(strlen(trim($commandDescription))==0?" bgColor='lightgreen' ":"")."><td>#{$commandDescription}<br />#{$commandID}</td><td><textarea cols=\"40\" rows=\"5\" name=\"CommandDescription_{$commandID}\">".stripslashes($commandDescription).'</textarea></td><td> ';
							$outputCommandsTxt.='<a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=deleteInfraredCommandFromMasterDevice&from=irCodes&infraredGroupID='.$infraredGroupID.'&commandID='.$commandID.'\',\'width=100,height=100,toolbars=true,resizable=1,scrollbars=1\');">Delete</a>';
							$outputCommandsTxt.='</td></tr>';
						}
						
						$dspModeCommandsTxt='<tr><td><B>DSPMode Commands</B></td></tr>';
						foreach($dspModeCommands AS $commandID=>$commandDescription){
							$dspModeCommandsTxt.="<tr ".(strlen(trim($commandDescription))==0?" bgColor='lightgreen' ":"")."><td>#{$commandDescription}<br />#{$commandID}</td><td><textarea cols=\"40\" rows=\"5\" name=\"CommandDescription_{$commandID}\">".stripslashes($commandDescription).'</textarea></td><td> ';
							$dspModeCommandsTxt.='<a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=deleteInfraredCommandFromMasterDevice&from=irCodes&infraredGroupID='.$infraredGroupID.'&commandID='.$commandID.'\',\'width=100,height=100,toolbars=true,resizable=1,scrollbars=1\');">Delete</a>';
							$dspModeCommandsTxt.='</td></tr>';
						}
						
					}
					$queryGetCommands = "
						SELECT PK_Command_Input,PK_Command_Output,PK_Command_DSPMode,PK_Command,Command.Description 
						FROM Command 
							LEFT JOIN Command AS Input ON Input.PK_Command=Command.PK_Command AND Input.FK_CommandCategory=22
							LEFT JOIN Command AS Output ON Output.PK_Command=Command.PK_Command AND Input.FK_CommandCategory=27
							LEFT JOIN Command AS DSPMode ON DSPMode.PK_Command=Command.PK_Command AND Input.FK_CommandCategory=21
						WHERE PK_Command NOT IN (".join(",",$commandsDisplayed).") ORDER BY Description ASC";
					$otherCommands = '<option value="0">-please select-</option>';
					$otherInputCommands = '<option value="0">-please select-</option>';
					$otherOutputCommands = '<option value="0">-please select-</option>';
					$otherDSPModeCommands = '<option value="0">-please select-</option>';
					$res = $dbADO->Execute($queryGetCommands);
					if ($res->RecordCount()==0) {
						$otherCommands = '<option value="0">No commands left</option>';
					}
					if ($res) {
						while ($row=$res->FetchRow()) {
							if(is_null($row['PK_Command_Input']) && is_null($row['PK_Command_Output']) && is_null($row['PK_Command_DSPMode']))
								$otherCommands.="<option value='{$row['PK_Command']}'>".stripslashes($row['Description']).'</option>';
							if(!is_null($row['PK_Command_Input']) && $row['PK_Command_Input']!='')
								$otherInputCommands.="<option value='{$row['PK_Command']}'>".stripslashes($row['Description']).'</option>';
							if(!is_null($row['PK_Command_Output']) && $row['PK_Command_Output']!='')
								$otherOutputCommands.="<option value='{$row['PK_Command']}'>".stripslashes($row['Description']).'</option>';
							if(!is_null($row['PK_Command_DSPMode']) && $row['PK_Command_DSPMode']!='')
								$otherDSPModeCommands.="<option value='{$row['PK_Command']}'>".stripslashes($row['Description']).'</option>';
			
						}
					}
					$otherCommands .= '</select>';
					$otherInputCommands .= '</select>';
					$otherOutputCommands .= '</select>';
					$otherDSPModeCommands .= '</select>';

					$out.=$normalCommandsTxt.'<tr><td colspan="2" align="right">Add command to group: <select name="addNewCommandToDeviceCommandGroup">'.$otherCommands.'</td><td><input type="submit" class="button" name="submitX" value="Add"></td></tr>';
					$out.=$inputCommandsTxt.'<tr><td colspan="2" align="right">Add input command to group: <select name="addNewCommandToDeviceCommandGroup">'.$otherInputCommands.'</td><td><input type="submit" class="button" name="submitX" value="Add"></td></tr>';
					$out.=$outputCommandsTxt.'<tr><td colspan="2" align="right">Add output command to group: <select name="addNewCommandToDeviceCommandGroup">'.$otherOutputCommands.'</td><td><input type="submit" class="button" name="submitX" value="Add"></td></tr>';
					$out.=$dspModeCommandsTxt.'<tr><td colspan="2" align="right">Add DSPMode command to group: <select name="addNewCommandToDeviceCommandGroup">'.$otherDSPModeCommands.'</td><td><input type="submit" class="button" name="submitX" value="Add"></td></tr>';
					$out.='
								</table>
							<br />';
					$out.='<br /><a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=addCommand&from=editInfraredGroupFromMasterDevice\',\'width=400,height=300,toolbars=true,resizable=1,scrollbars=1\');">Create new command</a>';
					$out.='<input type="hidden" name="displayedCommands" value="'.(join(",",$commandsDisplayed)).'">
					</fieldset>
					<div align="center"><input type="submit" class="button" name="submitX" value="Update"></div>';
				}
				$out.='
				</td>
			</tr>
		</table>
	';
	$out.='
		</form>
	';	
	} else {
	// process DT	
		$displayedCommands = explode(",",@$_POST['displayedCommands']);
		if (!is_array($displayedCommands)) {$displayedCommands=array();}
		foreach ($displayedCommands as $elem) {
			$elem = (int)$elem;
			$commandAssocDescription = cleanString(@$_POST['CommandDescription_'.$elem]);
			if ($commandAssocDescription!='') {
				//get old value
				$query = "select Description from InfraredGroup_Command where FK_InfraredGroup = ? and FK_Command = ?";
				$resD = $dbADO->Execute($query,array($infraredGroupID,$elem));
				if ($resD && $resD->RecordCount()==1) {
					$rowD = $resD->FetchRow();
					if ($commandAssocDescription!=$rowD['Description']) {
						$lastAction='CommandDescription_'.$elem;
					}
				}
				$query = "UPDATE InfraredGroup_Command SET Description = ? WHERE FK_InfraredGroup = ? AND FK_Command = ?";
				$dbADO->Execute($query,array(stripslashes($commandAssocDescription),$infraredGroupID,$elem));
			}
		}
		
		
		$addNewCommandToThisCommandGroup = (int)$_POST['addNewCommandToDeviceCommandGroup'];
		if ($addNewCommandToThisCommandGroup!=0) {
			$query = "INSERT INTO InfraredGroup_Command (FK_InfraredGroup,FK_Command) values(?,?)";
			$dbADO->Execute($query,array($infraredGroupID,$addNewCommandToThisCommandGroup));			
			$lastAction='CommandDescription_'.$addNewCommandToThisCommandGroup;
		}
		
		header("Location: index.php?section=irCodes&from=$from&deviceID=$deviceID&infraredGroupID=$infraredGroupID&lastAction=".@$lastAction);
}

$output->setBody($out);
$output->setTitle(APPLICATION_NAME.' :: IR Codes');			
$output->output();  

}
?>
