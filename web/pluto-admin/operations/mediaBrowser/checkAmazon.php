<?
function checkAmazon($output,$mediadbADO,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/editMediaFile.lang.php');
	
	/* @var $mediadbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$fileID=(int)@$_REQUEST['fileID'];
	
	$fileData=getFieldsAsArray('File','Filename,Path,PK_File',$mediadbADO,'WHERE PK_File='.$fileID);
	if(count($fileData)==0){
		error_redirect('Invalid file ID','');
	}
	$filename=str_replace('.mp3','',$fileData['Filename'][0]);
	$_SESSION['lastFileID']=$fileID;
	
	if($action=='form'){
		
		
		$out.='
			<div align="center" class="err">'.@$_REQUEST['error'].'</div>
			<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
		
			<form action="index.php" method="POST" name="checkAmazon" target="AmazonFrame">
			<input type="hidden" name="section" value="searchAmazon">
			<input type="hidden" name="fileID" value="'.$fileID.'">
		
			<table>
				<tr>
					<td><B>'.$TEXT_SEARCH_PHRASE_CONST.'</B></td>
					<td><input type="text" name="sstring" value="'.$filename.'" style="width:200px;"></td>
					<td><input type="submit" class="button" name="sbtn" value="'.$TEXT_SEARCH_AMAZON_CONST.'"></td>
				</tr>
			</table>
		</form>
		<iframe name="AmazonFrame" src="index.php?section=searchAmazon&sstring='.urlencode($filename).'&fileID='.$fileID.'" width="950" height="550"></iframe>
		';
	}else{
	// process area
		
		header('Location: index.php?section=checkAmazon&fileID='.$fileID);
	}
	
	$output->setReloadLeftFrame(false);
	$output->setMenuTitle($TEXT_FILES_AND_MEDIA_CONST.' |');
	$output->setPageTitle($TEXT_GET_INFO_ON_AMAZON_CONST);	
	$output->setNavigationMenu(array($TEXT_EDIT_MEDIA_FILE_CONST=>'index.php?section=editMediaFile&fileID='.$fileID));
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_GET_INFO_ON_AMAZON_CONST);
	$output->output();
}