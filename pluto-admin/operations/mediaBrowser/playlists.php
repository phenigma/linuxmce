<?
function playlists($output,$mediadbADO) {
	/* @var $mediadbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$userID=$_SESSION['userID'];
	
	if($action=='form'){
		
		$out.='
			<div align="center" class="err">'.@$_REQUEST['error'].'</div>
			<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
			<h3>Playlists</h3>
			<form action="index.php" method="POST" name="playlists">
			<input type="hidden" name="section" value="playlists">
			<input type="hidden" name="action" value="update">
		
			<table cellpadding="3" cellspacing="0">
				<tr bgcolor="lightblue">
					<td align="center"><B>#</B></td>
					<td align="center"><B>Playlist</B></td>
					<td>&nbsp;</td>
				</tr>
		';
		$resPlaylist=$mediadbADO->Execute('SELECT * FROM Playlist WHERE EK_User=?',$userID);
		if($resPlaylist->RecordCount()==0){
			$out.='				
				<tr>
					<td align="center" colspan="3">No records</td>
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
					<td align="center"><a href="index.php?section=editPlaylist&plID='.$rowPlaylist['PK_Playlist'].'">Edit</a> <a href="#" onClick="if(confirm(\'Are you sure you want to delete this playlist?\'))self.location=\'index.php?section=playlists&action=delete&plID='.$rowPlaylist['PK_Playlist'].'\'">Delete</a></td>
				</tr>
			';
		}
		$out.='
				<tr>
					<td align="center" colspan="3">&nbsp;</td>
				</tr>
				<tr>
					<td align="center">&nbsp;</td>
					<td align="center"><B>Add playlist</B></td>
					<td align="center"><input type="text" name="newPlaylist" value=""> <input type="submit" name="add" value="Add"></td>
				</tr>
			</table>
		</form>';
	}else{
	// process area
		if(isset($_POST['add'])){
			$newPlaylist=cleanString($_POST['newPlaylist']);
			$mediadbADO->Execute('INSERT INTO Playlist (EK_User, Name) VALUES (?,?)',array($userID,$newPlaylist));
			
			header('Location: index.php?section=playlists&msg=New playlist added.');
		}
	
		if(isset($_REQUEST['plID'])){
			$plID=$_REQUEST['plID'];
			$mediadbADO->Execute('DELETE FROM Playlist WHERE PK_Playlist=?',$plID);
			
			header('Location: index.php?section=playlists&msg=The playlist was deleted.');
		}
	}
	
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}