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

$fotArray=array();
$resFOT=$dbADO->Execute('SELECT * FROM FloorplanObjectType WHERE FK_FloorplanType=? ORDER BY Description ASC',$GLOBALS['EntertainmentZone']);
while($rowFOT=$resFOT->FetchRow()){
	$fotArray[$rowFOT['PK_FloorplanObjectType']]=$rowFOT['Description'];
}

$out.='<h3>Rooms</h3>';

if ($action=='form') {
$queryRooms = 'SELECT * FROM Room WHERE FK_Installation = ?';
$resRooms = $dbADO->Execute($queryRooms,array($installationID));

$displayedRooms = array();

	if ($resRooms) {
		if(isset($_GET['eaid'])){
			$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
			if (!$canModifyInstallation){
				header("Location: index.php?section=rooms&error=You are not authorised to change this installation!");
				exit(0);
			}

			$deleteEA='DELETE FROM EntertainArea WHERE PK_EntertainArea=?';
			$dbADO->Execute($deleteEA,$_GET['eaid']);
		}
		
		$out.=setLeftMenu($dbADO)."
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,'',attributes);
			}
		</script>";
		$out.='<div class="err">'.(isset($_GET['error'])?stripslashes($_GET['error']):'').'</div>';
		$out.='
		<table border="0">
			<form action="index.php" method="post" name="rooms" enctype="multipart/form-data">
			<input type="hidden" name="section" value="rooms">
			<input type="hidden" name="action" value="add">
			<input type="hidden" name="lastAction" value="">
		<tr>
			<td align="center" bgcolor="#F0F3F8"><B>Room Description</B></td>
			<td align="center" bgcolor="#F0F3F8"><B>Picture *</B></td>
			<td align="center" bgcolor="#F0F3F8"><B>Don’t show room on Orbiter’s</B></td>
			<td align="center" bgcolor="#DADDE4"><B>Entertain areas</B></td>
		</tr>
		';
		
		$roomsFormValidation='';
		$displayedEntertainArea=array();
		while ($rowRoom = $resRooms->FetchRow()) {
			
			$filePath=$GLOBALS['roomsPicsPath'].$rowRoom['PK_Room'].'.png';
			if(file_exists($filePath)){
				$randNumber=rand(0,99999);
				$roomImage='<img src="include/image.php?imagepath='.$filePath.'&rand='.$randNumber.'"><br>';
			}else
				$roomImage='';
			$out.='
			<tr>
				<td align="center" valign="top"><input type="text" name="roomDesc_'.$rowRoom['PK_Room'].'" value="'.$rowRoom['Description'].'"><br><a href="javascript:void(0);" onClick="if(confirm(\'Are you sure you want to delete this room?\'))windowOpen(\'index.php?section=deleteRoomFromInstallation&from=rooms&roomID='.$rowRoom['PK_Room'].'\',\'status=0,resizable=1,width=200,height=200,toolbars=true\');">Delete Room</a></td>
				<td valign="top">'.@$roomImage.'<input type="file" name="pic_'.$rowRoom['PK_Room'].'"></td>
				<td valign="top" align="center"><input type="checkbox" name="hidden_'.$rowRoom['PK_Room'].'" value="1" '.(($rowRoom['HideFromOrbiter']==1)?'checked':'').'></td>
				';
			$displayedRooms[]=$rowRoom['PK_Room'];
			$out.='<td bgcolor="#DADDE4">';
			$out.='<input type="checkbox" name="manually_'.$rowRoom['PK_Room'].'" value="1" '.(($rowRoom['ManuallyConfigureEA']==1)?'checked':'').' onclick="self.location=\'index.php?section=rooms&action=edit&manID='.$rowRoom['PK_Room'].'&val='.(($rowRoom['ManuallyConfigureEA']==1)?'0':'1').'\'"> Mannually configure.<br>';
			if($rowRoom['ManuallyConfigureEA']==1){
				$queryEntertain='SELECT * FROM EntertainArea WHERE FK_Room=?';
				$resEntertain=$dbADO->Execute($queryEntertain,$rowRoom['PK_Room']);
				
				while($rowEntertain=$resEntertain->FetchRow()){
					$displayedEntertainArea[]=$rowEntertain['PK_EntertainArea'];
					$out.='
					<input type="text" name="entertainArea_'.$rowEntertain['PK_EntertainArea'].'" value="'.$rowEntertain['Description'].'">
					<select name="fot_'.$rowEntertain['PK_EntertainArea'].'">
						<option value="0"></option>
					';
					foreach($fotArray AS $fotID=>$fotDescription){
						$out.='<option value="'.$fotID.'" '.(($fotID==$rowEntertain['FK_FloorplanObjectType'])?'selected':'').'>'.$fotDescription.'</option>';
					}
					$out.='</select>
					<input type="checkbox" name="private_'.$rowEntertain['PK_EntertainArea'].'" '.(($rowEntertain['Private']==1)?'checked':'').' value="1"> Private <a href="index.php?section=rooms&eaid='.$rowEntertain['PK_EntertainArea'].'">Delete area</a> <br>';
				}
				$out.='  <input type="submit" class="button" name="addEA_'.$rowRoom['PK_Room'].'" value="Add EA">';
			}
				$out.='</td>
			</tr>';
			$roomsFormValidation.='
				frmvalidator.addValidation("roomDesc_'.$rowRoom['PK_Room'].'","req","Please enter a description");
			';
		}
			
		$out.='
			<tr>
				<td colspan="4" align="center"> &nbsp;</td>
			</tr>
			<tr>
				<td colspan="4" align="center"> <input type="submit" class="button" name="add" value="Add room"> <input type="submit" class="button" name="save" value="Save changes"></td>
			</tr>
			<tr>
				<td colspan="4" align="left">* Pictures allowed must be JPG or PNG; if will be resized to 160x160 px if it has other resolution</td>
			</tr>
		
			<input type="hidden" name="displayedEntertainArea" value="'.join(",",$displayedEntertainArea).'">
			<input type="hidden" name="displayedRooms" value="'.join(",",$displayedRooms).'">
			</form>
		
		<script>
		 	var frmvalidator = new formValidator("rooms"); 		
			'.$roomsFormValidation.'
		</script>
			
		</table>
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
	if(isset($_REQUEST['manID'])){
		$roomID=(int)$_REQUEST['manID'];
		$newVal=(int)$_REQUEST['val'];
		
		$dbADO->Execute('UPDATE Room SET ManuallyConfigureEA=? WHERE PK_Room=?',array($newVal,$roomID));
		header("Location: index.php?section=rooms&msg=Saved!");
		exit();
	}
	
	$errNotice='';
	if (isset($_POST['add'])) {
		$queryInsertRoom = 'INSERT INTO Room (Description,FK_Installation) values(?,?)';
		$res = $dbADO->Execute($queryInsertRoom,array('New room',$installationID));
		$lastInsert = $dbADO->Insert_ID();
		addScenariosToRoom($lastInsert, $installationID, $dbADO);
		$locationGoTo = "roomDesc_{$lastInsert}";		
	}
	
	$displayedRooms = cleanString($_POST['displayedRooms']);

	$displayedRoomsArray = explode(",",$displayedRooms);
	if (!is_array($displayedRoomsArray) || $displayedRoomsArray===array()) {
		$displayedRoomsArray=array();
	}

	foreach ($displayedRoomsArray as $room) {
		$queryOldValue = "SELECT Description,HideFromOrbiter  FROM Room WHERE PK_Room = ?";
		$resOldValue = $dbADO->Execute($queryOldValue,array($room));
		$oldDesc = '';
		$oldRoomType = '';

		if ($resOldValue) {
			$rowOldValue = $resOldValue->FetchRow();
			$oldDesc = $rowOldValue['Description'];
			$oldHideFromOrbiter=$rowOldValue['HideFromOrbiter'];
		}

		$newDesc = cleanString(@$_POST['roomDesc_'.$room]);
		$newRoomType = (@$_POST['roomType_'.$room]!=0)?cleanInteger(@$_POST['roomType_'.$room]):NULL;
		$HideFromOrbiter=(int)@$_POST['hidden_'.$room];
		
		if ($newDesc!=$oldDesc || $oldHideFromOrbiter!=$HideFromOrbiter)  {
			$query = 'UPDATE Room set Description=?,HideFromOrbiter=? WHERE PK_Room = ?';
			$resUpdRoom = $dbADO->Execute($query,array($newDesc,$HideFromOrbiter,$room));
			$locationGoTo = "roomDesc_".$room;
		}

		// upload room picture
		$imgType=(ereg('png',$_FILES['pic_'.$room]['type']))?'png':'jpg';
		$filePath=$GLOBALS['roomsPicsPath'].$room.'.'.$imgType;
		if($_FILES['pic_'.$room]['name']!=''){
			switch($_FILES['pic_'.$room]['type']){
				case 'image/x-png':
				case 'image/png':
				case 'image/jpg':
				case 'image/pjpeg':
				case 'image/jpeg':				
					$invalidType=false;
				break;
				default:
					$invalidType=true;
				break;
			}
						
			if($invalidType===false){
				if(!move_uploaded_file($_FILES['pic_'.$room]['tmp_name'],$filePath)){
					$errNotice.=$_FILES['pic_'.$room]['name'].' wasn\'t uploaded, check the rights for '.$filePath;
				}else{
					// convert to png and/or resize if necessary
					exec('chmod 777 -R '.$filePath);
					$imageinfo = @getimagesize( $filePath );
					if($imageinfo[0]!=160 && $imageinfo[1]!=160){
						$flag=resizeImage($filePath,$filePath,160,160,1);
					}					
				}
			}else 
				$errNotice.=$_FILES['pic_'.$room]['name'].' is not a valid PNG file.<br>';
		}

		if(isset($_POST['addEA_'.$room])){
			$insertEntertainArea='INSERT INTO EntertainArea (Description,FK_Room) VALUES (?,?)';
			$dbADO->Execute($insertEntertainArea,array($oldDesc,$room));
			$lastInsert = $dbADO->Insert_ID();
			$locationGoTo = "entertainArea_{$lastInsert}";
		}

		$displayedEntertainAreaArray=explode(',',$_POST['displayedEntertainArea']);
		foreach ($displayedEntertainAreaArray as $key => $value){
			$entertainAreaDescription=stripslashes(@$_POST['entertainArea_'.$value]);
			$private=isset($_POST['private_'.$value])?$_POST['private_'.$value]:0;
			$fot=(isset($_POST['fot_'.$value]) && (int)$_POST['fot_'.$value]>0)?$_POST['fot_'.$value]:NULL;

			$updateEntertainArea='UPDATE EntertainArea SET Description=?, Private=?, FK_FloorplanObjectType=? WHERE PK_EntertainArea=?';
			$dbADO->Execute($updateEntertainArea,array($entertainAreaDescription,$private,$fot,$value));
		}
	}

	$commandToSend='/usr/pluto/bin/UpdateEntArea -h localhost';
	exec($commandToSend);
	
	if (strstr($locationGoTo,"#")) {
		header("Location: index.php?section=rooms&msg=Saved!".$locationGoTo.'&error='.$errNotice);
	} else {
		header("Location: index.php?section=rooms&msg=Saved!&lastAction=".$locationGoTo.'&error='.$errNotice);
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
