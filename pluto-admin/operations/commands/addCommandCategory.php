<?php
function addCommandCategory($output,$dbADO) {
	//$dbADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	
		
	$querySelectCategoryMainForDevice = "
		select  PK_CommandCategory,Description from CommandCategory 
		where  FK_CommandCategory_Parent IS NULL
		order by Description asc
	";
	$commandsCategsTxt = '<option value="0">-please select-</option>';
	$rs = $dbADO->_Execute($querySelectCategoryMainForDevice);
	while ($row = $rs->FetchRow()) {
				$commandsCategsTxt.='<option value="'.$row['PK_CommandCategory'].'">'.$row['Description'].'</option>';
				$querySelectCategoryForDevice = "
					select  PK_CommandCategory,Description from CommandCategory
					where  
						FK_CommandCategory_Parent = {$row['PK_CommandCategory']}
					order by Description asc
						";
				$rs2 = $dbADO->_Execute($querySelectCategoryForDevice);				
					while ($row2 = $rs2->FetchRow()) {
						$commandsCategsTxt.='<option value="'.$row2['PK_CommandCategory'].'">'.$row['Description'].'-'.$row2['Description'].'</option>';
					}
			
	}
	$rs->Close();
						
	if ($action=='form') {		
		$out.='
		<form action="index.php" method="post" name="addCommandCategory">
		<input type="hidden" name="section" value="addCommandCategory">
		<input type="hidden" name="action" value="add">		
		<input type="hidden" name="from" value="'.$from.'">
			<table>			
				<tr>
					<td>Command category name:</td>
					<td><input type="text" size="15" name="CommandCategoryDescription" value=""></td>
				</tr>				
				<tr>
					<td>Command category parent:</td>
					<td>
						<select name="parentCateg" >
						'.$commandsCategsTxt.'
						</select>
					</td>
				</tr>
				<tr>
					<td colspan="2" align="center"><input type="submit" name="submitX" value="Add"></td>
				</tr>
			</table>
		</form>
		<script>
		 	var frmvalidator = new formValidator("addCommandCategory"); 			
			frmvalidator.addValidation("CommandCategoryDescription","req","Please enter a name for this Command Category!");
		</script>
		';
		
	} else {

		$commandCategoryDescription = cleanString(@$_POST['CommandCategoryDescription'],50);		
		$parentCateg = ((int)@$_POST['parentCateg']!=0)?(int)@$_POST['parentCateg']:NULL;

		
		if ($commandCategoryDescription!='') {
		
			$dbADO->Execute('INSERT INTO CommandCategory (Description,FK_CommandCategory_Parent) VALUES (?,?)',array($commandCategoryDescription,$parentCateg));
			
				$out.="
				<script>
					alert('Command Category added!');
				    opener.document.forms.{$from}.action.value='form';
					opener.document.forms.{$from}.submit();
					self.close();
				</script>
				";			
				
		} else {
			header("Location: index.php?section=addCommandCategory&from=$from");
		}
				
	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Add command');			
	$output->output();
}
?>