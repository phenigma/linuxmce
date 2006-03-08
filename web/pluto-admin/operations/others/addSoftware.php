<?
function addSoftware($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/addSoftware.lang.php');
	
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
	
	if($action=='form'){
		
		$out.='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
			function enableRepName()
			{
				if(document.addSoftware.debianRepository.checked){
					document.addSoftware.repositoryName.disabled=false;
				}else{
					document.addSoftware.repositoryName.disabled=true;
					document.addSoftware.repositoryName.value=\'\';
				}
			}
		</script>		
			<div align="center" class="err">'.@$_REQUEST['error'].'</div>
			<div align="center"><B>'.@$_REQUEST['msg'].'</B></div>
			
			<h3 align="left">'.$TEXT_ADD_SOFTWARE_CONST.'</h3>
			<form action="index.php" method="POST" name="addSoftware">
			<input type="hidden" name="section" value="addSoftware">
			<input type="hidden" name="action" value="add">
		
		<table>
			<tr>
				<td><B>'.$TEXT_ADD_SOFTWARE_ON_WHICH_COMPUTER_CONST.'</B></td>
				<td><select name="deviceIP">';
		$deviceCategory=$GLOBALS['rootComputerID'];

		$computersDTArray=getDeviceTemplatesFromCategory($deviceCategory,$dbADO);
		if(count($computersDTArray)==0)
			$computersDTArray[]=0;
		
		$queryComputers='
			SELECT Description,IPaddress,PK_Device FROM Device 
			WHERE FK_DeviceTemplate IN ('.join(',',$computersDTArray).') AND FK_Installation=? 
			ORDER BY Description ASC';
		$resComputers=$dbADO->Execute($queryComputers,array($installationID));
		while($row=$resComputers->FetchRow()){
			$out.='<option value="'.$row['PK_Device'].':'.$row['IPaddress'].'" '.(($row['PK_Device'].':'.$row['IPaddress']==@$_SESSION['deviceIP'])?'selected':'').'>'.$row['Description'].'</option>';
		}
		$out.='</select>
				</td>
			</tr>
			<tr>
				<td><B>'.$TEXT_SOFTWARE_PACKAGE_NAME_CONST.'</B></td>
				<td><input type="text" name="packageName" value="'.@$_SESSION['packageName'].'"></td>
			</tr>
			<tr>
				<td colspan="2"><br><br>'.$TEXT_NONPLUTO_MANTAINED_CONST.'</td>
			</tr>
			<tr>
				<td><B>'.$TEXT_REPOSITORY_URL_CONST.'</B></td>
				<td><input type="text" name="url" value="'.@$_SESSION['url'].'" size="50"></td>
			</tr>
			<tr>
				<td><B>'.$TEXT_DEBIAN_REPOSITORY_CONST.'</B></td>
				<td><input type="checkbox" name="debianRepository" value="1" '.((@$_SESSION['debianRepository']==1)?'checked':'').' onClick="enableRepName();"> </td>
			</tr>		
			<tr>
				<td><B>'.$TEXT_REPOSITORY_NAME_CONST.'</B></td>
				<td><input type="text" name="repositoryName" value="'.@$_SESSION['repositoryName'].'" '.((@$_SESSION['debianRepository']!=1)?'disabled':'').'></td>
			</tr>
			<tr>
				<td colspan="2" align="center"><input type="submit" class="button" name="add" value="'.$TEXT_ADD_SOFTWARE_CONST.'"></td>
			</tr>
		</table>
		</form>';
	}else{
	// process area
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=addSoftware&error=$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST");
			exit();
		}	

		if(isset($_POST['add'])){
			$deviceParts=explode(':',$_POST['deviceIP']);
			$deviceIP=$deviceParts[1];
			$packageName=$_POST['packageName'];
			$url=((isset($_POST['debianRepository']) && (int)$_POST['debianRepository']==1)?'deb ':'').$_POST['url'];
			$repositoryName=@$_POST['repositoryName'];

			$cmd="/usr/pluto/bin/InstallSoftware.sh '$deviceIP' '$packageName' '$url' '$repositoryName'";
			$msg=exec($cmd);
			$_SESSION['deviceIP']=$deviceParts[0].':'.$deviceIP;
			$_SESSION['packageName']=$packageName;
			$_SESSION['url']=$_POST['url'];
			$_SESSION['debianRepository']=(isset($_POST['debianRepository']) && (int)$_POST['debianRepository']==1)?1:0;
			$_SESSION['repositoryName']=$repositoryName;
		}
		
		header('Location: index.php?section=addSoftware&msg='.$msg);
	}
	
	$output->setNavigationMenu(array($TEXT_ADD_SOFTWARE_CONST=>'index.php?section=addSoftware'));	
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_ADD_SOFTWARE_CONST);
	$output->output();
}
?>
