<?
	/* @var $publicADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
	$GLOBALS['dtID']=(int)@$_REQUEST['dtID'];	
	$GLOBALS['commandGroupID']=(int)@$_REQUEST['commandGroupID'];	// it's DeviceCommandGroupID !!!
	
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
			<form action="index.php" method="POST" name="commandsPicker">
			<input type="hidden" name="section" value="commandsPicker">
			<input type="hidden" name="action" value="update">
			<input type="hidden" name="dtID" value="'.$GLOBALS['dtID'].'">
			<input type="hidden" name="commandGroupID" value="'.$GLOBALS['commandGroupID'].'">
		
		<table cellpadding="3" align="center">
			<tr bgcolor="lightblue">
				<td align="center" width="120"><B>Command category</B></td>
				<td align="center"><B>Command description</b></td>
				<td align="center"><B>Action</b></td>
			</tr>
		';
		$GLOBALS['displayedCommands']=array();
		$GLOBALS['oldCheckedCommands']=array();
		$queryRootCC='
			SELECT Description,PK_CommandCategory
			FROM CommandCategory
			WHERE FK_CommandCategory_Parent IS NULL
			ORDER BY CommandCategory.Description ASC';
		$resRootCC=$publicADO->Execute($queryRootCC);
		$out.=formatOutput($resRootCC,$publicADO,0);
		$out.='
			<tr>
				<td colspan="3" align="center"><input type="submit" name="submit" class="button" value="Save"></td>
			</tr>		
			<tr>
				<td align="left" colspan="3"><a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=addCommandCategory&from=commandsPicker\',\'width=400,height=300,toolbars=true,resizable=1,scrollbars=1\');">Add Command Category</a></td>
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
				if(!in_array($commandID,$oldCheckedCommands)){
					$publicADO->Execute('INSERT INTO DeviceCommandGroup_Command (FK_DeviceCommandGroup,FK_Command) VALUES (?,?)',array($GLOBALS['commandGroupID'],$commandID));
				}
			}else{
				if(in_array($commandID,$oldCheckedCommands)){
					$publicADO->Execute('DELETE FROM DeviceCommandGroup_Command WHERE FK_DeviceCommandGroup=? AND FK_Command=?',array($GLOBALS['commandGroupID'],$commandID));
				}
			}
		}
		
		$out.='
			<script>
				opener.location.reload();
				self.close();
			</script>';
	}
	
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();


function getCommandCategoryChilds($CommandCategory,$publicADO,$level)
{
	$out='';
	$queryCC='
			SELECT Description,PK_CommandCategory
			FROM CommandCategory
			WHERE FK_CommandCategory_Parent=?
			ORDER BY CommandCategory.Description ASC';
	$resCC=$publicADO->Execute($queryCC,$CommandCategory);
	$out.=formatOutput($resCC,$publicADO,$level);
	return $out;
}

function formatOutput($resRootCC,$publicADO,$level)
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
				<td colspan="3">'.$indent.' <a href="#" onClick="windowOpen(\'index.php?section=editCommandCategory&from=commandsPicker&ccID='.$rowRootCC['PK_CommandCategory'].'\',\'width=400,height=300,toolbars=true,resizable=1,scrollbars=1\');"><B>'.$rowRootCC['Description'].'</B></a></td>
			</tr>';
		$resCommands=$publicADO->Execute('
			SELECT Command.*,FK_DeviceCommandGroup 
			FROM Command
			LEFT JOIN DeviceCommandGroup_Command ON FK_Command=PK_Command AND FK_DeviceCommandGroup=?
			WHERE FK_CommandCategory=?
			ORDER BY Command.Description ASC',array($GLOBALS['commandGroupID'],$rowRootCC['PK_CommandCategory']));
		$cmdPos=0;
		while($rowCommands=$resCommands->FetchRow()){
			$cmdPos++;
			$out.='
				<tr>
					<td>&nbsp;</td>
					<td bgcolor="'.(($cmdPos%2==0)?'#FFFFFF':'#EBEFF9').'"><a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=editCommand&from=commandsPicker&commandID='.$rowCommands['PK_Command'].'\',\'width=400,height=300,toolbars=true,resizable=1,scrollbars=1\');">'.$rowCommands['Description'].'<a></td>
					<td bgcolor="'.(($cmdPos%2==0)?'#FFFFFF':'#EBEFF9').'" align="center"><input type="checkbox" name="command_'.$rowCommands['PK_Command'].'" value="1" '.(($rowCommands['FK_DeviceCommandGroup']!='')?'checked':'').'></td>
				</tr>';
			$GLOBALS['displayedCommands'][]=$rowCommands['PK_Command'];
			if($rowCommands['FK_DeviceCommandGroup']!='')
				$GLOBALS['oldCheckedCommands'][]=$rowCommands['PK_Command'];
		}
		$out.=getCommandCategoryChilds($rowRootCC['PK_CommandCategory'],$publicADO,$level+1);
	}
	return $out;
}
?>