<?php
function editDeviceData($output,$dbADO) {
	//$dbADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	
	$deviceID = (int)@$_REQUEST['deviceID'];
	$deviceDataID = (int)@$_REQUEST['deviceDataID'];
		
	$queryDeviceData = "select * from DeviceData where PK_DeviceData = $deviceDataID";
	$res = $dbADO->_Execute($queryDeviceData);
	 if ($res) {
	 	$row=$res->FetchRow();
	 	$ddDescription = $row['Description'];
	 	$ddParameterType = $row['FK_ParameterType'];
	 }
	 
	$queryParams = "select * from ParameterType order by Description";
	$resParams = $dbADO->_Execute($queryParams);
	$paramsSelect = '<option value="0">-please select-</option>';
	if ($resParams) {
		while ($rowParams = $resParams->FetchRow()) {
			$paramsSelect.='<option '.($ddParameterType==$rowParams['PK_ParameterType']?' selected="selected" ':'').' value="'.$rowParams['PK_ParameterType'].'">'.$rowParams['Description'].'</option>';
		}
	}
	$resParams->Close();
	if ($action=='form') {		
		$out.='
		<form action="index.php" method="post" name="editDeviceData">
		<input type="hidden" name="section" value="editDeviceData">
		<input type="hidden" name="action" value="add">
		<input type="hidden" name="deviceID" value="'.$deviceID.'">
		<input type="hidden" name="deviceDataID" value="'.$deviceDataID.'">
		<input type="hidden" name="from" value="'.$from.'">
			<table>			
				<tr>
					<td>Parameter Name:</td>
					<td><input type="text" size="15" name="ParameterName" value="'.$ddDescription.'"></td>
				</tr>
				<tr>
					<td>Parameter Type:</td>
					<td>
						<select name="ParameterType" >
						'.$paramsSelect.'
						</select>
					</td>
				</tr>
				<tr>
					<td colspan="2" align="center"><input type="submit" name="submitX" value="Save"></td>
				</tr>
			</table>
		</form>
		<script>
		 	var frmvalidator = new formValidator("editDeviceData"); 			
			frmvalidator.addValidation("ParameterName","req","Please enter a parameter name!");
			frmvalidator.addValidation("ParameterType","dontselect=0","Please select a parameter type!");
		</script>
		';
		
	} else {
		
		
		$deviceID = isset($_POST['deviceID'])?cleanString($_POST['deviceID']):'0';
		$parameterName = cleanString(@$_POST['ParameterName'],50);
		$parameterType = (int)@$_POST['ParameterType'];
		
		
		if ($parameterName!='' && $parameterType!=0 && $deviceDataID!=0) {
			$queryInsertParameter = 'update DeviceData set Description=?,FK_ParameterType=? where PK_DeviceData = ?';
			$dbADO->Execute($queryInsertParameter,array($parameterName,$parameterType,$deviceDataID));
			
			if($from=='editMasterDevice'){
				$out.="
				<script>
					alert('Parameter modified!');
				    opener.document.forms.{$from}.action.value='form';
					opener.document.forms.{$from}.lastAction.value='Data_Description_$deviceDataID';
					opener.document.forms.{$from}.submit();
					self.close();
				</script>
				";			
			}else{
				$out.="
				<script>
					alert('Parameter modified!');
				    opener.location.reload();
					self.close();
				</script>
				";			
			}
		} else {
			header("Location: index.php?section=editDeviceData&from=$from&deviceID=$deviceID&deviceDataID=$deviceDataID");
		}
				
	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();
}
?>