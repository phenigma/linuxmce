<?
function infraredCommands($output,$dbADO) {
	/* @var $dbADO ADOConnection */
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
		
		<table cellpadding="3" align="center">
			<tr bgcolor="lightblue">
				<td align="center" width="120"><B>Command category</B></td>
				<td align="center"><B>Command description</b></td>
				<td align="center"><B>Action</b></td>
			</tr>
		';
		$GLOBALS['displayedCommands']=array();
		$GLOBALS['oldCheckedCommands']=array();
		if(!isset($_REQUEST['rootNode'])){
			$queryRootCC='
				SELECT Description,PK_CommandCategory
				FROM CommandCategory
				WHERE FK_CommandCategory_Parent =? AND PK_CommandCategory NOT IN ('.join(',',$GLOBALS['excludedCategories']).')
				ORDER BY CommandCategory.Description ASC';
			$resRootCC=$dbADO->Execute($queryRootCC,$GLOBALS['AVCommandCategory']);
		}else{
			$queryRootCC='
				SELECT Description,PK_CommandCategory
				FROM CommandCategory
				WHERE FK_CommandCategory_Parent IS NULL 
				ORDER BY CommandCategory.Description ASC';
			$resRootCC=$dbADO->Execute($queryRootCC);
		
		}
		$out.=formatOutput($resRootCC,$dbADO,0);
		$out.='
			<tr>
				<td colspan="3" align="center"><input type="submit" name="submit" class="button" value="Save"></td>
			</tr>		
			<tr>
				<td align="left" colspan="3"><a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=addCommandCategory&from=infraredCommands\',\'width=400,height=300,toolbars=true,resizable=1,scrollbars=1\');">Add Command Category</a></td>
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
					$dbADO->Execute('INSERT INTO InfraredGroup_Command (FK_InfraredGroup,FK_Command, FK_Device, FK_DeviceTemplate, FK_Users) VALUES (?,?,?,?,?)',array($GLOBALS['infraredGroup'],$commandID, $GLOBALS['deviceID'],$GLOBALS['dtID'],$_SESSION['userID']));
					
					$igcID=$dbADO->Insert_ID();
					$dbADO->Execute('INSERT INTO InfraredGroup_Command_Preferred (FK_InfraredGroup_Command,FK_Installation) VALUES (?,?)',array($igcID,$_SESSION['installationID']));
				}
			}else{
				if(in_array($commandID,$oldCheckedCommands)){
					$dbADO->Execute('
						DELETE InfraredGroup_Command_Preferred 
						FROM InfraredGroup_Command_Preferred
						INNER JOIN InfraredGroup_Command ON FK_InfraredGroup_Command=PK_InfraredGroup_Command
						WHERE FK_InfraredGroup=?',$GLOBALS['infraredGroup']);
					$dbADO->Execute('DELETE FROM InfraredGroup_Command WHERE (FK_InfraredGroup=? OR FK_InfraredGroup IS NULL) AND FK_Command=? AND FK_Users=?',array($GLOBALS['infraredGroup'],$commandID,$_SESSION['userID']));
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


function getCommandCategoryChilds($CommandCategory,$dbADO,$level)
{
	$out='';
	$queryCC='
			SELECT Description,PK_CommandCategory
			FROM CommandCategory
			WHERE FK_CommandCategory_Parent=? AND PK_CommandCategory NOT IN ('.join(',',$GLOBALS['excludedCategories']).')
			ORDER BY CommandCategory.Description ASC';
	$resCC=$dbADO->Execute($queryCC,$CommandCategory);
	$out.=formatOutput($resCC,$dbADO,$level);
	return $out;
}

function formatOutput($resRootCC,$dbADO,$level)
{
	$installationID = (int)@$_SESSION['installationID'];
	$out='';
	$pos=0;
	while($rowRootCC=$resRootCC->FetchRow()){
		$pos++;
		$indent='';
		for($i=0;$i<3*$level;$i++)
			$indent.='&nbsp;-';

		$out.='
			<tr bgcolor="#EEEEEE">
				<td colspan="3">'.$indent.' <a href="#" onClick="windowOpen(\'index.php?section=editCommandCategory&from=infraredCommands&ccID='.$rowRootCC['PK_CommandCategory'].'\',\'width=400,height=300,toolbars=true,resizable=1,scrollbars=1\');"><B>'.$rowRootCC['Description'].'</B></a></td>
			</tr>';
//		$dbADO->debug=true;
		$resCommands=$dbADO->Execute('
			SELECT Command.*,PK_InfraredGroup_Command 
			FROM Command
			LEFT JOIN InfraredGroup_Command ON FK_Command=PK_Command AND (FK_InfraredGroup=? OR FK_InfraredGroup IS NULL) AND FK_Device=? AND FK_DeviceTemplate=?
			WHERE FK_CommandCategory=? AND PK_Command NOT IN ('.join(',',$GLOBALS['excludedCommands']).')
			ORDER BY Description ASC',array($GLOBALS['infraredGroup'],$GLOBALS['deviceID'],$GLOBALS['dtID'],$rowRootCC['PK_CommandCategory']));
		$dbADO->debug=false;
		$cmdPos=0;
		while($rowCommands=$resCommands->FetchRow()){
			$cmdPos++;
			$out.='
				<tr>
					<td>&nbsp;</td>
					<td bgcolor="'.(($cmdPos%2==0)?'#FFFFFF':'#EBEFF9').'"><a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=editCommand&from=infraredCommands&commandID='.$rowCommands['PK_Command'].'\',\'width=400,height=300,toolbars=true,resizable=1,scrollbars=1\');">'.$rowCommands['Description'].'<a></td>
					<td bgcolor="'.(($cmdPos%2==0)?'#FFFFFF':'#EBEFF9').'" align="center"><input type="checkbox" name="command_'.$rowCommands['PK_Command'].'" value="1" '.(($rowCommands['PK_InfraredGroup_Command']!='')?'checked':'').'></td>
				</tr>';
			$GLOBALS['displayedCommands'][]=$rowCommands['PK_Command'];
			if($rowCommands['PK_InfraredGroup_Command']!='')
				$GLOBALS['oldCheckedCommands'][]=$rowCommands['PK_Command'];
		}
		$out.=getCommandCategoryChilds($rowRootCC['PK_CommandCategory'],$dbADO,$level+1);
	}
	return $out;
}
?>