<?php
function editCriteria($output,$dbADO) {
	/* @var $dbADO ADOConnection */
	$out='';
	$installationID = (int)@$_SESSION['installationID'];
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$lastAction = isset($_REQUEST['lastAction'])?cleanString($_REQUEST['lastAction']):'';
	$eventHandlerID=$_REQUEST['ehID'];
	$GLOBALS['displayedCriteriaParms']=array();
	
	$queryEventHandler='
		SELECT EventHandler.*,Criteria.FK_CriteriaParmNesting, Criteria.PK_Criteria, Criteria.Description AS CriteriaDescription, CriteriaList.Description AS CriteriaListDescription 
		FROM EventHandler
			INNER JOIN Criteria ON FK_Criteria=PK_Criteria
			INNER JOIN CriteriaList ON FK_CriteriaList=PK_CriteriaList
		WHERE EventHandler.FK_Installation=? AND PK_EventHandler=?
	';
	$resEH=$dbADO->Execute($queryEventHandler,array($installationID,$eventHandlerID));
	$rowEH=$resEH->FetchRow();
	$commandGroupID = $rowEH['FK_CommandGroup'];
	$FK_CriteriaParmNesting=$rowEH['FK_CriteriaParmNesting'];

	
	if ($action=='form') {
		
		$out.='
		<div align="center" class="err">'.@$_REQUEST['error'].'</div>
		<div align="center"><B>'.@$_REQUEST['msg'].'</B></div>
		<form action="index.php" method="post" name="editCriteria">
		<input type="hidden" name="section" value="editCriteria">
		<input type="hidden" name="action" value="update">
		<input type="hidden" name="ehID" value="'.$eventHandlerID.'">
		<input type="hidden" name="changedCP" value="">
		
		<div align="center"><h3>Edit Criteria</h3></div>
		<table border="0" align="center">';
		$out.='
			<tr>
				<td align="right"><B>Criteria</B></td>
				<td><input type="text" name="criteriaDescription" value="'.$rowEH['CriteriaDescription'].'"></td>
				<td><B>Template</B></td>
				<td>'.$rowEH['CriteriaListDescription'].'</td>
			</tr>
			<tr>
				<td colspan="4">
			';
		$out.=displayCriteria($FK_CriteriaParmNesting,$eventHandlerID,$installationID,$dbADO);
		$out.='</td>
			</tr>
			<tr>
				<td colspan="6" align="center"><input type="submit" name="update" value="update"></td>
			</tr>
		
		</table>
		</form>
		';
		
	} else {
		// processing area
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$installationID,$dbADO);
		if(!$canModifyInstallation){
			header("Location: index.php?section=editCriteria&ehID=".$eventHandlerID."&error=You are not authorised to modify installation.");
			exit();
		}

		if(isset($_GET['dcpID'])){
			$dcpID=$_GET['dcpID'];
			$dbADO->Execute('DELETE FROM CriteriaParm WHERE PK_CriteriaParm=?',$dcpID);
			header('Location: index.php?section=editCriteria&ehID='.$eventHandlerID.'&msg=The criteria was deleted');
		}
	
		if(isset($_REQUEST['fkCPN'])){
			$fkCPN=$_REQUEST['fkCPN'];
			$dbADO->Execute('INSERT INTO CriteriaParm (FK_CriteriaParmNesting) VALUES(?)',$fkCPN);
		}
		
		$changedCP=(int)@$_POST['changedCP'];
		if($changedCP>0){
			$newCPL=$_POST['CriteriaParmID_'.$changedCP];
			$newParm=($newCPL==$GLOBALS['PK_EventParameterParmList'])?(isset($_POST['CriteriaParmParm_'.$changedCP])?(int)$_POST['CriteriaParmParm_'.$changedCP]:NULL):NULL;
			$dbADO->Execute('UPDATE CriteriaParm SET FK_CriteriaParmList=?, Parm=?,Value=NULL, Operator=1 WHERE PK_CriteriaParm=?',array($newCPL,$newParm,$changedCP));
		}

		if(isset($_POST['update'])){
			$displayedCriteriaParmArray=explode(',',$_POST['displayedCriteriaParm']);
			foreach ($displayedCriteriaParmArray AS $cpID){
				$operator=@$_POST['CriteriaParmOperator_'.$cpID];
				$value=@$_POST['CriteriaParmValue_'.$cpID];
				$dbADO->Execute('UPDATE CriteriaParm SET Operator=?, Value=? WHERE PK_CriteriaParm=?',array($operator,$value,$cpID));
			}
		}

		
		if(isset($_REQUEST['chCPN'])){
			$chCPN=(int)$_REQUEST['chCPN'];

			if(isset($_REQUEST['setNot'])){
				$setNot=(int)$_REQUEST['setNot'];
				$dbADO->Execute('UPDATE CriteriaParmNesting SET IsNot=? WHERE PK_CriteriaParmNesting=?',array($setNot,$chCPN));
			}
			if(isset($_REQUEST['setAnd'])){
				$setAnd=(int)$_REQUEST['setAnd'];
				$dbADO->Execute('UPDATE CriteriaParmNesting SET IsAnd=? WHERE PK_CriteriaParmNesting=?',array($setAnd,$chCPN));
			}
		}
		
		if(isset($_REQUEST['delCPN'])){
			$cpnID=$_REQUEST['delCPN'];
			if($cpnID==$FK_CriteriaParmNesting){
				header("Location: index.php?section=editCriteria&ehID=".$eventHandlerID."&error=You cannot delete root level nesting.");
				exit();
			}
			deleteCriteriaParmNesting($cpnID,$dbADO);
			header('Location: index.php?section=editCriteria&ehID='.$eventHandlerID.'&msg=The nesting was deleted');
		}
		
		
		if(isset($_REQUEST['parentCPN'])){
			$parentCPN=(int)$_REQUEST['parentCPN'];
			$dbADO->Execute('INSERT INTO CriteriaParmNesting (FK_CriteriaParmNesting_Parent,IsAnd,IsNot) VALUES (?,0,0)',$parentCPN);
		}
		
		header('Location: index.php?section=editCriteria&ehID='.$eventHandlerID.'&msg=The criteria was updated');
	}
	
	$output->setNavigationMenu(array("Advanced Events"=>'index.php?section=advancedEvents'));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();
}
?>