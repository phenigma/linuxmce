<?php
	//$publicADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	$FK_Package_Directory = cleanInteger(@$_REQUEST['Package_Directory']);
	
	$deviceID = (int)@$_REQUEST['deviceID'];
		
	if ($action=='form') {		
		$out.='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		</script>
		<form action="index.php" method="post" name="addDirectoryFile">
		<input type="hidden" name="section" value="addDirectoryFile">
		<input type="hidden" name="action" value="add">
		<input type="hidden" name="deviceID" value="'.$deviceID.'">
		<input type="hidden" name="from" value="'.$from.'">
		<input type="hidden" name="Package_Directory" value="'.$FK_Package_Directory.'">
			<table>			
				<tr>
					<td>File:</td>
					<td><input type="text" size="15" name="File" value=""></td>
				</tr>
				<tr>
					<td>Search:</td>
					<td><textarea name="Search" rows="10" colls="30"></textarea></td>
				</tr>
				<tr>
					<td colspan="2" align="center"><input type="submit" class="button" name="submitX" value="Save"></td>
				</tr>
			</table>
		</form>
		<script>
		 	var frmvalidator = new formValidator("addDirectoryFile");
 			frmvalidator.addValidation("File","req","Please enter a file name");
		</script>
		';
		
	} else {
		
		
		$deviceID = isset($_POST['deviceID'])?cleanString($_POST['deviceID']):'0';
		$File = cleanString(@$_POST['File'],30);
		$Search = cleanString(@$_POST['Search'],1000);
		
		if ($File!='') {
			$queryInsertDirectoryFile = 'INSERT INTO Package_Directory_File (FK_Package_Directory,File,Search) values(?,?,?)';
			$publicADO->Execute($queryInsertDirectoryFile,array($FK_Package_Directory,$File,$Search));
			
			$out.="
				<script>
					alert('File added!');					
				    opener.document.forms.{$from}.action.value='form';
					opener.document.forms.{$from}.submit();
					self.close();
				</script>
				";		
		} else {
			header("Location: index.php?section=addPackageToMasterDevice&from=$from&deviceID=$deviceID&subAction=edit");
		}
				
	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();
?>
