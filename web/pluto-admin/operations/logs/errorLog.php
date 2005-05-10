<?
function errorLog($output,$dbADO) {

	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */

	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$deviceID=(int)@$_REQUEST['deviceID'];
	$installationID=(int)@$_SESSION['installationID'];
	if($deviceID==0){
		die('Invalid Device ID specified.');
	}

	if ($action == 'form') {
		$selectDevice='
			SELECT Device.Description,PK_Device,DeviceTemplate.Description AS Template, DeviceCategory.Description AS Category,Manufacturer.Description AS Manufacturer, FK_Manufacturer,FK_DeviceCategory
			FROM Device
			INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
			INNER JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory
			INNER JOIN Manufacturer ON FK_Manufacturer=PK_Manufacturer
			WHERE PK_Device=?';
		$resDevice=$dbADO->Execute($selectDevice,$deviceID);
		$rowDevice=$resDevice->FetchRow();
		$logName='/var/log/pluto/'.$deviceID.'_'.str_replace(array(' ','\''),array('_','_'),$rowDevice['Template']).'.log';
		
		if(file_exists($logName)){
			exec('grep \'^01\' '.$logName.' | /usr/pluto/bin/ansi2html',$retArray);
			$formatedText = join('<br>',$retArray);
		}else{
			$formatedText ='Log not found: '.$logName;
		}
		
		$out.='
		<div class="err">'.strip_tags(@$_GET['error']).'</div>
		<div class="confirm"><B>'.stripslashes(@$_GET['msg']).'</B></div>
		<form action="index.php" method="POST" name="errorLog" enctype="multipart/form-data">
		<input type="hidden" name="section" value="errorLog">
		<input type="hidden" name="deviceID" value="'.$deviceID.'">
		<input type="hidden" name="action" value="add">	
		<h3>Error log</h3>
			<table width="100%">
				<tr>
					<td valign="top" colspan="3">Device <B>'.$rowDevice['Description'].'</B>, # <B>'.$rowDevice['PK_Device'].'</B> <td>
				</tr>
				<tr>
					<td valign="top" colspan="3">Device template <B>'.$rowDevice['Template'].'</B>, category <B>'.$rowDevice['Category'].'</B> and manufacturer <B>'.$rowDevice['Manufacturer'].'</B>.<td>
				</tr>
				<tr>
					<td valign="top" colspan="3">&nbsp;<td>
				</tr>		
				<tr>
					<td valign="top" colspan="3" style="background:black;color:white;"><pre>'.$formatedText.'</pre><td>
				</tr>		
			</table>
			</form>
		</form>
		';
	} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=errorLog&deviceID=$deviceID&error=You are not authorised to change the installation.");
			exit(0);
		}
		
		header("Location: index.php?section=errorLog&deviceID=$deviceID");
		
	}

	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Error Log');
	$output->output();
}
?>