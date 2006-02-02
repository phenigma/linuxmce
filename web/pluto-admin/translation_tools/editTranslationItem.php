<?
function editTranslationItem($output,$dbADO) {
	global $dbPlutoMainServer,$dbPlutoMainUser,$dbPlutoMainPass;
	
	/* @var $dbADO ADOConnection */
	// todo: move records in pluto_main database
	$trADO = &ADONewConnection('mysql');
	$trADO->NConnect($dbPlutoMainServer,urlencode($dbPlutoMainUser),urlencode($dbPlutoMainPass),'translation'); 
	
	$trID=(int)@$_REQUEST['trID'];
	$selectedLang=(isset($_REQUEST['language']))?$_REQUEST['language']:0;
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	
	$languagesArray=getAssocArray('Language','PK_Language','Description',$dbADO,'WHERE PK_Language!=1');
	$record=getFieldsAsArray('Translation','PK_Translation,Filename,Variable,Translation.Value AS Value,Translation_Language.Value AS Translated',$trADO,'LEFT JOIN Translation_Language ON FK_Translation=PK_Translation AND FK_Language='.$selectedLang.' WHERE PK_Translation='.$trID);

	if($action=='form'){
		$out='<h3>Translate item</h3>
		<form action="index.php" method="post" name="editTranslationItem">
		<input type="hidden" name="section" value="editTranslationItem">
		<input type="hidden" name="action" value="edit">
		<input type="hidden" name="trID" value="'.$trID.'">

		<div class="err" align="center">'.@$_GET['error'].'</div>
		<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>	
		
		<a href="index.php?section=translationItems&language='.$selectedLang.'">Back</a>
		<table cellpadding=3" cellspacing="0">
			<tr>
				<td><B>Language</B></td>
				<td>'.pulldownFromArray($languagesArray,'language',$selectedLang,'onchange="document.editTranslationItem.submit();"').'</td>
			</tr>
			<tr>
				<td><B>Filename</B></td>
				<td>'.$record['Filename'][0].'</td>
			</tr>
			<tr>
				<td><B>Variable name</B></td>
				<td>'.$record['Variable'][0].'</td>
			</tr>
			<tr bgcolor="#F0F3F8">
				<td><B>English value</B></td>
				<td>'.$record['Value'][0].'</td>
			</tr>
			<tr>
				<td><B>Translated value</B></td>
				<td><textarea name="translated">'.$record['Translated'][0].'</textarea></td>
			</tr>
			<tr>
				<td colspan="2" align="center"><input type="submit" class="button" name="add" value="Update"></td>
			</tr>	
		</table>
		</form>	
		<script>
			var frmvalidator = new formValidator("editTranslationItem");
			frmvalidator.addValidation("language","dontselect=0","Please select the language.");
 			frmvalidator.addValidation("translated","req","Please type the value for translated text.");
		</script>		
		';
	}else{
		$translated=stripslashes($_POST['translated']);
		$trADO->Execute('REPLACE INTO Translation_Language (FK_Translation,FK_Language,Value) VALUES (?,?,?)',array($trID,$selectedLang,$translated));
		
		header('Location: index.php?section=editTranslationItem&trID='.$trID.'&language='.$selectedLang.'&msg=The item was translated');
	}
	
	
	
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Translation items');
	$output->output();  
}
?>