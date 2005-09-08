<?
	$multiInputsCategs=array(103,77,126,98,109);
	$deviceID=(int)@$_REQUEST['deviceID'];
	$return=(int)@$_REQUEST['return'];
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
		
	if($action=='form'){
		switch((int)@$dtDataArray['FK_DeviceCategory'][0]){
			// Amps/Preamps/Receivers/Tuners
			case 103:
				$categoryInfo='<B>Amps/Preamps/Receivers/Tuners</B><br>Normally "Amps/Preamps/Receivers/Tuners" do have multiple inputs.  So normally you will leave #2 selected below, and then below that you will check off all the inputs this device has: "Video 1", "DVD", etc.  If this has a built-in tuner, like most do, then when you check off the inputs the tuner uses (like Tuner, AM, FM, etc.) be sure to also select the type of media on that input, such as "over the air radio".  All the other inputs which do not have built-in sources, in other words which you hook extra devices up to like a DVD player, leave at "external device".';
			break;
			// Audio Zone
			case 95:
				$categoryInfo='Audio Zone';
			break;
			// 	Cable Boxes
			case 105:
				$categoryInfo='<B>Cable Boxes</B><br>Normally "cable boxes" and "satellite boxes" do not have inputs; that is you do not plug other devices into them and they do not have multiple sources.  So we selected #1 below for you, and you probably do not need to make any changes unless your model is nonstandard.';
			break;
			// 	Cassette Decks
			case 104:
				$categoryInfo='Cassette Decks';
			break;
			// CD Players/Recorders
			case 106:
				$categoryInfo='<B>CD Players/Recorders</B><br>Normally "CD Players/Recorders" do not have inputs; that is you do not plug other devices into them and they do not have multiple sources.  So we selected #1 below for you, and you probably do not need to make any changes unless your model is nonstandard.';
			break;
			// Combo Units
			case 129:
				$categoryInfo='<B>Combo Units</B><br>If this is a combo unit that includes a VCR or DVD player, then for those inputs which have an internal device, select the corresponding media type ("VideoTape", "DVD").  And for those inputs which you connect an external device to, leave "External Device".';
			break;
			// DVD Players/Recorders
			case 107:
				$categoryInfo='<B>DVD Players/Recorders</B><br>Normally "DVD Players/Recorders" do not have inputs; that is you do not plug other devices into them and they do not have multiple sources.  So we selected #1 below for you, and you probably do not need to make any changes unless your model is nonstandard.';
			break;
			// Laser Disc
			case 135:
				$categoryInfo='<B>Laser Disc</B><br>Normally "Laser Discs" do not have inputs; that is you do not plug other devices into them and they do not have multiple sources.  So we selected #1 below for you, and you probably do not need to make any changes unless your model is nonstandard.';
			break;
			// Other/Misc
			case 136:
				$categoryInfo='Other/Misc';
			break;
			// PVR Capture Cards
			case 75:
				$categoryInfo='PVR Capture Cards';
			break;
			// Radios
			case 134:
				$categoryInfo='<B>Radios</B><br>If this "Radio" only has one frequency, then that means there is only one tuner or one source and you would leave #1 selected.  However, if the radio has multiple inputs, such as AM, FM, WB, etc., then you would select #2 if the remote control has separate buttons for each of the inputs, or #3 if there is only a single button that toggles between the inputs.';
			break;
			// Satellite Boxes
			case 108:
				$categoryInfo='Satellite Boxes';
			break;
			// Switches
			case 126:
				$categoryInfo='<B>Switches</B><br>Normally "Switches" do have multiple inputs.  So normally you will leave #2 selected below, and then below that you will check off all the inputs this device has: "Video 1", "DVD", etc.  Since this is just a Switch, and not a receiver, it probably does not have any built in sources on any of the inputs, so you can leave all the inputs as "external device".';
			break;
			// TVs
			case 77:
				$categoryInfo='<B>TVs</B><br>Normally TV\'s and Projectors do have multiple inputs, so you will probably select #2 or #3 below.  Hopefully it will have discrete buttons to select the various inputs.  Some TVs, like Sony, may only have a single toggle input button on the remote, but there exists separate, discrete buttons for all of the inputs.  On most TVs one or more inputs has a built-in source, for example "Tuner" may be a built-in source for live TV, while "Video 1" is an input for connecting an external device.  Some TVs have multiple tuners, ie "Tuner 1", "Tuner 2".  Off all the inputs this TV has, and select "Live TV" for each input that has a built-in tuner, and leave "external device" for the inputs you connect extra devices to.';
			break;
			// TV/VCR Combo
			case 109:
				$categoryInfo='<B>TV/VCR Combo</B><br>"Combo units" will have several built-in devices on various inputs, and perhaps also inputs for plugging in extra devices.  Check off all the inputs below, and if there is a built-in device on that input, choose the type of media it serves.  For example.  If there is a built-in DVD player, which you select with the "DVD" input, and a built-in VCR, which you select with the "video" input, and also an "Aux" input to plug in an external device, you would check off three inputs below: DVD, video, aux.  Select for the media type: "Dvd", "Video Tape" and "External Device".';
			break;
			// VCR
			case 98:
				$categoryInfo='VCR';
			break;
			
			default:
				$categoryInfo='&nbsp;';
			break;
		}
		
		$connectorsArray=getAssocArray('ConnectorType','PK_ConnectorType','Description',$publicADO,'','ORDER BY Description ASC');
		$mediaTypesArray=getAssocArray('MediaType','PK_MediaType','Description',$publicADO,'WHERE DCEAware=0','ORDER BY Description ASC');

		$inputSelectedTxt='
			<p class="normaltext"> Please select all the inputs this device has.  If your device has an input that is not on the list, but there is a close match, choose that.  It’s not important that the wording be exactly the same.  Only add a new input if your device’s input is totally unlike anything on this list.  Connector is optional.
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
					$checkedCommands[$row['PK_Command']]=$row['Input_Desc'];
					$rs=$publicADO->Execute('
					SELECT 
					FK_DeviceTemplate_ControlledVia,
					DeviceTemplate_DeviceTemplate_ControlledVia.FK_DeviceTemplate,
					FK_MediaType,FK_Command
					FROM DeviceTemplate_DeviceTemplate_ControlledVia
					LEFT JOIN DeviceTemplate_MediaType ON DeviceTemplate_MediaType.FK_DeviceTemplate=DeviceTemplate_DeviceTemplate_ControlledVia.FK_DeviceTemplate
					INNER JOIN DeviceTemplate_Input ON DeviceTemplate_DeviceTemplate_ControlledVia.FK_DeviceTemplate=DeviceTemplate_Input.FK_DeviceTemplate
					WHERE FK_DeviceTemplate_ControlledVia=? AND RerouteMessagesToParent=1 AND FK_Command=?',array($dtID,$row['PK_Command']));
					
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


		$inputSelectedTxt.='</table>
		<input type="hidden" name="oldMTArray" value="'.urlencode(serialize($oldMTArray)).'">
		<input type="hidden" name="oldCheckedCommands" value="'.join(',',array_keys($checkedCommands)).'">';
		
		if(!isset($_REQUEST['is'])){
			if(is_null($dtDataArray['FK_MediaType'][0])){
				// hard coded: if receivers, default 2
				//$is=($dtDataArray['FK_DeviceCategory'][0]==103)?2:1;
				$is=($dtDataArray['ToggleInput'][0]==0)?2:3;
			}else
				$is=1;	
		}else{
			$is=(int)$_REQUEST['is'];
		}
		
	if($return==0){
		$navigationButtons='<div align="right" class="normaltext"><a href="index.php?section=addModel&dtID='.$dtID.'&step='.($step-1).'&deviceID='.$deviceID.'">&lt;&lt;</a> <a href="index.php?section=addModel&dtID='.$dtID.'&step='.($step+1).'&deviceID='.$deviceID.'">&gt;&gt;</a></div>';
		$submitLabel='Next';
	}else{
		$submitLabel='Save';
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
		'.@$navigationButtons.'
		<B>Question 5 of 6 - What Inputs?</B><br><br>';
		$out.='
		<p class="normaltext">If this device has multiple inputs, or sources, you will check off all the inputs, and for each indicate if it is a built-in source, or if the input is for connecting an external device to.
		<p class="normaltext">'.$categoryInfo.'
		<form action="index.php" method="POST" name="addModel" onSubmit="setOrder();">
			<input type="hidden" name="section" value="addModel">
			<input type="hidden" name="step" value="'.$step.'">
			<input type="hidden" name="action" value="add">
			<input type="hidden" name="dtID" value="'.$dtID.'">
			<input type="hidden" name="deviceID" value="'.$deviceID.'">
			<input type="hidden" name="commandsOrder" value="">
			<input type="hidden" name="return" value="'.$return.'">
		';
		
		$out.='
		<table class="normaltext" cellpadding="5" cellspacing="0">
			<tr>
				<td>
					<input type="radio" name="is" value="1" '.(($is==1)?'checked':'').' onClick="self.location=\'index.php?section=addModel&step=5&dtID='.$dtID.'&is=1&deviceID='.$deviceID.'&return='.$return.'\'"> #1 - My device does not have multiple input sources, it only provides this 1 type of media: '.pulldownFromArray($mediaTypesArray,'dtMediaType',@$dtDataArray['FK_MediaType'][0],($is!=1)?'disabled':'').'
				</td>
			</tr>
			<tr>
				<td>
					<input type="radio" name="is" value="2" '.(($is==2)?'checked':'').' onClick="self.location=\'index.php?section=addModel&step=5&dtID='.$dtID.'&is=2&deviceID='.$deviceID.'&return='.$return.'\'"> #2 - My device does have multiple inputs, and there are separate, discrete buttons or commands to select the correct input (this type of device works well).
				</td>
			</tr>
			<tr>
				<td>
					<input type="radio" name="is" value="3" '.(($is==3)?'checked':'').' onClick="self.location=\'index.php?section=addModel&step=5&dtID='.$dtID.'&is=3&deviceID='.$deviceID.'&return='.$return.'\'"> #3 - My device does have multiple inputs, but unfortunately there is only a single button or command that toggles between all the inputs.  Note it is especially difficult to control a device, like a TV, that has lots of inputs and only has a single toggle button.  When hooking your equipment up you will likely want to use another device, like a receiver, that can handle the inputs discretely.  Regardless of how you hook it up, please specify the inputs this device has.		
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
				<td class="normaltext"><B> REMEMBER:</B> <em>If the device has a built-in source of media on that input, be sure you choose it from the ‘source’ list.  For example, if you have a receiver, normally the ‘am’ and ‘fm’ inputs have a built in source (over the air radio), but the ‘cd’ input is for an external device.  If you have a TV, normally the ‘tuner’ input is a built-in source (live tv) and video 1, 2, etc. are external.  For VCR’s there are normally only 2 inputs, tuner and VHS, and both have built-in sources (Live tv and videotape).  You may have a combo unit that has lots of built-in sources, like a built-in radio, vcr, dvd.  In that case be sure to indicate which inputs have built-in sources vs. external.  You don’t need to specify the connector type if you don’t know it; it’s only used when displaying a visual diagram of your equipment’s connections.</em></td>
			</tr>
			';
		}else{
			$out.='<input type="hidden" name="oldDT_MT" value="'.@$dtDataArray['FK_MediaType'][0].'">';
		}
		
			$out.='
			<tr>
				<td align="center"><input type="submit" class="button" name="next" value="'.$submitLabel.'"></td>
			</tr>
		</table>
		<br>
		</form>
		';
		
	}else{
		// process

		if($dtID!=0){
			$is=(int)$_REQUEST['is'];
			if($is==1){
				$dtMediaType=(int)$_POST['dtMediaType'];
				$oldDT_MT=(int)$_POST['oldDT_MT'];
				if($dtMediaType>0){
					if($oldDT_MT==0){
						$publicADO->Execute('INSERT INTO DeviceTemplate_MediaType (FK_DeviceTemplate,FK_MediaType) VALUES (?,?)',array($dtID,$dtMediaType));	
					}else{
						$publicADO->Execute('UPDATE DeviceTemplate_MediaType SET FK_MediaType=? WHERE FK_DeviceTemplate=? AND FK_MediaType=?',array($dtMediaType,$dtID,$oldDT_MT));	
					}
				}
			}else{
				$publicADO->Execute('DELETE FROM DeviceTemplate_MediaType WHERE FK_DeviceTemplate=?',$dtID);
				
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
							createEmbeddedDeviceTemplate($commandName.' - '.$mediaTypesArray[$mediaType],$dtDataArray['FK_Manufacturer'][0],$dtDataArray['FK_DeviceCategory'][0],$userID,$dtID,$commandID,$mediaType,$publicADO);
						}else{
							if(isset($oldMTArray[$commandID]['FK_MediaType']) && $oldMTArray[$commandID]['FK_MediaType']!=0){
								deleteEmbeddedDT($oldMTArray[$commandID]['FK_EmbeddedDevice'],$publicADO);
							}
						}
					}else{
						if(isset($oldMTArray[$commandID]['FK_MediaType']) && $oldMTArray[$commandID]['FK_MediaType']!=0){
							deleteEmbeddedDT($oldMTArray[$commandID]['FK_EmbeddedDevice'],$publicADO);
						}
						
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
		if($return==0){
			header('Location: index.php?section=addModel&step='.$nextStep.'&dtID='.$dtID.'&deviceID='.$deviceID);
		}elseif($is==3){
			header('Location: index.php?section=addModel&step=5b&dtID='.$dtID.'&deviceID='.$deviceID.'&return=1');
		}else{
			header('Location: index.php?section=irCodes&dtID='.$dtID.'&deviceID='.$deviceID);
		}	
		
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
