<?php
function advancedEvents($output,$dbADO) {
	/* @var $dbADO ADOConnection */
	$out='';
	$installationID = (int)@$_SESSION['installationID'];
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$lastAction = isset($_REQUEST['lastAction'])?cleanString($_REQUEST['lastAction']):'';
	$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$installationID,$dbADO);
	$timedEventArray=array(1=>'Interval', 2=>'Day of week',3=>'Day of month',4=>'Absolute');
	
	if(isset($_REQUEST['dID']) && $_REQUEST['dID']!=0){
		if(!$canModifyInstallation){
			header("Location: index.php?section=advancedEvents&error=You are not authorised to modify installation.");
			exit();
		}

		$eventHandlerID=$_REQUEST['dID'];
		$queryEventsHandler='
			SELECT EventHandler.*, Criteria.FK_CriteriaParmNesting 
			FROM EventHandler 
				INNER JOIN Criteria ON FK_Criteria=PK_Criteria
			WHERE PK_EventHandler=?';
		$resEH=$dbADO->Execute($queryEventsHandler,$eventHandlerID);
		if($resEH->RecordCount()!=0){
			$rowEH=$resEH->FetchRow();
			deleteCommandGroup($rowEH['FK_CommandGroup'],$dbADO);
			
			$deleteCriteriaParmNesting='DELETE FROM CriteriaParmNesting WHERE PK_CriteriaParmNesting=?';
			$dbADO->Execute($deleteCriteriaParmNesting,$rowEH['FK_CriteriaParmNesting']);
			
			$deleteCriteria='DELETE FROM Criteria WHERE PK_Criteria=?';
			$dbADO->Execute($deleteCriteria,$rowEH['FK_Criteria']);
			
			$deleteCriteriaParms='DELETE FROM CriteriaParm WHERE FK_CriteriaParmNesting=?';
			$dbADO->Execute($deleteCriteriaParms,$rowEH['FK_CriteriaParmNesting']);
			
			$deleteEventHandler='DELETE FROM EventHandler WHERE PK_EventHandler=?';
			$dbADO->Execute($deleteEventHandler,$eventHandlerID);
			
			$_REQUEST['msg']='The event was deleted.';
		}
	}
	if ($action=='form') {
		
		$out.='
		<div align="center" class="err">'.@$_REQUEST['error'].'</div>
		<div align="center"><B>'.@$_REQUEST['msg'].'</B></div>
		<form action="index.php" method="post" name="advancedEvents">
		<input type="hidden" name="section" value="advancedEvents">
		<input type="hidden" name="action" value="add">
		<div align="center"><h3>Advanced Events</h3></div>
		<table border="0" align="center" cellpading="3">
			<tr bgcolor="lightblue">
				<td align="center"><B>Event</B></td>
				<td align="center"><B>Description</B></td>
				<td align="center"><B>FK_Event</B></td>
				<td align="center"><B>Timed event</B></td>
				<td align="center"><B>Template</B></td>
				<td align="center"><B>Criteria</B></td>
				<td align="center"><B>Commands</B></td>
				<td align="center">&nbsp;</td>
			</tr>
		';
		$queryEvents='
			SELECT EventHandler.*, Event.Description AS EventDescription, CannedEvents.Description AS TemplateDescription 
			FROM EventHandler
			LEFT JOIN Event ON EventHandler.FK_Event=PK_Event
			LEFT JOIN CannedEvents ON FK_CannedEvents=PK_CannedEvents
			WHERE EventHandler.FK_Installation=?
		';
		$resEvents=$dbADO->Execute($queryEvents,$installationID);
		if($resEvents->RecordCount()==0){
			$out.='
				<tr>
					<td colspan="8">No records.</td>
				</tr>';
		}
		$lineCount=0;
		$displayedEH=array();
		while($rowEvents=$resEvents->FetchRow()){
			$displayedEH[]=$rowEvents['PK_EventHandler'];
			$lineCount++;
			$out.='
				<tr bgcolor="'.(($lineCount%2==0)?'#E7E7E7':'#FFFFFF').'">
					<td align="center">'.$rowEvents['PK_EventHandler'].'</td>
					<td><input type="text" name="eventDescription_'.$rowEvents['PK_EventHandler'].'" value="'.$rowEvents['Description'].'"></td>
					<td>'.$rowEvents['EventDescription'].'</td>
					<td>'.@$timedEventArray[$rowEvents['TimedEvent']].'</td>
					<td>'.$rowEvents['TemplateDescription'].'</td>
					<td align="center"><a href="index.php?section=editCriteria&ehID='.$rowEvents['PK_EventHandler'].'">Edit</a></td>
					<td align="center"><a href="index.php?section=editEventCommands&ehID='.$rowEvents['PK_EventHandler'].'">Edit</a></td>
					<td align="center"><a href="#" onClick="if(confirm(\'Are you sure you want to delete the event?\'))self.location=\'index.php?section=advancedEvents&dID='.$rowEvents['PK_EventHandler'].'\'">Delete</a></td>
				</tr>';
		}
		if(count($displayedEH)!=0)
			$out.='
			<tr bgcolor="#E7E7E7">
				<td colspan="8" align="center"><input type="submit" name="update" value="Update"></td>
			</tr>';
		$out.='
			<tr>
				<td colspan="8">&nbsp;</td>
			</tr>';
		$out.='
		</table>
			<input type="hidden" name="displayedEH" value="'.join(',',$displayedEH).'">
		</form>
		';
		
	} else {
		// processing area
		if(!$canModifyInstallation){
			header("Location: index.php?section=advancedEvents&error=You are not authorised to modify installation.");
			exit();
		}
		$displayedEHArray=explode(',',$_POST['displayedEH']);
		foreach ($displayedEHArray AS $ehID){
			$description=cleanString($_POST['eventDescription_'.$ehID]);
			$dbADO->Execute('UPDATE EventHandler SET Description=? WHERE PK_EventHandler=?',array($description,$ehID));
		}
		
		header("Location: index.php?section=advancedEvents&msg=The event description was updated.");
	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Timed Events');			
	$output->output();
}
?>