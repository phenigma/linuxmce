<?
function firewall($output,$dbADO) {
	/* @var $dbADO ADOConnection */
	/* @var $res ADORecordSet */
//	$dbADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];

	
	if ($action == 'form') {
		$out.='
	<script>
	function enableDestination()
	{
		if(document.firewall.RuleType.value=="port_forward"){
			document.firewall.DestinationPort.disabled=false;
			document.firewall.DestinationIP.disabled=false;
		}else{
			document.firewall.DestinationPort.disabled=true;
			document.firewall.DestinationIP.disabled=true;
		}
	}
	</script>
	<div class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
	<form action="index.php" method="POST" name="firewall">
	<input type="hidden" name="section" value="firewall">
	<input type="hidden" name="action" value="add">
	<div align="center"><B>'.(isset($_GET['msg'])?strip_tags($_GET['msg']):'').'</B></div>
	<table border="0" align="center">
		<tr>
			<td colspan="6" align="center"><b>Firewall rules</b></td>
		</tr>
		<tr bgcolor="#EEEEEE">
			<td align="center"><B>Protocol</B></td>
			<td align="center"><B>Source Port</B></td>
			<td align="center"><B>Destination Port</B></td>
			<td align="center"><B>Destination IP</B></td>
			<td align="center"><B>Rule Type</B></td>
			<td>&nbsp;</td>
		</tr>';
		$res=$dbADO->Execute('SELECT * FROM Firewall');
		while($row=$res->FetchRow()){
			$out.='
				<tr>
					<td align="center">'.$row['Protocol'].'</td>
					<td align="center">'.$row['SourcePort'].' to '.$row['SourcePortEnd'].'</B></td>
					<td align="center">'.$row['DestinationPort'].'</td>
					<td align="center">'.$row['DestinationIP'].'</td>
					<td align="center">'.$row['RuleType'].'</td>
					<td align="center"><a href="index.php?section=firewall&action=del&delid='.$row['PK_Firewall'].'">Delete</a></td>
				</tr>';
		}
		$out.='
		<tr>
			<td colspan="6" align="center">&nbsp;</td>
		</tr>		
		<tr>
			<td colspan="6" align="center" bgcolor="#EEEEEE"><B>Add new firewall rule</B></td>
		</tr>
		<tr bgcolor="#EEEEEE">
			<td align="center"><B>Protocol</B></td>
			<td align="center"><B>Source Port</B></td>
			<td align="center"><B>Destination Port</B></td>
			<td align="center"><B>Destination IP</B></td>
			<td align="center"><B>Rule Type</B></td>
			<td>&nbsp;</td>
		</tr>				
		<tr>
			<td align="center"><select name="protocol">
				<option value="tcp">tcp</option>
				<option value="udp">udp</option>
			</select></td>
			<td align="center"><input type="text" name="SourcePort" size="2"> to <input type="text" name="SourcePortEnd" size="2"></td>
			<td align="center"><input type="text" name="DestinationPort" size="2" disabled></td>
			<td align="center"><input type="text" name="DestinationIP" size="8" disabled></td>
			<td align="center"><select name="RuleType" onChange="enableDestination()">
				<option value="core_input">core_input</option>
				<option value="port_forward">port_forward</option>
			</select></td>
			<td align="center">&nbsp;</td>
		</tr>		
		<tr>
			<td colspan="6" align="center" bgcolor="#EEEEEE"><input type="submit" class="button" name="add" value="Add"></td>
		</tr>		
	</table>	
	</form>
		<script>
		 	var frmvalidator = new formValidator("firewall");
// 			frmvalidator.addValidation("internalCoreIP_1","req","Please enter an IP address.");
		</script>
	';
	} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=firewall&error=You are not authorised to change the installation.");
			exit(0);
		}
		if(isset($_POST['add'])){
			$Protocol=@$_POST['protocol'];
			$SourcePort=@$_POST['SourcePort'];
			$SourcePortEnd=@$_POST['SourcePortEnd'];
			$DestinationPort=isset($_POST['DestinationPort'])?$_POST['DestinationPort']:0;
			$DestinationIP=isset($_POST['DestinationIP'])?$_POST['DestinationIP']:0;
			$RuleType=@$_POST['RuleType'];
			
			$insert='INSERT INTO Firewall (Protocol, SourcePort, SourcePortEnd, DestinationPort, DestinationIP, RuleType) VALUES (?,?,?,?,?,?)';
			$dbADO->Execute($insert,array($Protocol,$SourcePort,$SourcePortEnd,$DestinationPort,$DestinationIP,$RuleType));
		}

		if(isset($_REQUEST['delid'])){
			$delid=$_REQUEST['delid'];
			$dbADO->Execute('DELETE FROM Firewall WHERE PK_Firewall=?',$delid);
		}
		
		exec('sudo -u root /usr/pluto/bin/Network_Firewall.sh');
		
		header("Location: index.php?section=firewall&msg=Firewall rules updated.");
	}
	$output->setScriptCalendar('null');

	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Network Settings');
	$output->output();
}
?>
