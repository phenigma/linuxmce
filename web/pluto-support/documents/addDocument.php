<?
include("scripts/fckeditor/fckeditor.php") ;


/* @var $dbADO ADOConnection */
/* @var $rs ADORecordSet */
$out='<br>';
$dbADO->debug=false;
$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
$section=$_REQUEST['section'];

$parentID=(isset($_REQUEST['parentID']))?$_REQUEST['parentID']:0;

if($action=='form'){

	$oFCKeditor = new FCKeditor('Contents') ;
	$oFCKeditor->Config["CustomConfigurationsPath"] = "../config.js";
	$oFCKeditor->ToolbarSet = 'reduced' ;
	$oFCKeditor->BasePath = 'scripts/fckeditor/';
	$oFCKeditor->Width  = '700' ;
	$oFCKeditor->Height = '400' ;
	$oFCKeditor->Value = '' ;
	$contentsFormElement=$oFCKeditor->CreateHtml();
	
	$out.='
		<base target="_self" />
		<form action="right.php" method="post" name="addDocument">
		<input type="hidden" name="section" value="'.$section.'">
		<input type="hidden" name="action" value="add">
			<table>
				<tr>
					<td><b>Title</b></td>
					<td><input type="text" name="Title" value="'.((isset($_SESSION['docTitle']))?stripslashes($_SESSION['docTitle']):'').'"></td>
				</tr>
				<tr>
					<td><b>Order</b></td>
					<td><input type="text" name="docOrder" value="'.((isset($_SESSION['docOrder']))?stripslashes($_SESSION['docOrder']):'0').'"></td>
				</tr>
				<tr>
					<td><b>Parent ID</b></td>
					<td><input type="text" name="parentID" value="'.((isset($_SESSION['parentID']))?stripslashes($_SESSION['parentID']):$parentID).'"></td>
				</tr>
				<tr>
					<td><input type="submit" name="add" value="Add" align="center" colspan="2"></td>
				</tr>
				<tr>
					<td colspan="2">'.$contentsFormElement.'</td>
				</tr>
				<tr>
					<td><input type="submit" name="add" value="Add" align="center" colspan="2"></td>
				</tr>
			</table>
		</form>
		<em>If you want to include a picture or diagram you should include the link as HTML within the document, and e-mail the graphic to <a href="mailto:support@plutohome.com">support@plutohome.com</a>, with an explanation of what document it belongs to.</em>
		';
}else{
	if(isset($_POST['add'])){
		$contents=(isset($_POST['Contents']))?stripslashes($_POST['Contents']):$_SESSION['docContents'];
		$title=cleanString($_POST['Title']);
		$parentID=cleanInteger($_POST['parentID']);
		$docOrder=cleanInteger($_POST['docOrder']);

		$insertDocument='INSERT INTO Document (Title,Contents,`Order`,FK_Document_Parent) VALUES (?,?,?,?)';
		$dbADO->Execute($insertDocument,array($title,$contents,$docOrder,$parentID));

		$docID=$dbADO->Insert_ID();
		$out='
				<script>
					try{
						// if add document was called from pluto admin, reload opener
						top.opener.location.reload();
					}
					catch(e){
						//
					}
					top.location="index.php?section=document&docID='.$docID.'";
				</script>
			';

	}

}

$output->setBody($out);
$output->setTitle(APPLICATION_NAME);
$output->output();

?>