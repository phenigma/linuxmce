<?php
function deleteCategory($output,$dbADO) {
	//$dbADO->debug=true;
	$out='';
	
	$from = cleanString($_REQUEST['from']);		
	$categSelected = (int)$_REQUEST['categSelected'];
	
	$action = (isset($_REQUEST['action']) && cleanString($_REQUEST['action'])!='null')?cleanString($_REQUEST['action']):'';

		
	if ($action=='commands') {
		$label = "Commands Category";
		$tableName = 'CommandCategory';		
		$columnNamePK = 'PK_CommandCategory';	
		$columnNameFK = 'FK_CommandCategory_Parent';
	} elseif ($action=='events') {
		$label = "Events Category";
		$tableName = 'EventCategory';
		$columnNamePK = 'PK_EventCategory';	
		$columnNameFK = 'FK_EventCategory_Parent';
	}

	$GLOBALS['childsFromAnyCategory'] = array();
	$x=getChildsForCategory($tableName,$columnNamePK,$columnNameFK,$categSelected,$dbADO);
	$childsToDeleteAray = cleanArray($GLOBALS['childsFromAnyCategory']);

	$GLOBALS['childsFromAnyCategory'][]=$categSelected;
	

	if ((int)$categSelected!=0) {
			$deleteObjFromCateg = 'delete from '.$tableName.' where '.$columnNamePK.' IN 
			('.join(",",$GLOBALS['childsFromAnyCategory']).')';
			$query = $dbADO->_Execute($deleteObjFromCateg);
			$out.="
			<script>
				alert('$label deleted!');
			    opener.document.forms.{$from}.actionX.value='form';				
				opener.document.forms.{$from}.submit();
				self.close();
			</script>
			";			
		} else {
			$out = 'Nothing to delete.&nbsp;<a href="javascript:window.close();">Close</a>';
		}		
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();
}
?>