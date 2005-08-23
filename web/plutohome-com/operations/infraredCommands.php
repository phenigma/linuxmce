<?
function infraredCommands($output) {
	global $dbPlutoAdminServer,$dbPlutoAdminUser,$dbPlutoAdminPass,$dbPlutoAdminDatabase;
	include_once('include/plutoAdminUtils.inc.php');

	$publicADO = &ADONewConnection('mysql');
	$publicADO->NConnect($dbPlutoAdminServer,urlencode($dbPlutoAdminUser),urlencode($dbPlutoAdminPass),urlencode($dbPlutoAdminDatabase));
	
	/* @var $publicADO ADOConnection */
	/* @var $rs ADORecordSet */

	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
	$GLOBALS['excludedCategories']=array($GLOBALS['DSPModeCommandCategory'], $GLOBALS['InputsCommandCategory'],$GLOBALS['OutputsCommandCategory']);
	$GLOBALS['excludedCommands']=array($GLOBALS['genericONCommand'],$GLOBALS['genericOFFCommand'],$GLOBALS['powerCommand']);
	$GLOBALS['infraredGroup']=(int)@$_REQUEST['infraredGroup'];	
	$GLOBALS['deviceID']=((int)@$_REQUEST['deviceID']!=0)?(int)@$_REQUEST['deviceID']:NULL;	
	$GLOBALS['dtID']=(int)@$_REQUEST['dtID'];	
	
	if($action=='form'){
		
		$out.='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		</script>		
			<div align="center" class="err">'.@$_REQUEST['error'].'</div>
			<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
			<h3 align="center">Commands</h3>
			<form action="index.php" method="POST" name="infraredCommands">
			<input type="hidden" name="section" value="infraredCommands">
			<input type="hidden" name="action" value="update">
			<input type="hidden" name="infraredGroup" value="'.$GLOBALS['infraredGroup'].'">
			<input type="hidden" name="deviceID" value="'.$GLOBALS['deviceID'].'">
			<input type="hidden" name="dtID" value="'.$GLOBALS['dtID'].'">
		
		<table cellpadding="3" align="center" class="normaltext">
			<tr bgcolor="lightblue">
				<td align="center" width="120"><B>Command category</B></td>
				<td align="center"><B>Command description</b></td>
				<td align="center"><B>Action</b></td>
			</tr>
		';
		$GLOBALS['oldCheckedCommands']=array();
		$queryOld='SELECT DISTINCT FK_Command FROM InfraredGroup_Command WHERE (FK_Device=? OR FK_Device IS NULL) AND FK_DeviceTemplate=?';
		$resOld=$publicADO->Execute($queryOld,array($GLOBALS['deviceID'],$GLOBALS['dtID']));
		while($row=$resOld->FetchRow()){
			$GLOBALS['oldCheckedCommands'][]=$row['FK_Command'];
		}

		$GLOBALS['displayedCommands']=array();
		$query='
			SELECT DISTINCT
			Command.Description, 
			PK_Command,
			PK_CommandCategory,
			CommandCategory.Description AS CommandCategory
			FROM Command
			INNER JOIN CommandCategory ON FK_CommandCategory=PK_CommandCategory
			WHERE '.((isset($_REQUEST['rootNode']))?'FK_CommandCategory_Parent IS NULL':'FK_CommandCategory_Parent='.$GLOBALS['AVCommandCategory']).' AND PK_CommandCategory NOT IN ('.join(',',$GLOBALS['excludedCategories']).') 
			ORDER BY CommandCategory.Description ASC';
		$resRootCC=$publicADO->Execute($query);
		$cc=0;
		$cmdPos=0;
		while($rowRootCC=$resRootCC->FetchRow()){
			$GLOBALS['displayedCommands'][]=$rowRootCC['PK_Command'];
			if($cc!=$rowRootCC['PK_CommandCategory']){
				$cc=$rowRootCC['PK_CommandCategory'];
				$cmdPos=0;
				$out.='
				<tr bgcolor="#EEEEEE">
					<td colspan="3"><B>'.$rowRootCC['CommandCategory'].'</B></td>
				</tr>';
			}
			$cmdPos++;
			
			$out.='
				<tr>
					<td>&nbsp;</td>
					<td bgcolor="'.(($cmdPos%2==0)?'#FFFFFF':'#EBEFF9').'">'.$rowRootCC['Description'].'</td>
					<td bgcolor="'.(($cmdPos%2==0)?'#FFFFFF':'#EBEFF9').'" align="center"><input type="checkbox" name="command_'.$rowRootCC['PK_Command'].'" value="1" '.((in_array($rowRootCC['PK_Command'],$GLOBALS['oldCheckedCommands']))?'checked':'').'></td>
				</tr>';
		}

		$out.='
			<tr>
				<td colspan="3" align="center"><input type="submit" name="submit" class="button" value="Save"></td>
			</tr>		
		</table>
			<input type="hidden" name="displayedCommands" value="'.join(',',$GLOBALS['displayedCommands']).'">
			<input type="hidden" name="oldCheckedCommands" value="'.join(',',$GLOBALS['oldCheckedCommands']).'">
		</form>';
	}else{
	// process area
		$displayedCommands=explode(',',$_POST['displayedCommands']);
		$oldCheckedCommands=explode(',',$_POST['oldCheckedCommands']);

		foreach ($displayedCommands AS $commandID){
			if(isset($_POST['command_'.$commandID])){
				$GLOBALS['infraredGroup']=($GLOBALS['infraredGroup']==0)?NULL:$GLOBALS['infraredGroup'];
				if(!in_array($commandID,$oldCheckedCommands)){
					$publicADO->Execute('INSERT INTO InfraredGroup_Command (FK_InfraredGroup,FK_Command, FK_Device, FK_DeviceTemplate, FK_Users,psc_user) VALUES (?,?,?,?,?,?)',array($GLOBALS['infraredGroup'],$commandID, $GLOBALS['deviceID'],$GLOBALS['dtID'],$_SESSION['userID'],$_SESSION['userID']));
					
					$igcID=$publicADO->Insert_ID();
				}
			}else{
				if(in_array($commandID,$oldCheckedCommands)){
					$publicADO->Execute('DELETE FROM InfraredGroup_Command WHERE (FK_InfraredGroup=? OR FK_InfraredGroup IS NULL) AND FK_Command=? AND FK_Users=? AND psc_user=?',array($GLOBALS['infraredGroup'],$commandID,$_SESSION['userID'],$_SESSION['userID']));
				}
			}
		}
		
		$out.='
			<script>
				opener.location.reload();
				self.close();
			</script>';
		//header('Location: index.php?section=infraredCommands&infraredGroup='.$GLOBALS['infraredGroup'].'&msg=InfraredGroup - Commands updated');
	}
	
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}
?>