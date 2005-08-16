<?
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
	$dtDataArray=getFieldsAsArray('DeviceTemplate','FK_DeviceCategory,DeviceCategory.Description,FK_Manufacturer,FK_DeviceCategory,FK_MediaType,ToggleInput',$publicADO,$whereClause);
	if(count($dtDataArray)==0){
		header('Location: index.php');
		exit();
	}

	$is=(isset($_REQUEST['is']))?(int)$_REQUEST['is']:1;
	
	
	if($action=='form'){
		
		$connectorsArray=getAssocArray('ConnectorType','PK_ConnectorType','Description',$publicADO,'','ORDER BY Description ASC');
		$mediaTypesArray=getAssocArray('MediaType','PK_MediaType','Description',$publicADO,'WHERE DCEAware=0','ORDER BY Description ASC');

		$inputSelectedTxt='
			<p class="normaltext"> Please select all the inputs this device has.  If your device has an input that is not on the list, but there is a close match, choose that.  It’s not important that the wording be exactly the same.  Only add a new input if your device’s input is totally unlike anything on this list.
			<table border="0" class="normaltext" align="center" cellpadding="3" cellspacing="0">
				<tr bgcolor="lightblue">
					<td align="center"><B>Inputs</b><br> (check all that apply)</td>
					<td align="center"><B>Source</B></td>
					<td align="center"><B>Connector</B></td>
				</tr>			
			';

		$queryInput="
			SELECT 
				DeviceTemplate_Input.*,
				Command.Description as Input_Desc, 
				FK_ConnectorType,
				PK_Command
			FROM Command 	
			LEFT JOIN DeviceTemplate_Input ON PK_Command = FK_Command AND DeviceTemplate_Input.FK_DeviceTemplate='$dtID'
			WHERE FK_CommandCategory=22
			ORDER BY Input_Desc ASC";
		$commandsArray=array();
		$checkedCommands=array();
		$checkedCommandsAssoc=array();
		$oldMTArray=array();

		$resInput= $publicADO->Execute($queryInput);
		if ($resInput) {
			while ($row=$resInput->FetchRow()) {
				$commandsArray[$row['PK_Command']]=$row['Input_Desc'];
				if(!is_null($row['FK_DeviceTemplate'])){
					$checkedCommands['PK_Command'][$row['OrderNo']]=$row['PK_Command'];
					$checkedCommands['Description'][$row['OrderNo']]=$row['Input_Desc'];
					
					$rs=$publicADO->Execute('
						SELECT FK_MediaType,FK_DeviceTemplate_ControlledVia,DeviceTemplate_MediaType.FK_DeviceTemplate 
						FROM DeviceTemplate_MediaType 
						INNER JOIN MediaType ON FK_MediaType=PK_MediaType
						INNER JOIN DeviceTemplate ON DeviceTemplate_MediaType.FK_DeviceTemplate=PK_DeviceTemplate AND DeviceTemplate.Description LIKE concat(\''.$row['Input_Desc'].' - \',MediaType.Description)
						LEFT JOIN DeviceTemplate_DeviceTemplate_ControlledVia ON DeviceTemplate_DeviceTemplate_ControlledVia.FK_DeviceTemplate=DeviceTemplate_MediaType.FK_DeviceTemplate AND AutoCreateChildren=1
						WHERE FK_DeviceTemplate_ControlledVia=?',$dtID);
					$rowMT=$rs->FetchRow();
				}
				$inputSelectedTxt.='
					<tr>
						<td><input type="checkbox" name="cmd_'.$row['PK_Command'].'" value="'.$row['Input_Desc'].'" '.((!is_null($row['FK_DeviceTemplate']))?'checked':'').' onClick="enableObjects('.$row['PK_Command'].');">'.$row['Input_Desc'].'</td>
						<td>'.pulldownFromArray($mediaTypesArray,'mediaType_'.$row['PK_Command'],@$rowMT['FK_MediaType'],((is_null($row['FK_DeviceTemplate']))?'disabled':''),'key','External device').'</td>
						<td>'.pulldownFromArray($connectorsArray,'connector_'.$row['PK_Command'],$row['FK_ConnectorType'],((is_null($row['FK_DeviceTemplate']))?'disabled':'')).'</td>
					</tr>';
				if(isset($rowMT['FK_MediaType'])){
					$oldMTArray[$row['PK_Command']]['FK_MediaType']=$rowMT['FK_MediaType'];
					$oldMTArray[$row['PK_Command']]['FK_EmbeddedDevice']=$rowMT['FK_DeviceTemplate'];
					unset($rowMT['FK_MediaType']);
				}
			}
		}
		
		$resInput->close();
		@ksort($checkedCommands['PK_Command']);

		if(count(@$checkedCommands['PK_Command'])>0){
			foreach ($checkedCommands['PK_Command'] AS $key=>$value){
				$checkedCommandsAssoc[$checkedCommands['PK_Command'][$key]]=$checkedCommands['Description'][$key];
			}
		}
		
		$inputSelectedTxt.='</table>
		<input type="hidden" name="oldMTArray" value="'.urlencode(serialize($oldMTArray)).'">
		<input type="hidden" name="oldCheckedCommands" value="'.join(',',array_keys($checkedCommandsAssoc)).'">';
		
		if(!isset($_REQUEST['is'])){
			if(count($checkedCommandsAssoc)==0 || count($checkedCommandsAssoc)==1){
				$is=1;
			}else{
				$is=($dtDataArray['ToggleInput'][0]==0)?2:3;
			}	
		}

		
		
		$out='
		<script>
		function enableObjects(val)
		{
			eval("flag=!document.addModel.cmd_"+val+".checked");
			eval("flag=document.addModel.mediaType_"+val+".disabled="+flag);
			eval("flag=document.addModel.connector_"+val+".disabled="+flag);
			if(flag==false){
				eval("newText=document.addModel.cmd_"+val+".value");
			}
		}
			
		</script>
		
		<br>
		<div align="right" class="normaltext"><a href="index.php?section=addModel&dtID='.$dtID.'&step='.($step-1).'">&lt;&lt;</a> <a href="index.php?section=addModel&dtID='.$dtID.'&step='.($step+1).'">&gt;&gt;</a></div>
		<B>Step 5 - Inputs</B><br><br>
		
		<form action="index.php" method="POST" name="addModel" onSubmit="setOrder();">
			<input type="hidden" name="section" value="addModel">
			<input type="hidden" name="step" value="'.$step.'">
			<input type="hidden" name="action" value="add">
			<input type="hidden" name="dtID" value="'.$dtID.'">
			<input type="hidden" name="commandsOrder" value="">

		';
		if(!in_array($dtDataArray['FK_DeviceCategory'][0],$multiInputsCategs)){
			$out.='<p class="normaltext">'.$dtDataArray['Description'][0].' devices normally don’t have multiple inputs that other devices can connect to, or multiple sources.  They just do one thing.<br><br>
 

If this device is something like a receiver or tv where you can connect other devices to it, then you will need to assign inputs.  A VCR also has multiple inputs, or sources, because it has two built in sources: a video cassette player and also a tuner for live tv, and there is usually a ‘toggle input’ button to switch between those 2 sources.';
		}
		if(in_array($dtDataArray['FK_DeviceCategory'][0],$mediaTypesDT)){
			$out.='<div class="normaltext" align="center"><B>Media Type:</B><br>'.getMediaTypeCheckboxes($dtID,$publicADO).'</div>';
		}
		
		$out.='
		<table class="normaltext" cellpadding="5" cellspacing="0">
			<tr>
				<td>
					<input type="radio" name="is" value="1" '.(($is==1)?'checked':'').' onClick="self.location=\'index.php?section=addModel&step=5&dtID='.$dtID.'&is=1\'"> My device does not have multiple input sources, it only provides this 1 type of media: '.pulldownFromArray(getAssocArray('MediaType','PK_MediaType','Description',$publicADO,'WHERE DCEAware=0','ORDER BY Description ASC'),'dtMediaType',0).'
				</td>
			</tr>
			<tr>
				<td>
					<input type="radio" name="is" value="2" '.(($is==2)?'checked':'').' onClick="self.location=\'index.php?section=addModel&step=5&dtID='.$dtID.'&is=2\'"> My device does have multiple inputs, and there are separate, discrete buttons or commands to select the correct input (discrete inputs work well).
				</td>
			</tr>
			<tr>
				<td>
					<input type="radio" name="is" value="3" '.(($is==3)?'checked':'').' onClick="self.location=\'index.php?section=addModel&step=5&dtID='.$dtID.'&is=3\'"> My device does have multiple inputs, but unfortunately there is only a single button or command that toggles between all the inputs.  VCR’s are almost always like this.  Note it is especially difficult to control a device, like a TV, that has lots of inputs and only has a single toggle button.  When hooking your equipment up you will likely want to use another device, like a receiver, that can handle the inputs.  Regardless of how you hook it up, please specify the inputs this device has.		
				</td>
			</tr>';
		if($is>1){
			$out.='
			<tr>
				<td align="center"><table>
					<tr>
						<td>'.$inputSelectedTxt.'</td>
					</tr>
				</table>
			</tr>
			<input type="hidden" name="commandsArray" value="'.urlencode(serialize($commandsArray)).'">
			<tr>
				<td class="normaltext"><B> NOTE:</B> <em>If the device has a built-in source on that input, be sure you choose it from the ‘source’ list.  For example, if you have a receiver, normally the ‘am’ and ‘fm’ inputs have a built in source (over the air radio), but the ‘cd’ input is for an external device.  If you have a TV, normally the ‘tuner’ input is a built-in source (live tv) and video 1, 2, etc. are external.  For VCR’s there are normally only 2 inputs, tuner and VHS, and both have built-in sources (Live tv and videotape).  You may have a combo unit that has lots of built-in sources, like a built-in radio, vcr, dvd.  In that case be sure to indicate which inputs have built-in sources vs. external.  You don’t need to specify the connector type if you don’t know it; it’s only used when displaying a visual diagram of your equipment’s connections.</em></td>
			</tr>
			';
		}
		
			$out.='
			<tr>
				<td align="center"><input type="submit" name="next" value="Next"> <input type="button" name="skip" value="Go to IR Codes" onclick="self.location=\'index.php?section=irCodes&dtID='.$dtID.'\'"></td>
			</tr>
		</table>
		<br>
		</form>
		';
		
	}else{
		// process

		if($dtID!=0){
			if($is==1){
				$dtMediaType=(int)$_POST['dtMediaType'];
				if($dtMediaType>0){
					$publicADO->Execute('INSERT INTO DeviceTemplate_MediaType (FK_DeviceTemplate,FK_MediaType) VALUES (?,?)',array($dtID,$dtMediaType));	
				}
			}else{
				$mediaTypesArray=getAssocArray('MediaType','PK_MediaType','Description',$publicADO,'','ORDER BY Description ASC');
				$commandsArray=unserialize(urldecode($_POST['commandsArray']));
				$oldMTArray=unserialize(urldecode($_POST['oldMTArray']));
				$oldCheckedCommands=explode(',',$_POST['oldCheckedCommands']);
				if($is==3){
					$commandOrder=array_flip(explode(',',$_POST['commandsOrder']));
				}

				foreach ($commandsArray AS $commandID=>$commandName){
					if(isset($_POST['cmd_'.$commandID])){
	
						$connectorType=((int)$_POST['connector_'.$commandID]>0)?(int)$_POST['connector_'.$commandID]:NULL;
						$mediaType=(int)$_POST['mediaType_'.$commandID];
	
						if(!in_array($commandID,$oldCheckedCommands)){
							$publicADO->Execute('INSERT IGNORE INTO DeviceTemplate_Input (FK_DeviceTemplate,FK_Command,FK_ConnectorType) VALUES (?,?,?)',array($dtID,$commandID,$connectorType));
						}else{
							$publicADO->Execute('UPDATE DeviceTemplate_Input SET FK_ConnectorType=? WHERE FK_DeviceTemplate=? AND FK_Command=?',array($connectorType,$dtID,$commandID));
						}

	
						if($mediaType!=0){
							if(isset($oldMTArray[$commandID]['FK_MediaType']) && $oldMTArray[$commandID]['FK_MediaType']!=$mediaType){
								deleteEmbeddedDT($oldMTArray[$commandID]['FK_EmbeddedDevice'],$publicADO);
							}
							
							// create embedded device template
							$publicADO->Execute('INSERT INTO DeviceTemplate (Description,FK_Manufacturer,FK_DeviceCategory,psc_user) VALUES (?,?,?,?)',array($commandName.' - '.$mediaTypesArray[$mediaType],$dtDataArray['FK_Manufacturer'][0],$dtDataArray['FK_DeviceCategory'][0],$userID));
							$embeddedID=$publicADO->Insert_ID();
							
							$publicADO->Execute('
								INSERT INTO DeviceTemplate_DeviceTemplate_ControlledVia 
									(FK_DeviceTemplate,FK_DeviceTemplate_ControlledVia,RerouteMessagesToParent,AutoCreateChildren)
								VALUES
									(?,?,1,1)',array($embeddedID,$dtID));
							$insertID=$publicADO->Insert_ID();
	
							$publicADO->Execute('
								INSERT INTO DeviceTemplate_DeviceTemplate_ControlledVia_Pipe
									(FK_DeviceTemplate_DeviceTemplate_ControlledVia,FK_Pipe,FK_Command_Input)
								VALUES 
									(?,?,?)',array($insertID,1,$commandID));
							$publicADO->Execute('
								INSERT INTO DeviceTemplate_DeviceTemplate_ControlledVia_Pipe
									(FK_DeviceTemplate_DeviceTemplate_ControlledVia,FK_Pipe,FK_Command_Input)
								VALUES 
									(?,?,?)',array($insertID,2,$commandID));
							$publicADO->Execute('INSERT INTO DeviceTemplate_Input (FK_DeviceTemplate,FK_Command) VALUES (?,?)',array($embeddedID,$commandID));	
							$publicADO->Execute('INSERT INTO DeviceTemplate_MediaType (FK_DeviceTemplate,FK_MediaType) VALUES (?,?)',array($embeddedID,$mediaType));	
						}else{
							if(isset($oldMTArray[$commandID]['FK_MediaType']) && $oldMTArray[$commandID]['FK_MediaType']!=0){
								deleteEmbeddedDT($oldMTArray[$commandID]['FK_EmbeddedDevice'],$publicADO);
							}
						}
					}else{
						
						if(in_array($commandID,$oldCheckedCommands)){
							$publicADO->Execute('DELETE FROM DeviceTemplate_Input WHERE FK_DeviceTemplate=? AND FK_Command=?',array($dtID,$commandID));
						}
					}
				}
				$ToggleInput=($is==2)?0:1;
				$publicADO->Execute('UPDATE DeviceTemplate_AV SET ToggleInput=? WHERE FK_DeviceTemplate=?',array($ToggleInput,$dtID));
			}
		}
		
		$nextStep=($is==3)?'5b':6;
		header('Location: index.php?section=addModel&step='.$nextStep.'&dtID='.$dtID);
		exit();
	}
	
function deleteEmbeddedDT($dtID,$publicADO){
	$publicADO->Execute('
	DELETE DeviceTemplate_DeviceTemplate_ControlledVia_Pipe 
	FROM DeviceTemplate_DeviceTemplate_ControlledVia_Pipe 
	INNER JOIN DeviceTemplate_DeviceTemplate_ControlledVia ON FK_DeviceTemplate_DeviceTemplate_ControlledVia=PK_DeviceTemplate_DeviceTemplate_ControlledVia
	WHERE FK_DeviceTemplate=?',$dtID);
	
	$publicADO->Execute('DELETE FROM DeviceTemplate_DeviceTemplate_ControlledVia WHERE FK_DeviceTemplate=?',$dtID);
	
	$publicADO->Execute('DELETE FROM DeviceTemplate WHERE PK_DeviceTemplate=?',$dtID);
}
?>