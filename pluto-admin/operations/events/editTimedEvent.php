<?php
function editTimedEvent($output,$dbADO) {
	/* @var $dbADO ADOConnection */
	$out='';
	$installationID = (int)@$_SESSION['installationID'];
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$lastAction = isset($_REQUEST['lastAction'])?cleanString($_REQUEST['lastAction']):'';
	$eventHandlerID=$_REQUEST['ehID'];
	$dayNamesArray= array(1=>"Monday", 2=>"Tuesday", 3=>"Wensday", 4=>"Thursday", 5=>"Friday", 6=>"Saturday", 7=>"Sunday");
	$monthNamesArray = array(1=>"January", 2=>"February", 3=>"March", 4=>"April", 5=>"May", 6=>"June", 7=>"July", 8=>"August", 9=>"September", 10=>"October", 11=>"November", 12=>"December"); 	
	
	$queryEventHandler='
		SELECT EventHandler.*,Criteria.FK_CriteriaParmNesting 
		FROM EventHandler
			INNER JOIN Criteria ON FK_Criteria=PK_Criteria
		WHERE EventHandler.FK_Installation=? AND PK_EventHandler=?
	';
	$resEH=$dbADO->Execute($queryEventHandler,array($installationID,$eventHandlerID));
	$rowEH=$resEH->FetchRow();
	$commandGroupID = $rowEH['FK_CommandGroup'];
	$FK_CriteriaParmNesting=$rowEH['FK_CriteriaParmNesting'];
	$timedEventType=$rowEH['TimedEvent'];

	if(isset($_REQUEST['cgcID']) && (int)$_REQUEST['cgcID']!=0){
		$objID=$_REQUEST['cgcID'];
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$installationID,$dbADO);
		if(!$canModifyInstallation){
			header("Location: index.php?section=editTimedEvent&error=You are not authorised to modify installation.");
			exit();
		}
		$deleteObjFromDevice = 'DELETE FROM CommandGroup_Command WHERE PK_CommandGroup_Command = ?';
		$dbADO->Execute($deleteObjFromDevice,$objID);

		$deleteParamValues = 'DELETE FROM CommandGroup_Command_CommandParameter WHERE FK_CommandGroup_Command = ?';
		$dbADO->Execute($deleteParamValues,$objID);
		
		$_REQUEST['msg']='The command was deleted from the command group.';
	}
	
	if ($action=='form') {
		
		$out.='
		<script>
		function showOptions(val)
		{
			for (i=1;i<5;i++){
				eval("document.getElementById(\'type_"+i+"\').style.display=\'none\';");
			}
			eval("document.getElementById(\'type_"+val+"\').style.display=\'\';");
		}
		</script>
		<div align="center" class="err">'.@$_REQUEST['error'].'</div>
		<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
		<form action="index.php" method="post" name="editTimedEvent">
		<input type="hidden" name="section" value="editTimedEvent">
		<input type="hidden" name="action" value="update">
		<input type="hidden" name="ehID" value="'.$eventHandlerID.'">
		
		<div align="center"><h3>Edit Timed</h3></div>
		<table border="0" align="center" width="700">
		';
		$out.='
			<tr>
				<td><input type="radio" name="timedEventType" value="1" '.(($timedEventType==1)?'checked':'').' onClick="showOptions(1);"> <B>Interval based:</B></td>
				<td colspan="2">Do something every x minutes, or every other hour</td>
			</tr>	
			<tr>
				<td><input type="radio" name="timedEventType" value="2" '.(($timedEventType==2)?'checked':'').' onClick="showOptions(2);"> <B>Day of week based:</B></td>
				<td colspan="2">Do something at 7:00 and 9:00 on Monday, Wednesday and Friday</td>
			</tr>		
			<tr>
				<td><input type="radio" name="timedEventType" value="3" '.(($timedEventType==3)?'checked':'').' onClick="showOptions(3);"> <B>Day of month based:</B></td>
				<td colspan="2">Do something at 8:00 on the 1st and 15th of each month</td>
			</tr>		
			<tr>
				<td><input type="radio" name="timedEventType" value="4" '.(($timedEventType==4)?'checked':'').' onClick="showOptions(4);"> <B>Absolute:</B></td>
				<td colspan="2">Do something on 5 Mar 2005 at 11:15</td>
			</tr>
		
			<tr bgcolor="#E7E7E7">
				<td><B>Description:</B></td>
				<td><input type="text" name="description" value="'.$rowEH['Description'].'"></td>
			</tr>
			<tr>
				<td colspan="2">';
		switch($timedEventType){
			case 1:
				$resOldValue=$dbADO->Execute('SELECT * FROM CriteriaParm WHERE FK_CriteriaParmNesting=? AND FK_CriteriaParmList=?',array($FK_CriteriaParmNesting,$GLOBALS['TimeOfDayParmList']));
				if($resOldValue->RecordCount()!=0){
					$rowOldValues=$resOldValue->FetchRow();
					$oldIntervalNumber=(ereg('h',$rowOldValues['Value']))?str_replace('h','',$rowOldValues['Value']):$rowOldValues['Value'];
					$oldIntervalNumber=(ereg('m',$oldIntervalNumber))?str_replace('m','',$oldIntervalNumber):$oldIntervalNumber;
					$oldIntervalType=(ereg('h',$rowOldValues['Value']))?1:((ereg('m',$rowOldValues['Value']))?2:3);	// 3 is error case
				}

				$out.='<input type="hidden" name="oldInterval" value="'.$rowOldValues['Value'].'">';
			break;
			case 2:
				$resOldValue=$dbADO->Execute('SELECT * FROM CriteriaParm WHERE FK_CriteriaParmNesting=? AND FK_CriteriaParmList=?',array($FK_CriteriaParmNesting,$GLOBALS['DayOfWeekParmList']));
				if($resOldValue->RecordCount()!=0){
					$rowOldValues=$resOldValue->FetchRow();
					$checkedArray=explode(',',$rowOldValues['Value']);
				}
				$resOldTime=$dbADO->Execute('SELECT * FROM CriteriaParm WHERE FK_CriteriaParmNesting=? AND FK_CriteriaParmList=?',array($FK_CriteriaParmNesting,$GLOBALS['TimeOfDayParmList']));
				if($resOldTime){
					$rowOldTime=$resOldTime->FetchRow();
				}
				$oldDayOfWeek=@$rowOldValues['Value'].@$rowOldTime['Value'];
				$out.='<input type="hidden" name="oldDayOfWeek" value="'.@$oldDayOfWeek.'">';
			break;
			case 3:
				$resOldMonthValue=$dbADO->Execute('SELECT * FROM CriteriaParm WHERE FK_CriteriaParmNesting=? AND FK_CriteriaParmList=?',array($FK_CriteriaParmNesting,$GLOBALS['DayOfMonthParmList']));
				if($resOldMonthValue->RecordCount()!=0){
					$rowOldMonthValues=$resOldMonthValue->FetchRow();
				}
				$resOldMonthTime=$dbADO->Execute('SELECT * FROM CriteriaParm WHERE FK_CriteriaParmNesting=? AND FK_CriteriaParmList=?',array($FK_CriteriaParmNesting,$GLOBALS['TimeOfDayParmList']));
				if($resOldMonthTime){
					$rowOldMonthTime=$resOldMonthTime->FetchRow();
				}
				$oldDayOfMonth=@$rowOldMonthValues['Value'].@$rowOldMonthTime['Value'];

				$out.='<input type="hidden" name="oldDayOfMonth" value="'.@$oldDayOfMonth.'">';
			break;
			case 4:
				$resOldValue=$dbADO->Execute('SELECT * FROM CriteriaParm WHERE FK_CriteriaParmNesting=? AND FK_CriteriaParmList=?',array($FK_CriteriaParmNesting,$GLOBALS['SpecificDateParmList']));
				if($resOldValue->RecordCount()!=0){
					$rowOldValues=$resOldValue->FetchRow();
					$oldAbsolute=$rowOldValues['Value'];
					$dataParts=explode(' ',$rowOldValues['Value']);
					echo $rowOldValues['PK_CriteriaParm'];
				}
				$out.='<input type="hidden" name="oldAbsolute" value="'.@$oldAbsolute.'">';
			break;
		}

		$out.='<span id="type_1" style="display:'.(($timedEventType!=1)?'none':'').';">Every <input type="text" name="intervalNumber" value="'.@$oldIntervalNumber.'" size="2"> <select name="intervalType"><option value="1" '.((@$oldIntervalType==1)?'selected':'').'>Hours</option><option value="2" '.((@$oldIntervalType==2)?'selected':'').'>Minutes</option></select></span>';
		$out.='<span id="type_2" style="display:'.(($timedEventType!=2)?'none':'').';">';
		$out.='What days of the week: ';
			foreach ($dayNamesArray AS $key=>$dayName){
				$out.='<input type="checkbox" name="dayOfWeek_'.$key.'" '.((is_array(@$checkedArray) && in_array($key,$checkedArray))?'checked':'').'> '.$dayName;
			}
		$out.='<br>Time: <input type="text" name="dayOfWeekTime" value="'.((isset($rowOldTime['Value']))?$rowOldTime['Value']:date('h:i')).'"> (comma separated list for multiple values)</span>';
		$out.='<span id="type_3" style="display:'.(($timedEventType!=3)?'none':'').';">What day of the month (e.g. 2,7)? <input type="text" name="dayOfMonths" value="'.((isset($rowOldMonthValues['Value']))?$rowOldMonthValues['Value']:'').'"> Time: (e.g. 10:00,21:30)<input type="text" name="dayOfMonthTime" value="'.((isset($rowOldMonthTime['Value']))?$rowOldMonthTime['Value']:date('h:i')).'"></span>';
		$out.='<span id="type_4" style="display:'.(($timedEventType!=4)?'none':'').';">What date day/month/year? <input type="text" name="absoluteDate" size="7" value="'.((isset($dataParts[0]))?formatMySQLDate($dataParts[0],'d/m/Y'):date('d/m/Y')).'">  What time? <input type="text" name="absoluteTime" value="'.((isset($dataParts[1]))?$dataParts[1]:date('h:i')).'"> (comma separated list for multiple values)</span>';
		$out.='</td>
			</tr>		
			';
			$out.='
			<tr bgcolor="#E7E7E7">
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
						<input type="button" name="editA" value="Remove" onClick="self.location=\'index.php?section=editTimedEvent&ehID='.$eventHandlerID.'&cgcID='.$rowCommandAssigned['PK_CommandGroup_Command'].'\'">
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
					<td colspan="2"><select name="device" onChange="document.editTimedEvent.action.value=\'form\';document.editTimedEvent.submit()">
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
					<td><select name="addNewDeviceCommand" onChange="document.editTimedEvent.submit()">
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
			header("Location: index.php?section=editTimedEvent&ehID=".$eventHandlerID."&error=You are not authorised to modify installation.");
			exit();
		}
		$description=$_POST['description'];
		$timedEventType=(int)$_POST['timedEventType'];
		
		if(isset($_POST['continue'])){
			$updateEventHandler='UPDATE EventHandler SET Description=?, TimedEvent=? WHERE PK_EventHandler=?';
			$dbADO->Execute($updateEventHandler,array($description,$timedEventType,$eventHandlerID));
			
			switch($timedEventType){
				case 1:
					$oldInterval=@$_POST['oldInterval'];
					$intervalNumber=isset($_POST['intervalNumber'])?$_POST['intervalNumber']:0;
					$intervalType=(@$_POST['intervalType']==1)?'h':'m';
					$newInterval=$intervalNumber.$intervalType;
					if($oldInterval!=$newInterval){
						$dbADO->Execute('DELETE FROM CriteriaParm WHERE FK_CriteriaParmNesting=? AND FK_CriteriaParmList IN ('.$GLOBALS['TimeOfDayParmList'].','.$GLOBALS['DayOfWeekParmList'].','.$GLOBALS['DayOfMonthParmList'].','.$GLOBALS['SpecificDateParmList'].')',$FK_CriteriaParmNesting);
						$insertCriteriaParm='
							INSERT INTO CriteriaParm 
								(FK_CriteriaParmNesting,FK_CriteriaParmList,Operator,Value)
							VALUES
								(?,?,?,?)';
						$dbADO->Execute($insertCriteriaParm,array($FK_CriteriaParmNesting,$GLOBALS['TimeOfDayParmList'],1,$newInterval));
					}
	
				break;
				case 2:
					$newDayOfWeekArray=array();
					foreach ($dayNamesArray AS $key=>$dayName){
						if(isset($_POST['dayOfWeek_'.$key]))
							$newDayOfWeekArray[]=$key;
					}
					$dayOfWeekTime=isset($_POST['dayOfWeekTime'])?$_POST['dayOfWeekTime']:date('h:i');
					$oldDayOfWeek=@$_POST['oldDayOfWeek'];
					$newDayOfWeek=join(',',$newDayOfWeekArray).$dayOfWeekTime;
					if($newDayOfWeek!=$oldDayOfWeek){
						$dbADO->Execute('DELETE FROM CriteriaParm WHERE FK_CriteriaParmNesting=? AND FK_CriteriaParmList IN ('.$GLOBALS['TimeOfDayParmList'].','.$GLOBALS['DayOfWeekParmList'].','.$GLOBALS['DayOfMonthParmList'].','.$GLOBALS['SpecificDateParmList'].')',$FK_CriteriaParmNesting);
						$insertCriteriaParm='
							INSERT INTO CriteriaParm 
								(FK_CriteriaParmNesting,FK_CriteriaParmList,Operator,Value)
							VALUES
								(?,?,?,?)';
						$dbADO->Execute($insertCriteriaParm,array($FK_CriteriaParmNesting,$GLOBALS['DayOfWeekParmList'],1,join(',',$newDayOfWeekArray)));
						$dbADO->Execute($insertCriteriaParm,array($FK_CriteriaParmNesting,$GLOBALS['TimeOfDayParmList'],1,$dayOfWeekTime));
					}

				break;
				case 3:
					$dayOfMonths=@$_POST['dayOfMonths'];
					$dayOfMonthTime=isset($_POST['dayOfMonthTime'])?$_POST['dayOfMonthTime']:date('h:i');
					$oldDayOfMonth=@$_POST['oldDayOfMonth'];
					$newDayOfMonth=$dayOfMonths.$dayOfMonthTime;
					if($newDayOfMonth!=$oldDayOfMonth){
						$dbADO->Execute('DELETE FROM CriteriaParm WHERE FK_CriteriaParmNesting=? AND FK_CriteriaParmList IN ('.$GLOBALS['TimeOfDayParmList'].','.$GLOBALS['DayOfWeekParmList'].','.$GLOBALS['DayOfMonthParmList'].','.$GLOBALS['SpecificDateParmList'].')',$FK_CriteriaParmNesting);
						$insertCriteriaParm='
							INSERT INTO CriteriaParm 
								(FK_CriteriaParmNesting,FK_CriteriaParmList,Operator,Value)
							VALUES
								(?,?,?,?)';
							if($dayOfMonths!=''){
								$dbADO->Execute($insertCriteriaParm,array($FK_CriteriaParmNesting,$GLOBALS['DayOfMonthParmList'],1,$dayOfMonths));
								$dbADO->Execute($insertCriteriaParm,array($FK_CriteriaParmNesting,$GLOBALS['TimeOfDayParmList'],1,$dayOfMonthTime));
							}
					}
				break;
				case 4:
					$oldAbsolute=@$_POST['oldAbsolute'];
					$dateArray=explode('/',isset($_POST['absoluteDate'])?$_POST['absoluteDate']:date('d/m/Y'));
					$absoluteDate=@$dateArray[2].'-'.@$dateArray[1].'-'.@$dateArray[0];
					$absoluteTime=isset($_POST['absoluteTime'])?$_POST['absoluteTime']:date('h:i');
					$newAbsolute=$absoluteDate.' '.$absoluteTime;
					if($newAbsolute!=$oldAbsolute){
						$dbADO->Execute('DELETE FROM CriteriaParm WHERE FK_CriteriaParmNesting=? AND FK_CriteriaParmList IN ('.$GLOBALS['TimeOfDayParmList'].','.$GLOBALS['DayOfWeekParmList'].','.$GLOBALS['DayOfMonthParmList'].','.$GLOBALS['SpecificDateParmList'].')',$FK_CriteriaParmNesting);
						$insertCriteriaParm='
							INSERT INTO CriteriaParm 
								(FK_CriteriaParmNesting,FK_CriteriaParmList,Operator,Value)
							VALUES
								(?,?,?,?)';
						$dbADO->Execute($insertCriteriaParm,array($FK_CriteriaParmNesting,$GLOBALS['SpecificDateParmList'],1,$newAbsolute));
					}
					
				break;
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
		$updateEventHandler='UPDATE EventHandler SET Description=?  WHERE PK_EventHandler=?';
		$dbADO->Execute($updateEventHandler,array($description,$eventHandlerID));
		
		
		header('Location: index.php?section=editTimedEvent&ehID='.$eventHandlerID.'&msg=The event handler was updated');
	}
	
	$output->setNavigationMenu(array("Timed Events"=>'index.php?section=timedEvents'));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();
}
?>