<?
function phoneLines($output,$dbADO) {
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];
		
	$queryDefaultLine="SELECT id,appdata FROM extensions_table	WHERE context='outgoing-extern-selectline' AND exten='_.' AND appdata LIKE 'DIALLINE=%'";
	$resDefaultLine=$dbADO->Execute($queryDefaultLine);
	if($resDefaultLine->RecordCount()>0){
		$rowDefaultLine=$resDefaultLine->FetchRow();
		$defaultLineName=str_replace('DIALLINE=','',$rowDefaultLine['appdata']);
		$defaultLineID=$rowDefaultLine['id'];
	}
		
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
				<td align="center"><B>Name</B></td>
				<td align="center"><B>Username</B></td>
				<td align="center"><B>Registrar IP</B></td>
				<td align="center"><B>Registrar Port</B></td>
				<td align="center"><B>Registration timeout</B></td>
				<td align="center"><B>Default</B></td>
				<td>&nbsp;</td>
			</tr>
		';
		
		$resLines=$dbADO->Execute("SELECT * FROM sip_buddies WHERE type='peer' ORDER BY name ASC");
		$pos=0;
		$linesArray=array();
		while($row=$resLines->FetchRow()){
			$linesArray[]=$row['uniqueid'];
			$pos++;
			$out.='
			<input type="hidden" name="oldName_'.$row['uniqueid'].'" value="'.$row['name'].'">
			<tr bgcolor="'.(($pos%2==0)?'#EEEEEE':'#E5E5E5').'">
				<td><input type="text" name="name_'.$row['uniqueid'].'" value="'.$row['name'].'"></td>
				<td><input type="text" name="username_'.$row['uniqueid'].'" value="'.$row['username'].'"></td>
				<td><input type="text" name="host_'.$row['uniqueid'].'" value="'.$row['host'].'"></td>
				<td align="center"><input type="text" name="port_'.$row['uniqueid'].'" value="'.$row['port'].'"></td>
				<td align="center"><input type="text" name="rtptimeout_'.$row['uniqueid'].'" value="'.$row['rtptimeout'].'"></td>
				<td align="center"><input type="radio" name="newDefaultLine" value="'.$row['name'].'" '.((@$defaultLineName==$row['name'])?'checked':'').'></td>
				<td align="center"><a href="#" onClick="if(confirm(\'Are you sure you want to delete this phone line?\'))self.location=\'index.php?section=phoneLines&dID='.$row['uniqueid'].'&name='.urlencode($row['name']).'&action=del\'">Delete</a></td>
			</tr>
			';
		}
		if(count($linesArray)>0){
			$out.='
				<tr>
					<td colspan="5" align="center"><input type="submit" class="button" name="update" value="Update"></td>
				</tr>
			';
		}
		$out.='
			<input type="hidden" name="linesArray" value="'.join(',',$linesArray).'">
		</table>
		</form>
		<br>
		<h3 align="center">New VOIP phone line</h3>
		<p align="center">Click "help" to see what these values mean and how to get them for the most popular VOIP phone providers.</p>
		
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
				<td><B>Name</B></td>
				<td><input type="text" name="name" value=""></td>
			</tr>
			<tr>
				<td><B>Registrar IP</B></td>
				<td><input type="text" name="host" value=""></td>
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
				<td colspan="2" align="center"><input type="submit" class="button" name="add" value="Add phone line"></td>
			</tr>
		</table>
		</form>
		<script>
		 	var frmvalidator = new formValidator("addPhoneLines");
 			frmvalidator.addValidation("username","req","Please enter an authentification ID.");
			frmvalidator.addValidation("secret","req","Please type the password.");
			frmvalidator.addValidation("host","req","Please enter the IP address.");
			frmvalidator.addValidation("port","req","Please enter the port.");
			frmvalidator.addValidation("rtptimeout","req","Please enter the registration timeout.");
		</script>
		';
	}else{
	// process area
		if(isset($_POST['add'])){
			$username=$_POST['username'];
			$secret=$_POST['secret'];
			$name=$_POST['name'];
			$host=$_POST['host'];
			$port=$_POST['port'];
			$phoneNumber=$_POST['phoneNumber'];
			$rtptimeout=$_POST['rtptimeout'];
			$isDefault=(isset($_POST['defaultLine']))?1:0;
			
			$resNameExist=$dbADO->Execute('SELECT * FROM sip_buddies WHERE name=?',$name);
			if($resNameExist->RecordCount()>0){
				header('Location: index.php?section=phoneLines&error=A phone line called '.$name.' exist.');
				exit();
			}
			
			$insertPhoneLine='INSERT INTO sip_buddies (username, secret, name, host, port, rtptimeout, type, context, accountcode, fromdomain, nat, fromuser) VALUES (?,?,?,?,?,?,?,?,?,?,?,?)';
			$dbADO->Execute($insertPhoneLine,array($username,$secret,$name,$host,$port,$rtptimeout,'peer','registered-lines',$phoneNumber,$host,'Y',$phoneNumber));
			
			if(!isset($defaultLineID)){
				$dbADO->Execute('INSERT INTO extensions_table (context, exten, priority, app, appdata) VALUES (?,?,?,?,?)',array('outgoing-extern-selectline','_.',1,'SetVar','DIALLINE='.$name));
			}else{
				if($isDefault==1){
					$dbADO->Execute('UPDATE extensions_table SET appdata=? WHERE id=?',array('DIALLINE='.$name,$defaultLineID));
				}
			}
			$sync=exec('/usr/pluto/bin/SyncronizeAsterisk.sh');
			if($sync!=0){
				header("Location: index.php?section=phoneLines&error=Synchronisation failed.");
				exit();
			}
			header('Location: index.php?section=phoneLines&msg=The phone line was added.');
			exit();
		}

		if(isset($_REQUEST['dID'])){
			$uniqueID=$_REQUEST['dID'];
			$name=$_REQUEST['name'];
			if($name==$defaultLineName){
				$resFirstOtherLine=$dbADO->Execute('SELECT name FROM sip_buddies WHERE name!=? AND type=? LIMIT 0,1',array($name,'peer'));
				if($resFirstOtherLine->RecordCount()!=0){
					$rowFirstOtherLine=$resFirstOtherLine->FetchRow();
					$dbADO->Execute('UPDATE extensions_table SET appdata=? WHERE id=?',array('DIALLINE='.$rowFirstOtherLine['name'],$defaultLineID));
				}
			}
			
			$dbADO->Execute('DELETE FROM sip_buddies WHERE uniqueid=?',$uniqueID);
			$dbADO->Execute('DELETE FROM extensions_table WHERE appdata=?','DIALLINE='.$name);						
			
			header('Location: index.php?section=phoneLines&msg=The phone line was deleted.');
			exit();
		}

		if(isset($_POST['update'])){
			$linesArray=explode(',',$_POST['linesArray']);
			foreach($linesArray AS $lineID){
				$username=@$_POST['username_'.$lineID];
				$name=@$_POST['name_'.$lineID];
				$oldName=@$_POST['oldName_'.$lineID];
				$host=@$_POST['host_'.$lineID];
				$port=@$_POST['port_'.$lineID];
				$rtptimeout=@$_POST['rtptimeout_'.$lineID];
								
				$dbADO->Execute('UPDATE sip_buddies SET username=?, name=?, host=?, port=?, rtptimeout=? WHERE uniqueid=?',array($username,$name,$host,$port,$rtptimeout,$lineID));
				if($name!=$oldName){
					$dbADO->Execute('UPDATE extensions_table SET appdata=? WHERE appdata=?',array('DIALLINE='.$name,'DIALLINE='.$oldName));
				}
			}
			$newDefaultLine=@$_POST['newDefaultLine'];
			if(@$defaultLineName!=$newDefaultLine){
				$dbADO->Execute('UPDATE extensions_table SET appdata=? WHERE id=?',array('DIALLINE='.@$newDefaultLine,$defaultLineID));
			}
			
			$sync=exec('/usr/pluto/bin/SyncronizeAsterisk.sh',$retArray);

			if($sync!=0){
				header("Location: index.php?section=phoneLines&error=Synchronisation failed.");
				exit();
			}
			
			header('Location: index.php?section=phoneLines&msg=The phone lines was updated.');
			exit();
		}
		
		
		header('Location: index.php?section=phoneLines');
	}
	
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}

?>
