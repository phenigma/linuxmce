<?
function dceCommands($output,$dbADO) {
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
			<h3 align="center">Commands</h3>
			<form action="index.php" method="POST" name="dceCommands">
			<input type="hidden" name="section" value="dceCommands">
			<input type="hidden" name="action" value="update">
		<table cellpadding="3" align="center">
			<tr bgcolor="lightblue">
				<td align="center" width="120"><B>Command category</B></td>
				<td align="center"><B>Command description</b></td>
				<td align="center"><B>Used by</B></td>
			</tr>
		';
		
		$queryRootCC='
			SELECT Description,PK_CommandCategory
			FROM CommandCategory
			WHERE FK_CommandCategory_Parent IS NULL
			ORDER BY CommandCategory.Description ASC';
		$resRootCC=$dbADO->Execute($queryRootCC);
		$out.=formatOutput($resRootCC,$dbADO,0);
		$out.='
			<tr>
				<td align="left" colspan="3"><a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=addCommandCategory&from=dceCommands\',\'width=400,height=300,toolbars=true,resizable=1,scrollbars=1\');">Add Command Category</a></td>
			</tr>
		</table>
		</form>';
	}else{
	// process area
			
		header('Location: index.php?section=dceCommands');
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
			WHERE FK_CommandCategory_Parent=?
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
				<td colspan="3">'.$indent.' <a href="#" onClick="windowOpen(\'index.php?section=editCommandCategory&from=dceCommands&ccID='.$rowRootCC['PK_CommandCategory'].'\',\'width=400,height=300,toolbars=true,resizable=1,scrollbars=1\');"><B>'.$rowRootCC['Description'].'</B></a></td>
			</tr>';
		$resCommands=$dbADO->Execute('SELECT * FROM Command WHERE FK_CommandCategory=? ORDER BY Description ASC',$rowRootCC['PK_CommandCategory']);
		$cmdPos=0;
		while($rowCommands=$resCommands->FetchRow()){
			$cmdPos++;
			$queryCG_C='
				SELECT Description,PK_CommandGroup 
				FROM CommandGroup_Command
				INNER JOIN CommandGroup ON FK_CommandGroup=PK_CommandGroup
				WHERE FK_Command=? AND FK_Installation=?
				ORDER BY CommandGroup.Description ASC';
			$resCG_C=$dbADO->Execute($queryCG_C,array($rowCommands['PK_Command'],$installationID));
			$cgLinksArray=array();
			while($rowCG_C=$resCG_C->FetchRow()){
				$cgLinksArray[]='<a href="index.php?section=editCommandGroup&cgID='.$rowCG_C['PK_CommandGroup'].'">'.$rowCG_C['Description'].'</a>';
			}
			$out.='
				<tr>
					<td>&nbsp;</td>
					<td bgcolor="'.(($cmdPos%2==0)?'#FFFFFF':'#EBEFF9').'"><a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=editCommand&from=dceCommands&commandID='.$rowCommands['PK_Command'].'\',\'width=400,height=300,toolbars=true,resizable=1,scrollbars=1\');">'.$rowCommands['Description'].'<a></td>
					<td bgcolor="'.(($cmdPos%2==0)?'#FFFFFF':'#EBEFF9').'">'.join(', ',$cgLinksArray).'</td>
				</tr>';
		}
		$out.=getCommandCategoryChilds($rowRootCC['PK_CommandCategory'],$dbADO,$level+1);
	}
	return $out;
}
?>