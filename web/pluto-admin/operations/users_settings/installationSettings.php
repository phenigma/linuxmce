<?php
function installationSettings($output,$dbADO) {
	//$dbADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	
	$installationID = cleanInteger($_SESSION['installationID']);
		
	$query = "
		SELECT Installation.*,Version.Description AS V_Desc FROM Installation 
			LEFT JOIN Version on FK_Version	= PK_Version 
		WHERE 
		PK_Installation = ?
	";
	$selectInstallation = $dbADO->Execute($query,array($installationID));
	
	if ($selectInstallation) {
		$rowInstallation = $selectInstallation->FetchRow();			
	}
	
	if ($action=='form') {		
		$out.='
		<div class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
		<form action="index.php" method="post" name="installationSettings">
		<input type="hidden" name="section" value="installationSettings">
		<input type="hidden" name="action" value="add">		
		<input type="hidden" name="lastAction" value="">
		<input type="hidden" name="from" value="'.$from.'">
			<table width="300">			
				<tr>
					<td width="100"><B>Description</B>:</td>
					<td><input type="text" size="30" name="Description" value="'.$rowInstallation['Description'].'"></td>
				</tr>
				<tr>
					<td><B>Name</B>:</td>
					<td><input type="text" size="30" name="Name" value="'.$rowInstallation['Name'].'"></td>
				</tr>
				<tr>
					<td><B>Address</B>:</td>
					<td><input type="text" size="30" name="Address" value="'.$rowInstallation['Address'].'"></td>
				</tr>
				<tr>
					<td colspan="2">This information is optional and presently not used. In the future some plug-ins, like security monitoring, may require address information.</td>
				</tr>
				<tr>
					<td><B>City</B>:</td>
					<td><input type="text" size="30" name="City" value="'.$rowInstallation['City'].'"></td>
				</tr>
				<tr>
					<td><B>State</B>:</td>
					<td><input type="text" size="30" name="State" value="'.$rowInstallation['State'].'"></td>
				</tr>
				<tr>
					<td><B>Country</B>:</td>
					<td>'.generatePullDown('country','Country','PK_Country','Description',$rowInstallation['FK_Country'],$dbADO).'</td>
				</tr>		
				<tr>
					<td><B>Zip/Postal Code</B>:</td>
					<td><input type="text" size="30" name="Zip" value="'.$rowInstallation['Zip'].'"></td>
				</tr>
				<tr>
					<td colspan="2" align="center"><input type="submit" class="button" name="submitX" value="Save"  ></td>
				</tr>
			</table>
		</form>
		
		<script>
		 	var frmvalidator = new formValidator("installationSettings");
 			frmvalidator.addValidation("Description","req","Please enter a description");
			frmvalidator.addValidation("Name","req","Please enter a name");			
		</script>
		';
		
	} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=installationSettings&error=You are not authorised to change the installation.");
			exit(0);
		}
		
		
		
		$description = cleanString(@$_POST['Description'],50);
		$name = cleanString(@$_POST['Name'],50);
		$address = cleanString(@$_POST['Address'],50);
		$city = cleanString(@$_POST['City'],50);
		$state = cleanString(@$_POST['State'],50);
		$zip = cleanString(@$_POST['Zip'],50);
		$country=($_POST['country']!='0')?(int)$_POST['country']:NULL;
		
		if ($installationID!=0 && $description!='' && $name!='') {
			
			$queryUpdate = 'UPDATE Installation Set Description=?,Name=?,Address=?,City=?,State=?,Zip=?,FK_Country=? 
							WHERE PK_Installation = ?';
			$dbADO->Execute($queryUpdate,array($description,$name,$address,$city,$state,$zip,$country,$installationID));
			
			$out.="
				<script>
					alert('Installation modified!');									    
				</script>
				";				
			header("Location: index.php?section=installationSettings&from=$from");
		} else {
			header("Location: index.php?section=installationSettings&saved=yes");
		}
				
	}
	
	$output->setNavigationMenu(array("Settings"=>"index.php?section=installationSettings"));
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();
}
?>
