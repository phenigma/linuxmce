<?
/* @var $dbADO ADOConnection */
/* @var $rs ADORecordSet */

$out='<br>';
$dbADO->debug=false;
$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';

$docID=(isset($_REQUEST['docID']))?$_REQUEST['docID']:0;
$edit=(isset($_SESSION['editVar']))?$_SESSION['editVar']:0;

if($action=='form'){
	if($docID!=0){
		$queryDocument='SELECT * FROM Document Where PK_Document=?';
		$res=$dbADO->Execute($queryDocument,$docID);
		if($res->RecordCount()==0)
		$out.='<br><br><br>Document not found';
		else{
			$rowDocument=$res->FetchRow();

			getDocumentChildsNo($docID,$dbADO);
			$childsToDelete = $GLOBALS['childsDocumentNo'];

			$out.='
		<form action="right.php" method="post" name="documentDisplay" target="_self">
		<input type="hidden" name="section" value="'.$section.'">	
		<input type="hidden" name="action" value="edit">
		<input type="hidden" name="docID" value="'.$docID.'">
				<table border="0">';
			if(userIsAdmin(@$_SESSION['userID']) && $edit>0){
				$out.='
					<tr>
						<td><input type="button" name="addChild" value="Add child" onClick="javascript:self.location=\'right.php?section=documents/addDocument&parentID='.$docID.'\'"> <input type="button" name="deleteDocument" value="Delete" onClick="javascript: if (confirm(\'Are you sure you want to delete this document?'.($childsToDelete==1?' There is 1 child to delete!':($childsToDelete>0?'There are '.$childsToDelete.' childs to delete!':'')).'\')) self.location=\'right.php?section=documents/deleteDocument&docID='.$docID.'\';" /></td>
					</tr>';
			}
			$out.='
					<tr>
						<td><b>'.$rowDocument['Title'].'</b></td>
					</tr>
					<tr>
						<td>'.$rowDocument['Contents'].'</td>
					</tr>';
			if(userIsAdmin(@$_SESSION['userID']) && $edit>0){
				$out.='
					<tr>
						<td><input type="submit" name="edit" value="Edit"></td>
					</tr>';
			}
			$out.='
				</table>
		</form>
				';
		}
	}
}elseif($action=='edit'){
	$queryDocument='SELECT * FROM Document Where PK_Document=?';
	$res=$dbADO->Execute($queryDocument,$docID);
	if($res->RecordCount()==0)
	$out.='<br><br><br>Document not found';
	else{
		$rowDocument=$res->FetchRow();
		$out.='
		<form action="right.php" method="post" name="documentDisplay">
		<input type="hidden" name="section" value="'.$section.'">
		<input type="hidden" name="action" value="save">
		<input type="hidden" name="docID" value="'.$docID.'">
				<table>
					<tr>
						<td><b>Title</b></td>
						<td><input type="text" name="Title" value="'.((isset($_SESSION['docTitle']))?stripslashes($_SESSION['docTitle']):$rowDocument['Title']).'"></td>
					</tr>
					<tr>
						<td><b>Order</b></td>
						<td><input type="text" name="docOrder" value="'.((isset($_SESSION['docOrder']))?stripslashes($_SESSION['docOrder']):$rowDocument['Order']).'"></td>
					</tr>
					<tr>
						<td><b>Parent ID</b></td>
						<td><input type="text" name="parentID" value="'.((isset($_SESSION['parentID']))?stripslashes($_SESSION['parentID']):$rowDocument['FK_Document_Parent']).'"></td>
					</tr>
					<tr>
						<td><input type="submit" name="save" value="Save"></td>
						<td align="right"><input type="button" name="preview" value="Preview" onClick="document.documentDisplay.action.value=\'preview\';document.documentDisplay.submit();"></td>
					</tr>
					<tr>
						<td colspan="2"><textarea name="Contents" rows="25" cols="100">'.((isset($_SESSION['docContents']))?stripslashes($_SESSION['docContents']):$rowDocument['Contents']).'</textarea></td>
					</tr>
					<tr>
						<td><input type="submit" name="save" value="Save"></td>
						<td align="right"><input type="button" name="preview" value="Preview" onClick="document.documentDisplay.action.value=\'preview\';document.documentDisplay.submit();"></td>
					</tr>
				</table>
		</form>		
		';
		if(isset($_SESSION['docTitle'])){
			unset($_SESSION['docTitle']);
			unset($_SESSION['docContents']);
			unset($_SESSION['docOrder']);
			unset($_SESSION['parentID']);
		}


	}
}elseif($action=='preview'){
	$_SESSION['docTitle']=cleanString($_POST['Title']);
	$_SESSION['docContents']=stripslashes($_POST['Contents']);
	$_SESSION['docOrder']=cleanInteger($_POST['docOrder']);
	$_SESSION['parentID']=cleanInteger($_POST['parentID']);

	$out.='
		<form action="documentDisplay.php" method="post" name="documentDisplay">
		<input type="hidden" name="action" value="save">
		<input type="hidden" name="docID" value="'.$docID.'">
		<input type="hidden" name="Title" value="'.$_SESSION['docTitle'].'">
		<input type="hidden" name="Contents" value="'.$_SESSION['docContents'].'">
		<input type="hidden" name="parentID" value="'.$_SESSION['parentID'].'">				
		<input type="hidden" name="docOrder" value="'.$_SESSION['docOrder'].'">
				<table>
					<tr>
						<td><b>'.$_SESSION['docTitle'].'</b></td>
					</tr>
					<tr>
						<td>'.$_SESSION['docContents'].'</td>
					</tr>
					<tr>
						<td align="center"><input type="button" name="back" value="Back" onClick="self.location=\'right.php?section=documents/documentDisplay&action=edit&docID='.$docID.'\'"> <input type="submit" name="save" value="Save"></td>
					</tr>
				</table>
		</form>		
		';
}
else{
	if(isset($_POST['save'])){
		$contents=stripslashes($_POST['Contents']);
		$title=cleanString($_POST['Title']);
		$docID=cleanInteger($_POST['docID']);
		$parentID=cleanInteger($_POST['parentID']);
		$docOrder=cleanInteger($_POST['docOrder']);

		$getOldValues='SELECT * FROM Document WHERE PK_document=?';
		$resOldValues=$dbADO->Execute($getOldValues,$docID);
		$rowOldValues=$resOldValues->FetchRow();

		$updateDocument='UPDATE Document SET Title=?, Contents=?, `Order`=?, FK_Document_Parent=? WHERE PK_Document=?';
		$dbADO->Execute($updateDocument,array($title,$contents,$docOrder,$parentID,$docID));

		$out='
				<script>';
		if($rowOldValues['FK_Document_Parent']==$parentID && $rowOldValues['Order']==$docOrder)
		$out.='self.location="right.php?section=documents/documentDisplay&docID='.$docID.'"';
		else
		$out.='top.location="document.php?docID='.$docID.'"';
		$out.='</script>
			';

	}

}

$output->setHelpSrc('http://helpdevice.com/');
$output->setBody($out);
$output->setTitle(APPLICATION_NAME);
$output->output();

?>