<?php
function createEventParameter($output,$dbADO) {
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	
	
	
	if ($action=='form') {
		
		
		$querySelectParameterType = "select PK_ParameterType,Description from ParameterType ORDER BY Description asc";
		$resSelectEventParameters = $dbADO->_Execute($querySelectParameterType);
		
		$eventParametersTxt = '<option value="0">-please select-</option>';
		if ($resSelectEventParameters) {
			while ($rowSelectEventParams=$resSelectEventParameters->FetchRow()) {
				$eventParametersTxt.= '<option '.(0==$rowSelectEventParams['PK_ParameterType']?' selected="selected" ':'').' value="'.$rowSelectEventParams['PK_ParameterType'].'">'.$rowSelectEventParams['Description'].'</option>';
			}
		}
		$out.='
		
		<form action="index.php" method="post" name="createEventParameter">
		<input type="hidden" name="section" value="createEventParameter">
		<input type="hidden" name="action" value="add">		
		<input type="hidden" name="from" value="'.$from.'">
			<table>	
				<tr>
					<td>
						Parameter Type:</td><td><select name="eventParameterType">'.$eventParametersTxt.'</select>							
					</td>
				</tr>		
				<tr>
					<td>Description:</td><td><input type="text" maxlength="50" name="eventParameterDescription" value=""></td>
				</tr>		
				<tr>
					<td>Comments:</td><td><textarea name="eventParameterComments" cols="40" rows="4"></textarea></td>
				</tr>		
				<tr>
					<td colspan="2" align="center">						
						<input type="submit" class="button" name="submitX" value="Save">
					</td>
				</tr>
			</table>
		</form>
		<script>
		 	var frmvalidator = new formValidator("createEventParameter");			
 			frmvalidator.addValidation("eventParameterDescription","req","Please enter a name for this Event Parameter !");
			frmvalidator.addValidation("eventParameterType","dontselect=0","Please select a parameter type!");
		</script>
		';
		
	} else {
		
		$eventParameterDescription = isset($_POST['eventParameterDescription'])?cleanString($_POST['eventParameterDescription']):'';
		$eventParameterComments = isset($_POST['eventParameterComments'])?cleanString($_POST['eventParameterComments']):'';
		$parameterType = cleanInteger($_POST['eventParameterType']);
		
		if ($parameterType!= 0 && $eventParameterDescription!='') {
			$insertEvent = 'insert into EventParameter (Description,Comments,FK_ParameterType) values(?,?,?)';
			$res=$dbADO->Execute($insertEvent,array($eventParameterDescription,$eventParameterComments,$parameterType));			
			$out.="
			<script>
				alert('Event Parameter created!');
			    opener.document.forms.{$from}.action.value='form';				
				opener.document.forms.{$from}.submit();
				self.close();
			</script>
			";			
		} else {				
			header("Location: index.php?section=createEventParameter&from=$from");
		}		
	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();
}
?>
