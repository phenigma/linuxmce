<?
function softwareVersions($output,$dbADO) {
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
	
	if($action=='form'){
		
		$out.='
			<div align="center" class="err">'.@$_REQUEST['error'].'</div>
			<div align="center"><B>'.@$_REQUEST['msg'].'</B></div>
			<h3 align="left">Software versions</h3>
			<form action="index.php" method="POST" name="softwareVersions">
			<input type="hidden" name="section" value="softwareVersions">
			<input type="hidden" name="action" value="form">
		<table>
			<tr>
				<td><B>Which computer?</B></td>
				<td><select name="computer">';
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
				<td colspan="2" align="center"><input type="submit" class="button" name="display" value="Display"></td>
			</tr>
		</table>
		</form>';
		if(isset($_POST['display'])){
			$computer=$_POST['computer'];
			$parts=explode(':',$computer);
			if(isCore($parts[0],$dbADO)){
				$command='sudo -u root /usr/pluto/bin/ListSoftware.sh install 127.0.0.1';	
			}
			elseif($parts[1]!=''){
				$command='sudo -u root /usr/pluto/bin/ListSoftware.sh install '.$parts[1];	
			}else {
				header('Location: index.php?section=softwareVersions&error=No IP for media director selected.');
				exit();
			}
			exec($command,$retArray);
			$out.=implode('<br>',$retArray);
		}
	}else{
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=softwareVersions&error=You are not authorised to change the installation.");
			exit();
		}	
		
		header('Location: index.php?section=softwareVersions&msg='.$msg);
	}
	
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}
?>
