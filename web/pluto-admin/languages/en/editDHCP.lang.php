<?
function editDHCP($output,$dbADO) {
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
	$dhcpID=(int)$_REQUEST['dhcpID'];
	
	if($action=='form'){
		
		$out.='
		<div align="center" class="err">'.@$_REQUEST['error'].'</div>
		<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
		<h3 align="center">Edit DHCP Device</h3>
		<form action="index.php" method="POST" name="editDHCP">
		<input type="hidden" name="section" value="editDHCP">
		<input type="hidden" name="action" value="update">
		<input type="hidden" name="dhcpID" value="'.$dhcpID.'">
		
		<table cellpadding="3" align="center">
			<tr bgcolor="lightblue">
				<td><B>Device Data</B></td>
				<td><B>Parameters</B></td>
				<td><B>Action</B></td>
			</tr>';
		if($dhcpID!=0){
			$resData=$dbADO->Execute('
				SELECT DHCPDevice_DeviceData.*, DeviceData.Description AS DeviceData 
				FROM DHCPDevice_DeviceData 
				INNER JOIN DeviceData ON FK_DeviceData=PK_DeviceData
				WHERE FK_DHCPDevice=?',$dhcpID);
			$displayedDDArray=array();
			if($resData->RecordCount()==0){
				$out.='
				<tr>
					<td colspan="3">No records.</td>
				</tr>';
			}
			while($rowData=$resData->FetchRow()){
				$displayedDDArray[]=$rowData['FK_DeviceData'];
				$out.='
					<tr>
						<td>'.$rowData['DeviceData'].'</td>
						<td><input type="text" name="data_'.$rowData['FK_DeviceData'].'" value="'.$rowData['IK_DeviceData'].'"></td>
						<td><input type="button" class="button" name="del" value="Delete" onClick="self.location=\'index.php?section=editDHCP&action=del&dhcpID='.$dhcpID.'&delDD='.$rowData['FK_DeviceData'].'\'"></td>
					</tr>';
			}
			if($resData->RecordCount()!=0){
				$out.='
				<tr>
					<td colspan="3" align="center"><input type="submit" class="button" name="update" value="Update"></td>
				</tr>
				<tr>
					<td colspan="3">&nbsp;</td>
				</tr>';
			}
		
			$out.='<input type="hidden" name="displayedDDArray" value="'.join(',',$displayedDDArray).'">';
			if(count($displayedDDArray)==0)
				$displayedDDArray[]=0;
			$resRemainingDD=$dbADO->Execute('SELECT * FROM DeviceData WHERE PK_DeviceData NOT IN ('.join(',',$displayedDDArray).') ORDER BY Description ASC');
			$out.='
				<tr>
					<td><select name="newDeviceData">
						<option value="0">- Please select -</option>';
				while($rowDD=$resRemainingDD->FetchRow()){
					$out.='<option value="'.$rowDD['PK_DeviceData'].'">'.$rowDD['Description'].'</option>';
				}			
				$out.='
					</select></td>
					<td><input type="text" name="deviceDataValue" value=""></td>
					<td><input type="submit" name="add" value="Add"></td>
				</tr>
			';
		}
		$out.='	
			<tr>
				<td colspan="3" align="center"><input type="button" class="button" name="cancel" value="Close window" onClick="self.close();"></td>
			</tr>
		</table>
		</form>';
	}else{
	// process area
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$installationID,$dbADO);
	
		if (!$canModifyInstallation) {
			header('Location: index.php?section=editDHCP&dhcpID='.$dhcpID.'&error=You are not authorised to change the installation');
			exit();
		}
	
		if(isset($_POST['add'])){
			$newDeviceData=(int)$_POST['newDeviceData'];
			$deviceDataValue=$_POST['deviceDataValue'];
			$dbADO->Execute('INSERT INTO DHCPDevice_DeviceData (FK_DHCPDevice,FK_DeviceData,IK_DeviceData) VALUES (?,?,?)',array($dhcpID,$newDeviceData,$deviceDataValue));
			
			header('Location: index.php?section=editDHCP&dhcpID='.$dhcpID.'&msg=Device Data added.');
			exit();
		}
		
		if(isset($_POST['update'])){
			$displayedDDArray=explode(',',$_POST['displayedDDArray']);
			foreach ($displayedDDArray AS $ddID){
				$deviceDataValue=@$_POST['data_'.$ddID];
				$dbADO->Execute('UPDATE DHCPDevice_DeviceData SET IK_DeviceData=? WHERE FK_DeviceData=? AND FK_DHCPDevice=?',array($deviceDataValue,$ddID,$dhcpID));
			}
			header('Location: index.php?section=editDHCP&dhcpID='.$dhcpID.'&msg=Device Data updated.');
			exit();
		}

		if(isset($_REQUEST['delDD'])){
			$delDD=(int)$_REQUEST['delDD'];
			$dbADO->Execute('DELETE FROM DHCPDevice_DeviceData WHERE FK_DeviceData=? AND FK_DHCPDevice=?',array($delDD,$dhcpID));

			header('Location: index.php?section=editDHCP&dhcpID='.$dhcpID.'&msg=Device Data deleted from device.');
			exit();
		}
		
		header('Location: index.php?section=editDHCP&dhcpID='.$dhcpID);
	}
	
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Edit DHCP Device Data');
	$output->output();
}

?>