<?php
function eventsHandler($output,$dbADO) {
	/* @var $dbADO ADOConnection */
	$out='';
	$installationID = (int)@$_SESSION['installationID'];
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$lastAction = isset($_REQUEST['lastAction'])?cleanString($_REQUEST['lastAction']):'';
	$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$installationID,$dbADO);
	
	if(isset($_REQUEST['dID']) && $_REQUEST['dID']!=0){
		if(!$canModifyInstallation){
			header("Location: index.php?section=eventsHandler&error=You are not authorised to modify installation.");
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
			
			$_REQUEST['msg']='The event handler was deleted.';
		}
	}
	if ($action=='form') {
		
		$out.='
		<div align="center" class="err">'.@$_REQUEST['error'].'</div>
		<div align="center"><B>'.@$_REQUEST['msg'].'</B></div>
		<form action="index.php" method="post" name="eventsHandler">
		<input type="hidden" name="section" value="eventsHandler">
		<input type="hidden" name="action" value="add">
		<div align="center"><h3>Events handler</h3></div>
		<table border="0" align="center">
			<tr bgcolor="lightblue">
				<td align="center"><B>Description</B></td>
				<td align="center"><B>Event</B></td>
				<td align="center">&nbsp;</td>
			</tr>
		';
		$queryEvents='
			SELECT EventHandler.*, CannedEvents.Description AS CannedEvent
			FROM EventHandler
				INNER JOIN CannedEvents ON FK_CannedEvents=PK_CannedEvents
			WHERE FK_Installation=?
		';
		$resEvents=$dbADO->Execute($queryEvents,$installationID);
		if($resEvents->RecordCount()==0){
			$out.='
				<tr>
					<td colspan="3">No records.</td>
				</tr>';
		}
		$lineCount=0;
		while($rowEvents=$resEvents->FetchRow()){
			$lineCount++;
			$out.='
				<tr bgcolor="'.(($lineCount%2==0)?'#E7E7E7':'#FFFFFF').'">
					<td>'.$rowEvents['Description'].'</td>
					<td>'.$rowEvents['CannedEvent'].'</td>
					<td align="center"><a href="index.php?section=editEventHandler&ehID='.$rowEvents['PK_EventHandler'].'">Edit</a> <a href="#" onClick="if(confirm(\'Are you sure you want to delete the event?\'))self.location=\'index.php?section=eventsHandler&dID='.$rowEvents['PK_EventHandler'].'\'">Delete</a></td>
				</tr>';
		}
		$queryCannedEvents='SELECT * FROM CannedEvents ORDER By Description ASC';
		$resCannedEvents=$dbADO->Execute($queryCannedEvents);
		$out.='
			<tr>
				<td colspan="3">&nbsp;</td>
			</tr>
			<tr>
				<td><B>Description</B>:</td>
				<td colspan="2"><input type="text" name="Description" value=""></td>
			</tr>		
			<tr>
				<td><B>New event</B>: </td>
				<td colspan="2"><select name="cannedEvent">
					<option value="0">- Please select an event -</option>';

		while($rowCE=$resCannedEvents->FetchRow()){
			$out.='<option value="'.$rowCE['PK_CannedEvents'].'">'.$rowCE['Description'].'</option>';
		}
		$out.='
			</select></td>
			<tr>
				<td colspan="3" align="center"><input type="submit" name="continue" value="Add"></td>
			</tr>';
		$out.='
		</table>
		</form>
		<script>
		 	var frmvalidator = new formValidator("eventsHandler");			
 			frmvalidator.addValidation("Description","req","Please enter the description!");
			frmvalidator.addValidation("cannedEvent","dontselect=0","Please select an event!");
		</script>		
		';
		
	} else {
		// processing area
		if(!$canModifyInstallation){
			header("Location: index.php?section=eventHandler&error=You are not authorised to modify installation.");
			exit();
		}

		$description=cleanString($_POST['Description']);
		$cannedEvent=cleanInteger($_POST['cannedEvent']);
		$insertCriteriaParmNesting='INSERT INTO CriteriaParmNesting (IsAnd,IsNot) VALUES (1,0)';
		$dbADO->Execute($insertCriteriaParmNesting);
		$insertID=$dbADO->Insert_ID();
		
		$insertCriteria='INSERT INTO Criteria(FK_CriteriaParmNesting,FK_CriteriaList,Description) VALUES (?,?,?)';
		$dbADO->Execute($insertCriteria,array($insertID,$GLOBALS['EventCriteriaList'],'event'));
		$criteriaID=$dbADO->Insert_ID();
		
		$insertCommandGroup='
			INSERT INTO CommandGroup 
				(FK_Array,FK_Installation,Description,CanTurnOff,AlwaysShow,CanBeHidden,FK_Template)
			VALUES
				(?,?,?,?,?,?,?)';
		$dbADO->Execute($insertCommandGroup,array($GLOBALS['EventsHandlerArray'],$installationID,'Event ActionGroup',0,0,0,$GLOBALS['EventsHandlerTemplate']));
		$cgID=$dbADO->Insert_ID();
		
		$insertEventHandler='
			INSERT INTO EventHandler 
				(Description, FK_Criteria, FK_Installation, FK_CommandGroup,FK_CannedEvents)
			VALUES
				(?,?,?,?,?)';
		$dbADO->Execute($insertEventHandler,array($description,$criteriaID,$installationID,$cgID,$cannedEvent));
		$ehID=$dbADO->Insert_ID();
		header("Location: index.php?section=editEventHandler&ehID=".$ehID);
	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();
}
?>