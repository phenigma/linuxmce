<?
	$mID=$_REQUEST['mID'];
	$dcID=$_REQUEST['dcID'];
	$deviceID=(int)@$_REQUEST['deviceID'];

	$manufArray=getAssocArray('Manufacturer','PK_Manufacturer','Description',$publicADO,'WHERE PK_Manufacturer='.$mID,'ORDER BY Description ASC');
	$categArray=getAssocArray('DeviceCategory','PK_DeviceCategory','Description',$publicADO,'WHERE PK_DeviceCategory='.$dcID,'ORDER BY Description ASC');
	
	$commMethodArray=getAssocArray('CommMethod','PK_CommMethod','Description',$publicADO,'','ORDER BY PK_CommMethod ASC');
	$commMethodRadioButtons='';
	$selectedCommMethod=(isset($_SESSION['selectedCommMethod']))?$_SESSION['selectedCommMethod']:1;
	foreach ($commMethodArray AS $key=>$value){
		$commMethodRadioButtons.='<input type="radio" name="commMethod" value="'.$key.'" '.(($key==$selectedCommMethod)?'checked':'').'>'.$value.'<br>';
	}
	
	
	if($action=='form'){
		$out='
		<h3 align="center">Add model</h3><br>
		<B>Question 1 of 6 - How to control?</B><br>
		<p class="normaltext">We need to ask a few basic questions about this device, which allows you to control it much more intuitively <a href="http://plutohome.com/support/index.php?section=document&docID=216">without having to write macros!</a> 
		 This should only take a minute or two and we’ll try to fill in default answers for you whenever possible.
		
		<form action="index.php" method="POST" name="addModel">
			<input type="hidden" name="section" value="addModel">
			<input type="hidden" name="step" value="'.$step.'">
			<input type="hidden" name="action" value="add">
			<input type="hidden" name="mID" value="'.$mID.'">
			<input type="hidden" name="dcID" value="'.$dcID.'">
			<input type="hidden" name="deviceID" value="'.$deviceID.'">
		
		<table class="normaltext" align="center">
			<tr>
				<td align="right"><B>Manufacturer</B></td>
				<td>'.$manufArray[$mID].'</td>
				<td><B>Device Category</B></td>
				<td>'.$categArray[$dcID].'</td>
			</tr>
			<tr>
				<td><B>What is the name or model #:</B></td>
				<td colspan="3"><input type="text" name="description" value=""></td>
			</tr>
			<tr>
				<td><B>How do you control this model:</B></td>
				<td colspan="3">'.$commMethodRadioButtons.'</td>
			</tr>
			<tr>
				<td colspan="4" align="center"><input type="submit" class="button" name="add" value="next"></td>
			</tr>
		</table><br>
		</form>
		
		<script>
			var frmvalidator = new formValidator("addModel");
 			frmvalidator.addValidation("description","req","Please type the name of the model.");
		</script>		
		';
	}else{
		// process
		$description=stripslashes($_POST['description']);
		
		if($dcID!=0 && $mID!=0 && $description!=''){
			$publicADO->Execute('INSERT INTO DeviceTemplate (Description,FK_DeviceCategory,FK_Manufacturer,psc_user) values(?,?,?,?)',array($description,$dcID,$mID,$userID));
			$dtID=$publicADO->Insert_ID();
			
			$publicADO->Execute('INSERT INTO DeviceTemplate_AV (FK_DeviceTemplate,psc_user) VALUES (?,?)',array($dtID,$userID));			
			$_SESSION['selectedCommMethod']=(int)$_POST['commMethod'];
			
			// hardcoded corespondence between AV categories cd, casette, dvd etc. and media types
			$dc_mtArray=array(106=>19,107=>12,108=>11,105=>11,135=>17);

			if(isset($dc_mtArray[$dcID])){
				$publicADO->Execute('INSERT IGNORE INTO DeviceTemplate_MediaType (FK_DeviceTemplate,FK_MediaType) VALUES (?,?)',array($dtID,$dc_mtArray[$dcID]));
			}
			// hardcoded: add embedded tv and vcr if device category is TV/VCR combo
			
			if($dcID==109 || $dcID==98){
				$publicADO->Execute('INSERT INTO DeviceTemplate_Input (FK_DeviceTemplate,FK_Command) VALUES (?,?)',array($dtID,161));
				createEmbeddedDeviceTemplate('TV - LiveTV',$mID,$dcID,$userID,$dtID,161,11,$publicADO);
				
				$publicADO->Execute('INSERT INTO DeviceTemplate_Input (FK_DeviceTemplate,FK_Command) VALUES (?,?)',array($dtID,282));
				createEmbeddedDeviceTemplate('VCR-1 - Video Tape',$mID,$dcID,$userID,$dtID,282,16,$publicADO);
			}
			
			header('Location: index.php?section=addModel&step=2&dtID='.$dtID.'&isDef=1&deviceID='.$deviceID);
			exit();
		}
		
		header('Location: index.php?section=addModel&step=1&mID='.$mID.'&dcID='.$dcID.'&deviceID='.$deviceID);
	}
?>