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
				<td align="center"><B>#</b></td>
				<td align="center"><B>Command description</b></td>
				<td align="center" colspan="2"><B>Used by</B></td>
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
				<td align="left" colspan="4"><a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=addCommandCategory&from=dceCommands\',\'width=400,height=300,toolbars=true,resizable=1,scrollbars=1\');">Add Command Category</a></td>
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
				<td align="center"><B>Device Templates</B></td>
				<td align="center"><B>Devices</B></td>
			</tr>';
		$resCommands=$dbADO->Execute('SELECT * FROM Command WHERE FK_CommandCategory=? ORDER BY Description ASC',$rowRootCC['PK_CommandCategory']);
		$cmdPos=0;
		while($rowCommands=$resCommands->FetchRow()){
			$cmdPos++;
			$query='
				SELECT DeviceTemplate_DeviceCommandGroup.FK_DeviceTemplate, DeviceTemplate.Description AS TemplateName,PK_Device,Device.Description AS DeviceName
				FROM DeviceTemplate_DeviceCommandGroup
				INNER JOIN DeviceTemplate ON DeviceTemplate_DeviceCommandGroup.FK_DeviceTemplate=PK_DeviceTemplate
				INNER JOIN Device ON DeviceTemplate_DeviceCommandGroup.FK_DeviceTemplate=Device.FK_DeviceTemplate
				INNER JOIN DeviceCommandGroup_Command ON DeviceCommandGroup_Command.FK_DeviceCommandGroup=DeviceTemplate_DeviceCommandGroup.FK_DeviceCommandGroup
				WHERE FK_Command=? AND Device.FK_Installation=?';
			$res=$dbADO->Execute($query,array($rowCommands['PK_Command'],$installationID));
			$deviceTemplates=array();
			$devices=array();
			while($row=$res->FetchRow()){
				if(!in_array($row['TemplateName'],$deviceTemplates))
					$deviceTemplates[$row['FK_DeviceTemplate']]=$row['TemplateName'];
				if(!in_array($row['DeviceName'],$devices))
					$devices[$row['PK_Device']]=$row['DeviceName'];
			}
			$dtLinks=array();
			foreach($deviceTemplates as $dtID=>$description){
				$dtLinks[]='<a href="index.php?section=editMasterDevice&model='.$dtID.'&from=dceCommands">'.$description.'</a>';
			}
			
			$devicesLinks=array();
			foreach($devices as $deviceID=>$description){
				$devicesLinks[]='<a href="index.php?section=editDeviceParams&deviceID='.$deviceID.'">'.$description.'</a>';
			}

			$out.='
				<tr>
					<td>&nbsp;</td>
					<td bgcolor="'.(($cmdPos%2==0)?'#FFFFFF':'#EBEFF9').'">'.$rowCommands['PK_Command'].'</td>
					<td bgcolor="'.(($cmdPos%2==0)?'#FFFFFF':'#EBEFF9').'"><a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=editCommand&from=dceCommands&commandID='.$rowCommands['PK_Command'].'\',\'width=400,height=300,toolbars=true,resizable=1,scrollbars=1\');">'.$rowCommands['Description'].'<a></td>
					<td bgcolor="'.(($cmdPos%2==0)?'#FFFFFF':'#EBEFF9').'">'.join(', ',$dtLinks).'</td>
					<td bgcolor="'.(($cmdPos%2==0)?'#FFFFFF':'#EBEFF9').'">'.join(', ',$devicesLinks).'</td>
				</tr>';
		}
		$out.=getCommandCategoryChilds($rowRootCC['PK_CommandCategory'],$dbADO,$level+1);
	}
	return $out;
}
?>