<?php
	//$publicADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	
	if ($action=='form') {		
		$parameterTypes='<option value="0">-please select-</option>';
		$query = "select Description,PK_ParameterType from ParameterType order by Description asc";
		$res = $publicADO->Execute($query);
			if ($res) {
				while ($row = $res->FetchRow()) {
					$parameterTypes.="<option value='{$row['PK_ParameterType']}'>{$row['Description']}</option>\n";
				}
			}
		$out.='
		<form action="index.php" method="post" name="createParameterForCommand">
		<input type="hidden" name="section" value="createParameterForCommand">
		<input type="hidden" name="action" value="add">		
		<input type="hidden" name="from" value="'.$from.'">
			<table>			
				<tr>
					<td>Description:</td>
					<td><input type="text" size="15" name="Description" value=""></td>
				</tr>				
				<tr>
					<td>Parameter Type:</td>
					<td><select name="paramType">'.$parameterTypes.'</select></td>
				</tr>
				<tr>
					<td colspan="2" align="center"><input type="submit" class="button" name="submitX" value="Save"></td>
				</tr>
			</table>
		</form>
		<script>
		 	var frmvalidator = new formValidator("createParameterForCommand");
 			frmvalidator.addValidation("Description","req","Please enter a description");			
			frmvalidator.addValidation("paramType","dontselect=0","Please enter a parameter type!");			
		</script>
		';
		
	} else {
		
		
		
		$description = cleanString(@$_POST['Description'],50);		
		$parameterType = cleanInteger(@$_POST['paramType']);
		
		if ($description!='' && $parameterType!=0) {
			$queryInsertParameter = 'insert into CommandParameter(Description,FK_ParameterType) values(?,?)';
			$publicADO->Execute($queryInsertParameter,array($description,$parameterType));
			$packageID = $publicADO->Insert_ID();
						
			$out.="
				<script>
					alert('Parameter added!');					
				    opener.document.forms.{$from}.action.value='form';					
					opener.document.forms.{$from}.submit();
					self.close();
				</script>
				";		
		} else {
			header("Location: index.php?section=createParameterForCommand&from=$from");
		}
				
	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Command Parameter');			
	$output->output();
?>
