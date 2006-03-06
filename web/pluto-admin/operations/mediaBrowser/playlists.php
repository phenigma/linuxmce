<?
function playlists($output,$mediadbADO) {
	// include language file
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/playlists.lang.php');
	
	/* @var $mediadbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$userID=$_SESSION['userID'];
	
	if($action=='form'){
		
		$out.='
			<div align="center" class="err">'.@$_REQUEST['error'].'</div>
			<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
			
			<h3>'.$TEXT_PLAYLISTS_CONST.'</h3>
			<form action="index.php" method="POST" name="playlists">
			<input type="hidden" name="section" value="playlists">
			<input type="hidden" name="action" value="update">
		
			<table cellpadding="3" cellspacing="0" border="0">
				<tr bgcolor="lightblue">
					<td align="center"><B>#</B></td>
					<td align="center"><B>'.$TEXT_PLAYLIST_CONST.'</B></td>
					<td align="center"><B>'.$TEXT_ACTION_CONST.'</B></td>
				</tr>
		';
		$resPlaylist=$mediadbADO->Execute('SELECT * FROM Playlist WHERE EK_User=? || EK_User=0',$userID);
		if($resPlaylist->RecordCount()==0){
			$out.='				
				<tr>
					<td align="center" colspan="3">'.$TEXT_NO_RECORDS_CONST.'</td>
				</tr>
			';
		}
		$pos=0;
		while($rowPlaylist=$resPlaylist->FetchRow()){
			$pos++;
			$out.='
				<tr bgcolor="'.(($pos%2==0)?'#EEEEEE':'#EBEFF9').'">
					<td align="center">'.$pos.'</td>
					<td align="center">'.$rowPlaylist['Name'].'</td>
					<td align="center"><a href="index.php?section=editPlaylist&plID='.$rowPlaylist['PK_Playlist'].'">'.$TEXT_EDIT_CONST.'</a> <a href="#" onClick="if(confirm(\''.$TEXT_CONFIRM_DELETE_PLAYLIST_CONST.'\'))self.location=\'index.php?section=playlists&action=delete&plID='.$rowPlaylist['PK_Playlist'].'\'">'.$TEXT_DELETE_CONST.'</a></td>
				</tr>
			';
		}
		$out.='
				<tr>
					<td align="center" colspan="3">&nbsp;</td>
				</tr>
				<tr>
					<td align="center" colspan="3"><B>'.$TEXT_ADD_PLAYLIST_CONST.'</B> <input type="text" name="newPlaylist" value=""> <input type="submit" class="button" name="add" value="'.$TEXT_ADD_CONST.'"></td>
				</tr>
			</table>
		</form>';
	}else{
	// process area
		if(isset($_POST['add'])){
			$newPlaylist=cleanString($_POST['newPlaylist']);
			$mediadbADO->Execute('INSERT INTO Playlist (EK_User, Name) VALUES (?,?)',array($userID,$newPlaylist));
			
			header('Location: index.php?section=playlists&msg='.$TEXT_NEW_PLAYLIST_ADDED_CONST);
		}
	
		if(isset($_REQUEST['plID'])){
			$plID=$_REQUEST['plID'];
			$mediadbADO->Execute('DELETE FROM Playlist WHERE PK_Playlist=?',$plID);
			
			header('Location: index.php?section=playlists&msg='.$TEXT_PLAYLIST_DELETED_CONST);
		}
	}
	
	$output->setNavigationMenu(array($TEXT_PLAYLISTS_CONST=>'index.php?section=playlists'));	
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_PLAYLISTS_CONST);
	$output->output();
}
