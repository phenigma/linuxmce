<?
function dceDeviceData($output,$dbADO) {
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	
	
	if($action=='form'){
		
		$out.='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		</script>		
			<div align="center" class="err">'.@$_REQUEST['error'].'</div>
			<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
			<h3 align="center">Device Data</h3>
			<form action="index.php" method="POST" name="dceDeviceData">
			<input type="hidden" name="section" value="dceDeviceData">
			<input type="hidden" name="action" value="update">
		<table cellpadding="3" align="center">
			<tr bgcolor="lightblue">
				<td><B>Description</B></td>
				<td><B>Type</B></td>
				<td><B>Used by</B></td>
			</tr>';
		$queryDD='
			SELECT DeviceData.Description, ParameterType.Description AS Type, PK_DeviceData
			FROM DeviceData
			INNER JOIN ParameterType ON FK_ParameterType=PK_ParameterType
			ORDER BY DeviceData.Description ASC';
		$resDD=$dbADO->Execute($queryDD);
		$pos=0;
		while($rowDD=$resDD->FetchRow()){
			$queryDT_DD='
				SELECT DeviceTemplate.Description, PK_DeviceTemplate, DeviceTemplate_DeviceData.Description AS Tooltip
				FROM DeviceTemplate_DeviceData
				INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
				WHERE FK_DeviceData=?';
			$resDT_DD=$dbADO->Execute($queryDT_DD,$rowDD['PK_DeviceData']);
			$dtLinksArray=array();
			while($rowDT_DD=$resDT_DD->FetchRow()){
				$dtLinksArray[]='<a href="index.php?section=editMasterDevice&model='.$rowDT_DD['PK_DeviceTemplate'].'" title="'.$rowDT_DD['Tooltip'].'">'.$rowDT_DD['Description'].'</a>';
			}
			
			$pos++;
			$out.='
				<tr bgcolor="'.(($pos%2==0)?'#EEEEEE':'').'">
					<td><a href="javascript:void(0);" onClick="windowOpen(\'index.php?section=editDeviceData&from=dceDeviceData&deviceDataID='.$rowDD['PK_DeviceData'].'\',\'status=0,resizable=1,width=500,height=250,toolbars=true\');">'.$rowDD['Description'].'</a></td>
					<td>'.$rowDD['Type'].'</td>
					<td>'.join(', ',$dtLinksArray).'</td>
				</tr>';
		}
		$out.='
		</table>
		</form>';
	}else{
	// process area
			
		header('Location: index.php?section=dceDeviceData');
	}
	
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}