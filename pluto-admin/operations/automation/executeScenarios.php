<?
function executeScenarios($output,$dbADO) {
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
	
	if($action=='form'){
		
		$out.='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		</script>		
			<div align="center" class="err">'.@$_REQUEST['error'].'</div>
			<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
			<h3 align="center">Execute scenarios</h3>
			<p align="center">Click on a scenario to execute it just like you had touched the button on the Orbiter.</p>
			<form action="index.php" method="POST" name="executeScenarios">
			<input type="hidden" name="section" value="executeScenarios">
			<input type="hidden" name="action" value="update">
		<table cellpadding="4" cellspacing="0" border="0" align="center">
			<tr bgcolor="lightblue">
				<td align="center"><B>Scenario description</B></td>
				<td align="center"><B>Scenario type</B></td>
				<td align="center">&nbsp;</td>
			</tr>
		';
		$queryEntAreas='
			SELECT 
				EntertainArea.*,
				Room.Description AS RoomName 
			FROM EntertainArea
				INNER JOIN Room on FK_Room=PK_Room
			WHERE FK_Installation=?
			ORDER BY RoomName ASC, Description ASC';
		$resEntAreas=$dbADO->Execute($queryEntAreas,$installationID);
		$initRoom=0;
		$displayedCG=array();
		while($rowEntAreas=$resEntAreas->FetchRow()){
			if($rowEntAreas['FK_Room']!=$initRoom){
				$out.='
				<tr bgcolor="#D1D9EA">
					<td colspan="3" align="left"><B>Room: '.$rowEntAreas['RoomName'].'</B></td>
				</tr>';
				$queryCG_R='
					SELECT CommandGroup.Description AS ScenarioName, PK_CommandGroup, Array.Description AS ScenarioType
					FROM CommandGroup
					INNER JOIN CommandGroup_Room ON FK_CommandGroup=PK_CommandGroup
					INNER JOIN Array ON FK_Array=PK_Array
					WHERE FK_Installation=? AND FK_Room=?
					ORDER BY FK_Array ASC, ScenarioName ASC';
				$resCG_R=$dbADO->Execute($queryCG_R,array($installationID,$rowEntAreas['FK_Room']));
				while($rowCG_R=$resCG_R->FetchRow()){
					$displayedCG[]=$rowCG_R['PK_CommandGroup'];
					$out.='
						<tr>
							<td><a href="index.php?section=executeScenarios&action=execute&cgID='.$rowCG_R['PK_CommandGroup'].'" title="Click to execute scenario">'.$rowCG_R['ScenarioName'].'</a></td>
							<td>'.$rowCG_R['ScenarioType'].'</td>
							<td align="center"><a href="index.php?section=editCommandGroup&cgID='.$rowCG_R['PK_CommandGroup'].'">Edit</a></td>
						</tr>';
				}
				$initRoom=$rowEntAreas['FK_Room'];
			}
			$out.='
				<tr bgcolor="#EEEEEE">
					<td colspan="3" align="left"><B>Entertain area: '.$rowEntAreas['Description'].'</B></td>
				</tr>';
			$queryCG_E='
				SELECT CommandGroup.Description AS ScenarioName, PK_CommandGroup, Array.Description AS ScenarioType
				FROM CommandGroup
				INNER JOIN CommandGroup_EntertainArea ON FK_CommandGroup=PK_CommandGroup
				INNER JOIN Array ON FK_Array=PK_Array
				WHERE FK_Installation=? AND FK_EntertainArea=?';
			$resCG_E=$dbADO->Execute($queryCG_E,array($installationID,$rowEntAreas['PK_EntertainArea']));
			while($rowCG_E=$resCG_E->FetchRow()){
				$displayedCG[]=$rowCG_E['PK_CommandGroup'];
				$out.='
					<tr>
						<td <a href="index.php?section=executeScenarios&action=execute&cgID='.$rowCG_E['PK_CommandGroup'].'" title="Click to execute scenario">'.$rowCG_E['ScenarioName'].'</a></td>
						<td>'.$rowCG_E['ScenarioType'].'</td>
						<td align="center"><a href="index.php?section=editCommandGroup&cgID='.$rowCG_E['PK_CommandGroup'].'">Edit</a></td>
					</tr>';
			}
		}
		$out.='
			<tr bgcolor="#D1D9EA">
				<td colspan="3" align="left"><B>Unassigned to any room or Entertain Area</B></td>
			</tr>';		
		if(count($displayedCG)==0)
			$displayedCG[]=0;
		$queryCG='
			SELECT CommandGroup.Description AS ScenarioName, PK_CommandGroup, Array.Description AS ScenarioType
			FROM CommandGroup
			INNER JOIN Array ON FK_Array=PK_Array
			WHERE FK_Installation=? AND PK_CommandGroup NOT IN ('.join(',',$displayedCG).')';
		$resCG=$dbADO->Execute($queryCG,$installationID);
		while($rowCG=$resCG->FetchRow()){
			$out.='
				<tr>
					<td <a href="index.php?section=executeScenarios&action=execute&cgID='.$rowCG['PK_CommandGroup'].'" title="Click to execute scenario">'.$rowCG['ScenarioName'].'</a></td>
					<td>'.$rowCG['ScenarioType'].'</td>
					<td align="center"><a href="index.php?section=editCommandGroup&cgID='.$rowCG['PK_CommandGroup'].'">Edit</a></td>
				</tr>';
		}
		$out.='
		</form>';
	}else{
	// process area
		if(isset($_REQUEST['cgID'])){
			$cgID=(int)$_REQUEST['cgID'];
			$commandToSend='/usr/pluto/bin/MessageSend localhost 0 0 10 '.$cgID;
			system($commandToSend);
			
			header('Location: index.php?section=executeScenarios&msg=The scenario was executed.');
			exit();
		}
		
		header('Location: index.php?section=executeScenarios');
	}
	
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}
?>