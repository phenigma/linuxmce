<?php
function timedEvents($output,$dbADO) {
	/* @var $dbADO ADOConnection */
	$out='';
	$installationID = (int)@$_SESSION['installationID'];
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$lastAction = isset($_REQUEST['lastAction'])?cleanString($_REQUEST['lastAction']):'';
	$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$installationID,$dbADO);
	
	if(isset($_REQUEST['dID']) && $_REQUEST['dID']!=0){
		if(!$canModifyInstallation){
			header("Location: index.php?section=timedEvents&error=You are not authorised to modify installation.");
			exit();
		}

		$eventHandlerID=$_REQUEST['dID'];
		$queryEventsHandler='
			SELECT EventHandler.*, Criteria.FK_CriteriaParmNesting 
			FROM EventHandler 
				INNER JOIN Criteria ON FK_Criteria=PK_Criteria
			WHERE PK_EventHandler=? AND TimedEvent IS NOT NULL';
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
		
		$out.=setLeftMenu($dbADO).'
		<div align="center" class="err">'.@$_REQUEST['error'].'</div>
		<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
		<form action="index.php" method="post" name="timedEvents">
		<input type="hidden" name="section" value="timedEvents">
		<input type="hidden" name="action" value="add">
		<div align="center"><h3>Timed Events</h3></div>
		<table border="0" align="center" cellpading="3">
			<tr bgcolor="lightblue">
				<td align="center"><B>Description</B></td>
				<td align="center">&nbsp;</td>
			</tr>
		';
		$queryEvents='
			SELECT EventHandler.* FROM EventHandler
			WHERE EventHandler.FK_Installation=? AND TimedEvent IS NOT NULL
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
					<td align="center">'.$rowEvents['Description'].'</td>
					<td align="center"><a href="index.php?section=editTimedEvent&ehID='.$rowEvents['PK_EventHandler'].'">Edit</a> <a href="#" onClick="if(confirm(\'Are you sure you want to delete the event?\'))self.location=\'index.php?section=timedEvents&dID='.$rowEvents['PK_EventHandler'].'\'">Delete</a></td>
				</tr>';
		}
		$out.='
			<tr>
				<td colspan="3">&nbsp;</td>
			</tr>
			<tr>
				<td><B>New timed event description</B>:</td>
				<td colspan="2"><input type="text" name="Description" value=""></td>
			</tr>	
			<tr>
				<td><input type="radio" name="timedEventType" value="1" checked> <B>Interval based:</B></td>
				<td colspan="2">Do something every x minutes, or every other hour</td>
			</tr>	
			<tr>
				<td><input type="radio" name="timedEventType" value="2"> <B>Day of week based:</B></td>
				<td colspan="2">Do something at 7:00 and 9:00 on Monday, Wednesday and Friday</td>
			</tr>		
			<tr>
				<td><input type="radio" name="timedEventType" value="3"> <B>Day of month based:</B></td>
				<td colspan="2">Do something at 8:00 on the 1st and 15th of each month</td>
			</tr>		
			<tr>
				<td><input type="radio" name="timedEventType" value="4"> <B>Absolute:</B></td>
				<td colspan="2">Do something on 5 Mar 2005 at 11:15</td>
			</tr>
			<tr>
				<td colspan="3" align="center"><input type="submit" class="button" name="continue" value="Add"></td>
			</tr>';
		$out.='
		</table>
		</form>
		<script>
		 	var frmvalidator = new formValidator("timedEvents");			
 			frmvalidator.addValidation("Description","req","Please enter the description!");
		</script>		
		';
		
	} else {
		// processing area
		if(!$canModifyInstallation){
			header("Location: index.php?section=eventHandler&error=You are not authorised to modify installation.");
			exit();
		}

		$description=cleanString($_POST['Description']);
		$timedEventType=cleanInteger($_POST['timedEventType']);
		$insertCriteriaParmNesting='INSERT INTO CriteriaParmNesting (IsAnd,IsNot) VALUES (1,0)';
		$dbADO->Execute($insertCriteriaParmNesting);
		$insertID=$dbADO->Insert_ID();
		
		$insertCriteria='INSERT INTO Criteria(FK_CriteriaParmNesting,FK_CriteriaList,Description,FK_Installation) VALUES (?,?,?,?)';
		$dbADO->Execute($insertCriteria,array($insertID,$GLOBALS['EventCriteriaList'],'event',$installationID));
		$criteriaID=$dbADO->Insert_ID();
		
		$insertCommandGroup='
			INSERT INTO CommandGroup 
				(FK_Array,FK_Installation,Description,CanTurnOff,AlwaysShow,CanBeHidden,FK_Template)
			VALUES
				(?,?,?,?,?,?,?)';
		$dbADO->Execute($insertCommandGroup,array($GLOBALS['EventsHandlerArray'],$installationID,'Event ActionGroup',0,0,0,$GLOBALS['EventsHandlerTemplate']));
		$cgID=$dbADO->Insert_ID();
		
		$insertEventHandler="
			INSERT INTO EventHandler 
				(Description, FK_Criteria, FK_Installation, FK_CommandGroup,FK_CannedEvents,FK_Event,TimedEvent)
			VALUES
				(?,?,?,?,?,?,?) ";
		$dbADO->Execute($insertEventHandler,array($description,$criteriaID,$installationID,$cgID,NULL,NULL,$timedEventType));
		$ehID=$dbADO->Insert_ID();
		header("Location: index.php?section=editTimedEvent&ehID=".$ehID);
	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Timed Events');			
	$output->output();
}
?>
