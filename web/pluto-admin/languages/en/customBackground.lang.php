<?
function customBackground($output,$dbADO) {
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
//	$dbADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
	$oID=(int)$_REQUEST['oID'];
	
	$roomsArray=getAssocArray('Room','PK_Room','Description',$dbADO,'WHERE FK_Installation='.$installationID,'');
	
	$orbiterData=getFieldsAsArray('Device','Description',$dbADO,'WHERE PK_Device='.$oID);	

	if ($action == 'form') {
		$existingPics=grabFiles($GLOBALS['orbiterBackgroundPath'],'-type f',$oID);
		$out.='
	<div class="err" align="center">'.stripslashes(@$_GET['err']).'</div>
	<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
		
	<form action="index.php" method="POST" name="customBackground" enctype="multipart/form-data">
	<input type="hidden" name="section" value="customBackground">
	<input type="hidden" name="action" value="add">	
	<input type="hidden" name="oID" value="'.$oID.'">	
		
	<h2>Custom background for orbiter #'.$oID.', '.$orbiterData['Description'][0].'</h2>
	
	Add a custom main menu background:<br>
	<input type="file" name="pic" value=""> '.pulldownFromArray($roomsArray,'room',0,'','key','- Any room -').'	<input type="submit" class="button" name="add" value="Add picture">
	<br><br>';
	if(count($existingPics)>0){
		$out.='	
		Existing backgrounds:
		<table>
			<tr bgcolor="lightblue">
				<td><B>Room</B></td>
				<td><B>Picture</B></td>
				<td><B>Action</B></td>
			</tr>';
		$pos=0;
		foreach ($existingPics AS $pic){
			$pos++;
			$color=($pos%2==0)?'#F0F3F8':'#FFFFFF';
			$roomName=(strpos($pic,'_')!==false)?$roomsArray[(int)str_replace(array($oID.'_','.png'),array('',''),$pic)]:'Any';
			$out.='
			<tr bgcolor="'.$color.'">
				<td>'.$roomName.'</td>
				<td><a href="orbiter_bg/'.$pic.'" target="_blank"><img src="orbiter_bg/'.$pic.'" width="100" height="100" title="Click for larger view" border="0"></a></td>
				<td><a href="javascript:if(confirm(\'Are you sure you want to delete this picture?\'))self.location=\'index.php?section=customBackground&oID='.$oID.'&action=del&dpic='.$pic.'\'">Delete</a></td>
			</tr>';
		}
		$out.='
		</table>';
	}
	$out.='
	</form>';
	
	} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=customBackground&type=$type&error=You are not authorised to change the installation.");
			exit(0);
		}

		if(isset($_POST['add']) && $_FILES['pic']['name']!=''){
			$room=(int)$_REQUEST['room'];
			$picName=$oID.(($room>0)?'_'.$room:'').'.png';
			
			if(move_uploaded_file($_FILES['pic']['tmp_name'],$GLOBALS['orbiterBackgroundPath'].$_FILES['pic']['name'])){
				// recreate pic as png 
				$resizeFlag=resizeImage($GLOBALS['orbiterBackgroundPath'].$_FILES['pic']['name'], $GLOBALS['orbiterBackgroundPath'].$picName,0,0);
				if($resizeFlag!=0){
					$error='Convertion to PNG failed';
				}else{
					@unlink($GLOBALS['orbiterBackgroundPath'].$_FILES['pic']['name']);
				}
			}else{
				//upload fail, prompt error message
				$error='Upload fail, check the rights for '.$GLOBALS['orbiterBackgroundPath'].' directory.';
			}
		}
		
		if(isset($_REQUEST['dpic'])){
			if(!@unlink($GLOBALS['orbiterBackgroundPath'].$_REQUEST['dpic']))
				$error='The picture cannot be deleted, please check the rights for '.$GLOBALS['orbiterBackgroundPath'].$_REQUEST['dpic'];
		}
		
		$sufix=(isset($error)?'&err='.$error:'&msg=Orbiter background was updated.');
		header("Location: index.php?section=customBackground&oID=$oID".$sufix);	
		exit();	
	}

	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Custom background');
	$output->output();
}
?>
