<?
/* @var $dbADO ADOConnection */
/* @var $rs ADORecordSet */
$out='<br>';
$dbADO->debug=false;
$docID=(isset($_REQUEST['docID']))?(int)$_REQUEST['docID']:0;

if($docID!=0){

	$resParent=$dbADO->Execute('SELECT FK_Document_Parent FROM Document WHERE PK_Document=?',$docID);
	$rowParent=$resParent->FetchRow();
	$parentID=(int)$rowParent['FK_Document_Parent'];
	$parentID=($parentID!=0)?$parentID:1;
	
	getDocumentChildsArray($docID,$dbADO);
	$toDelete = cleanArray($GLOBALS['childsArray']);
	$toDelete[]=$docID;

	if (!is_array($toDelete)) {
		$toDelete=array();
	}
	foreach ($toDelete as $elem) {
		$queryDelDocument = 'DELETE FROM Document WHERE PK_Document = ?';
		$dbADO->Execute($queryDelDocument,$elem);
	}

	$out='
				<script>
					top.location="index.php?section=document&docID='.$parentID.'";
				</script>
			';
}

$output->setBody($out);
$output->setTitle(APPLICATION_NAME);
$output->output();

?>