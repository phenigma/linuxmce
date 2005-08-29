<?
	$dtID=$_REQUEST['dtID'];
	$deviceID=(int)@$_REQUEST['deviceID'];
	if($dtID==0){
		header('Location: index.php');
		exit();
	}
	
	$cdCategsArray=array(104,106,107,129,137);
	$dtDataArray=getFieldsAsArray('DeviceTemplate','DeviceTemplate.FK_DeviceCategory,DeviceCategory.Description,FK_CommMethod',$publicADO,'INNER JOIN DeviceCategory ON DeviceTemplate.FK_DeviceCategory=PK_DeviceCategory LEFT JOIN InfraredGroup ON FK_InfraredGroup=PK_InfraredGroup WHERE PK_DeviceTemplate='.$dtID);
	if(count($dtDataArray)==0){
		header('Location: index.php');
		exit();
	}
	$dtArray=getFieldsAsArray('DeviceTemplate_AV','NumericEntry',$publicADO,'WHERE FK_DeviceTemplate='.$dtID);
	
	$isEnter=(strpos(@$dtArray['NumericEntry'][0],'E')!==false)?1:0;
	$isEnter=((int)@$_REQUEST['isDef']==1)?1:$isEnter;
	
	$digits=str_replace('E','',@$dtArray['NumericEntry'][0]);
	$digits=((int)@$_REQUEST['isDef']==1)?'':$digits;
	
	$cdContent=(in_array($dtDataArray['FK_DeviceCategory'][0],$dtDataArray))?'<p class="normaltext">'.$dtDataArray['Description'][0].' devices don’t normally tune to stations or channels, so you can probably ignore this step and just click next.':'';
	
	
	if(@$_SESSION['selectedCommMethod']!=1){
		$comMethod=(isset($_SESSION['selectedCommMethod']))?$_SESSION['selectedCommMethod']:$dtDataArray['FK_CommMethod'][0];
		$commMethodArray=getAssocArray('CommMethod','PK_CommMethod','Description',$publicADO,'','ORDER BY PK_CommMethod ASC');
		$comMethodName=($comMethod!=0)?$commMethodArray[$comMethod]:'This';
		$Content='
			<p class="normaltext">'.$comMethodName.' devices don’t normally require any special rules to tune.<br><br><br>
			<div align="center"><input type="button" class="button" name="next" value="Next" onClick="self.location=\'index.php?section=addModel&step=4&dtID='.$dtID.'&deviceID='.$deviceID.'\'"></div>';
	}else{
		$Content='
		<table class="normaltext">
			<tr>
				<td>By default when it’s time to tune to a station, channel or frequency, we assume this device wants you to punch in the number on the remote, and then hit ‘enter’.  If that’s correct, or if there are no number buttons on the remote for this device, click next.  Otherwise:</td>
			</tr>
			<tr>
				<td>&nbsp;</td>
			</tr>		
			<tr>
				<td>Is there an Enter button on the remote you can hit after typing in numbers to make it tune right away? <input type="radio" name="enterButton" value="E" '.(($isEnter==1)?'checked':'').'> Yes <input type="radio" name="enterButton" value="" '.(($isEnter==0)?'checked':'').'> No</td>
			</tr>
			<tr>
				<td>&nbsp;</td>
			</tr>		
		
			<tr>
				<td>Does the number of digits need to be padded to a fixed length in order to tune right away?  Leave blank if ‘no’.  Enter 3 for example if you always enter 3 digits, such as 012 to tune to ‘12’ and 005 to tune to ‘5’. Number of digits: <input type="text" name="digits" value="'.$digits.'" size="2"></td>
			</tr>
			<tr>
				<td align="center">&nbsp;</td>
			</tr>		
			<tr>
				<td align="center"><input type="submit" class="button" name="next" value="Next"> </td>
			</tr>
		
		</table>		
		
		';
	}
	
	if($action=='form'){
		$out='<br>
		<div align="right" class="normaltext"><a href="index.php?section=addModel&dtID='.$dtID.'&step='.($step-1).(((int)@$_REQUEST['isDef']==1)?'&isDef=1':'').'&deviceID='.$deviceID.'">&lt;&lt;</a> <a href="index.php?section=addModel&dtID='.$dtID.'&step='.($step+1).(((int)@$_REQUEST['isDef']==1)?'&isDef=1':'').'&deviceID='.$deviceID.'">&gt;&gt;</a></div>
		<B>Question 3 of 6 - How to tune?</B><br><br>
		'.$cdContent.'
		
		<form action="index.php" method="POST" name="addModel">
			<input type="hidden" name="section" value="addModel">
			<input type="hidden" name="step" value="'.$step.'">
			<input type="hidden" name="action" value="add">
			<input type="hidden" name="dtID" value="'.$dtID.'">
			<input type="hidden" name="isDef" value="'.@$_REQUEST['isDef'].'">
			<input type="hidden" name="deviceID" value="'.$deviceID.'">
		
		'.$Content.'
		
		<br>
		</form>
	
		';
	}else{
		// process
		
		if($dtID!=0){
			$enterButton=$_POST['enterButton'];
			$digits=$_POST['digits'];
			$NumericEntry=$digits.$enterButton;
			$publicADO->Execute('UPDATE DeviceTemplate_AV SET NumericEntry=? WHERE FK_DeviceTemplate=?',array($NumericEntry,$dtID));
			header('Location: index.php?section=addModel&step=4&dtID='.$dtID.(((int)@$_REQUEST['isDef']==1)?'&isDef=1':'').'&deviceID='.$deviceID);
			exit();
		}
	}
?>
