<?
/* @var $dbADO ADOConnection */
/* @var $rs ADORecordSet */
$out='<br>';
$dbADO->debug=false;
$docID=(isset($_REQUEST['docID']))?$_REQUEST['docID']:0;

if($docID!=0){

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
					top.location="index.php?section=document";
				</script>
			';
}

$output->setBody($out);
$output->setTitle(APPLICATION_NAME);
$output->output();

?>