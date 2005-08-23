<?

	$manufArray=getAssocArray('Manufacturer','PK_Manufacturer','Description',$publicADO,'WHERE PK_Manufacturer='.$mID,'ORDER BY Description ASC');
	$categArray=getAssocArray('DeviceCategory','PK_DeviceCategory','Description',$publicADO,'WHERE PK_DeviceCategory='.$dcID,'ORDER BY Description ASC');
	
	if($action=='form'){
		$out='
		<h3 align="center">Add model</h3><br>
		<p class="normaltext">
		
		<form action="index.php" method="POST" name="addModel">
			<input type="hidden" name="section" value="addModel">
			<input type="hidden" name="step" value="'.$step.'">
			<input type="hidden" name="action" value="add">
			<input type="hidden" name="mID" value="'.$mID.'">
			<input type="hidden" name="dcID" value="'.$dcID.'">
		
		
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
				<td colspan="4" align="center"><input type="submit" class="button" name="add" value="Add"></td>
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
			
			header('Location: index.php?section=deviceTemplatePicker&deviceCategSelected='.$dtID.'&manufacturers='.$mID.'&allowAdd=1');
			exit();
		}
		
		header('Location: index.php?section=deviceTemplatePicker&deviceCategSelected='.$dtID.'&manufacturers='.$mID.'&allowAdd=1');
	}
?>