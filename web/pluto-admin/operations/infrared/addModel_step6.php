<?
	$ampReceivers=array(103);
	
	$dtID=$_REQUEST['dtID'];
	if($dtID==0){
		header('Location: index.php');
		exit();
	}
	
	$dtDataArray=getFieldsAsArray('DeviceTemplate','FK_DeviceCategory,DeviceCategory.Description,FK_Manufacturer,FK_DeviceCategory,ToggleDSP',$publicADO,'INNER JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory LEFT JOIN DeviceTemplate_AV ON DeviceTemplate_AV.FK_DeviceTemplate=PK_DeviceTemplate  WHERE PK_DeviceTemplate='.$dtID);
	if(count($dtDataArray)==0){
		header('Location: index.php');
		exit();
	}

	$dsp=(isset($_REQUEST['dsp']))?(int)$_REQUEST['dsp']:1;
	
	if($action=='form'){
		$inputSelectedTxt='
				<table border="0" class="normaltext" align="center" cellpadding="3" cellspacing="0">
					<tr bgcolor="lightblue">
						<td align="center"><B>DSP Modes</b><br> (check all that apply)</td>
					</tr>			
			';
		$queryDSPMode="
				SELECT 
					DeviceTemplate_DSPMode.*,
					Command.Description as DSPMode_Desc, 
					PK_Command
				FROM Command 	
				LEFT JOIN DeviceTemplate_DSPMode ON PK_Command = FK_Command AND FK_DeviceTemplate='$dtID'
				WHERE FK_CommandCategory=21
				ORDER BY DSPMode_Desc ASC";
		$commandsArray=array();
		$checkedCommands=array();
		$checkedCommandsAssoc=array();

		$resDSPMode= $publicADO->Execute($queryDSPMode);
		if ($resDSPMode) {
			while ($row=$resDSPMode->FetchRow()) {
				$commandsArray[$row['PK_Command']]=$row['DSPMode_Desc'];
				if(!is_null($row['FK_DeviceTemplate'])){
					$checkedCommands['PK_Command'][$row['OrderNo']]=$row['PK_Command'];
					$checkedCommands['Description'][$row['OrderNo']]=$row['DSPMode_Desc'];
				}
				$inputSelectedTxt.='
							<tr>
								<td><input type="checkbox" name="cmd_'.$row['PK_Command'].'" value="'.$row['DSPMode_Desc'].'" '.((!is_null($row['FK_DeviceTemplate']))?'checked':'').' onClick="enableObjects('.$row['PK_Command'].');">'.$row['DSPMode_Desc'].'</td>
							</tr>';
			}
		}
		$resDSPMode->close();
		@ksort($checkedCommands['PK_Command']);

		if(count(@$checkedCommands['PK_Command'])>0){
			foreach ($checkedCommands['PK_Command'] AS $key=>$value){
				$checkedCommandsAssoc[$checkedCommands['PK_Command'][$key]]=$checkedCommands['Description'][$key];
			}
		}

		$inputSelectedTxt.='</table>
			<input type="hidden" name="oldCheckedCommands" value="'.join(',',array_keys($checkedCommandsAssoc)).'">';

		
		if(!isset($_REQUEST['dsp'])){
			if(count($checkedCommandsAssoc)==0){
				$dsp=1;
			}else{
				$dsp=($dtDataArray['ToggleDSP'][0]==0)?2:3;
			}	
		}
		
		$out='
		<script>
		function enableObjects(val)
		{
			eval("flag=!document.addModel.cmd_"+val+".checked");
			if(flag==false){
				eval("newText=document.addModel.cmd_"+val+".value");
				addToPulldown(\'document.addModel.orderItem\',val,newText)

			}else{
				removeFromPulldown(\'document.addModel.orderItem\',val);
			}
		}
			
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
		<div align="right" class="normaltext"><a href="index.php?section=addModel&dtID='.$dtID.'&step='.($step-1).'">&lt;&lt;</a> <a href="index.php?section=addModel&dtID='.$dtID.'&step='.($step+1).'">&gt;&gt;</a></div>
		<B>Last Question, 6 - DSP mode?</B><br><br>
		
		<form action="index.php" method="POST" name="addModel" onSubmit="setOrder();">
			<input type="hidden" name="section" value="addModel">
			<input type="hidden" name="step" value="'.$step.'">
			<input type="hidden" name="action" value="add">
			<input type="hidden" name="dtID" value="'.$dtID.'">
			<input type="hidden" name="commandsOrder" value="">

		';
		if(!in_array($dtDataArray['FK_DeviceCategory'][0],$ampReceivers)){
			$out.='<p class="normaltext">'.$dtDataArray['Description'][0].' devices normally don’t have multiple DSP Modes, like “Church”, “Concert hall”, “Dolby Digital”, etc.  You can probably ignore this step and click next.<br><br>';
		}else{
			$out.='<p class="normaltext">If this device has multiple DSP Modes, like “Church”, “Concert hall”, “Dolby Digital”, please check off all the modes. .  If your device has a dspmode that is not on the list, but there is a close match, choose that.  It’s not important that the wording be exactly the same.  Only add a new dsp mode if your device’s input is totally unlike anything on this list.';
		}
		
		$out.='
		<table class="normaltext" cellpadding="5" cellspacing="0">
			<tr>
				<td>
					<input type="radio" name="dsp" value="1" '.(($dsp==1)?'checked':'').' onClick="self.location=\'index.php?section=addModel&step=6&dtID='.$dtID.'&dsp=1\'"> My device doesn’t have DSP Modes
				</td>
			</tr>
			<tr>
				<td>
					<input type="radio" name="dsp" value="2" '.(($dsp==2)?'checked':'').' onClick="self.location=\'index.php?section=addModel&step=6&dtID='.$dtID.'&dsp=2\'"> My device does have DSP Modes, and there are separate, discrete buttons to select the modes (this works well)
				</td>
			</tr>
			<tr>
				<td>
					<input type="radio" name="dsp" value="3" '.(($dsp==3)?'checked':'').' onClick="self.location=\'index.php?section=addModel&step=6&dtID='.$dtID.'&dsp=3\'"> My device does have DSP Modes, but unfortunately there’s just 1 button that toggles through all the modes so it will be difficult to control		
				</td>
			</tr>';
		if($dsp>1){
			
			$out.='
			<tr>
				<td align="center">'.$inputSelectedTxt.'</td>
			</tr>
			<input type="hidden" name="commandsArray" value="'.urlencode(serialize($commandsArray)).'">
			';
		}
		
			$out.='
			<tr>
				<td align="center"><input type="submit" class="button" name="next" value="Next"></td>
			</tr>
		</table>
		<br>
		</form>
	
		';
	}else{
		// process
		
		if($dtID!=0){
			if($dsp!=1){
				$commandsArray=unserialize(urldecode($_POST['commandsArray']));
				$oldCheckedCommands=explode(',',$_POST['oldCheckedCommands']);
				if($dsp==3){
					$commandOrder=array_flip(explode(',',$_POST['commandsOrder']));
				}
				foreach ($commandsArray AS $commandID=>$commandName){
					if(isset($_POST['cmd_'.$commandID])){
						if(!in_array($commandID,$oldCheckedCommands)){
							$publicADO->Execute('INSERT IGNORE INTO DeviceTemplate_DSPMode (FK_DeviceTemplate,FK_Command) VALUES (?,?)',array($dtID,$commandID));
						}
						
					}else{
						if(in_array($commandID,$oldCheckedCommands)){
							$publicADO->Execute('DELETE FROM DeviceTemplate_DSPMode WHERE FK_DeviceTemplate=? AND FK_Command=?',array($dtID,$commandID));
						}
					}
				}

				$ToggleDSP=($dsp==2)?0:1;
				$publicADO->Execute('UPDATE DeviceTemplate_AV SET ToggleDSP=? WHERE FK_DeviceTemplate=?',array($ToggleDSP,$dtID));
				
			}

			$nextStep=($dsp==3)?'6b':7;
			header('Location: index.php?section=addModel&step='.$nextStep.'&dtID='.$dtID);
			exit();
		}
	}
?>