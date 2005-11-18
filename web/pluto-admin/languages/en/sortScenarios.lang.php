<?php
function sortScenarios($output,$dbADO) {
/* @var $dbADO ADOConnection */
/* @var $rs ADORecordSet */

$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
$out='';

$installationID = (int)@$_SESSION['installationID'];
$categoryArray=getAssocArray('Template','PK_Template','Description',$dbADO,'','');
$categoryArray['']='Not defined';
$sortBy=(!isset($_REQUEST['sortBy']) || $_REQUEST['sortBy']=='Room')?'Room':'EntertainArea';


$areasArray=getAssocArray($sortBy,'PK_'.$sortBy,'Description',$dbADO,'','');
$areasArray['']='Not assigned';

if($action=='form') {
	

	$out.=setLeftMenu($dbADO).'
	<div align="center" class="err">'.strip_tags(@$_GET['error']).'</div>
	<div align="center" class="confirm"><B>'.(isset($_GET['msg'])?strip_tags($_GET['msg'].'<br>'):'').'</B></div>
	<h2 align="center">Sort scenarios</h2>';

	$out.='
	<form action="index.php" method="POST" name="sortScenarios">
		<input type="hidden" name="section" value="sortScenarios">
		<input type="hidden" name="action" value="add">	
		<input type="hidden" name="'.$sortBy.'ID" value="">
		<input type="hidden" name="editedCgID" value="">	
		<input type="hidden" name="sortBy" value="'.$sortBy.'">
	
	<table width="100%" cellpadding="4" cellspacing="0" border="0">';
	if(count($areasArray)==0){
		$out.='There are no rooms/entertain areas in this installation. Go to <a href="index.php?section=rooms">rooms</a> section to add rooms or entertain areas.';
	}
	$displayedCommandGroups=array();
	$selectCommandGroups='
			SELECT * 
			FROM CommandGroup
			LEFT JOIN CommandGroup_'.$sortBy.' ON FK_CommandGroup=PK_CommandGroup
			WHERE FK_Installation=?
			ORDER BY FK_'.$sortBy.' ASC,Sort ASC';
	$resCommandGroups=$dbADO->Execute($selectCommandGroups,array($installationID));
	$pos=0;
	$posInArea=array();
	while($rowCG=$resCommandGroups->FetchRow()){
		if($rowCG['FK_'.$sortBy]!='')
			$posInArea[$rowCG['PK_CommandGroup']]=$rowCG['Sort'];
	}
	
	$resCommandGroups->MoveFirst();
	$startArea=-1;
	while($rowCG=$resCommandGroups->FetchRow()){
		if($rowCG['FK_'.$sortBy]!=$startArea){
			$out.='
			<tr bgcolor="#D1D9EA">
				<td colspan="3"><B>'.$areasArray[$rowCG['FK_'.$sortBy]].'</B></td>
			</tr>';
			$startArea=$rowCG['FK_'.$sortBy];
		}
		if(!in_array($rowCG['PK_CommandGroup'],$displayedCommandGroups))
		$firstAreaArray[$rowCG['PK_CommandGroup']]=$areasArray[$rowCG['FK_'.$sortBy]];
		$pos++;
		$out.='
			<tr bgcolor="'.(($rowCG['PK_CommandGroup']==@$_REQUEST['lastAdded'])?'lightgreen':'').'">
				<td width="80" align="center">'.(($rowCG['FK_'.$sortBy.'']!='')?'
					<input type="button" class="button" name="posDown" value="U" onClick="self.location=\'index.php?section=sortScenarios&cgID='.$rowCG['PK_CommandGroup'].'&action=process&'.$sortBy.'ID='.$rowCG['FK_'.$sortBy.''].'&operation=down&posInArea='.urlencode(serialize($posInArea)).'&sortBy='.$sortBy.'\'"> 
					<input type="button" class="button" name="posUp" value="D" onClick="self.location=\'index.php?section=sortScenarios&cgID='.$rowCG['PK_CommandGroup'].'&action=process&'.$sortBy.'ID='.$rowCG['FK_'.$sortBy.''].'&operation=up&posInArea='.urlencode(serialize($posInArea)).'&sortBy='.$sortBy.'\'">':'').'</td>
				<td> Description: '.((!in_array($rowCG['PK_CommandGroup'],$displayedCommandGroups))?'<input type="text" name="commandGroup_'.$rowCG['PK_CommandGroup'].'" value="'.$rowCG['Description'].'"> Hint: <input type="text" name="hintCommandGroup_'.$rowCG['PK_CommandGroup'].'" value="'.$rowCG['Hint'].'">':'<b>'.$rowCG['Description'].': </b>Hint: <b>'.$rowCG['Hint'].'</b> (See '.$firstAreaArray[$rowCG['PK_CommandGroup']].')').'
				Category: <B>'.$categoryArray[$rowCG['FK_Template']].'</B>
				</td>
				<td>'.(($pos==1)?'Default ON':'').(($pos==2)?'Default OFF':'').'</td>
			</tr>
			';
		$displayedCommandGroups[]=$rowCG['PK_CommandGroup'];
	}
	
	if(count($displayedCommandGroups)>0){
		$out.='
		<tr>
			<td colspan="3" align="center"><input type="submit" class="button" name="updateCG" value="Update"  ></td>
		</tr>';
	}
	$out.='
		</table>
	<input type="hidden" name="displayedCommandGroups" value="'.join(',',$displayedCommandGroups).'">	
	</form>
	';
							
}else{	
	// action='add'
	// insert command group in specified room
	$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$installationID,$dbADO);
	if(!$canModifyInstallation){
		Header('Location: index.php?section=sortScenarios&error=You are not allowed to modify installation.');
		exit();
	}

	
	if(isset($_REQUEST['operation'])){
		$fromCgID=$_REQUEST['cgID'];
		$areaID=@$_REQUEST[''.$sortBy.'ID'];
		$operation=$_REQUEST['operation'];
		$posInArea=unserialize(stripslashes($_REQUEST['posInArea']));
		asort($posInArea);
		if($operation=='up'){
			$offset=array_keys($posInArea);
			$fromPos=array_search($fromCgID,$offset);
			if(isset($offset[$fromPos+1])){
				$toCgID=$offset[$fromPos+1];
			
				$fromSort=$posInArea[$fromCgID];
				$toSort=$posInArea[$toCgID];
	
				$updateCG_R='UPDATE CommandGroup_'.$sortBy.' SET Sort=? WHERE FK_CommandGroup=? AND FK_'.$sortBy.'=?';
				$dbADO->Execute($updateCG_R,array($toSort,$fromCgID,$areaID));
				$dbADO->Execute($updateCG_R,array($fromSort,$toCgID,$areaID));
				$msg="Positions were updated";
			}
		}else{
			$offset=array_keys($posInArea);
			$fromPos=array_search($fromCgID,$offset);

			if(isset($offset[$fromPos-1])){
				$toCgID=$offset[$fromPos-1];
			
				$fromSort=$posInArea[$fromCgID];
				$toSort=$posInArea[$toCgID];
	
				$updateCG_R='UPDATE CommandGroup_'.$sortBy.' SET Sort=? WHERE FK_CommandGroup=? AND FK_'.$sortBy.'=?';
				$dbADO->Execute($updateCG_R,array($toSort,$fromCgID,$areaID));
				$dbADO->Execute($updateCG_R,array($fromSort,$toCgID,$areaID));
				$msg="Positions were updated";
			}
		}
	}


	header("Location: index.php?section=sortScenarios&msg=".@$msg.'&sortBy='.$sortBy);
}

	$output->setNavigationMenu(array("My Scenarios"=>'index.php?section=myScenarios',"Sort Scenarios"=>'index.php?section=sortScenarios&sortBy='.$sortBy));
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Sort Scenarios');
	$output->output();

}
?>