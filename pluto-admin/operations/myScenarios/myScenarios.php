<?php
function myScenarios($output,$dbADO) {
/* @var $dbADO ADOConnection */
/* @var $rs ADORecordSet */

$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'showBasicInfo';
$out='';
$installationID = (int)@$_SESSION['installationID'];


if ($action == 'showBasicInfo') {
	$out='
		All the scenarios in your installation are shown in the tree to the left.  The scenarios correspond to buttons on your Orbiters.
		It is usually faster to use the wizard to create basic scenarios, like lighting, climate, etc.  This section allows you to 
		select a scenario and manually edit the commands that it will fire.
	';	
} elseif($action=='showArrayCommands') {
	$arrayID = cleanInteger(@$_REQUEST['array']);	
	$queryGetCommands = 'SELECT * FROM CommandGroup WHERE FK_Installation = ? AND FK_Array = ?';
	$resGetCommands = $dbADO->Execute($queryGetCommands,array($installationID,$arrayID));

	while ($rowGetCommands = $resGetCommands->FetchRow()) {
		$out.='<a href="index.php?section=editCommandGroup&cgID='.$rowGetCommands['PK_CommandGroup'].'">'.$rowGetCommands['Description'].'</a> <br />';
	}
	
	$out.='
	<script>
				function windowOpen(locationA,attributes) {
					window.open(locationA,\'\',attributes);
				}
	</script>
	<input type="button" name="addCommandGroup" value="Add my scenario" onClick="windowOpen(\'index.php?section=addCommandGroupAsScenario&from=myScenarios&arrayID='.$arrayID.'\',\'width=600,height=600,toolbars=true,resizable=1,scrollbars=1\');"">
	';
							
}
	



$output->setNavigationMenu(array("My Scenarios"=>'index.php?section=myScenarios'));

$output->setScriptCalendar('null');

$output->setBody($out);
$output->setTitle(APPLICATION_NAME.' :: Scenarios');			
$output->output(); 

}
?>