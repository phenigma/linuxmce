<?php
function addCommandGroup($output,$dbADO) {
	//$dbADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	
	$deviceID = (int)@$_REQUEST['deviceID'];

	$queryMasterDeviceCategories_parents = '
							select PK_DeviceCategory,Description from DeviceCategory where FK_DeviceCategory_Parent is null order by Description asc';
	$rs = $dbADO->_Execute($queryMasterDeviceCategories_parents);
	$deviceCategsTxt = '<option value="0">-please select-</option>';
		while ($row = $rs->FetchRow()) {
			$deviceCategsTxt.='<option '.($row['PK_DeviceCategory']==0?' selected ': ' ').' value="'.$row['PK_DeviceCategory'].'">'.$row['Description'].'</option>';
			$deviceCategsTxt.=getDeviceCategoryChildsOptions($row['PK_DeviceCategory'],$row['Description'],0,'',$dbADO);
		}
	$rs->Close();
						
	
						
	if ($action=='form') {		
		$out.='
		<form action="index.php" method="post" name="addCommandGroup">
		<input type="hidden" name="section" value="addCommandGroup">
		<input type="hidden" name="action" value="add">
		<input type="hidden" name="deviceID" value="'.$deviceID.'">
		<input type="hidden" name="from" value="'.$from.'">
			<table>			
				<tr>
					<td>Command group name:</td>
					<td><input type="text" size="15" name="CommandGroupDescription" value=""></td>
				</tr>
				<tr>
					<td>Device Category:</td>
					<td>
						<select name="deviceCategs" >
						'.$deviceCategsTxt.'
						</select>
					</td>
				</tr>
				<tr>
					<td colspan="2" align="center"><input type="submit" class="button" name="submitX" value="Save"></td>
				</tr>
			</table>
		</form>
		<script>
		 	var frmvalidator = new formValidator("addCommandGroup"); 			
			frmvalidator.addValidation("CommandGroupDescription","req","Please enter a name for this Command group!");
			frmvalidator.addValidation("deviceCategs","dontselect=0","Please select a device category!");
		</script>
		';
		
	} else {
		
		
		$deviceID = isset($_POST['deviceID'])?cleanString($_POST['deviceID']):'0';
		$commandGroupDescription = cleanString(@$_POST['CommandGroupDescription'],50);
		$deviceCategs = (int)@$_POST['deviceCategs'];
		
		
		if ($commandGroupDescription!='' && $deviceCategs!=0) {
			$queryInsertCmdGroup = 'insert into DeviceCommandGroup (Description,FK_DeviceCategory) values(?,?)';
			$dbADO->Execute($queryInsertCmdGroup,array($commandGroupDescription,$deviceCategs));
			$insertID = $dbADO->Insert_ID();
				$out.="
				<script>
					alert('Command group added!');
				    opener.document.forms.{$from}.action.value='form';
					opener.document.forms.{$from}.lastAction.value='DeviceCommandGroup_{$insertID}';
					opener.document.forms.{$from}.submit();
					self.close();
				</script>
				";			
				
		} else {
			header("Location: index.php?section=addCommandGroup&from=$from&deviceID=$deviceID");
		}
				
	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();
}
?>
