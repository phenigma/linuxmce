<?
function editPlaylist($output,$mediadbADO) {
	/* @var $mediadbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$playlistID=(int)$_REQUEST['plID'];
	
	if($action=='form'){
		
		$out.='
			<div align="center" class="err">'.@$_REQUEST['error'].'</div>
			<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
			<h3>Edit Playlist</h3>
			<form action="index.php" method="POST" name="editPlaylist">
			<input type="hidden" name="section" value="editPlaylist">
			<input type="hidden" name="plID" value="'.$playlistID.'">
			<input type="hidden" name="action" value="update">
		
			<table cellpadding="3" cellspacing="2" align="center">
				<tr bgcolor="lightblue">
					<td align="center"><B>#</B></td>
					<td align="center"><B>Filename</B></td>
					<td align="center"><B>Path</B></td>
					<td>&nbsp;</td>
				</tr>';
		
		$queryPlaylistEntry="
			SELECT PlaylistEntry.*,File.Filename AS FileFilename, File.Path AS FilePath 
			FROM PlaylistEntry
			INNER JOIN File ON FK_File=PK_File
			WHERE FK_Playlist=?
			ORDER BY 'Order' ASC";
		$resPlaylistEntry=$mediadbADO->Execute($queryPlaylistEntry,$playlistID);
		if($resPlaylistEntry->RecordCount()==0){
			$out.='
				<tr>
					<td align="center" colspan="6">No records</td>
				</tr>			
			';
		}
		$pos=0;
		while($rowPlaylistEntry=$resPlaylistEntry->FetchRow()){
			$pos++;
			$out.='
				<tr>
					<td>'.$pos.'</td>
					<td>'.(($rowPlaylistEntry['Filename']!='')?$rowPlaylistEntry['Filename']:$rowPlaylistEntry['FileFilename']).'</td>
					<td>'.(($rowPlaylistEntry['Path']!='')?$rowPlaylistEntry['Path']:$rowPlaylistEntry['FilePath']).'</td>
					<td><a href="index.php?section=editMediaFile&fileID='.$rowPlaylistEntry['FK_File'].'">Edit</a> <a href="#" onClick="if(confirm(\'Are you sure you want to delete this file from playlist?\'))self.location=\'index.php?section=editPlaylist&action=delete&plID='.$playlistID.'&entryID='.$rowPlaylistEntry['PK_PlaylistEntry'].'\'">Delete</a>
					<input type="button" name="plus" value="+" size="2" onClick="self.location=\'index.php?section=editPlaylist&action=upd&plID='.$playlistID.'&increaseID='.$rowPlaylistEntry['PK_PlaylistEntry'].'&oldOrder='.$rowPlaylistEntry['Order'].'\'"> 
					<input type="button" name="plus" value="-" size="2" onClick="self.location=\'index.php?section=editPlaylist&action=upd&plID='.$playlistID.'&decreaseID='.$rowPlaylistEntry['PK_PlaylistEntry'].'&oldOrder='.$rowPlaylistEntry['Order'].'\'">
					</td>
				</tr>';
		}
		
		$out.='
				<tr>
					<td align="center" colspan="6">&nbsp;</td>
				</tr>			
				<tr>
					<td align="left">&nbsp;</td>
					<td align="left"><B>Search for files:</B></td>
					<td align="left"><input type="text" name="searchForFiles" value="'.@$_POST['searchForFiles'].'"></td>
					<td align="left"><input type="button" name="searchBtn" value="Search" onClick="document.editPlaylist.action.value=\'form\';document.editPlaylist.submit();"></td>
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
					<td colspan="4"><input type="submit" name="add" value="Add selected files to playlist">
						<input type="hidden" name="displayedFilesArray" value="'.join(',',$displayedFilesArray).'">
					</td>
				</tr>
			';
		}
		$out.='			
			<table>
		</form>';
	}else{
	// process area
		if(isset($_POST['add'])){
			$displayedFilesArray=explode(',',$_POST['displayedFilesArray']);
			foreach ($displayedFilesArray AS $fileID){
				if(isset($_POST['file_'.$fileID]) && (int)$_POST['file_'.$fileID]==1){
					$mediadbADO->Execute('INSERT INTO PlaylistEntry (FK_File, FK_Playlist) VALUES (?,?)',array($fileID,$playlistID));
					$insertID=$mediadbADO->Insert_ID();
					$mediadbADO->Execute("UPDATE PlaylistEntry SET `Order`=? WHERE PK_PlaylistEntry=?",array($insertID,$insertID));
				}
			}
			
			header('Location: index.php?section=editPlaylist&plID='.$playlistID.'&msg=The file(s) was added to playlist.');
		}
		
		if(isset($_REQUEST['entryID'])){
			$entryID=(int)$_REQUEST['entryID'];
			$mediadbADO->Execute('DELETE FROM PlaylistEntry WHERE PK_PlaylistEntry=?',$entryID);
			
			header('Location: index.php?section=editPlaylist&plID='.$playlistID.'&msg=The file was deleted from playlist.');
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

			header('Location: index.php?section=editPlaylist&plID='.$playlistID.'&msg=The file order in playlist was increased.');
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

			header('Location: index.php?section=editPlaylist&plID='.$playlistID.'&msg=The file order in playlist was decreased.');
		}
		

	}
	
	$output->setNavigationMenu(array("Playlists"=>'index.php?section=playlists'));
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}