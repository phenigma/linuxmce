<?
include("scripts/fckeditor/fckeditor.php") ;
/* @var $dbADO ADOConnection */
/* @var $rs ADORecordSet */

$out='<br>';
$dbADO->debug=false;
$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';

$docID=(isset($_REQUEST['docID']))?$_REQUEST['docID']:0;
$edit=(isset($_SESSION['editVar']))?$_SESSION['editVar']:0;

if($action=='form'){
	$queryDocument='SELECT * FROM Document Where PK_Document='.$docID;
	$res=$dbADO->_Execute($queryDocument);
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
		if(@$GLOBALS['disableDocumentsEdit']!=1 && $edit>0){
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
		if(@$GLOBALS['disableDocumentsEdit']!=1 && $edit>0){
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
}elseif($action=='edit'){
	$queryDocument='SELECT * FROM Document Where PK_Document=?';
	$res=$dbADO->Execute($queryDocument,$docID);
	if($res->RecordCount()==0)
	$out.='<br><br><br>Document not found';
	else{
		$rowDocument=$res->FetchRow();
		$docContent=((isset($_SESSION['docContents']))?@$_SESSION['docContents']:$rowDocument['Contents']);
		// clean page
	
		$oFCKeditor = new FCKeditor('Contents') ;
		$oFCKeditor->Config["CustomConfigurationsPath"] = "../config.js";
		$oFCKeditor->ToolbarSet = 'reduced' ;
		$oFCKeditor->BasePath = 'scripts/fckeditor/';
		$oFCKeditor->Width  = '700' ;
		$oFCKeditor->Height = '400' ;
		$oFCKeditor->Value = $docContent ;
		$contentsFormElement=$oFCKeditor->CreateHtml();		
		
		$out.='
		<base target="_self" />
		
		<form action="right.php" method="post" name="documentDisplay" target="_self"  onsubmit="return submitForm();">
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
						<td colspan="2" align="center"><input type="submit" name="save" value="Save"></td>
					</tr>
					<tr>
						<td colspan="2">'.$contentsFormElement.'</td>
					</tr>
					<tr>
						<td colspan="2" align="center"><input type="submit" name="save" value="Save"></td>					
					</tr>
				</table>
		</form>	
		<em>If you want to include a picture or diagram you should include the link as HTML within the document, and e-mail the graphic to <a href="mailto:support@plutohome.com">support@plutohome.com</a>, with an explanation of what document it belongs to.</em>	
		';
	}
}else{
	if(isset($_POST['save'])){
		$contents=(isset($_POST['Contents']))?stripslashes($_POST['Contents']):$_SESSION['docContents'];
		$title=cleanString($_POST['Title']);
		$docID=cleanInteger($_POST['docID']);
		$parentID=($_POST['parentID']!='')?cleanInteger($_POST['parentID']):NULL;
		$docOrder=cleanInteger($_POST['docOrder']);

		$getOldValues='SELECT * FROM Document WHERE PK_document=?';
		$resOldValues=$dbADO->Execute($getOldValues,$docID);
		$rowOldValues=$resOldValues->FetchRow();

		$updateDocument='UPDATE Document SET Title=?, Contents=?, `Order`=?, FK_Document_Parent=? WHERE PK_Document=?';
		$dbADO->Execute($updateDocument,array($title,$contents,$docOrder,$parentID,$docID));

		$out='
			<script>
				self.location="right.php?section=documents/documentDisplay&docID='.$docID.'";
			</script>
			';

	}

}

$output->setHelpSrc('index.php?section=help');
$output->setBody($out);
$output->setTitle(APPLICATION_NAME);
$output->output();

?>