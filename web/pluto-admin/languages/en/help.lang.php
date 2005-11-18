<?
function help($output,$dbADO) {
	global $dbPlutoMainDatabase;
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$userID = (int)@$_SESSION['userID'];
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID=(int)$_SESSION['installationID'];
	$deviceID=(int)@$_REQUEST['deviceID'];
	if($deviceID==0){
		die('Invalid device template.');
	}
	
	if ($action == 'form') {
		$devicesToShow=getAncestorsForDevice($deviceID,$dbADO);
		$deviceFields=getFieldsAsArray('Device','PK_Device,Device.Description,FK_Document,DeviceTemplate.Description AS Template',$dbADO,'INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE PK_Device IN ('.join(',',$devicesToShow).')');
		$documentsTree=getDocumentTree($dbADO);

		$documents='';
		for($i=0;$i<count($deviceFields['PK_Device']);$i++){
			if($deviceFields['PK_Device'][$i]==$deviceID){
				$deviceName=$deviceFields['Description'][$i];
			}
			
			$documents.='<br>Device Template: <B>'.$deviceFields['Template'][$i].'</B><br>'.getDocumentsChilds($deviceFields['FK_Document'][$i],$documentsTree);
		}
		
	$out.='
	<script>
		function windowOpen(locationA,attributes) {
			window.open(locationA,\'\',attributes);
		}
	</script>
		
	<div class="err">'.strip_tags(@$_GET['error']).'</div>
	<div class="confirm">'.strip_tags(@$_GET['msg']).'</div>
	
	<form action="index.php" method="POST" name="help">
	<input type="hidden" name="section" value="help">
	<input type="hidden" name="action" value="add">	
	<p>Here are all the documents that can be of help when using your <B>'.$deviceName.'</B>. Pluto\'s documentation is like a wiki. While viewing a document you can click "activate edit mode" to make your own changes or additions. You can also add your own document by clicking the "add new document" button below. Any changes or additions will be merged back into Pluto\'s main documentation tree, and pending review, will then become a part of every other Pluto user\'s documentation, and they will see your documents when they click the help button. So feel free to make additions, it helps the whole community.<br><br>
	'.$documents.'
	</form>
	';
	} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=help&parentID=$parentID&error=You are not authorised to change the installation.");
			exit(0);
		}



	}

	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Help');
	$output->output();
}

function getDocumentTree($dbADO){
	$res=$dbADO->Execute('SELECT * FROM Document');
	$documentsTree=array();
	while($row=$res->FetchRow()){
		$documentsTree[$row['PK_Document']]['Title']=$row['Title'];
		if(!is_null($row['FK_Document_Parent'])){
			$documentsTree[$row['FK_Document_Parent']]['Childs'][]=$row['PK_Document'];
		}
	}
	
	return $documentsTree;
}

function getDocumentsChilds($docID,$documentsTree,$spacer=''){
	$out='';
	if($docID==0){
		return '|--No documents.';
	}
	if(in_array($docID,array_keys($documentsTree))){
		$out.='<a href="../support/index.php?section=document&docID='.$docID.'" target="_blank">'.$documentsTree[$docID]['Title'].'</a><br>';
		foreach ($documentsTree[$docID]['Childs'] AS $childID){
			$out.=$spacer.'&nbsp;&nbsp;&nbsp;&nbsp;|--'.getDocumentsChilds($childID,$documentsTree,'&nbsp;&nbsp;&nbsp;&nbsp;'.$spacer);
		}
		$out.=$spacer.'&nbsp;&nbsp;&nbsp;&nbsp;|--<a href="../support/index.php?section=addDocument&parentID='.$docID.'&edit=1" target="_blank">[Add new document here]</a><br>';
	}
	
	return $out;
}
?>