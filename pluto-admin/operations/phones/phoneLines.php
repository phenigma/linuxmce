<?
function phoneLines($output,$dbADO) {
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
		<form action="index.php" method="POST" name="phoneLines">
			<input type="hidden" name="section" value="phoneLines">
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
				<td><input type="text" name="username_'.$row['uniqueid'].'" value="'.$row['username'].'"></td>
				<td><input type="text" name="ipaddr_'.$row['uniqueid'].'" value="'.$row['ipaddr'].'"></td>
				<td align="center"><input type="text" name="port_'.$row['uniqueid'].'" value="'.$row['port'].'"></td>
				<td align="center"><input type="text" name="rtptimeout_'.$row['uniqueid'].'" value="'.$row['rtptimeout'].'"></td>
				<td align="center"><a href="#" onClick="if(confirm(\'Are you sure you want to delete this phone line?\'))self.location=\'index.php?section=phoneLines&dID='.$row['uniqueid'].'&action=del\'">Delete</a></td>
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
		<h3 align="center">New phone line</h3>
		
		<form action="index.php" method="POST" name="addPhoneLines">
			<input type="hidden" name="section" value="phoneLines">
			<input type="hidden" name="action" value="update">

		<table  align="center">
			<tr>
				<td><B>Authentification ID (username)</B></td>
				<td><input type="text" name="username" value=""></td>
			</tr>
			<tr>
				<td><B>Password</B></td>
				<td><input type="password" name="secret" value=""></td>
			</tr>
			<tr>
				<td><B>Registrar IP</B></td>
				<td><input type="text" name="ipaddr" value=""></td>
			</tr>
			<tr>
				<td><B>Registrar port</B></td>
				<td><input type="text" name="port" value="5060"></td>
			</tr>
			<tr>
				<td><B>Phone Number</B></td>
				<td><input type="text" name="phoneNumber" value=""></td>
			</tr>
			<tr>
				<td><B>Registration timeout (s)</B></td>
				<td><input type="text" name="rtptimeout" value="10"></td>
			</tr>
			<tr>
				<td><B>Default</B></td>
				<td><input type="checkbox" name="defaultLine" value="1"></td>
			</tr>		
			<tr>
				<td colspan="2" align="center"><input type="submit" name="add" value="Add phone line"></td>
			</tr>
		</table>
		</form>
		<script>
		 	var frmvalidator = new formValidator("addPhoneLines");
 			frmvalidator.addValidation("username","req","Please enter an authentification ID.");
			frmvalidator.addValidation("secret","req","Please type the password.");
			frmvalidator.addValidation("ipaddr","req","Please enter the IP address.");
			frmvalidator.addValidation("port","req","Please enter the port.");
			frmvalidator.addValidation("rtptimeout","req","Please enter the registration timeout.");
		</script>
		';
	}else{
	// process area
		if(isset($_POST['add'])){
			$username=$_POST['username'];
			$secret=$_POST['secret'];
			$ipaddr=$_POST['ipaddr'];
			$port=$_POST['port'];
			$phoneNumber=$_POST['phoneNumber'];
			$rtptimeout=$_POST['rtptimeout'];
			
			// TODO: default processing
			$insertPhoneLine='INSERT INTO sip_buddies (username, secret, ipaddr, port, rtptimeout, type) VALUES (?,?,?,?,?,?)';
			$dbADO->Execute($insertPhoneLine,array($username,$secret,$ipaddr,$port,$rtptimeout,'peer'));
			
			header('Location: index.php?section=phoneLines&msg=The phone line was added.');
			exit();
		}

		if(isset($_REQUEST['dID'])){
			$uniqueID=$_REQUEST['dID'];
			$dbADO->Execute('DELETE FROM sip_buddies WHERE uniqueid=?',$uniqueID);
			
			header('Location: index.php?section=phoneLines&msg=The phone line was deleted.');
			exit();
		}

		if(isset($_POST['update'])){
			$linesArray=explode(',',$_POST['linesArray']);
			foreach($linesArray AS $lineID){
				$username=@$_POST['username_'.$lineID];
				$ipaddr=@$_POST['ipaddr_'.$lineID];
				$port=@$_POST['port_'.$lineID];
				$rtptimeout=@$_POST['rtptimeout_'.$lineID];
				
				$dbADO->Execute('UPDATE sip_buddies SET username=?, ipaddr=?, port=?, rtptimeout=? WHERE uniqueid=?',array($username,$ipaddr,$port,$rtptimeout,$lineID));

				header('Location: index.php?section=phoneLines&msg=The phone lines was updated.');
				exit();
			}
		}
		
		
		header('Location: index.php?section=phoneLines');
	}
	
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}

?>