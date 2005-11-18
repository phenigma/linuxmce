<?
function modeChangesLog($output,$securitydbADO,$dbADO) {
	/* @var $securitydbADO ADOConnection */
	/* @var $res ADORecordSet */
	
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';

	if (@$_SESSION['userLoggedIn']!=true) {
		header("Location: index.php?section=login");
	}
		$installationID = cleanInteger($_SESSION['installationID']);
	
	$out.='<div align="center"><h3>Mode changes log</h3></div>';
	
	if ($action=='form') {
		$out.='<div class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>';
		$out.='
			<form action="index.php" method="post" name="modeChangesLog">
			<input type="hidden" name="section" value="modeChangesLog">
			<input type="hidden" name="action" value="add">
			';
		$query='
			SELECT EK_HouseMode, EK_DeviceGroup, EK_Users, UNIX_TIMESTAMP(ChangeTime) AS ChangeTime  FROM ModeChange ORDER BY ChangeTime DESC';
		$out.=multipagemodeChangesLogs($query, 'index.php?section=modeChangesLog', (isset($_GET['page_no']))?$_GET['page_no']-1:0, 20,$securitydbADO,$dbADO);
		
		$out.='
			</table>
		</form>
		';
	} else {
		// no processing 
				
		header("Location: index.php?section=modeChangesLog&msg=Alerts types updated.");
	}
	
		
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();  		
}

function multipagemodeChangesLogs($query, $url, $page_no, $art_pagina,$securitydbADO,$dbADO)
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
		$output.=headerModeChangeLog();
		$art_index=0;
		$start = $page_no * $art_pagina;
		$res=$securitydbADO->Execute($query." LIMIT  $start,$art_pagina");
	    while($row = $res->FetchRow()){
			$art_index++;
			if($art_index-1 == $art_pagina)
		        break;
			$output.=formatmodeChangesLog($row, $art_index+$start,$securitydbADO,$dbADO);
		}
		$output.=footerModeChangeLog();
	}
	$output.='<div align="center">Found: '.$total.'. ';
	if($total!=0)
		$output.= 'Page '.($page_no+1).' from '.$max_pages.'.</div><br>';
	return $output;
}

function headerModeChangeLog()
{
	$output='<table border="0" align="center">
				<tr bgcolor="lightblue">
					<td align="center"><B>Change Time</B></td>
					<td align="center"><B>House mode</B></td>
					<td align="center"><B>Zone</B></td>
					<td align="center"><B>User</B></td>
				</tr>';
	return $output;
}

function footerModeChangeLog()
{
	$output='</table>';
	return $output;
}

function formatmodeChangesLog($row, $art_index,$securitydbADO,$dbADO)
{
	$houseMode='';
	if($row['EK_HouseMode']!=''){
		$queryHouseMode='SELECT * FROM HouseMode WHERE PK_HouseMode=?';
		$resHouseMode=$dbADO->Execute($queryHouseMode,(int)$row['EK_HouseMode']);
		if($resHouseMode){
			$rowHouseMode=$resHouseMode->FetchRow();
			$houseMode=$rowHouseMode['Description'];
		}
	}
	
	$user='';
	if($row['EK_Users']!=''){
		$queryUsers='SELECT * FROM Users WHERE PK_Users=?';
		$resUsers=$dbADO->Execute($queryUsers,$row['EK_Users']);
		if($resUsers){
			$rowUsers=$resUsers->FetchRow();
			$user=$rowUsers['UserName'];
		}
	}

	$deviceGroup='';
	if($row['EK_DeviceGroup']!=''){
		$queryDeviceGroup='SELECT * FROM DeviceGroup WHERE PK_DeviceGroup=?';
		$resDeviceGroup=$dbADO->Execute($queryDeviceGroup,$row['EK_DeviceGroup']);
		if($resDeviceGroup){
			$rowDeviceGroup=$resDeviceGroup->FetchRow();
			$deviceGroup=$rowDeviceGroup['Description'];
		}
	}
	
	$out='
		<tr bgcolor="'.(($art_index%2==0)?'#F0F3F8':'').'">
			<td align="center">'.date($GLOBALS['defaultDateFormat'],$row['ChangeTime']).'</td>
			<td align="center">'.$houseMode.'</td>
			<td align="center">'.$deviceGroup.'</td>
			<td align="center">'.$user.'</td>
		</tr>
	';
	
	return $out;
}

?>