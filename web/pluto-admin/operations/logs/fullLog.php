<?
function fullLog($output,$dbADO) {

	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */

	$out='';
	$rowsPerPage=5000;
	$from=(isset($_REQUEST['from']))?(int)$_REQUEST['from']:0;
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	
	$out='';
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
		
		$pagesArray=array();
		if(file_exists($logName)){
			exec('wc -l '.$logName,$retArray);	// (afisheaza: <nr. linii> <spatziu> <nume fishier>)
			$total=explode(' ',$retArray[0]);
			
			$to=(($from+$rowsPerPage)>=$total[0])?$total[0]:($from+$rowsPerPage);
			$nrPages=ceil($total[0]/$rowsPerPage);
			
			$currentPage=$from/$rowsPerPage;
			for($i=0;$i<$nrPages;$i++){
				$pagesArray[]=($i==$currentPage)?($i+1):'<a href="fullLog.php?deviceID='.$deviceID.'&from='.($i*$rowsPerPage).'">'.($i+1).'</a>';
			}
			
			$aqquireCommand='awk \'NR>='.$from.' && NR<='.($from+$rowsPerPage).'\' '.$logName.' | /usr/pluto/bin/ansi2html';
			exec($aqquireCommand,$linesArray);
			$formatedText = join('<br>',$linesArray);
			$numbersNotice=$total[0].' lines, displaying from '.$from.' to '.$to;
		}else{
			$formatedText ='Log not found: '.$logName;
		}
		
		
		$out= '
		<input type="hidden" name="action" value="add">	
		<h3>Complete log</h3>
			<table width="100%">
				<tr>
					<td valign="top" colspan="3">Device <B>'.$rowDevice['Description'].'</B>, # <B>'.$rowDevice['PK_Device'].'</B> <td>
				</tr>
				<tr>
					<td valign="top" colspan="3">Device template <B>'.$rowDevice['Template'].'</B>, category <B>'.$rowDevice['Category'].'</B> and manufacturer <B>'.$rowDevice['Manufacturer'].'</B>.<td>
				</tr>
				<tr>
					<td valign="top" colspan="3"><hr><td>
				</tr>		
				<tr>
					<td valign="top" colspan="3" align="center">'.@$numbersNotice.'<td>
				</tr>		
				<tr>
					<td valign="top" colspan="3" align="center">'.join(' ',$pagesArray).'<td>
				</tr>	
				<tr>
					<td valign="top" colspan="3" style="background:black;color:white;"><pre>'.$formatedText.'<td>
				</tr>		
			</table>
			</form>
		</form>
		';
	} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=fullLog&deviceID=$deviceID&error=You are not authorised to change the installation.");
			exit(0);
		}
		
		header("Location: index.php?section=fullLog&deviceID=$deviceID");
		
	}

	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Error Log');
	$output->output();
}
?>