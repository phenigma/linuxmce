<?
function callRouting($output,$dbADO) {
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
	
	if($action=='form'){
		
		$out.='
		<div align="center" class="err">'.@$_REQUEST['error'].'</div>
		<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
		<h3 align="center">Phone Lines</h3>
		<form action="index.php" method="POST" name="callRouting">
			<input type="hidden" name="section" value="callRouting">
			<input type="hidden" name="action" value="update">
		<table align="center" cellpadding="3">
			<tr bgcolor="lightblue">
				<td><B>Username</B></td>
				<td><B>Registrar IP</B></td>
				<td><B>Registrar Port</B></td>
				<td><B>Registration timeout</B></td>
				<td>&nbsp;</td>
			</tr>
		';
		$resLines=$dbADO->Execute("SELECT * FROM sip_buddies WHERE type='peer'");
		$pos=0;
		$linesArray=array();
		while($row=$resLines->FetchRow()){
			$linesArray[]=$row['uniqueid'];
			$pos++;
			$out.='
			<tr bgcolor="'.(($pos%2==0)?'#EEEEEE':'#E5E5E5').'">
				<td>'.$row['username'].'</td>
				<td>'.$row['ipaddr'].'</td>
				<td align="center">'.$row['port'].'</td>
				<td align="center">'.$row['rtptimeout'].'</td>
				<td align="center"><input type="radio" name="routing_'.$row['uniqueid'].'" value="international" checked> International <input type="radio" name="routing_'.$row['uniqueid'].'" value="national"> National</td>
			</tr>
			';
		}
		if(count($linesArray)>0){
			$out.='
				<tr>
					<td colspan="5" align="center"><input type="submit" name="update" value="Update"></td>
				</tr>
			';
		}
		$out.='
			<input type="hidden" name="linesArray" value="'.join(',',$linesArray).'">
		</table>
		</form>
		<br>
		';
	}else{
	// process area
	
		header('Location: index.php?section=callRouting');
	}
	
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}

?>