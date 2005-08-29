<?
	$deviceID=(int)@$_REQUEST['deviceID'];
	$multiInputsCategs=array(103,77,126,98,109);
	$mediaTypesDT=array(106,104,107,105,108,135);
	
	$dtID=$_REQUEST['dtID'];
	if($dtID==0){
		header('Location: index.php');
		exit();
	}
	
	$whereClause='
		INNER JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory 
		LEFT JOIN DeviceTemplate_AV ON DeviceTemplate_AV.FK_DeviceTemplate=PK_DeviceTemplate 
		LEFT JOIN DeviceTemplate_MediaType ON DeviceTemplate_MediaType.FK_DeviceTemplate=PK_DeviceTemplate 
		WHERE PK_DeviceTemplate='.$dtID;
	$dtDataArray=getFieldsAsArray('DeviceTemplate','FK_DeviceCategory,DeviceCategory.Description,FK_Manufacturer,FK_DeviceCategory,FK_MediaType,ToggleDSP',$publicADO,$whereClause);
	if(count($dtDataArray)==0){
		header('Location: index.php');
		exit();
	}

	$is=(isset($_REQUEST['is']))?(int)$_REQUEST['is']:1;
	
	
	if($action=='form'){
		
		$connectorsArray=getAssocArray('ConnectorType','PK_ConnectorType','Description',$publicADO,'','ORDER BY Description ASC');


		$queryDSPMode="
			SELECT 
				DeviceTemplate_DSPMode.*,
				Command.Description as DSPMode_Desc, 
				PK_Command
			FROM Command 	
			INNER JOIN DeviceTemplate_DSPMode ON PK_Command = FK_Command AND FK_DeviceTemplate='$dtID'
			WHERE FK_CommandCategory=21
			ORDER BY OrderNo ASC";
		$checkedCommands=array();
		$checkedCommands=array();
		$checkedCommandsAssoc=array();

		$resDSPMode= $publicADO->Execute($queryDSPMode);
		if ($resDSPMode) {
			while ($row=$resDSPMode->FetchRow()) {
				$checkedCommands[$row['PK_Command']]=$row['DSPMode_Desc'];
			}
		}

		$output->setScriptInHead(reorderMultiPulldownJs());
		
		$out='
		<script>
			
		function setOrder()
		{
			saveOrder="";
			for( var i = 0; i < document.addModel.orderItem.length; i++ ) { 
				if(i==0){
					saveOrder=document.addModel.orderItem[i].value;
				}else{
					saveOrder=saveOrder+","+document.addModel.orderItem[i].value;
				}
				document.addModel.commandsOrder.value=saveOrder;
			}
			return true;
		}
		</script>
		
		<br>
		<div align="right" class="normaltext"><a href="index.php?section=addModel&dtID='.$dtID.'&step=6&deviceID='.$deviceID.'">&lt;&lt;</a> <a href="index.php?section=addModel&dtID='.$dtID.'&step=7&deviceID='.$deviceID.'">&gt;&gt;</a></div>
		<B>Last Question, 6 - DSP mode order?</B><br><br>
		
		<form action="index.php" method="POST" name="addModel" onSubmit="setOrder();">
			<input type="hidden" name="section" value="addModel">
			<input type="hidden" name="step" value="'.$step.'">
			<input type="hidden" name="action" value="add">
			<input type="hidden" name="dtID" value="'.$dtID.'">
			<input type="hidden" name="commandsOrder" value="">
			<input type="hidden" name="deviceID" value="'.$deviceID.'">
			
			<table align="center">
				<tr>
					<td colspan="2"><B>Change order</B></td>
				</tr>
				<tr>
					<td colspan="2">Since this device uses 1 button to <b>toggle</b> DSP Modes, we need to know what order they are in so we can cycle through them.  Please confirm the order and click next.</td>
				</tr>
				<tr>				
					<td valign="top" align="right" width="50%">'.pulldownFromArray($checkedCommands,'orderItem',0,'size="10"','key','').'</td>
					<td valign="middle" align="left"><input type="button" class="button" value="U" onClick="moveUp(\'document.addModel.orderItem\');"> <br><input type="button" class="button" value="D" onClick="moveDown(\'document.addModel.orderItem\');"></td>
				</tr>
				<tr>
					<td colspan="2" class="normaltext"><input type="hidden" name="checkedCommands" value="'.urlencode(serialize($checkedCommands)).'"></td>
				</tr>
				<tr>
					<td align="center" colspan="2"><input type="submit" class="button" name="next" value="Next"></td>
				</tr>
			</table>
		<br>
		</form>
		';
		
	}else{
		// process

		if($dtID!=0){
			$checkedCommands=unserialize(urldecode($_POST['checkedCommands']));
			$commandOrder=array_flip(explode(',',$_POST['commandsOrder']));

				foreach ($checkedCommands AS $commandID=>$commandName){
					$publicADO->Execute('UPDATE DeviceTemplate_DSPMode SET OrderNo=? WHERE FK_DeviceTemplate=? AND FK_Command=?',array($commandOrder[$commandID],$dtID,$commandID));
				}
	
		}
		
		header('Location: index.php?section=addModel&step=7&dtID='.$dtID.'&deviceID='.$deviceID);
		exit();
	}
?>