<?
function cellphoneNotifications($output,$dbADO) {
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
//	$dbADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
	$scriptInHead='';	
	
	// TODO: extract Wap Server PK_Device 
	// using dummy device for testing
	$deviceID=555;
	
	if ($action == 'form') {
		$out.='
	<div class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
	<div align="center"><B>'.@$_REQUEST['msg'].'</B></div>
	<form action="index.php" method="POST" name="cellphoneNotifications">
	<input type="hidden" name="section" value="cellphoneNotifications">
	<input type="hidden" name="action" value="add">	
	<div align="center"><h2>Cellphone Notifications</h2>
			<h3>Mobile Orbiter Notifications</h3>';
			$queryDDD='SELECT * FROM Device_DeviceData WHERE FK_Device=? AND FK_DeviceData=?';
			$resDDD=$dbADO->Execute($queryDDD,array($deviceID,$GLOBALS['MobileOrbiterNotification']));
			$monArray=array();
			if($resDDD->RecordCount()>0){
				$rowDDD=$resDDD->FetchRow();
				$monArray=explode(',',$rowDDD['IK_DeviceData']);
			}
		$out.='
			Call Order: 
			<select name="mon_sequence" onchange="form_change()">
				<option value="0" '.(((int)@$monArray[0]==0)?'selected':'').'>Orbiters all at once</option>
				<option value="1" '.(((int)@$monArray[0]>0)?'selected':'').'>Orbiters one at a time</option>
			</select>
			<span id="span_mon_seconds" style="display:none">
				<input type="text" size="2" maxlength="2" name="mon_seconds" value="'.(((int)@$monArray[0]>0)?(int)@$monArray[0]:'10').'" /> seconds each
			</span>
		</div>
		<table align="center" cellpadding="5" cellspacing="0" border="0">
			<tr bgcolor="lightblue">
				<td align="center"><B>Orbiter Phone #</B></td>
				<td align="center"><B>Monitor Mode</B></td>
				<td align="center"><B>Security</B></td>
				<td align="center"><B>Fire</B></td>
				<td align="center"><B>Door Intercom</B></td>
			</tr>
			<input type="hidden" name="oldMon" value="'.@join(',',$monArray).'">';
			for($i=0;$i<4;$i++){
				$out.='
				<tr>
					<td align="center"><input type="text" name="tel_'.(5*$i+1).'" value="'.@$monArray[5*$i+1].'"/></td>
					<td align="center"><input type="checkbox" name="monitor_mode_'.(5*$i+2).'" value="1" '.(((int)@$monArray[5*$i+2]>0)?'checked':'').'/></td>
					<td align="center"><input type="checkbox" name="security_'.(5*$i+3).'" value="1" '.(((int)@$monArray[5*$i+3]>0)?'checked':'').'/></td>
					<td align="center"><input type="checkbox" name="fire_'.(5*$i+4).'" value="1" '.(((int)@$monArray[5*$i+4]>0)?'checked':'').'/></td>
					<td align="center"><input type="checkbox" name="door_intercom_'.(5*$i+5).'" value="1" '.(((int)@$monArray[5*$i+5]>0)?'checked':'').'/></td>
				</tr>';
			}
		$out.='
		</table>
		<div align="center">
		<h3>Other Phone Notifications</h3>';
			$queryDDD='SELECT * FROM Device_DeviceData WHERE FK_Device=? AND FK_DeviceData=?';
			$resDDD=$dbADO->Execute($queryDDD,array($deviceID,$GLOBALS['OtherPhoneNotifications']));
			$opnArray=array();
			if($resDDD->RecordCount()>0){
				$rowDDD=$resDDD->FetchRow();
				$opnArray=explode(',',$rowDDD['IK_DeviceData']);
			}
			
			$out.='
			<select name="opn_order">
				<option value="0" '.(((int)@$opnArray[1]==0)?'selected':'').'>Orbiters then Other</option>
				<option value="1" '.(((int)@$opnArray[1]==1)?'selected':'').'>Other then Orbiters</option>
				<option value="2" '.(((int)@$opnArray[1]==2)?'selected':'').'>Orbiters and Other at the same time</option>
			</select>
			<select name="opn_sequence" onchange="form_change()">
				<option value="0" '.(((int)@$opnArray[0]==0)?'selected':'').'>Other all at once</option>
				<option value="1" '.(((int)@$opnArray[1]>0)?'selected':'').'>Other one at a time</option>
			</select>
			<span id="span_opn_seconds" style="display:none">
				<input type="text" size="2" maxlength="2" name="opn_seconds" value="'.(((int)@$opnArray[0]>0)?@$opnArray[0]:'10').'" /> seconds each
			</span>
		</div>
		<input type="hidden" name="oldOpn" value="'.@join(',',$opnArray).'">
		<table align="center" cellpadding="5" cellspacing="0" border="0">
			<tr bgcolor="lightblue">
				<td align="center"><B>Orbiter Phone #</B></td>
				<td align="center"><B>Monitor Mode</B></td>
				<td align="center"><B>Security</B></td>
				<td align="center"><B>Fire</B></td>
				<td align="center"><B>Door Intercom</B></td>
			</tr>';
			for($i=0;$i<4;$i++){
				$out.='
				<tr>
					<td align="center"><input type="text" name="opn_tel_'.(5*$i+2).'" value="'.@$opnArray[5*$i+2].'"/></td>
					<td align="center"><input type="checkbox" name="opn_monitor_mode_'.(5*$i+3).'" value="1" '.(((int)@$opnArray[5*$i+3]>0)?'checked':'').'/></td>
					<td align="center"><input type="checkbox" name="opn_security_'.(5*$i+4).'" value="1" '.(((int)@$opnArray[5*$i+4]>0)?'checked':'').'/></td>
					<td align="center"><input type="checkbox" name="opn_fire_'.(5*$i+5).'" value="1" '.(((int)@$opnArray[5*$i+5]>0)?'checked':'').'/></td>
					<td align="center"><input type="checkbox" name="opn_door_intercom_'.(5*$i+6).'" value="1" '.(((int)@$opnArray[5*$i+6]>0)?'checked':'').'/></td>
				</tr>';
			}
		$out.='
		</table>
		<div align="center">
			<h3>Neighbors to Call<br /><font size="-1">(when call neighbors button is clicked)</font></h3>
		</div>
		<table align="center" cellpadding="5" cellspacing="0" border="0">
			<tr bgcolor="lightblue">
				<td align="center"><B>Name</B></td>
				<td align="center"><B>Number</B></td>
			</tr>';
			$queryDDD='SELECT * FROM Device_DeviceData WHERE FK_Device=? AND FK_DeviceData=?';
			$resDDD=$dbADO->Execute($queryDDD,array($deviceID,$GLOBALS['NeighborstoCall']));
			$ntcArray=array();
			if($resDDD->RecordCount()>0){
				$rowDDD=$resDDD->FetchRow();
				$ntcArray=explode(',',$rowDDD['IK_DeviceData']);
			}

			for ($i=0; $i < 4; $i++){
				$out.='
				<tr>
					<td><input type="text" name="nname_'.(2*$i).'" value="'.@$ntcArray[2*$i].'" /></td>
					<td><input type="text" name="nphone_'.(2*$i+1).'" value="'.@$ntcArray[2*$i+1].'" /></td>
				</tr>';
			}
		$out.='
		<input type="hidden" name="oldNtc" value="'.join(',',$ntcArray).'">
		</table>
		<div align="center"><input type="submit" name="submit" value="Update"></div>
	</form>';
	$scriptInHead='
	<script language="javascript" type="text/javascript">
		function form_change()
		{
			document.getElementById(\'span_mon_seconds\').style.display = (document.cellphoneNotifications.mon_sequence.value===\'1\') ? \'\' : \'none\';
			document.getElementById(\'span_opn_seconds\').style.display = (document.cellphoneNotifications.opn_sequence.value===\'1\') ? \'\' : \'none\';
		}
		</script>';
	
	} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=cellphoneNotifications&type=$type&error=You are not authorised to change the installation.");
			exit(0);
		}
		
		$oldMon=$_POST['oldMon'];
		$mon_sequence=((int)$_POST['mon_sequence']>0)?(int)$_POST['mon_seconds']:(int)$_POST['mon_sequence'];
		$MONDeviceData=$mon_sequence;
		for($i=0;$i<4;$i++){
			$tel=cleanString($_POST['tel_'.(5*$i+1)]);
			$monitor_mode=(int)@$_POST['monitor_mode_'.(5*$i+2)];
			$security=(int)@$_POST['security_'.(5*$i+3)];
			$fire=(int)@$_POST['fire_'.(5*$i+4)];
			$door_intercom=(int)@$_POST['door_intercom_'.(5*$i+5)];
			if($tel!=''){
				$MONDeviceData.=",$tel,$monitor_mode,$security,$fire,$door_intercom";
			}
		}
		if($oldMon==''){
			$insertMonDDD='INSERT INTO Device_DeviceData (FK_Device, FK_DeviceData, IK_DeviceData) VALUES (?,?,?)';
			$dbADO->Execute($insertMonDDD,array($deviceID,$GLOBALS['MobileOrbiterNotification'],$MONDeviceData));
		}elseif($MONDeviceData!=$oldMon){
			$updateMonDDD='UPDATE Device_DeviceData SET IK_DeviceData=? WHERE FK_Device=? AND FK_DeviceData=?';
			$dbADO->Execute($updateMonDDD,array($MONDeviceData,$deviceID,$GLOBALS['MobileOrbiterNotification']));
		}

		$oldOpn=$_POST['oldOpn'];
		$opn_sequence=((int)$_POST['opn_sequence']>0)?(int)$_POST['opn_seconds']:(int)$_POST['opn_sequence'];
		$opn_order=(int)$_POST['opn_order'];
		
		$OPNDeviceData=$opn_sequence.','.$opn_order;
		for($i=0;$i<4;$i++){
			$tel=cleanString($_POST['opn_tel_'.((5*$i+2))]);
			$monitor_mode=(int)@$_POST['opn_monitor_mode_'.((5*$i+3))];
			$security=(int)@$_POST['opn_security_'.((5*$i+4))];
			$fire=(int)@$_POST['opn_fire_'.((5*$i+5))];
			$door_intercom=(int)@$_POST['opn_door_intercom_'.((5*$i+6))];
			if($tel!=''){
				$OPNDeviceData.=",$tel,$monitor_mode,$security,$fire,$door_intercom";
			}
		}
		if($oldOpn==''){
			$insertOpnDDD='INSERT INTO Device_DeviceData (FK_Device, FK_DeviceData, IK_DeviceData) VALUES (?,?,?)';
			$dbADO->Execute($insertOpnDDD,array($deviceID,$GLOBALS['OtherPhoneNotifications'],$OPNDeviceData));
		}elseif($OPNDeviceData!=$oldOpn){
			$updateOpnDDD='UPDATE Device_DeviceData SET IK_DeviceData=? WHERE FK_Device=? AND FK_DeviceData=?';
			$dbADO->Execute($updateOpnDDD,array($OPNDeviceData,$deviceID,$GLOBALS['OtherPhoneNotifications']));
		}

		$oldNtc=$_POST['oldNtc'];
		$NTCDeviceDataArray=array();
		for($i=0;$i<4;$i++){
			$name=@$_POST['nname_'.(2*$i)];
			$nphone=@$_POST['nphone_'.(2*$i+1)];
			if($name!='' && $nphone!='')
				$NTCDeviceDataArray[]=$name.','.$nphone;
		}
		
		$NTCDeviceData=join(',',$NTCDeviceDataArray);
		if($oldNtc=='' && $NTCDeviceData!=''){
			$insertNTCDeviceData='INSERT INTO Device_DeviceData (FK_Device, FK_DeviceData, IK_DeviceData) VALUES (?,?,?)';
			$dbADO->Execute($insertNTCDeviceData,array($deviceID,$GLOBALS['NeighborstoCall'],$NTCDeviceData));
		}elseif($NTCDeviceData!=$oldNtc){
			$updateNtcDeviceData='UPDATE Device_DeviceData SET IK_DeviceData=? WHERE FK_Device=? AND FK_DeviceData=?';
			$dbADO->Execute($updateNtcDeviceData,array($NTCDeviceData,$deviceID,$GLOBALS['NeighborstoCall']));
		}
		
		header("Location: index.php?section=cellphoneNotifications&msg=Cellphone notifications was updated");		
	}

	$output->setScriptInHead($scriptInHead);
	$output->setScriptInBody('onLoad="form_change()"');
	$output->setNavigationMenu(array("Cellphone Notifications"=>'index.php?section=cellphoneNotifications'));
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Cellphone notifications');
	$output->output();
}
?>