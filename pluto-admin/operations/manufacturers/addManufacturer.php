<?php
function addManufacturer($output,$dbADO) {
	$out='';
	$action = isset($_POST['action'])?cleanString($_POST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	if ($action=='form') {
		$out.='
		<form action="index.php" method="post" name="addManufacturer">
		<input type="hidden" name="section" value="addManufacturer">
		<input type="hidden" name="action" value="add">
		<input type="hidden" name="from" value="'.$from.'">
			<table>			
				<tr>
					<td>Manufacturer:</td><td><input type="text" maxlength="50" name="manufacturerDescription" value="'.@$_SESSION['addManufacturer']['manufacturerDescription'].'"></td>
				</tr>
				<tr>
					<td colspan="2" align="center"><input type="submit" name="submitX" value="Save"></td>
				</tr>
			</table>
		</form>
		<script>
		 	var frmvalidator = new formValidator("addManufacturer");
 			frmvalidator.addValidation("manufacturerDescription","req","Please enter Manufacturer Name");
 			frmvalidator.addValidation("manufacturerDescription","maxlen=50","Max length for Manufacturer Name is 50"); 			
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
				alert('Manufacturer added!');
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
	$output->setTitle(APPLICATION_NAME.' :: Add manufacturer');			
	$output->output();
}
?>