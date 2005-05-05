<?php
function userPic($output,$dbADO) {
	global $changePassMasterUserUrl;
	$out='';
	$action = isset($_POST['action'])?cleanString($_POST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	$userID = isset($_REQUEST['userID'])?cleanInteger($_REQUEST['userID']):0;
	
	$installationID = cleanInteger($_SESSION['installationID']);
	
	$filePath=$GLOBALS['usersPicsPath'].$userID.'.jpg';
	if(file_exists($filePath)){
		$randNumber=rand(0,99999);
		$userImage='<img src="include/image.php?imagepath='.$filePath.'&rand='.$randNumber.'">';
	}
	
	if ($action=='form') {
		
		$out.='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		</script>
		<div class="err">'.stripslashes(@$_GET['error']).'</div>
		<div class="confirm">'.stripslashes(@$_GET['msg']).'</div>
		
		<form action="index.php" method="post" name="userPic" enctype="multipart/form-data">
			<input type="hidden" name="section" value="userPic">
			<input type="hidden" name="action" value="add">
			<input type="hidden" name="from" value="'.$from.'">
			<input type="hidden" name="userID" value="'.$userID.'">
		<h3>Update user\'s picture</h3>
		<table>	
			<tr>
				<td align="center">'.@$userImage.'</td>
			</tr>
			<tr>
				<td align="center">Choose picture * <input type="file" name="pic"> <input type="submit" class="button" name="add" value="Upload picture"></td>
			</tr>
			<tr>
				<td align="left">* Allowed file type is JPG. <br>
		&nbsp;&nbsp;&nbsp;Please use a file with a ratio width/height=1. <br>
		&nbsp;&nbsp;&nbsp;For best results, the picture should have 160px x 160px.</td>
			</tr>		
		</table>
		</form>
			';
			
			$out.='
				<script>
		 			var frmvalidator = new formValidator("userPic"); 		
					frmvalidator.addValidation("passOrPIN","req","Please enter old password or PIN");
				</script>
			';
		
	} else {
		
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$installationID,$dbADO);
		if(!$canModifyInstallation){
			header("Location: index.php?section=userPic&from=$from&userID=$userID&error=You are not allowed to modify installation.");
			exit();
		}
		
		if($_FILES['pic']['name']!=''){
			switch($_FILES['pic']['type']){
				case 'image/jpg':
				case 'image/pjpeg':
				case 'image/jpeg': $invalidType=false;
				break;
				default:
					$invalidType=true;
				break;
			}
			
			if($invalidType===true){
				header("Location: index.php?section=userPic&from=$from&userID=$userID&error=Invalid file type. Please upload a JPG file.");
				exit();
			}
			
			if(!move_uploaded_file($_FILES['pic']['tmp_name'],$filePath)){
				header("Location: index.php?section=userPic&from=$from&userID=$userID&error=Check the rights for $filePath.");
				exit();
			}else{
				$resizeFlag=resizeImage($filePath, $filePath, 160, 160);
			}
			
			header("Location: index.php?section=userPic&from=$from&userID=$userID&msg=Picture updated.");
		}
	}
		
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Update picture');			
	$output->output();
}
?>
