<?
function editCategory($output,$dbADO) {
/* @var $dbADO ADOConnection */
/* @var $rs ADORecordSet */
$out='';
//$dbADO->debug=false;


$from = cleanString($_REQUEST['from']);
$actionX = isset($_REQUEST['actionX'])?cleanString($_REQUEST['actionX']):'form';
$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'command';

$categSelected = (int)$_REQUEST['categSelected'];

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
	
	$query = "select * from $tableName
				WHERE $columnNamePK = ?";
	$res = $dbADO->Execute($query,array($categSelected));
	
	if ($res) {
		$row = $res->FetchRow();
		$parentID = $row[$columnNameFK];		
		$description = stripslashes($row['Description']);		
	}
	
	
	
	if ($actionX == 'form') {
	
	$out.='	
	<form method="post" action="index.php" name="editCategory">	
	<input type="hidden" name="from" value="'.$from.'">
	<input type="hidden" name="actionX" value="add">
	<input type="hidden" name="action" value="'.$action.'">
	<input type="hidden" name="section" value="editCategory">
	<input type="hidden" name="categSelected" value="'.$categSelected.'">	
	
	<table>
	<tr><td>Description:</td><td><input type="text" name="categoryDescription" value="'.$description.'" size="40"></td></tr>
	<tr><td>Parent:</td><td>
	<select name="parentID">
		<option value="0">-Is A Top Level Category-</option>
	';
	
	$queryCategories_parents = "select $columnNamePK,Description from $tableName where $columnNameFK is null and $columnNamePK != $categSelected order by Description asc";
		$rs = $dbADO->_Execute($queryCategories_parents);
							while ($row = $rs->FetchRow()) {
								$out.='<option '.($row[$columnNamePK]==$parentID?' selected ': ' ').' value="'.$row[$columnNamePK].'">'.$row['Description'].'</option>';								
								$out.=getChildsX($row[$columnNamePK],$row['Description'],$categSelected,$tableName,$columnNamePK,$columnNameFK,$dbADO);
							}
	
	$out.='		    </select>					
					</td>
				</tr>			
				<tr>
					<td colspan="2" align="center"><input type="submit" name="submitX" value="Save"></td>
				</tr>
	</table>
	</form>
	<script>
		 	var frmvalidator = new formValidator("editCategory");
 			frmvalidator.addValidation("categoryDescription","req","Please enter a description");
		</script>
	</table>
	';
	
	} else {
		$description = cleanString($_POST['categoryDescription']);		
		$parentID = cleanInteger($_POST['parentID']);
		if (trim($description)!='') {
			
			if ($parentID==0) {
				$query = "update $tableName set Description=?,$columnNameFK= NULL where $columnNamePK = $categSelected ";
			} else {
				$query = "update $tableName set Description=?,$columnNameFK= $parentID where $columnNamePK = $categSelected ";
			}
			$dbADO->Execute($query,array($description));
			$out.="
			<script>
					alert('$label Modified!');					
				    opener.document.forms.{$from}.actionX.value='form';
					opener.document.forms.{$from}.submit();
					self.close();
				</script>
			
			";
		} else {		
			header("Location: index.php?section=editCategory&action=$action&categSelected=$categSelected&from=$from");
		}
	}
				
$output->setBody($out);
$output->setTitle(APPLICATION_NAME);			
$output->output();  		
}

function getChildsX($parentID,$parentName,$selectedCateg,$table,$columnPK,$columnFK,$dbADO) {
	$outA='';
	$queryCategories_childs = "select * from $table where $columnFK = ? and $columnPK != $selectedCateg order by Description ASC";
	
		$rs2 = $dbADO->Execute($queryCategories_childs,array($parentID));
		while ($row2=$rs2->FetchRow()) {
			$outA.='<option '.($row2[$columnPK]==$selectedCateg?' selected ': ' ').' value="'.$row2[$columnPK].'">'.$parentName.' - '.$row2['Description'].'</option>';
			$outA.=getChildsX($row2[$columnPK],$parentName.' - '.$row2['Description'],$selectedCateg,$table,$columnPK,$columnFK,$dbADO);
		}		
	return $outA;
}
?>