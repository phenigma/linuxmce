<?
	$dtID=$_REQUEST['dtID'];
	$deviceID=(int)@$_REQUEST['deviceID'];
	$return=(int)@$_REQUEST['return'];
	if($dtID==0){
		header('Location: index.php');
		exit();
	}
	$dtArray=getFieldsAsArray('DeviceTemplate_AV','TogglePower,FK_InfraredGroup',$publicADO,'INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE FK_DeviceTemplate='.$dtID);
	if($return==0){
		$navigationButtons='<div align="right" class="normaltext"><a href="index.php?section=addModel&dtID='.$dtID.'&step='.($step-1).'&deviceID='.$deviceID.'">&lt;&lt;</a> <a href="index.php?section=addModel&dtID='.$dtID.'&step='.($step+1).'&deviceID='.$deviceID.'">&gt;&gt;</a></div>';
		$submitLabel='Next';
	}else{
		$submitLabel='Save';
	}	
	$irg=(int)$dtArray['FK_InfraredGroup'][0];
	
	if($action=='form'){
		$out='<br>
		'.@$navigationButtons.'
		<B>Question 4 of 6 - Toggle power or discrete?</B><br><br>
		
		<form action="index.php" method="POST" name="addModel">
			<input type="hidden" name="section" value="addModel">
			<input type="hidden" name="step" value="'.$step.'">
			<input type="hidden" name="action" value="add">
			<input type="hidden" name="dtID" value="'.$dtID.'">
			<input type="hidden" name="deviceID" value="'.$deviceID.'">
			<input type="hidden" name="return" value="'.$return.'">

		<table class="normaltext" cellpadding="10" cellspacing="0">
			<tr>
				<td>Discrete power means there are separate on and off commands or buttons.  So you can send the ‘on’ command and even if the device is already on, it won’t turn off.  This makes it easy to control the device.</td>
			</tr>
			<tr>
				<td>Toggle means there is only 1 button which toggles between on and off, called ‘Toggle Power’.  You hit it once and it’s on, hit it again and it’s off.  Whenever possible avoid devices with toggle because then it’s possible to get out of sync, where the system sends a Toggle Power to turn it on, but in fact it really turned it off.</td>
			</tr>
			<tr>
				<td style="padding-left:50px;"><input type="radio" name="TogglePower" value="0" '.((@$dtArray['TogglePower'][0]==0)?'checked':'').'> My device has discrete, separate on/off commands<br>
					<input type="radio" name="TogglePower" value="1" '.((@$dtArray['TogglePower'][0]==1)?'checked':'').'> My device only has a single toggle power button</td>
			</tr>
			<tr>
				<td align="center">&nbsp;</td>
			</tr>		
			<tr>
				<td align="center"><input type="submit" class="button" name="next" value="'.$submitLabel.'"> </td>
			</tr>
		
		</table>
		
		<br>
		</form>
	
		';
	}else{
		// process
		
		if($dtID!=0){
			$TogglePower=(int)$_POST['TogglePower'];
			
			$publicADO->Execute('UPDATE DeviceTemplate_AV SET TogglePower=? WHERE FK_DeviceTemplate=?',array($TogglePower,$dtID));
			
			$commandsToRemove=($TogglePower==1)?'192,193':'194';
			$removeFilter=($irg!=0)?' FK_InfraredGroup='.$irg:' FK_DeviceTemplate='.$dtID;

			$publicADO->Execute('DELETE InfraredGroup_Command_Preferred FROM InfraredGroup_Command_Preferred INNER JOIN InfraredGroup_Command ON FK_InfraredGroup_Command=PK_InfraredGroup_Command WHERE FK_Command in ('.$commandsToRemove.') AND (IRData IS NULL or IRData=\'\') AND '.$removeFilter);
			$publicADO->Execute('DELETE FROM InfraredGroup_Command WHERE FK_Command in ('.$commandsToRemove.') AND (IRData IS NULL or IRData=\'\') AND '.$removeFilter);
			
			if($return==0){
				header('Location: index.php?section=addModel&step=5&dtID='.$dtID.'&deviceID='.$deviceID);
			}else{
				header('Location: index.php?section=irCodes&dtID='.$dtID.'&deviceID='.$deviceID);
			}			

			exit();
		}
	}
?>