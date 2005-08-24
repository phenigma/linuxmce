<?
	$dtID=$_REQUEST['dtID'];
	
	$dtArray=getFieldsAsArray('DeviceTemplate_AV','IR_PowerDelay,IR_ModeDelay, DigitDelay',$publicADO,'WHERE FK_DeviceTemplate='.$dtID);

	if($action=='form'){
		$out='<br>
		<div align="right" class="normaltext"><a href="index.php?section=addModel&dtID='.$dtID.'&step='.($step+1).(((int)@$_REQUEST['isDef']==1)?'&isDef=1':'').'">&gt;&gt;</a></div>
		<B>Step 2 - Delays</B><br>
		<p class="normaltext">Most devices need some delays between commands.  We filled in the most common values for you.  Change them if necessary; you can also make changes later if these values do not work:
		
		<form action="index.php" method="POST" name="addModel">
			<input type="hidden" name="section" value="addModel">
			<input type="hidden" name="step" value="'.$step.'">
			<input type="hidden" name="action" value="add">
			<input type="hidden" name="dtID" value="'.$dtID.'">
			<input type="hidden" name="isDef" value="'.@$_REQUEST['isDef'].'">
		
		<table class="normaltext" align="center">
			<tr>
				<td>After sending a code to turn the power on wait <input type="text" name="IR_PowerDelay" value="'.(((int)@$_REQUEST['isDef']==1)?'7':@$dtArray['IR_PowerDelay'][0]).'" size="2"> seconds for the device to warm up before sending other codes.</td>
			</tr>
			<tr>
				<td>After changing inputs or modes on this device wait <input type="text" name="IR_ModeDelay" value="2" size="'.(((int)@$_REQUEST['isDef']==1)?'2':@$dtArray['IR_ModeDelay'][0]).'"> seconds before sending other codes.</td>
			</tr>		
			<tr>
				<td>When sending a series of codes, such as a sequence of digits to tune to a channel, wait <input type="text" name="DigitDelay" value="'.(((int)@$_REQUEST['isDef']==1)?'0.250':round(@$dtArray['DigitDelay'][0]/1000,3)).'" size="2"> seconds* between commands (up to 3 decimal places).</td>
			</tr>		
			<tr>
				<td align="center"><input type="submit" class="button" name="add" value="next"> <input type="button" class="button" name="skip" value="skip a/v properties wizard and just show me i/r codes" onclick="self.location=\'index.php?section=irCodes&dtID='.$dtID.'\'"></td>
			</tr>
			<tr>
				<td align="center">&nbsp;</td>
			</tr>
			<tr>
				<td><B>* </B> <em>The smaller the number the faster the device will be controlled. But if the number is too small codes may be sent faster than the device can recognize them. For example, you try to tune to channel 125. That means three codes, or buttons, are sent: 1, 2 and 5. But if your device goes to channel 15, then codes are probably coming in too fast and the device lost one of the codes so you’ll need to come back and increase this.</em></td>
			</tr>		
		</table><br>
		</form>
	
		';
	}else{
		// process
		if($dtID!=0){
			$IR_PowerDelay=$_POST['IR_PowerDelay'];
			$IR_ModeDelay=$_POST['IR_ModeDelay'];
			$DigitDelay=1000*$_POST['DigitDelay'];
			
			$publicADO->Execute('UPDATE DeviceTemplate_AV SET IR_PowerDelay=?, IR_ModeDelay=?, DigitDelay=? WHERE FK_DeviceTemplate=?',array($IR_PowerDelay,$IR_ModeDelay,$DigitDelay,$dtID));
			header('Location: index.php?section=addModel&step=3&dtID='.$dtID.(((int)@$_REQUEST['isDef']==1)?'&isDef=1':''));
			exit();
		}
		
		header('Location: index.php?section=addModel&step=2&dtID='.$dtID.(((int)@$_REQUEST['isDef']==1)?'&isDef=1':''));
	}
?>