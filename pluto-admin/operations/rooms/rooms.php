<?
function rooms($output,$dbADO) {
/* @var $dbADO ADOConnection */
/* @var $rs ADORecordSet */
/* @var $resRoomTypes ADORecordSet */
$out='';
$locationGoTo='';
$dbADO->debug=false;
$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
$lastAction = isset($_REQUEST['lastAction'])?cleanString($_REQUEST['lastAction']):'';

if (@$_SESSION['userLoggedIn']!=true) {
	header("Location: index.php?section=login&from=rooms");
}

$installationID = cleanInteger($_SESSION['installationID']);

$out.='<h3>Rooms</h3>';

if ($action=='form') {
$queryRooms = 'SELECT * FROM Room WHERE FK_Installation = ?';
$resRooms = $dbADO->Execute($queryRooms,array($installationID));

$queryRoomTypes = "SELECT * FROM RoomType Order by Description ASC";
$resRoomTypes = $dbADO->_Execute($queryRoomTypes);
$displayedRooms = array();

	if ($resRooms) {
		$out.="
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,'',attributes);
			}
		</script>";
		$out.='<div class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>';
		$out.="
		<table>
			<form action='index.php' method='post' name='rooms'>
			<input type='hidden' name='section' value='rooms'>
			<input type='hidden' name='action' value='add'>
			<input type='hidden' name='lastAction' value=''>
		";
		
		$roomsFormValidation='';
		while ($rowRoom = $resRooms->FetchRow()) {
			$out.='<tr><td><input type="text" name="roomDesc_'.$rowRoom['PK_Room'].'" value="'.$rowRoom['Description'].'"></td><td>';
			$displayedRooms[]=$rowRoom['PK_Room'];
			$resRoomTypes->MoveFirst();			
			$out.='<select name="roomType_'.$rowRoom['PK_Room'].'">';
			while ($rowRoomTypes = $resRoomTypes->FetchRow()) {
				$out.='<option '.($rowRoomTypes['PK_RoomType']==$rowRoom['FK_RoomType']?' selected ':'').' value="'.$rowRoomTypes['PK_RoomType'].'">'.$rowRoomTypes['Description'].'</option>';
			}			
			$out.='</select>';
			$out.="</td><td>
				<a href=\"javascript:void(0);\" onClick=\"windowOpen('index.php?section=deleteRoomFromInstallation&from=rooms&roomID={$rowRoom['PK_Room']}','status=0,resizable=1,width=200,height=200,toolbars=true');\">Delete</a>
			</td>";
			$out.='</tr>';
			$roomsFormValidation.='
				frmvalidator.addValidation("roomDesc_'.$rowRoom['PK_Room'].'","req","Please enter a description");
			';
		}
		
		$resRoomTypes->MoveFirst();			
		$roomTypesTxt='<select name="addNewRoomType">
			<option value="0">-please select-</option>
		';
			while ($rowRoomTypes = $resRoomTypes->FetchRow()) {
				$roomTypesTxt.='<option value="'.$rowRoomTypes['PK_RoomType'].'">'.$rowRoomTypes['Description'].'</option>';
			}			
		$resRoomTypes->Close();
		
		$out.='</select>';
			
		$out.='
			<tr><td colspan="2"> '.$roomTypesTxt.' <input type="submit" name="submitX" value="Add"></td></tr>
			<tr><td colspan="2"><input type="submit" name="submitX" value="Submit"></td></tr>
			<input type="hidden" name="displayedRooms" value="'.join(",",$displayedRooms).'">
			</form>
		
		<script>
		 	var frmvalidator = new formValidator("rooms"); 		
			'.$roomsFormValidation.'
		</script>
			
		</table>
		<br />
				<a href="index.php?section=entertainArea">Entertain Area</a>
		';
	}
} else {
	// check if the user has the right to modify installation
	$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
	if (!$canModifyInstallation){
		header("Location: index.php?section=rooms&error=You are not authorised to change this installation!");
		exit(0);
	}
	
	//process
	$addNewRoomType = cleanInteger($_POST['addNewRoomType']);
	
	if ($addNewRoomType!=0) {
		$queryInsertRoom = 'INSERT INTO Room (FK_RoomType,FK_Installation) values(?,?)';
		$res = $dbADO->Execute($queryInsertRoom,array($addNewRoomType,$installationID));
		$lastInsert = $dbADO->Insert_ID();
		$locationGoTo = "roomDesc_{$lastInsert}";		
	}
	
	$displayedRooms = cleanString($_POST['displayedRooms']);

	$displayedRoomsArray = explode(",",$displayedRooms);
	if (!is_array($displayedRoomsArray) || $displayedRoomsArray===array()) {
		$displayedRoomsArray=array();
	}
	
	foreach ($displayedRoomsArray as $room) {
		$queryOldValue = "SELECT Description, FK_RoomType FROM Room WHERE PK_Room = ?";
		$resOldValue = $dbADO->Execute($queryOldValue,array($room));
		$oldDesc = '';
		$oldRoomType = '';

		if ($resOldValue) {
			$rowOldValue = $resOldValue->FetchRow();
			$oldDesc = $rowOldValue['Description'];
			$oldRoomType = $rowOldValue['FK_RoomType'];
		}

		$newDesc = cleanString(@$_POST['roomDesc_'.$room]);
		$newRoomType = cleanString(@$_POST['roomType_'.$room]);

		if ($newDesc!=$oldDesc || $oldRoomType!=$newRoomType)  {
			$query = 'UPDATE Room set Description=?,FK_RoomType=? WHERE PK_Room = ?';
			$resUpdRoom = $dbADO->Execute($query,array($newDesc,$newRoomType,$room));
			$locationGoTo = "roomDesc_".$room;
		}
	}
	
	if (strstr($locationGoTo,"#")) {
		header("Location: index.php?section=rooms&msg=Saved!".$locationGoTo);
	} else {
		header("Location: index.php?section=rooms&msg=Saved!&lastAction=".$locationGoTo);
	}
	
}

	$onLoad='';
	if ($lastAction!=''?$onLoad.="if (document.forms.rooms.{$lastAction}) {document.forms.rooms.{$lastAction}.focus();} ":$onLoad.="")
	if (strlen($onLoad)>2) {
		$output->setScriptInBody("onLoad=\"javascript:eval('$onLoad');\"");
	}
	
	$output->setNavigationMenu(array("Rooms"=>'index.php?section=rooms'));
	
$output->setBody($out);
$output->setTitle(APPLICATION_NAME);			
$output->output();  		
}
?>