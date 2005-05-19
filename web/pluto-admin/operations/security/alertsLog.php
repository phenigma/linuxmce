<?
function alertsLog($output,$securitydbADO,$dbADO) {
	/* @var $securitydbADO ADOConnection */
	/* @var $res ADORecordSet */
	
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';

	if (@$_SESSION['userLoggedIn']!=true) {
		header("Location: index.php?section=login");
	}
		$installationID = cleanInteger($_SESSION['installationID']);
	
	$out.='<div align="center"><h3>Alerts log</h3></div>';
	
	if ($action=='form') {
		$out.='<div class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>';
		$out.='
			<form action="index.php" method="post" name="alertsLog">
			<input type="hidden" name="section" value="alertsLog">
			<input type="hidden" name="action" value="add">
			';
		$query='
			SELECT 
				PK_Alert,
				Description, 
				UNIX_TIMESTAMP(DetectionTime) AS DetectionTime, 
				UNIX_TIMESTAMP(ExpirationTime) AS ExpirationTime, 
				UNIX_TIMESTAMP(ResetTime) AS ResetTime,
				EK_Device,
				ResetBeforeExpiration,
				Benign,
				EK_Users
			FROM Alert
				INNER JOIN AlertType ON FK_Alerttype=PK_AlertType
			ORDER BY DetectionTime DESC';
		$out.=multipageAlertsLogs($query, 'index.php?section=alertsLog', (isset($_GET['page_no']))?$_GET['page_no']-1:0, 10,$securitydbADO,$dbADO);
		
		$out.='		
		</form>
		';
	} else {
		// no processing 
				
		header("Location: index.php?section=alertsLog&msg=Alerts types updated.");
	}
	
		
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();  		
}

function multipageAlertsLogs($query, $url, $page_no, $art_pagina,$securitydbADO,$dbADO)
{
	$res=$securitydbADO->Execute($query);
	$total=$res->RecordCount();
	$max_pages = $total/$art_pagina;
	if( $max_pages - floor($max_pages) >= 0 )
		$max_pages = floor($max_pages) + 1;
	else
		$max_pages = floor( $max_pages );

	if($total!=0){
		$output= '<div align="center">';
		for($i=0;$i<$max_pages;$i++){
			$output.=($page_no==$i)?'<font color="red">'.($i+1).'</font> ':'<a href='.$url.'&page_no='.($i+1).'>[ '.($i+1).' ]</a> ';
		}
		$output.='</div>';
	}
	if($total==0){
		$output.='No records';
	}
	else{
		$output.=headerAlertLog();
		$art_index=0;
		$start = $page_no * $art_pagina;
		$res=$securitydbADO->Execute($query." LIMIT  $start,$art_pagina");
	    while($row = $res->FetchRow()){
			$art_index++;
			if($art_index-1 == $art_pagina)
		        break;
			$output.=formatAlertsLog($row, $art_index+$start,$securitydbADO,$dbADO);
		}
		$output.=footerAlertLog();
	}
	$output.='<div align="center">Found: '.$total.'. ';
	if($total!=0)
		$output.= 'Page '.($page_no+1).' from '.$max_pages.'.</div><br>';
	return $output;
}

function headerAlertLog()
{
	$output='<table border="0" align="center">
				<tr bgcolor="lightblue">
					<td align="center"><B>Alert type</B></td>
					<td align="center"><B>Detection time</B></td>
					<td align="center"><B>Expiration time</B></td>
					<td align="center"><B>Reset before expiration</B></td>
					<td align="center"><B>Benign</B></td>
					<td align="center"><B>Reset time</B></td>
					<td align="center"><B>User</B></td>
				</tr>';
	return $output;
}

function footerAlertLog()
{
	$output='</table>';
	return $output;
}

function formatAlertsLog($row, $art_index,$securitydbADO,$dbADO)
{
	$device='';
	
	$user='';
	if($row['EK_Users']!=''){
		$queryUsers='SELECT * FROM Users WHERE PK_Users=?';
		$resUsers=$dbADO->Execute($queryUsers,$row['EK_Users']);
		if($resUsers){
			$rowUsers=$resUsers->FetchRow();
			$user=$rowUsers['UserName'];
		}
	}

	$out='
		<tr bgcolor="'.(($art_index%2==0)?'#F0F3F8':'').'">
			<td align="center">'.$row['Description'].'</td>
			<td align="center">'.date($GLOBALS['defaultDateFormat'],$row['DetectionTime']).'</td>
			<td align="center">'.date($GLOBALS['defaultDateFormat'],$row['ExpirationTime']).'</td>
			<td align="center"><input type="checkbox" name="checkb" '.(($row['ResetBeforeExpiration']==1)?'checked':'').' disabled></td>
			<td align="center"><input type="checkbox" name="checkb" '.(($row['Benign']==1)?'checked':'').' disabled></td>
			<td align="center">'.date($GLOBALS['defaultDateFormat'],$row['ResetTime']).'</td>
			<td align="center">'.$user.'</td>
		</tr>
	';
	$queryAD='SELECT PK_Alert_Device,EK_Device,UNIX_TIMESTAMP(DetectionTime) AS DetectionTime  FROM Alert_Device WHERE FK_Alert=?';
	$resAD=$securitydbADO->Execute($queryAD,$row['PK_Alert']);
	while($rowAD=$resAD->FetchRow()){
		$resD=$dbADO->Execute('SELECT * FROM Device WHERE PK_Device =?',$rowAD['EK_Device']);
		while($rowD=$resD->FetchRow()){
		$out.='
			<tr bgcolor="'.(($art_index%2==0)?'#F0F3F8':'').'">
				<td align="center"><img src="include/image.php?imagepath='.$GLOBALS['SecurityPicsPath'].$rowAD['PK_Alert_Device'].'.jpg"></td>
				<td align="center">'.date($GLOBALS['defaultDateFormat'],$rowAD['DetectionTime']).'</td>
				<td align="left" colspan="5"><B>Device: </B><a href="index.php?section=editDeviceParams&deviceID='.$rowD['PK_Device'].'">'.$rowD['Description'].'</a></td>
			</tr>
			';
		}
	}	
	
	$queryNotifications='SELECT * FROM Notification WHERE FK_Alert=?';
	$resNotifications=$securitydbADO->Execute($queryNotifications,$row['PK_Alert']);
	while($rowNotification=$resNotifications->FetchRow()){
		$out.='
		<tr bgcolor="'.(($art_index%2==0)?'#F0F3F8':'').'">
			<td align="center"><B>Notification</B></td>
			<td align="center">'.date($GLOBALS['defaultDateFormat'],$rowNotification['NotificationTime']).'</td>
			<td align="left" colspan="2"><B>Info: </B>'.$rowNotification['Info'].'</td>
			<td align="left" colspan="3"><B>Result: </B>'.$rowNotification['Result'].'</td>
		</tr>
		';
	}
	
	return $out;
}

?>