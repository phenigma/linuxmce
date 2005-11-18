<?
function tunerConfig($output,$dbADO) {
	global $dbPlutoMainDatabase;
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID=(int)$_SESSION['installationID'];
	$dtID=$_REQUEST['dtID'];
	$categoryID=$_REQUEST['categoryID'];


	if ($action == 'form') {
		$out.='
	<div class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
	<form action="index.php" method="POST" name="tunerConfig">
	<input type="hidden" name="section" value="tunerConfig">
	<input type="hidden" name="action" value="add">	
	<input type="hidden" name="dtID" value="'.$dtID.'">	
	<input type="hidden" name="categoryID" value="'.$categoryID.'">	
		
	<div align="center"><B>Tuner(s) setup</B></div>	
	<br><br>';
	if($categoryID!=$GLOBALS['AmpsReceivers']){
		$out.='
	<table>
		<tr>
			<td align="center" colspan="2" class="err"><B>WARNING: if you close this window without setting any tuner you won\'t be able to use the TV.</B></td>
		</tr>	
		<tr>
			<td align="right"><B>How many Tuners does this have?</B></td>
			<td><select name="tunerNo">
				<option value="1">1</option>
				<option value="2">2</option>
				<option value="3">3</option>
			</select>
		</td>
		</tr>	
		<tr>
			<td colspan="2">By tuner we mean an input for an antenna or RF cable input where there is an internal tuner that picks the channel/frequency. We\'re not asking about inputs, like composite, svideo, a/v, etc.--just tuners. Most TV\'s have only 1 tuners--in other words there is just 1 antenna input. But some high-end TV\'s have 2 or more tuners. If you\'re not sure, just leave it at 1</td>
		</tr>	
		<tr>
			<td colspan="2" align="center"><input type="submit" class="button" name="save" value="Save"></td>
		</tr>		
	</table>';
	}else{
	$out.='	<table>
		<tr>
			<td align="center" colspan="2" class="err"><B>WARNING: if you close this window without setting any tuner you won\'t be able to use the TV.</B></td>
		</tr>	
		<tr>
			<td align="center" colspan="2"><B>Tuner:</B> <input type="checkbox" name="command[]" value="'.$GLOBALS['AM_Command'].'" checked> AM <input type="checkbox" name="command[]" value="'.$GLOBALS['FM_Command'].'" checked> FM <input type="checkbox" name="command[]" value="'.$GLOBALS['WB_Command'].'"> WB <input type="checkbox" name="command[]" value="'.$GLOBALS['SW_Command'].'"> SW</td>
		</tr>	
		<tr>
			<td colspan="2">We need to know if this is a receiver with a built-in tuner. In other words, if it has various inputs for other devices (like dvd, cd, etc.), but also has a built in tuner for the radio, normally AM/FM.</td>
		</tr>	
		<tr>
			<td colspan="2" align="center">&nbsp;</td>
		</tr>	
		<tr>
			<td colspan="2" align="center"><input type="submit" class="button" name="save" value="Yes, it has a tuner, and I have indicated them above"> <input type="button" class="button" onClick="self.close();" value="No this isn\'t a receiver with a built-in tuner"></td>
		</tr>		
	</table>';
	}
		
$out.='		
	</form>
	';
	} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=tunerConfig&dtID=$dtID&error=You are not authorised to change the installation.");
			exit(0);
		}
		$controlPipeQuery='
			INSERT INTO DeviceTemplate_DeviceTemplate_ControlledVia_Pipe
				(FK_DeviceTemplate_DeviceTemplate_ControlledVia,FK_Pipe,FK_Command_Input)
			VALUES 
				(?,?,?)';
		$dt_InputQuery='INSERT INTO DeviceTemplate_Input (FK_DeviceTemplate,FK_Command) VALUES (?,?)';

		if($categoryID!=$GLOBALS['AmpsReceivers']){
			$tunerNo=(int)$_POST['tunerNo'];
		
			for($i=1;$i<=$tunerNo;$i++){
				$dbADO->Execute('
					INSERT INTO DeviceTemplate_DeviceTemplate_ControlledVia 
						(FK_DeviceTemplate,FK_DeviceTemplate_ControlledVia,RerouteMessagesToParent,AutoCreateChildren)
					VALUES
						(?,?,1,1)',array($GLOBALS['Tuner'],$dtID));
				$insertID=$dbADO->Insert_ID();
				$GLOBALS['tuner_'.$i]=($GLOBALS['tuner_'.$i]=='' || $GLOBALS['tuner_'.$i]==0)?NULL:$GLOBALS['tuner_'.$i];
				$dbADO->Execute('
					INSERT INTO DeviceTemplate_DeviceTemplate_ControlledVia_Pipe
						(FK_DeviceTemplate_DeviceTemplate_ControlledVia,FK_Pipe,FK_Command_Input)
					VALUES 
						(?,?,?)',array($insertID,1,$GLOBALS['tuner_'.$i]));
				$dbADO->Execute('
					INSERT INTO DeviceTemplate_DeviceTemplate_ControlledVia_Pipe
						(FK_DeviceTemplate_DeviceTemplate_ControlledVia,FK_Pipe,FK_Command_Input)
					VALUES 
						(?,?,?)',array($insertID,2,$GLOBALS['tuner_'.$i]));
				$dbADO->Execute($dt_InputQuery,array($dtID,$GLOBALS['tuner_'.$i]));
			}
		}else{
			$commandArray=$_POST['command'];
			foreach ($commandArray AS $command){
				$dbADO->Execute('
					INSERT INTO DeviceTemplate_DeviceTemplate_ControlledVia 
						(FK_DeviceTemplate,FK_DeviceTemplate_ControlledVia,RerouteMessagesToParent,AutoCreateChildren)
					VALUES
						(?,?,1,1)',array($GLOBALS['Tuner'],$dtID));
				$insertID=$dbADO->Insert_ID();
				
				$command=($command=='')?NULL:$command;
				$dbADO->Execute($controlPipeQuery,array($insertID,1,$command));
				$dbADO->Execute($controlPipeQuery,array($insertID,2,$command));
				$dbADO->Execute($dt_InputQuery,array($dtID,$command));
			}
		}	
		$out.='
		<script>
			self.close();
		</script>';
	}

	$output->setScriptCalendar('null');

	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}
