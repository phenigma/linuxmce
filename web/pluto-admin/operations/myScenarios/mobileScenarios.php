<?php
function mobileScenarios($output,$dbADO) {
/* @var $dbADO ADOConnection */
/* @var $rs ADORecordSet */

$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
$out='
<script>
function windowOpen(locationA,attributes) 
{
	window.open(locationA,\'\',attributes);
}
</script>
';

$installationID = (int)@$_SESSION['installationID'];
$arrayID = $GLOBALS['ArrayIDForMobileOrbiterScenarios'];	
$templateArray=array($GLOBALS['LightingScenariosTemplate']=>'Lighting Scenarios', $GLOBALS['ClimateScenariosTemplate']=>'Climate Scenarios');

$allTemplateArray=array();
$resAll=$dbADO->Execute('SELECT Description, PK_Template FROM Template ORDER BY Description ASC');
while($rowAll=$resAll->FetchRow()){
	$allTemplateArray[$rowAll['PK_Template']]=$rowAll['Description'];
}

if($action=='form') {
	
	$queryArray = 'SELECT * FROM Array WHERE PK_Array=?';	
	$resArray = $dbADO->Execute($queryArray,$arrayID);
	$rowArray=$resArray->FetchRow();

	$out.='
	<div align="center" class="confirm"><B>'.(isset($_GET['msg'])?strip_tags($_GET['msg'].'<br>'):'').'</B></div>
	<h2 align="center">'.$rowArray['Description'].'</h2>';

	$out.='
	<form action="index.php" method="POST" name="mobileScenarios">
		<input type="hidden" name="section" value="mobileScenarios">
		<input type="hidden" name="action" value="add">	
		<input type="hidden" name="editedCgID" value="">
		<input type="hidden" name="editedTemplate" value="">	
	
	<table width="100%" cellpadding="4" cellspacing="0" border="0">';
	$displayedCommandGroups=array();
	$out.='
		<tr bgcolor="#D1D9EA">
			<td colspan="4" align="right"><input type="button" class="button" name="add" value="Add scenario" onClick="javascript:document.mobileScenarios.action.value=\'addToRoom\';document.mobileScenarios.submit();"></td>
		</tr>';
	$selectCommandGroups='
			SELECT * 
			FROM CommandGroup
			WHERE FK_Array=? AND FK_Installation=?
			ORDER BY Description ASC';
	$resCommandGroups=$dbADO->Execute($selectCommandGroups,array($arrayID,$installationID));
	if($resCommandGroups->RecordCount()==0){
		$out.='
		<tr>
			<td colspan="3" align="center">No mobile scenarios.</td>
		</tr>';
	}
	$pos=0;
	while($rowCG=$resCommandGroups->FetchRow()){
		$pos++;
		$out.='
			<tr bgcolor="'.(($rowCG['PK_CommandGroup']==@$_REQUEST['lastAdded'])?'lightgreen':'').'">
				<td>Description: '.((!in_array($rowCG['PK_CommandGroup'],$displayedCommandGroups))?'<input type="text" name="commandGroup_'.$rowCG['PK_CommandGroup'].'" value="'.$rowCG['Description'].'"> Hint: <input type="text" name="hintCommandGroup_'.$rowCG['PK_CommandGroup'].'" value="'.$rowCG['Hint'].'">':'<b>'.$rowCG['Description'].': </b>Hint: <b>'.$rowCG['Hint'].'</b>').'</td>
				<td>&nbsp;</td>
				<td>&nbsp;</td>
				<td><a href="#" onclick="document.mobileScenarios.editedCgID.value='.$rowCG['PK_CommandGroup'].';document.mobileScenarios.submit();">Edit</a> <a href="#" onClick="javascript:if(confirm(\'Are you sure you want to delete this scenario?\'))self.location=\'index.php?section=mobileScenarios&action=delete&cgDelID='.$rowCG['PK_CommandGroup'].'\';">Delete</a></td>
			</tr>
			';
		$displayedCommandGroups[]=$rowCG['PK_CommandGroup'];
	}
	if(count($displayedCommandGroups)>0){
		$out.='
		<tr>
			<td colspan="3" align="center"><input type="submit" class="button" name="updateCG" value="Update"  ></td>
			<td>&nbsp;</td>
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
	if($action=='addToRoom'){
		$insertCommandGroup='INSERT INTO CommandGroup (Description,FK_Array,FK_Installation,FK_Template) VALUES (?,?,?,?)';
		$dbADO->Execute($insertCommandGroup,array('New Mobile orbiter scenario',$arrayID,$installationID,NULL));
		$insertID=$dbADO->Insert_ID();
		setOrbitersNeedConfigure($installationID,$dbADO);
		$msg='New Mobile orbiter scenario added.&lastAdded='.$insertID;
	}

	if(isset($_POST['updateCG']) || $action=='externalSubmit' || @(int)$_REQUEST['editedCgID']!=0 || $action=='addToRoom'){
		$displayedCommandGroupsArray=explode(',',$_POST['displayedCommandGroups']);
		foreach($displayedCommandGroupsArray as $elem){
			$cgDescription=cleanString(@$_POST['commandGroup_'.$elem]);
			$cgHint=cleanString(@$_POST['hintCommandGroup_'.$elem]);
			$updateCG='UPDATE CommandGroup SET Description=?, Hint=? WHERE PK_CommandGroup=?';
			$dbADO->Execute($updateCG,array($cgDescription,$cgHint,$elem));
		}
		setOrbitersNeedConfigure($installationID,$dbADO);
		
		if(@(int)$_REQUEST['editedCgID']!=0){
			header('Location: index.php?section=scenarioWizard&from=mobileScenarios&cgID='.$_REQUEST['editedCgID'].'&wizard=2');
			exit();
		}
		$msg=(isset($msg))?$msg:"Mobile Orbiter Scenario updated.";
	}
	

	
	
	if(isset($_GET['cgDelID']) && (int)$_GET['cgDelID']!=0){
		$cgToDelete=(int)$_GET['cgDelID'];
		deleteCommandGroup($cgToDelete,$dbADO);
		header("Location: index.php?section=mobileScenarios&msg=Lighting scenario deleted.");
	}
	
	header("Location: index.php?section=mobileScenarios&msg=".@$msg);
	exit();
}

	$output->setScriptInBody("onLoad=\"javascript:top.treeframe.location.reload();\"");
	$output->setNavigationMenu(array("My Scenarios"=>'index.php?section=myScenarios',"Mobile Orbiter Scenarios"=>'index.php?section=mobileScenarios'));
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Mobile Orbiter Scenarios');
	$output->output();

}
?>
