<?php
function editEventHandler($output,$dbADO) {
	/* @var $dbADO ADOConnection */
	$out='';
	$installationID = (int)@$_SESSION['installationID'];
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$lastAction = isset($_REQUEST['lastAction'])?cleanString($_REQUEST['lastAction']):'';
	$eventHandlerID=$_REQUEST['ehID'];

	$queryEventHandler='
		SELECT EventHandler.*,Criteria.FK_CriteriaParmNesting 
		FROM EventHandler
			INNER JOIN CannedEvents ON FK_CannedEvents=PK_CannedEvents
			INNER JOIN Criteria ON FK_Criteria=PK_Criteria
		WHERE EventHandler.FK_Installation=? AND PK_EventHandler=?
	';
	$resEH=$dbADO->Execute($queryEventHandler,array($installationID,$eventHandlerID));
	$rowEH=$resEH->FetchRow();
	$commandGroupID = $rowEH['FK_CommandGroup'];
	$FK_CriteriaParmNesting=$rowEH['FK_CriteriaParmNesting'];

	if(isset($_REQUEST['cgcID']) && (int)$_REQUEST['cgcID']!=0){
		$objID=$_REQUEST['cgcID'];
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$installationID,$dbADO);
		if(!$canModifyInstallation){
			header("Location: index.php?section=editEventHandler&error=You are not authorised to modify installation.");
			exit();
		}
		$deleteObjFromDevice = 'DELETE FROM CommandGroup_Command WHERE PK_CommandGroup_Command = ?';
		$dbADO->Execute($deleteObjFromDevice,$objID);

		$deleteParamValues = 'DELETE FROM CommandGroup_Command_CommandParameter WHERE FK_CommandGroup_Command = ?';
		$dbADO->Execute($deleteParamValues,$objID);
		
		$_REQUEST['msg']='The command was deleted from the command group.';
	}
	
	if ($action=='form') {
		$parmlistToDisplay=(isset($_POST['cannedEvent']) && (int)$_POST['cannedEvent']!=0)?(int)$_POST['cannedEvent']:$rowEH['FK_CannedEvents'];		
		
		$out.='
		<div align="center" class="err">'.@$_REQUEST['error'].'</div>
		<div align="center"><B>'.@$_REQUEST['msg'].'</B></div>
		<form action="index.php" method="post" name="editEventHandler">
		<input type="hidden" name="section" value="editEventHandler">
		<input type="hidden" name="action" value="update">
		<input type="hidden" name="ehID" value="'.$eventHandlerID.'">
		
		<div align="center"><h3>Edit Event Handler</h3></div>
		<table border="0" align="center">
		';
		$out.='
			<tr bgcolor="#E7E7E7">
				<td>Description:</td>
				<td><input type="text" name="description" value="'.$rowEH['Description'].'"></td>
			</tr>';
		$queryCannedEvents='SELECT * FROM CannedEvents ORDER By Description ASC';
		$resCannedEvents=$dbADO->Execute($queryCannedEvents);
		$out.='
			<tr>
				<td>Event:</td>
				<td colspan="2"><select name="cannedEvent" onChange="document.editEventHandler.action.value=\'form\'; document.editEventHandler.submit();">';

		while($rowCE=$resCannedEvents->FetchRow()){
			$out.='<option value="'.$rowCE['PK_CannedEvents'].'" '.(($rowCE['PK_CannedEvents']==$parmlistToDisplay)?'selected':'').'>'.$rowCE['Description'].'</option>';
		}
		$out.='
			</select></td>
			</tr>';
			$displayedCE_CP=array();
			$queryCriteriaParmlist='
				SELECT CannedEvents_CriteriaParmList.Description, CriteriaParmList.Description AS PK_Table, CriteriaParmList.FK_ParameterType,CannedEvents_CriteriaParmList.Operator,PK_CannedEvents_CriteriaParmList,CannedEvents_CriteriaParmList.FK_CriteriaParmList,DefaultValue
				FROM CannedEvents_CriteriaParmList 
					INNER JOIN CriteriaParmList ON CannedEvents_CriteriaParmList.FK_CriteriaParmList=PK_CriteriaParmList 
				WHERE FK_CannedEvents=?';

			$resCriteriaParmlist=$dbADO->Execute($queryCriteriaParmlist,$parmlistToDisplay);
			while($rowCriteria=$resCriteriaParmlist->FetchRow()){
				$queryOldValues='SELECT * FROM CriteriaParm WHERE FK_CannedEvents_CriteriaParmList=? AND FK_CriteriaParmNesting=?';
				$resOldValues=$dbADO->Execute($queryOldValues,array($rowCriteria['PK_CannedEvents_CriteriaParmList'],$FK_CriteriaParmNesting));
				if($resOldValues->RecordCount()>0){
					$rowOldValues=$resOldValues->FetchRow();
					$oldOperator=$rowOldValues['Operator'];
					$oldValue=$rowOldValues['Value'];
				}
				$displayedCE_CP[]=$rowCriteria['PK_CannedEvents_CriteriaParmList'];
				$selectedOperator=(!isset($oldOperator))?$rowCriteria['Operator']:@$oldOperator;
				$selectedValue=(!isset($oldValue))?$rowCriteria['DefaultValue']:@$oldValue;
				$out.='<input type="hidden" name="CriteriaParmList_'.$rowCriteria['PK_CannedEvents_CriteriaParmList'].'" value="'.$rowCriteria['FK_CriteriaParmList'].'">';
				$out.='
				<tr>
					<td>'.$rowCriteria['Description'].'</td>
					<td><select name="CriteriaParmOperator_'.$rowCriteria['PK_CannedEvents_CriteriaParmList'].'">
						<option value="1" '.(($selectedOperator==1)?'selected':'').'>=</option>
						<option value="2" '.(($selectedOperator==2)?'selected':'').'>&lt;&gt;</option>
					</select> ';
				
				if(substr($rowCriteria['PK_Table'],0,3)=='PK_'){
					$out.='<select name="CriteriaParmValue_'.$rowCriteria['PK_CannedEvents_CriteriaParmList'].'">
						<option value="0" ></option>';
					$tableName=substr($rowCriteria['PK_Table'],3);
					$filterTable=($tableName=='Device' || $tableName=='Room')?" WHERE FK_Installation='".$installationID."' ":'';
					$queryTable="SELECT * FROM $tableName $filterTable ORDER BY Description ASC";
					$resTable=$dbADO->Execute($queryTable);
					while($rowTable=$resTable->FetchRow()){
						$out.='<option value="'.$rowTable[$rowCriteria['PK_Table']].'" '.(($selectedValue==$rowTable[$rowCriteria['PK_Table']])?'selected':'').'>'.$rowTable['Description'].'</option>';
					}
					$out.='</select>';
				}elseif($rowCriteria['PK_Table']=='State'){
					$stateArray=array(5=>'Afternoon',1=>'Daylight',6=>'Evening',3=>'Morning',7=>'Night',2=>'Not Daylight',9=>'Weekday',8=>'Weekend');
					$out.='<select name="CriteriaParmValue_'.$rowCriteria['PK_CannedEvents_CriteriaParmList'].'">
								<option value=""></option>';
						foreach($stateArray AS $key => $value){
							$out.='<option value="'.$key.'" '.(($selectedValue==$key)?'selected':'').'>'.$value.'</option>';
						}
					$out.='			
					</select>';
				}
				else{
					$out.='<input type="text" name="CriteriaParmValue_'.$rowCriteria['PK_CannedEvents_CriteriaParmList'].'" value="">';
				}
				$out.='
					</td>
				</tr>				
				';
			}
			$out.='
			<input type="hidden" name="displayedCE_CP" value="'.join(',',$displayedCE_CP).'">
			<tr>
				<td colspan="3" align="center"><input type="submit" name="continue" value="Update"></td>
			</tr>';

			$selectCommandsAssigned = "
				SELECT CommandGroup_Command.* FROM 
					CommandGroup_Command				
					INNER JOIN 
						Device ON CommandGroup_Command.FK_Device = Device.PK_Device						
					WHERE 
						Device.FK_Installation = ?
					AND CommandGroup_Command.FK_CommandGroup = ?
				";
			$resCommandAssigned = $dbADO->Execute($selectCommandsAssigned,array($installationID,$commandGroupID));

			if ($resCommandAssigned->RecordCount()>0) {
				$out.='<tr>
					<td colspan="2">
							<fieldset>
								<legend>Commands</legend>
								<table border="0">
									';

				while ($rowCommandAssigned = $resCommandAssigned->FetchRow()) {
					$out.='
						<tr><td valign="top">
							<select name="device_'.$rowCommandAssigned['PK_CommandGroup_Command'].'" onChange="this.form.submit();">
								<option value="0">-please select-</option>
						';

					$query = 'SELECT * From Device WHERE FK_Installation = ? order by Description ASC';
					$resDevices=$dbADO->Execute($query,array($installationID));

					if ($resDevices) {
						while ($rowDevice = $resDevices->FetchRow()) {
							$out.='<option '.($rowDevice['PK_Device']==$rowCommandAssigned['FK_Device']?' selected="selected" ':'').' value="'.$rowDevice['PK_Device'].'">'.$rowDevice['Description'].'</option>';
						}
					}

					$out.='
					 		</select></td>
						<td>
						';

					$query = "SELECT PK_Command,Command.Description
												FROM Device 
													INNER JOIN DeviceTemplate_DeviceCommandGroup ON Device.FK_DeviceTemplate = DeviceTemplate_DeviceCommandGroup.FK_DeviceTemplate 
													INNER JOIN DeviceCommandGroup_Command ON DeviceTemplate_DeviceCommandGroup.FK_DeviceCommandGroup = DeviceCommandGroup_Command.FK_DeviceCommandGroup 
													INNER JOIN Command on DeviceCommandGroup_Command.FK_Command = Command.PK_Command
										   WHERE PK_Device=?";
					$resNewCommand = $dbADO->Execute($query,array($rowCommandAssigned['FK_Device']));
					if ($resNewCommand) {
						$out.='<select name="deviceCommand_'.$rowCommandAssigned['PK_CommandGroup_Command'].'" onChange="this.form.submit();">
								<option value="0">-please select-</option>';
						while ($rowNewCommand = $resNewCommand->FetchRow()) {
							$out.='<option '.($rowCommandAssigned['FK_Command']==$rowNewCommand['PK_Command']?'selected="selected"':'').' value="'.$rowNewCommand['PK_Command'].'">'.$rowNewCommand['Description'].'</option>';
						}
						if ($resNewCommand->RecordCount()==0) {
							$out.='<option value="0">-no command-</option>';
						}
						$out.='</select>';
					}


					$query = "SELECT Command_CommandParameter.FK_CommandParameter,Command_CommandParameter.Description as C_CP_Description,CommandParameter.Description as CP_Description,
										CommandGroup_Command_CommandParameter.IK_CommandParameter,
										ParameterType.Description as PT_Description
											FROM Command_CommandParameter 
												INNER JOIN Command on FK_Command = PK_Command
												INNER JOIN CommandParameter ON Command_CommandParameter.FK_CommandParameter = CommandParameter.PK_CommandParameter
												INNER JOIN ParameterType ON CommandParameter.FK_ParameterType = ParameterType.PK_ParameterType
												LEFT JOIN CommandGroup_Command_CommandParameter ON CommandGroup_Command_CommandParameter.FK_CommandParameter = Command_CommandParameter.FK_CommandParameter
									  WHERE FK_Command = ? AND FK_CommandGroup_Command =?
									  ORDER BY CommandParameter.Description asc
								";
					$resSelectParameters = $dbADO->Execute($query,array($rowCommandAssigned['FK_Command'],$rowCommandAssigned['PK_CommandGroup_Command']));
					if ($resSelectParameters) {
						$out.='<table border="0">';
						while ($rowSelectParameters=$resSelectParameters->FetchRow()) {
							$out.="<tr ".(strlen(trim($rowSelectParameters['CP_Description']))==0?" bgColor='green' ":"").">
												<td>#{$rowSelectParameters['FK_CommandParameter']} <span title=\"{$rowSelectParameters['C_CP_Description']}\">{$rowSelectParameters['CP_Description']}</span> ({$rowSelectParameters['PT_Description']})</td>
												<td><input type='text' name=\"CommandParameterValue_{$rowCommandAssigned['PK_CommandGroup_Command']}_{$rowSelectParameters['FK_CommandParameter']}\" value=\"{$rowSelectParameters['IK_CommandParameter']}\" >".'</td></tr>';
						}
						$out.='</table>';
					}
					$out.='
					
							</td>
						<td valign="top">
						<input type="button" name="editA" value="Remove" onClick="self.location=\'index.php?section=editEventHandler&ehID='.$eventHandlerID.'&cgcID='.$rowCommandAssigned['PK_CommandGroup_Command'].'\'">
						</td>						
					</tr>
					';
				}
				$out.='
				<tr>
					<td colspan="3" align="center"><input type="submit" name="addNewDeviceButton" value="Save changes"></td>
				</tr>
				</table>
				</fieldset>
			</td>
		</tr>';				
			}

			
			$out.='
				<tr>
					<td><B>Device</B>:</td>
					<td colspan="2"><select name="device" onChange="document.editEventHandler.action.value=\'form\';document.editEventHandler.submit()">
							<option value="">Select a device</option>';
				$queryDevice='
					SELECT Device.*,DeviceTemplate.Description AS Template FROM Device
						INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
					WHERE FK_Installation=?';
				$resDevice=$dbADO->Execute($queryDevice,$installationID);
				while($rowDevice=$resDevice->FetchRow()){
					$out.='<option value="'.$rowDevice['PK_Device'].'" '.(($rowDevice['PK_Device']==@$_REQUEST['device'])?'selected':'').'>('.$rowDevice['Template'].') '.$rowDevice['Description'].'</option>';
				}
			$out.='
					</select></td>
				</tr>';
		
			if(isset($_REQUEST['device']) && $_REQUEST['device']!=''){
				$query = "
					SELECT PK_Command,Command.Description
						FROM Device 
					INNER JOIN DeviceTemplate_DeviceCommandGroup ON Device.FK_DeviceTemplate = DeviceTemplate_DeviceCommandGroup.FK_DeviceTemplate 
						INNER JOIN DeviceCommandGroup_Command ON DeviceTemplate_DeviceCommandGroup.FK_DeviceCommandGroup = DeviceCommandGroup_Command.FK_DeviceCommandGroup 
						INNER JOIN Command on DeviceCommandGroup_Command.FK_Command = Command.PK_Command
					WHERE PK_Device=?";
				$resNewCommand = $dbADO->Execute($query,array(cleanInteger($_REQUEST['device'])));
				if ($resNewCommand) {
					$out.='
				<tr>
					<td>Command: </td>
					<td><select name="addNewDeviceCommand" onChange="document.editEventHandler.submit()">
						<option value="0">-please select-</option>';
							while ($rowNewCommand = $resNewCommand->FetchRow()) {
								$out.='<option value="'.$rowNewCommand['PK_Command'].'">'.$rowNewCommand['Description'].'</option>';
							}
							if ($resNewCommand->RecordCount()==0) {
								$out.='<option value="0">-no command-</option>';
							}
					$out.='</select> <input type="submit" name="submitX" value="Add">
					</td>
				</tr>';
				}
			}

		$out.='
		</table>
		</form>
		';
		
	} else {
		// processing area
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$installationID,$dbADO);
		if(!$canModifyInstallation){
			header("Location: index.php?section=editEventHandler&ehID=".$eventHandlerID."&error=You are not authorised to modify installation.");
			exit();
		}
		$description=$_POST['description'];
		$cannedEvent=(int)$_POST['cannedEvent'];
		$displayedCE_CPArray=explode(',',$_POST['displayedCE_CP']);
		
		foreach($displayedCE_CPArray AS $elem){
			$operator=@$_POST['CriteriaParmOperator_'.$elem];
			$value=@$_POST['CriteriaParmValue_'.$elem];
			$criteriaParmList=@$_POST['CriteriaParmList_'.$elem];
			
			$queryOldValues='SELECT * FROM CriteriaParm WHERE FK_CannedEvents_CriteriaParmList=? AND FK_CriteriaParmNesting=?';
			$resOldValues=$dbADO->Execute($queryOldValues,array($elem,$FK_CriteriaParmNesting));
			if($resOldValues->RecordCount()==0){
				// insert new values
				$insertCriteriaParm='
					INSERT INTO CriteriaParm 
						(FK_CriteriaParmNesting,FK_CriteriaParmList,Operator,Value,FK_CannedEvents_CriteriaParmList)
					VALUES
						(?,?,?,?,?)';
				$dbADO->Execute($insertCriteriaParm,array($FK_CriteriaParmNesting,$criteriaParmList,$operator,$value,$elem));
			}else{
				$rowOldValues=$resOldValues->FetchRow();
				$oldCriteriaParmList=$rowOldValues['FK_CriteriaParmList'];
				$oldOperator=$rowOldValues['Operator'];
				$oldValue=$rowOldValues['Value'];
				// update if changed
				if($criteriaParmList!=$oldCriteriaParmList || $operator!=$oldOperator || $value!=$oldValue){
					$updateCriteriaParm='
						UPDATE CriteriaParm 
							SET FK_CriteriaParmNesting=?, FK_CriteriaParmList=?, Operator=?, Value=? 
						WHERE PK_CriteriaParm=?';
					$dbADO->Execute($updateCriteriaParm,array($FK_CriteriaParmNesting,$criteriaParmList,$operator,$value,$rowOldValues['PK_CriteriaParm']));
				}
			}			
		}
		
		// command group process
		$x=cleanInteger(@$_POST['device']);
		$y=cleanInteger(@$_POST['addNewDeviceCommand']);
		if ($y!=0 && $x!=0) {
			$queryInsertCommandGroup_Command = "INSERT INTO CommandGroup_Command
				(FK_CommandGroup,FK_Command,FK_Device) values(?,?,?)";			
			$insertRs = $dbADO->Execute($queryInsertCommandGroup_Command,array($commandGroupID,$y,$x));
		}

		$selectCommandsAssigned = "
				SELECT CommandGroup_Command.* FROM 
					CommandGroup_Command				
					INNER JOIN 
						Device ON CommandGroup_Command.FK_Device = Device.PK_Device						
					WHERE 
						Device.FK_Installation = ?
				";								
		$resCommandAssigned = $dbADO->Execute($selectCommandsAssigned,array($installationID));

		$parametersUpdatedAlert = 'Parameters not updated!';

		if ($resCommandAssigned) {
			while ($rowCommandAssigned = $resCommandAssigned->FetchRow()) {

				$deviceSelected = isset($_POST['device_'.$rowCommandAssigned['PK_CommandGroup_Command']])?$_POST['device_'.$rowCommandAssigned['PK_CommandGroup_Command']]:$x;
				$commandSelected = isset($_POST['deviceCommand_'.$rowCommandAssigned['PK_CommandGroup_Command']])?$_POST['deviceCommand_'.$rowCommandAssigned['PK_CommandGroup_Command']]:$y;

				$updateCommandGroup_Command = 'UPDATE CommandGroup_Command SET FK_Device = ? WHERE PK_CommandGroup_Command = ? ';
				$resUpdateCommandGroup_Command  = $dbADO->Execute($updateCommandGroup_Command,array($deviceSelected,$rowCommandAssigned['PK_CommandGroup_Command']));

				if ($dbADO->Affected_Rows()==1) {//enter here only if the Device is changed
				$updateCommandGroup_Command = 'UPDATE CommandGroup_Command SET FK_Command = NULL WHERE PK_CommandGroup_Command = ? ';
				$resUpdateCommandGroup_Command  = $dbADO->Execute($updateCommandGroup_Command,array($rowCommandAssigned['PK_CommandGroup_Command']));
				//delete old parameters
				$deleteParamValues = 'DELETE FROM CommandGroup_Command_CommandParameter WHERE
									FK_CommandGroup_Command = ? 
						';
				$query = $dbADO->Execute($deleteParamValues,array($rowCommandAssigned['PK_CommandGroup_Command']));

				} else {
					$updateCommandGroup_Command = 'UPDATE CommandGroup_Command SET FK_Command = ? WHERE PK_CommandGroup_Command = ? ';
					$resUpdateCommandGroup_Command  = $dbADO->Execute($updateCommandGroup_Command,array($commandSelected,$rowCommandAssigned['PK_CommandGroup_Command']));
					if ($dbADO->Affected_Rows()==1) {//if we have changed the command, delete old values
					$deleteParamValues = 'DELETE FROM CommandGroup_Command_CommandParameter WHERE
									FK_CommandGroup_Command = ? 
							';
					$query = $dbADO->Execute($deleteParamValues,array($rowCommandAssigned['PK_CommandGroup_Command']));
					}
				}

				$query = "
					SELECT Command_CommandParameter.FK_CommandParameter,Command_CommandParameter.Description as C_CP_Description,CommandParameter.Description as CP_Description,
						CommandGroup_Command_CommandParameter.IK_CommandParameter,
						ParameterType.Description as PT_Description
					FROM Command_CommandParameter 
						INNER JOIN Command on FK_Command = PK_Command
						INNER JOIN CommandParameter ON Command_CommandParameter.FK_CommandParameter = CommandParameter.PK_CommandParameter
						INNER JOIN ParameterType ON CommandParameter.FK_ParameterType = ParameterType.PK_ParameterType
						LEFT JOIN CommandGroup_Command_CommandParameter ON CommandGroup_Command_CommandParameter.FK_CommandParameter = Command_CommandParameter.FK_CommandParameter
					WHERE FK_Command = ?
								";

				$resSelectParameters = $dbADO->Execute($query,array($commandSelected));

				if ($resSelectParameters) {
					while ($rowSelectParameters=$resSelectParameters->fetchRow()) {
						$value=cleanString(@$_POST['CommandParameterValue_'.$rowCommandAssigned['PK_CommandGroup_Command'].'_'.$rowSelectParameters['FK_CommandParameter']]);
						//see if we need to update or to insert
						$checkExists = "SELECT * FROM CommandGroup_Command_CommandParameter WHERE FK_CommandGroup_Command = ? AND  FK_CommandParameter = ?";
						$resExists=$dbADO->Execute($checkExists,array($rowCommandAssigned['PK_CommandGroup_Command'],$rowSelectParameters['FK_CommandParameter']));
						if ($resExists->RecordCount()==1) {
							$update = "UPDATE CommandGroup_Command_CommandParameter  SET  IK_CommandParameter  = ? WHERE FK_CommandGroup_Command = ? AND  FK_CommandParameter =? ";
							$dbADO->Execute($update,array($value,$rowCommandAssigned['PK_CommandGroup_Command'],$rowSelectParameters['FK_CommandParameter']));
						} else {
							$insert = "INSERT INTO CommandGroup_Command_CommandParameter (IK_CommandParameter,FK_CommandGroup_Command,FK_CommandParameter) values(?,?,?)";
							$dbADO->Execute($insert,array($value,$rowCommandAssigned['PK_CommandGroup_Command'],$rowSelectParameters['FK_CommandParameter']));
						}
						if ($dbADO->Affected_Rows()==1) {
							$parametersUpdatedAlert='Parameters updated!';
						}

					}
				}
			}
		}

		
		// update EventHandler
		$updateEventHandler='UPDATE EventHandler SET Description=?, FK_CannedEvents=? WHERE PK_EventHandler=?';
		$dbADO->Execute($updateEventHandler,array($description,$cannedEvent,$eventHandlerID));
		
		
		header('Location: index.php?section=editEventHandler&ehID='.$eventHandlerID.'&msg=The event handler was updated');
	}
	
	$output->setNavigationMenu(array("Events Handler"=>'index.php?section=eventsHandler'));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();
}
?>