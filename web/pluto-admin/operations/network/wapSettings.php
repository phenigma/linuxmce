<?
function wapSettings($output,$dbADO) {
	/* @var $dbADO ADOConnection */
	/* @var $res ADORecordSet */

	$settingsFile=$GLOBALS['wap.conf'];
	if(!file_exists($settingsFile)){
		$_REQUEST['error']='ERROR: The file '.$settingsFile.' does not exist.';
	}else{
		$fileArr=file($settingsFile);
		if($fileArr===false){
			$_REQUEST['error']='ERROR: The file '.$settingsFile.' ciuld not be opened.';
		}else{
			$wapURL=trim(implode('',$fileArr));
		}
	}
	
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID = cleanInteger($_SESSION['installationID']);
	
	
	$out.='<div align="left"><h3>WAP/GPRS settings</h3></div>';
	
	if ($action=='form') {
		

		$out.=setLeftMenu($dbADO).'
		
	<div class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
	<div class="err"><B>'.@$_REQUEST['error'].'</B></div>
	<form action="index.php" method="post" name="wapSettings" onSubmit="return validateInput();">
	<input type="hidden" name="section" value="wapSettings">
	<input type="hidden" name="action" value="add">
	<table>
		<tr>
			<td>Please enter WAP/GPRS URL</td>
			<td><input type="text" name="url" value="'.@$wapURL.'" size="50"></td>
		</tr>
		<tr>
			<td colspan="2" align="center"><input type="submit" class="button" name="save" value="Update"></td>
		</tr>
	</table>
	</form>
		';
	} else {
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=wapSettings&error=You are not authorised to change the installation.");
			exit();
		}
		
		if(isset($_POST['save'])){
			$url=$_POST['url'];
			
			$handle = @fopen($settingsFile, 'w');

			if (!@fwrite($handle, $url)) {
				header("Location: index.php?section=wapSettings&error=Cannot write to ".$settingsFile);
				exit();
			}
			fclose($handle);

			header("Location: index.php?section=wapSettings&msg=WAP/GPRS url was updated.");
			exit();
		}

		header("Location: index.php?section=wapSettings");
	}
	
		
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: WAP/GPRS settings');			
	$output->output();  		
}
?>
