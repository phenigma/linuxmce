<?php
function addManufacturer($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/addManufacturer.lang.php');
	
	$out='';
	$action = isset($_POST['action'])?cleanString($_POST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	if ($action=='form') {
		$out.='
		<form action="index.php" method="post" name="addManufacturer">
		<input type="hidden" name="section" value="addManufacturer">
		<input type="hidden" name="action" value="add">
		<input type="hidden" name="from" value="'.$from.'">
			<h3>'.$TEXT_ADD_MANUFACTURER_CONST.'</h3>
			<table>			
				<tr>
					<td>'.$TEXT_MANUFACTURER_CONST.' *</td><td><input type="text" maxlength="50" name="manufacturerDescription" value="'.@$_SESSION['addManufacturer']['manufacturerDescription'].'"></td>
				</tr>
				<tr>
					<td colspan="2" align="center"><input type="submit" class="button" name="submitX" value="'.$TEXT_SAVE_CONST.'"  ></td>
				</tr>
			</table>
		</form>
		<em>* '.$TEXT_REQUIRED_FIELDS_CONST.'</em>
		<script>
		 	var frmvalidator = new formValidator("addManufacturer");
 			frmvalidator.addValidation("manufacturerDescription","req","'.$TEXT_VALIDATE_MANUFACTURER_NAME_CONST.'");
 			frmvalidator.addValidation("manufacturerDescription","maxlen=50","'.$TEXT_VALIDATE_MANUFACTURER_LENGTH_CONST.'"); 			
		</script>
		';
		$_SESSION['addManufacturer']['manufacturerDescription']='';
	} else {
		
		$manufacturerDescription = isset($_POST['manufacturerDescription'])?cleanString($_POST['manufacturerDescription']):'';
		if ($manufacturerDescription!='') {
			$insertManufacturer = 'insert into Manufacturer(Description) values(?)';
			$query = $dbADO->Execute($insertManufacturer,array($manufacturerDescription));
			$out.="
			<script>
				alert('$TEXT_MANUFACTURER_ADDED_NOTIFICATION_CONST');
			    opener.document.forms.{$from}.action.value='form';
				opener.document.forms.{$from}.lastAction.value='Manufacturer';//packagesCtrl
				opener.document.forms.{$from}.submit();
				self.close();
			</script>
			";
			$_SESSION['addManufacturer']['manufacturerDescription']='';
		} else {
			
			header("Location: index.php?section=addManufacturer");
		}		
	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_ADD_MANUFACTURER_CONST);			
	$output->output();
}
?>
