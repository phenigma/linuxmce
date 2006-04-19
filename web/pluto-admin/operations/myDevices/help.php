<?
function help($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/help.lang.php');
	
	global $dbPlutoMainDatabase;
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$userID = (int)@$_SESSION['userID'];
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID=(int)$_SESSION['installationID'];
	$deviceID=(int)@$_REQUEST['deviceID'];
	if($deviceID==0){
		die($TEXT_ERROR_INVALID_DEVICE_TEMPLATE_CONST);
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
			
			$documents.='<br>'.$TEXT_DEVICE_TEMPLATE_CONST.': <B>'.$deviceFields['Template'][$i].'</B><br>'.getDocumentsChilds($deviceFields['FK_Document'][$i],$documentsTree);
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
	<a href="index.php?section=editDeviceParams&deviceID='.$deviceID.'">'.$TEXT_BACK_CONST.'</a>
	<p>'.$TEXT_HELP_DOCUMENTS_CONST.' <B>'.$deviceName.'</B>. '.$TEXT_HELP_DOCUMENTS_INFO_CONST.'<br><br>
	'.$documents.'
	</form>
	';
	} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=help&parentID=$parentID&error=$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST");
			exit(0);
		}



	}

	$output->setMenuTitle($TEXT_HELP_CONST.' |');
	$output->setPageTitle($TEXT_DEVICE_CONST.' #'.$deviceID.': '.$deviceName);
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_HELP_CONST);
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
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/help.lang.php');
	
	$out='';
	if($docID==0){
		return '|--'.$TEXT_NO_DOCUMENTS_CONST.'.';
	}
	if(in_array($docID,array_keys($documentsTree))){
		$out.='<a href="../support/index.php?section=document&docID='.$docID.'" target="_blank">'.$documentsTree[$docID]['Title'].'</a><br>';
		foreach ($documentsTree[$docID]['Childs'] AS $childID){
			$out.=$spacer.'&nbsp;&nbsp;&nbsp;&nbsp;|--'.getDocumentsChilds($childID,$documentsTree,'&nbsp;&nbsp;&nbsp;&nbsp;'.$spacer);
		}
		$out.=$spacer.'&nbsp;&nbsp;&nbsp;&nbsp;|--<a href="../support/index.php?section=addDocument&parentID='.$docID.'&edit=1" target="_blank">['.$TEXT_ADD_NEW_DOCUMENT_HERE_CONST.']</a><br>';
	}
	
	return $out;
}
?>