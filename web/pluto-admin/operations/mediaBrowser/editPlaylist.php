<?
function editPlaylist($output,$mediadbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/editPlaylist.lang.php');
	
	/* @var $mediadbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$playlistID=(int)$_REQUEST['plID'];
	
	$playlistInfo=getAssocArray('Playlist','PK_Playlist','Name',$mediadbADO,'WHERE PK_Playlist=',$playlistID);
	
	$scriptInHead='
		<script>
		function selAllCheckboxes()
		{
		   val=(document.editPlaylist.setAll.checked)?true:false;
		   for (i = 0; i < editPlaylist.elements.length; i++)
		   {
		     if (editPlaylist.elements[i].type == "checkbox")
		     {
		         editPlaylist.elements[i].checked = val;
		     }
		   }
		}
		</script>
	';
	if($action=='form'){
		
		$out.='
			<div align="center" class="err">'.@$_REQUEST['error'].'</div>
			<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
		
			<h3 align="center">'.$TEXT_EDIT_PLAYLIST_CONST.'# '.$playlistID.': '.$playlistInfo[$playlistID].'</h3>
			<form action="index.php" method="POST" name="editPlaylist">
			<input type="hidden" name="section" value="editPlaylist">
			<input type="hidden" name="plID" value="'.$playlistID.'">
			<input type="hidden" name="action" value="update">
			<input type="hidden" name="jumpFromTo" value="">
		
			<table cellpadding="3" cellspacing="2" align="center">
				<tr bgcolor="lightblue">
					<td align="center"><B>#</B></td>
					<td align="center"><B>'.$TEXT_FILENAME_CONST.'</B></td>
					<td align="center"><B>'.$TEXT_PATH_CONST.'</B></td>
					<td align="center"><B>'.$TEXT_ACTION_CONST.'</B></td>
				</tr>';
		
		$queryPlaylistEntry="
			SELECT PlaylistEntry.*,File.Filename AS FileFilename, File.Path AS FilePath 
			FROM PlaylistEntry
			INNER JOIN File ON FK_File=PK_File
			WHERE FK_Playlist=?
			ORDER BY 'Order' ASC";
		$resPlaylistEntry=$mediadbADO->Execute($queryPlaylistEntry,$playlistID);
		$oldOrderArray=array();
		$pos=0;
		while($rowPlaylistEntry=$resPlaylistEntry->FetchRow()){
			$pos++;
			$oldOrderArray[$pos]['entryID']=$rowPlaylistEntry['PK_PlaylistEntry'];
			$oldOrderArray[$pos]['order']=$rowPlaylistEntry['Order'];
		}
		if($resPlaylistEntry->RecordCount()==0){
			$out.='
				<tr>
					<td align="center" colspan="6">'.$TEXT_NO_RECORDS_CONST.'</td>
				</tr>			
			';
		}
		$pos=0;
		$resPlaylistEntry->MoveFirst();
		while($rowPlaylistEntry=$resPlaylistEntry->FetchRow()){
			$pos++;
			$out.='
				<tr bgcolor="'.(($pos%2==0)?'#EEEEEE':'#EBEFF9').'">
					<td>'.$pos.'</td>
					<td>'.(($rowPlaylistEntry['Filename']!='')?$rowPlaylistEntry['Filename']:$rowPlaylistEntry['FileFilename']).'</td>
					<td>'.(($rowPlaylistEntry['Path']!='')?$rowPlaylistEntry['Path']:$rowPlaylistEntry['FilePath']).'</td>
					<td><a href="index.php?section=editMediaFile&fileID='.$rowPlaylistEntry['FK_File'].'">'.$TEXT_EDIT_CONST.'</a> <a href="#" onClick="if(confirm(\''.$TEXT_DELETE_FILE_FROM_PLAYLIST_CONFIRMATION_CONST.'\'))self.location=\'index.php?section=editPlaylist&action=delete&plID='.$playlistID.'&entryID='.$rowPlaylistEntry['PK_PlaylistEntry'].'\'">'.$TEXT_DELETE_CONST.'</a>
					<input type="button" class="button" name="plus" value="+" size="2" onClick="self.location=\'index.php?section=editPlaylist&action=upd&plID='.$playlistID.'&increaseID='.$rowPlaylistEntry['PK_PlaylistEntry'].'&oldOrder='.$rowPlaylistEntry['Order'].'\'"> 
					<input type="button" class="button" name="plus" value="-" size="2" onClick="self.location=\'index.php?section=editPlaylist&action=upd&plID='.$playlistID.'&decreaseID='.$rowPlaylistEntry['PK_PlaylistEntry'].'&oldOrder='.$rowPlaylistEntry['Order'].'\'">
					<input type="text" name="jumpTo" value="" size="1" onBlur="document.editPlaylist.jumpFromTo.value=\''.$pos.',\'+this.value;document.editPlaylist.submit();"> <input type="button" class="button" name="go" value="'.$TEXT_GO_CONST.'">
					</td>
				</tr>';
		}
		
		$out.='
				<tr>
					<td align="center" colspan="6">&nbsp;</td>
				</tr>			
				<tr>
					<td align="left">&nbsp;</td>
					<td align="left"><B>'.$TEXT_SEARCH_FOR_FILES_CONST.':</B></td>
					<td align="left"><input type="text" name="searchForFiles" value="'.@$_POST['searchForFiles'].'"></td>
					<td align="left"><input type="button" class="button" name="searchBtn" value="'.$TEXT_SEARCH_CONST.'" onClick="document.editPlaylist.action.value=\'form\';document.editPlaylist.submit();"></td>
				</tr>';
		if(isset($_POST['searchForFiles']) && $_POST['searchForFiles']!=''){
			$searchString='%'.$_POST['searchForFiles'].'%';
			$resFiles=$mediadbADO->Execute('SELECT * FROM File WHERE Filename LIKE ?',$searchString);
			$displayedFilesArray=array();
			while($rowFiles=$resFiles->FetchRow()){
				$displayedFilesArray[]=$rowFiles['PK_File'];
				$out.='
				<tr>
					<td colspan="4"><input type="checkbox" name="file_'.$rowFiles['PK_File'].'" value="1" checked> '.$rowFiles['Path'].'/<B>'.$rowFiles['Filename'].'</B></td>
				</tr>';
			}
			$out.='
				<tr>
					<td colspan="4"><input type="checkbox" name="setAll" onClick="selAllCheckboxes()" checked> '.$TEXT_SELECT_UNSELECT_ALL_CONST.'</td>
				</tr>
				<tr>
					<td colspan="4"><input type="submit" class="button" name="add" value="'.$TEXT_ADD_SELECTED_FILES_CONST.'">
						<input type="hidden" name="displayedFilesArray" value="'.join(',',$displayedFilesArray).'">
					</td>
				</tr>
			';
		}
		$out.='			
			<table>
			<input type="hidden" name="oldOrderArray" value="'.str_replace('"','\'',urlencode(serialize($oldOrderArray))).'">
		</form>';
	}else{
	// process area
		if(isset($_POST['add'])){
			$displayedFilesArray=explode(',',$_POST['displayedFilesArray']);
			foreach ($displayedFilesArray AS $fileID){
				if(isset($_POST['file_'.$fileID]) && (int)$_POST['file_'.$fileID]==1){
					$mediadbADO->Execute("INSERT INTO PlaylistEntry (FK_File, FK_Playlist,Path,Filename) SELECT $fileID,$playlistID,Path,Filename FROM File WHERE PK_File=?",$fileID);
					$insertID=$mediadbADO->Insert_ID();
					$mediadbADO->Execute("UPDATE PlaylistEntry SET `Order`=? WHERE PK_PlaylistEntry=?",array($insertID,$insertID));
				}
			}
			
			header('Location: index.php?section=editPlaylist&plID='.$playlistID.'&msg='.$TEXT_FILES_ADDED_CONFIRMATION_CONST);
		}
		
		if(isset($_REQUEST['entryID'])){
			$entryID=(int)$_REQUEST['entryID'];
			$mediadbADO->Execute('DELETE FROM PlaylistEntry WHERE PK_PlaylistEntry=?',$entryID);
			
			header('Location: index.php?section=editPlaylist&plID='.$playlistID.'&msg='.$TEXT_FILE_DELETED_CONFIRMATION_CONST);
		}

		if(isset($_REQUEST['increaseID'])){
			$increaseID=(int)$_REQUEST['increaseID'];
			$oldOrder=(int)$_REQUEST['oldOrder'];
			$resFirstLowerOrder=$mediadbADO->Execute("SELECT PK_PlaylistEntry,`Order` FROM PlaylistEntry WHERE `Order`<? ORDER BY `Order` DESC LIMIT 0,1",$oldOrder);
			if($resFirstLowerOrder->RecordCount()>0){
				$rowFLO=$resFirstLowerOrder->FetchRow();
				$mediadbADO->Execute("UPDATE PlaylistEntry SET `Order`=? WHERE PK_PlaylistEntry=?",array($oldOrder,$rowFLO['PK_PlaylistEntry']));	
				$mediadbADO->Execute("UPDATE PlaylistEntry SET `Order`=? WHERE PK_PlaylistEntry=?",array($rowFLO['Order'],$increaseID));	
			}

			header('Location: index.php?section=editPlaylist&plID='.$playlistID.'&msg='.$TEXT_FILE_INCREASED_CONFIRMATION_CONST);
			exit();
		}

		if(isset($_REQUEST['decreaseID'])){
			$decreaseID=(int)$_REQUEST['decreaseID'];
			$oldOrder=(int)$_REQUEST['oldOrder'];
			$resFirstLowerOrder=$mediadbADO->Execute("SELECT PK_PlaylistEntry,`Order` FROM PlaylistEntry WHERE `Order`>? ORDER BY `Order` ASC LIMIT 0,1",$oldOrder);
			if($resFirstLowerOrder->RecordCount()>0){
				$rowFLO=$resFirstLowerOrder->FetchRow();
				$mediadbADO->Execute("UPDATE PlaylistEntry SET `Order`=? WHERE PK_PlaylistEntry=?",array($oldOrder,$rowFLO['PK_PlaylistEntry']));	
				$mediadbADO->Execute("UPDATE PlaylistEntry SET `Order`=? WHERE PK_PlaylistEntry=?",array($rowFLO['Order'],$decreaseID));	
			}

			header('Location: index.php?section=editPlaylist&plID='.$playlistID.'&msg='.$TEXT_FILE_DECREASED_CONFIRMATION_CONST);
			exit();
		}

		if(isset($_POST['jumpFromTo']) && $_POST['jumpFromTo']!=''){
			$oldOrderArray=unserialize(urldecode($_POST['oldOrderArray']));
			$tmpArray=explode(',',$_POST['jumpFromTo']);
			$fromPos=$tmpArray[0];
			$toPos=$tmpArray[1];
			if(in_array($toPos,array_keys($oldOrderArray))){
				if($fromPos!=$toPos){
					$updatePlaylistEntry='UPDATE PlaylistEntry SET `Order`=? WHERE PK_PlaylistEntry=?';
					$mediadbADO->Execute($updatePlaylistEntry,array($oldOrderArray[$toPos]['order'],$oldOrderArray[$fromPos]['entryID']));
				
					if($fromPos>$toPos){
						for($i=$toPos;$i<$fromPos;$i++){
							$mediadbADO->Execute($updatePlaylistEntry,array($oldOrderArray[$i+1]['order'],$oldOrderArray[$i]['entryID']));
						}
					}else{
						//$mediadbADO->debug=true;
						for($i=$fromPos;$i<$toPos;$i++){
							$mediadbADO->Execute($updatePlaylistEntry,array($oldOrderArray[$i]['order'],$oldOrderArray[$i+1]['entryID']));
						}
						
					}
				}
			}else{
				// do nothing, the position doesn't exist in the file list
				header('Location: index.php?section=editPlaylist&plID='.$playlistID.'&error='.$TEXT_ERROR_POSITION_DOESN_NOT_EXIST_CONST);
				exit();
			}
		}
		
		header('Location: index.php?section=editPlaylist&plID='.$playlistID);
	}
	
	$output->setScriptInHead($scriptInHead);	
	$output->setNavigationMenu(array("Playlists"=>'index.php?section=playlists'));
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_EDIT_PLAYLIST_CONST);
	$output->output();
}