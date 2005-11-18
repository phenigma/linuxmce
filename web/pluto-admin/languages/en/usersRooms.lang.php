<?
function usersRooms($output,$dbADO) {

	/* @var $dbADO ADOConnection */
	// WARNING: the checkboxes behavior is reversed, checked = Room_User doesn't exist
	$out='';

	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';

	if (@$_SESSION['userLoggedIn']!=true) {
		header("Location: index.php?section=login");
	}
	$installationID = cleanInteger($_SESSION['installationID']);

	$usersArray=array();
	$checkedArray=array();
	$resUsers=$dbADO->Execute('SELECT * FROM Users ORDER BY UserName ASC');
	while($rowUsers=$resUsers->FetchRow()){
		$usersArray[$rowUsers['PK_Users']]=$rowUsers['UserName'];
		$checkedArray[$rowUsers['PK_Users']][]=0;
	}
	
	$resRooms=$dbADO->Execute('SELECT * FROM Room WHERE FK_Installation=?',$installationID);
	$roomsArray=array();
	while($rowRooms=$resRooms->FetchRow()){
		$roomsArray[$rowRooms['PK_Room']]=$rowRooms['Description'];
	}
	$queryRoomsArray=$roomsArray;
	if(count($queryRoomsArray)==0)
		$queryRoomsArray[]=0;
	$queryUsersRooms='SELECT * FROM Room_Users WHERE FK_Room IN ('.join(',',array_keys($queryRoomsArray)).') ORDER BY FK_Users ASC';
	$resUsersRooms=$dbADO->Execute($queryUsersRooms);
	while($rowUsersRooms=$resUsersRooms->FetchRow()){
		$checkedArray[$rowUsersRooms['FK_Users']][]=$rowUsersRooms['FK_Room'];
	}
	
	$out.='<h3>Privacy Settings</h3>
	<p>Normally any user can choose to control any room in the house.  If you want to prevent some userse from being able to control certain rooms, uncheck the box.</p>';

	if ($action=='form') {
		$out.='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,"",attributes);
			}
		</script>
		<p class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'
		<div align="left" class="confirm"><B>'.(isset($_GET['msg'])?strip_tags($_GET['msg']):'').'</B></div>
		<form action="index.php" method="post" name="usersRooms">
		<input type="hidden" name="section" value="usersRooms">
		<input type="hidden" name="action" value="add">
		<p><B>Rooms</B>
		<table cellpadding="3" cellspacing="0">
			<tr bgcolor="#EEEEEE">
				<td><B>User/Room</B></td>';
		foreach($roomsArray AS $roomName){
			$out.='<td align="center"><B>'.$roomName.'</B></td>';
		}
		$out.='
			</tr>
		';
		$userCount=0;
		foreach($usersArray AS $userID=>$username){
			$userCount++;
			$out.='
			<tr bgcolor="'.(($userCount%2==0)?'#F0F3F8':'').'">
				<td><B>'.$username.'</B></td>';
			foreach($roomsArray AS $roomID=>$roomName){
				$out.='<td align="center"><input type="checkbox" name="user_'.$userID.'_room_'.$roomID.'" value="1" onClick="document.usersRooms.submit();" '.((in_array($roomID,$checkedArray[$userID]))?'':'checked').'></td>';
			}
		$out.='
			</tr>';
		}
		$out.='
		</table>
		</form>';
	} else {
		//check if current user canModifyInstallation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$installationID,$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=usersRooms&error=You are not authorised to change the installation.");
			exit(0);
		}
		foreach($usersArray AS $userID=>$username){
			foreach($roomsArray AS $roomID=>$roomName){
				if(!isset($_POST['user_'.$userID.'_room_'.$roomID])){
					if(!in_array($roomID,$checkedArray[$userID]))
						$dbADO->Execute('INSERT INTO Room_Users (FK_Users,FK_Room) VALUES (?,?)',array($userID,$roomID));
				}else{
					if(in_array($roomID,$checkedArray[$userID]))
						$dbADO->Execute('DELETE FROM Room_Users WHERE FK_Users=? AND FK_Room=?',array($userID,$roomID));
						
				}
			}
		}
		header("Location: index.php?section=usersRooms&msg=User/Room updated.");

	}

	

	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Privacy Settings');			
	$output->output();  		
}
?>