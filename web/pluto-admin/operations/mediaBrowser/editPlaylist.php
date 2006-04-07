<?
function editPlaylist($output,$mediadbADO,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/editPlaylist.lang.php');
	
	/* @var $mediadbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$playlistID=(int)$_REQUEST['plID'];
	
	$playlistInfo=getAssocArray('Playlist','PK_Playlist','Name',$mediadbADO,'WHERE PK_Playlist=',$playlistID);
	
	if(isset($_REQUEST['searchForFiles']) && strlen($_REQUEST['searchForFiles'])<3){
		$_REQUEST['error']=$TEXT_ERROR_SEARCHSTRING_TOO_SHORT_CONST;
	}
	
	// set Order field to PK_PlaylistEntry for the records who doesn't have it set
	$mediadbADO->Execute('UPDATE PlaylistEntry SET `Order`=PK_PlaylistEntry WHERE `Order` IS NULL');
	
	$scriptInHead='
		<script>
		function selAllCheckboxes(forced)
		{
			if(forced==0){
				val=(document.editPlaylist.setAll.checked)?true:false;
			}else{
				val=(document.editPlaylist.setAll.checked)?false:true;
				document.editPlaylist.setAll.checked=val;
			}
			for(i=1;i<records+1;i++){
				document.getElementById("file_"+i).checked=val;
			}
		}
		</script>
	';
	if($action=='form'){
		$commandGroups=getAssocArray('CommandGroup','PK_CommandGroup','Description',$dbADO);
		
		
		$out.='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		</script>		
		
			<div align="center" class="err">'.@$_REQUEST['error'].'</div>
			<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
		
			<h3 align="center">'.$TEXT_EDIT_PLAYLIST_CONST.'# '.$playlistID.': '.$playlistInfo[$playlistID].'</h3>
			<form action="index.php" method="POST" name="editPlaylist">
			<input type="hidden" name="section" value="editPlaylist">
			<input type="hidden" name="plID" value="'.$playlistID.'">
			<input type="hidden" name="action" value="update">
			<input type="hidden" name="jumpFromTo" value="">
		
			<table cellpadding="3" cellspacing="2" align="center">
				<tr class="tablehead">
					<td align="center"><B>#</B></td>
					<td align="center"><B>'.$TEXT_FILENAME_CONST.'</B> / <B>'.$TEXT_PATH_CONST.'</B></td>
					<td align="center"><B>'.$TEXT_START_COMMAND_GROUP_CONST.'</B></td>
					<td align="center"><B>'.$TEXT_STOP_COMMAND_GROUP_CONST.'</B></td>
					<td align="center"><B>'.$TEXT_DURATION_CONST.'</B></td>
					<td align="center"><B>'.$TEXT_ACTION_CONST.' *</B></td>
				</tr>';
		
		// get bookmark array
		$resB=$mediadbADO->Execute('
			SELECT PK_Bookmark,Bookmark.Description, File.Filename,FK_File
			FROM Bookmark
			LEFT JOIN File ON FK_File=PK_File
			WHERE IsAutoResume!=1
			ORDER BY Bookmark.Description ASC,Filename ASC');
		$bookmarksArray=array();
		while($rowB=$resB->FetchRow()){
			$bookmarksArray[$rowB['PK_Bookmark']]=$rowB['Description'].((!is_null($rowB['FK_File']))?' ('.$rowB['Filename'].')':'');
		}
				
		$queryPlaylistEntry="
			SELECT PlaylistEntry.*,File.Filename AS FileFilename, File.Path AS FilePath
			FROM PlaylistEntry
			LEFT JOIN File ON PlaylistEntry.FK_File=PK_File
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
					<td align="center" colspan="5">'.$TEXT_NO_RECORDS_CONST.'</td>
				</tr>			
			';
		}
		$pos=0;
		$resPlaylistEntry->MoveFirst();
		$playlistEntries=array();
		while($rowPlaylistEntry=$resPlaylistEntry->FetchRow()){
			$playlistEntries[]=$rowPlaylistEntry['PK_PlaylistEntry'];
			$pos++;
			$filename=(($rowPlaylistEntry['Filename']!='')?$rowPlaylistEntry['Filename']:$rowPlaylistEntry['FileFilename']);
			$filename=(!is_null($rowPlaylistEntry['FK_Bookmark']))?$bookmarksArray[$rowPlaylistEntry['FK_Bookmark']]:$filename;
			$editFileLink=(!is_null($rowPlaylistEntry['FK_File']))?'<a href="index.php?section=editMediaFile&fileID='.$rowPlaylistEntry['FK_File'].'">'.$TEXT_EDIT_CONST.'</a>':'';
			$out.='
				<tr bgcolor="'.(($pos%2==0)?'#EEEEEE':'#EBEFF9').'">
					<td>'.$pos.'</td>
					<td><B>'.$filename.'</B><br>'.(($rowPlaylistEntry['Path']!='')?$rowPlaylistEntry['Path']:$rowPlaylistEntry['FilePath']).'</td>
					<td>
						<table width="100%">
						 	<tr>
								<td align="center">'.((!is_null($rowPlaylistEntry['EK_CommandGroup_Start']))?$commandGroups[$rowPlaylistEntry['EK_CommandGroup_Start']]:'N/A').'</td>
								<td width="30"><a href="javascript:windowOpen(\'index.php?section=pickScenario&plID='.$playlistID.'&mode=start&entryID='.$rowPlaylistEntry['PK_PlaylistEntry'].'\',\'width=800,height=600,toolbars=true\');">'.$TEXT_PICK_COMMAND_GROUP_CONST.'</a></td>
							</tr>
						</table>
					</td>
					<td><table width="100%" border="0">
						 	<tr>
								<td align="center">'.((!is_null($rowPlaylistEntry['EK_CommandGroup_Stop']))?$commandGroups[$rowPlaylistEntry['EK_CommandGroup_Stop']]:'N/A').'</td>
								<td width="30"><a href="javascript:windowOpen(\'index.php?section=pickScenario&plID='.$playlistID.'&mode=stop&entryID='.$rowPlaylistEntry['PK_PlaylistEntry'].'\',\'width=800,height=600,toolbars=true\');">'.$TEXT_PICK_COMMAND_GROUP_CONST.'</a></td>
							</tr>
						</table></td>
					<td align="center"><input type="text" name="duration_'.$rowPlaylistEntry['PK_PlaylistEntry'].'" value="'.$rowPlaylistEntry['Duration'].'" style="width:50px;"></td>
					<td>'.$editFileLink.' <a href="#" onClick="if(confirm(\''.$TEXT_DELETE_FILE_FROM_PLAYLIST_CONFIRMATION_CONST.'\'))self.location=\'index.php?section=editPlaylist&action=delete&plID='.$playlistID.'&entryID='.$rowPlaylistEntry['PK_PlaylistEntry'].'\'">'.$TEXT_DELETE_CONST.'</a>
					<input type="button" class="button" name="plus" value="U" size="2" onClick="self.location=\'index.php?section=editPlaylist&action=upd&plID='.$playlistID.'&increaseID='.$rowPlaylistEntry['PK_PlaylistEntry'].'&oldOrder='.$rowPlaylistEntry['Order'].'\'"> 
					<input type="button" class="button" name="plus" value="D" size="2" onClick="self.location=\'index.php?section=editPlaylist&action=upd&plID='.$playlistID.'&decreaseID='.$rowPlaylistEntry['PK_PlaylistEntry'].'&oldOrder='.$rowPlaylistEntry['Order'].'\'">
					<input type="text" name="jumpTo" value="" size="1" onBlur="document.editPlaylist.jumpFromTo.value=\''.$pos.',\'+this.value;document.editPlaylist.submit();"> <input type="button" class="button" name="go" value="'.$TEXT_GO_CONST.'">
					</td>
				</tr>';
		}
		if($resPlaylistEntry->RecordCount()>0){
			$out.='
				<tr>
					<td align="left" colspan="6"><em>* '.$TEXT_CHANGE_POSITION_INFO_CONST.'</em></td>
				</tr>			
				<tr>
					<td colspan="5" align="center">
						<input type="submit" class="button" name="update" value="'.$TEXT_UPDATE_CONST.'">
						<input type="reset" class="button" name="cancelBtn" value="'.$TEXT_CANCEL_CONST.'">
						<input type="hidden" class="button" name="playlistEntries" value="'.join(',',$playlistEntries).'">
					</td>
				</tr>
				';
		}
		
	
		$out.='
				<tr>
					<td align="center" colspan="6">&nbsp;</td>
				</tr>			
				<tr>
					<td align="center" colspan="5">
						<B>'.$TEXT_SEARCH_FOR_FILES_CONST.':</B> 
						<input type="text" name="searchForFiles" value="'.@$_REQUEST['searchForFiles'].'">
						<input type="button" class="button" name="searchBtn" value="'.$TEXT_SEARCH_CONST.'" onClick="document.editPlaylist.action.value=\'form\';document.editPlaylist.submit();">
						&nbsp;&nbsp;&nbsp;&nbsp;<b>'.$TEXT_ADD_BOOKMARK_CONST.'</b> 
						'.pulldownFromArray($bookmarksArray,'bookmark',0).'
						<input type="submit" class="button" name="add_bookmark" value="'.$TEXT_ADD_CONST.'">
					</td>
				</tr>';
		if(isset($_REQUEST['searchForFiles']) && strlen($_REQUEST['searchForFiles'])>2){
			$searchString='%'.$_REQUEST['searchForFiles'].'%';
			$resFiles=$mediadbADO->Execute('SELECT * FROM File WHERE Filename LIKE ?',$searchString);
			$displayedFilesArray=array();
			$pos=0;
			$out.='
				<tr bgcolor="#EBEFF9">
					<td colspan="5" align="center"><B>'.$TEXT_SEARCH_RESULTS_CONST.'</B></td>				
				</tr>';
			
			// multipage display variables
			$filesPerPage=30;
			$page=(isset($_REQUEST['page']))?(int)$_REQUEST['page']:1;
			$noRecords=$resFiles->RecordCount();
			$noPages=ceil($noRecords/$filesPerPage);
			$linkPages=array();
			for($i=1;$i<$noPages+1;$i++){
				$url=(strpos($_SERVER['QUERY_STRING'],'editPlaylist')===false)?'section=editPlaylist&plID='.$playlistID.'&searchForFiles='.urlencode($_REQUEST['searchForFiles']):'';
				$linkPages[]=($i==$page)?'<span class="normal_row">'.$i.'</span>':'<a class="red_link" href="index.php?'.$url.str_replace('&page='.$page,'',$_SERVER['QUERY_STRING']).'&page='.$i.'">'.$i.'</a>';
			}
			$linksBar=join(' ',$linkPages);
			if($noRecords==0){
				$out.='
				<tr>
					<td colspan="5" align="center">'.$TEXT_NO_RECORDS_CONST.'</td>				
				</tr>';
			}

			$fileCounter=0;		
			while($rowFiles=$resFiles->FetchRow()){
				$pos++;
				if($pos>($page-1)*$filesPerPage && $pos<=$page*$filesPerPage){
					$fileCounter++;
					$color=($pos%2==0)?'#F0F3F8':'#FFFFFF';
					$displayedFilesArray[]=$rowFiles['PK_File'];
					$out.='
					<tr bgcolor="'.$color.'">
						<td colspan="5"><input type="checkbox" name="file_'.$rowFiles['PK_File'].'" value="1" checked id="file_'.$fileCounter.'"> '.$rowFiles['Path'].'/<B>'.$rowFiles['Filename'].'</B></td>
					</tr>';
				}
			}
			if($resFiles->RecordCount()>0){
				$out.='
				<tr bgcolor="#EBEFF9">
					<td colspan="2" align="left"><input type="checkbox" name="setAll" onClick="selAllCheckboxes(0)" checked> <a href="javascript:selAllCheckboxes(1)">'.$TEXT_SELECT_UNSELECT_ALL_CONST.'</a></td>				
					<td colspan="3" align="right">'.$linksBar.'</td>
				</tr>
				<tr>
					<td colspan="5"><input type="submit" class="button" name="add" value="'.$TEXT_ADD_SELECTED_FILES_CONST.'">
						<input type="hidden" name="displayedFilesArray" value="'.join(',',$displayedFilesArray).'">
					</td>
				</tr>
				<script>var records='.$fileCounter.';</script>
				';
			}
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
			exit();
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
		
		if(isset($_POST['update']) || isset($_POST['add_bookmark'])){
			$playlistEntries=explode(',',@$_POST['playlistEntries']);
			foreach ($playlistEntries AS $playlistEntry){
				if(isset($_POST['duration_'.$playlistEntry])){
					$duration=((int)$_POST['duration_'.$playlistEntry]>0)?(int)$_POST['duration_'.$playlistEntry]:NULL;
					$mediadbADO->Execute('UPDATE PlaylistEntry SET Duration=? WHERE PK_PlaylistEntry=?',array($duration,$playlistEntry));
				}
			}
			
			
			if((int)$_POST['bookmark']>0){
				$bookmark=(int)$_POST['bookmark'];
				$pathArr=array_values(getAssocArray('File','PK_File','Path',$mediadbADO,'INNER JOIN Bookmark ON FK_File=PK_File WHERE PK_Bookmark='.$bookmark));

				$mediadbADO->Execute('INSERT INTO PlaylistEntry (FK_Playlist,FK_Bookmark,FK_File,Path) VALUES (?,?,?,?)',array($playlistID,$bookmark,NULL,$pathArr[0]));
				$entryID=$mediadbADO->Insert_ID();
				$mediadbADO->Execute('UPDATE PlaylistEntry SET `Order`=? WHERE PK_Playlistentry=?',array($entryID,$entryID));
			}
			
			header('Location: index.php?section=editPlaylist&plID='.$playlistID.'&msg='.$TEXT_PLAYLIST_ENTRIES_UPDATED_CONST);
			exit();
		}

		if(isset($_REQUEST['updatedEntryID'])){
			$entryID=(int)$_REQUEST['updatedEntryID'];
			$cgID=((int)$_REQUEST['cgID']==0)?NULL:(int)$_REQUEST['cgID'];
			$mode=$_REQUEST['mode'];
			$field=($mode=='start')?'EK_CommandGroup_Start':'EK_CommandGroup_Stop';
			$mediadbADO->Execute('UPDATE PlaylistEntry SET '.$field.'=? WHERE PK_PlaylistEntry=?',array($cgID,$entryID));
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