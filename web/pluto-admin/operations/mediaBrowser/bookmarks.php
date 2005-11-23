<?
function bookmarks($output,$mediadbADO,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/bookmarks.lang.php');
	
	/* @var $mediadbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	
	//$mediaTypes=getAssocArray('MediaType','PK_MediaType','Description',$dbADO,'','ORDER BY Description ASC');
	$type=(int)$_REQUEST['type'];
	switch($type){
		case 1:
			$mediaTypes=array(1,6);
		break;
		case 2:
			$mediaTypes=array(4,5,7);
		break;
		case 3:
			$mediaTypes=array(2,3);
		break;
	}
	
	if($action=='form'){
		
		
		$out.='
			<div align="center" class="err">'.@$_REQUEST['error'].'</div>
			<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>

		<h3 align="center">Bookmarks</h3>
		<form action="index.php" method="POST" name="bookmarks">
			<input type="hidden" name="section" value="bookmarks">
			<input type="hidden" name="action" value="update">
		
		<table celspacing="0" cellpadding="3" align="center">
			<tr bgcolor="lightblue">
				<td><B>'.$TEXT_ICON_CONST.'</B></td>
				<td><B>'.$TEXT_MEDIA_PROVIDER_CONST.'</B></td>
				<td><B>'.$TEXT_DESCRIPTION_CONST.'</B></td>
				<td><B>'.$TEXT_CHANNEL_CONST.'</B></td>
				<td><B>'.$TEXT_ACTION_CONST.'</B></td>
			</tr>';
		$bookmarksArray=getFieldsAsArray('Bookmark','PK_Bookmark,FK_MediaProvider,EK_MediaType,FK_Picture,Description,Position',$mediadbADO,'WHERE EK_MediaType IN ('.join(',',$mediaTypes).')');
		if(count(@$bookmarksArray['PK_Bookmark'])==0){
			$out.='
			<tr>
				<td colspan="5" align="center" bgcolor="#F0F3F8"> '.$TEXT_NO_BOOKMARKS_CONST.'</td>
			</tr>';
		}
		for($i=0;$i<count(@$bookmarksArray['PK_Bookmark']);$i++){
			$color=($i%2==0)?'#F0F3F8':'#FFFFFF';
			$out.='
			<tr bgcolor="'.$color.'">
				<td>'.$bookmarksArray['FK_Picture'][$i].'</td>
				<td>'.$bookmarksArray['FK_MediaProvider'][$i].'</td>
				<td>'.$bookmarksArray['Description'][$i].'</td>
				<td>'.$bookmarksArray['Position'][$i].'</td>
				<td><a href="javascript:if(confirm(\''.$TEXT_DELETE_BOOKMARK_CONFIRMATION_CONST.'\'))self.location=\'index.php?section=bookmarks&type='.$type.'&action=del&did='.$bookmarksArray['PK_Bookmark'][$i].'\'">'.$TEXT_DELETE_CONST.'</a></td>
			</tr>';
		}
		$out.='
		</table>

		
		</form>';
	}else{
		$did=(int)$_REQUEST['did'];
		print_array($_REQUEST);
		if($did>0){
			$mediadbADO->Execute('DELETE FROM Bookmark WHERE PK_Bookmark=?',array($did));
			$msg='The bookmark was deleted.';
		}
		
		$suffix=(isset($error))?'&error='.$error:'&msg='.@$msg;
		header('Location: index.php?section=bookmarks&type='.$type.$suffix);
	}
	
	$output->setNavigationMenu(array($TEXT_BOOKMARKS_CONST=>'index.php?section=bookmarks&type='.$type));
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_BOOKMARKS_CONST);
	$output->output();
}
?>