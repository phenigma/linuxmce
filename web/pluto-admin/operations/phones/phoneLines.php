<?
function phoneLines($output,$astADO,$dbADO) {
	/* @var $astADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];

	$queryDefaultLineType="SELECT id,appdata FROM extensions_table	WHERE context='outgoing-extern-selectline' AND exten='_XXXX.' AND appdata LIKE 'DIALLINETYPE=%'";
	$resDefaultLineType=$astADO->Execute($queryDefaultLineType);
	if($resDefaultLineType->RecordCount()>0){
		$rowDefaultLineType=$resDefaultLineType->FetchRow();
		$defaultLineType=str_replace('DIALLINETYPE=','',$rowDefaultLineType['appdata']);
		$defaultLineTypeID=$rowDefaultLineType['id'];
	}
	
	$queryDefaultLine="SELECT id,appdata FROM extensions_table	WHERE context='outgoing-extern-selectline' AND exten='_XXXX.' AND appdata LIKE 'DIALLINE=%'";
	$resDefaultLine=$astADO->Execute($queryDefaultLine);
	if($resDefaultLine->RecordCount()>0){
		$rowDefaultLine=$resDefaultLine->FetchRow();
		$defaultLineName=str_replace('DIALLINE=','',$rowDefaultLine['appdata']);
		$defaultLineID=$rowDefaultLine['id'];
	}

	if($action=='form'){
		
		$out.=setLeftMenu($dbADO).'
		<div align="center" class="err">'.@$_REQUEST['error'].'</div>
		<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
		<h3 align="center">Phone Lines</h3>
		<form action="index.php" method="POST" name="phoneLines">
			<input type="hidden" name="section" value="phoneLines">
			<input type="hidden" name="action" value="update">
		<table align="center" cellpadding="3">
			<tr bgcolor="lightblue">
				<td align="center"><B>Name</B></td>
				<td align="center"><B>Type</B></td>
				<td align="center"><B>Username</B></td>
				<td align="center"><B>Registrar IP</B></td>
				<td align="center"><B>Registrar Port</B></td>
				<td align="center"><B>Registration timeout</B></td>
				<td align="center"><B>Default</B></td>
				<td>&nbsp;</td>
			</tr>
		';
		
		$resLines=$astADO->Execute("SELECT * FROM sip_buddies WHERE type='peer' ORDER BY name ASC");
		$pos=0;
		$sipLinesArray=array();
		while($row=$resLines->FetchRow()){
			$sipLinesArray[]=$row['uniqueid'];
			$pos++;
			$out.='
			<input type="hidden" name="oldName_'.$row['uniqueid'].'" value="'.$row['name'].'">
			<tr bgcolor="'.(($pos%2==0)?'#EEEEEE':'#E5E5E5').'">
				<td><input type="text" name="name_'.$row['uniqueid'].'" value="'.$row['name'].'"></td>
				<td><select name="type_'.$row['uniqueid'].'">
					<option value="SIP">SIP</option>
					<option value="IAX2">IAX2</option>
				</select></td>
				<td><input type="text" name="username_'.$row['uniqueid'].'" value="'.$row['username'].'"></td>
				<td><input type="text" name="host_'.$row['uniqueid'].'" value="'.$row['host'].'"></td>
				<td align="center"><input type="text" name="port_'.$row['uniqueid'].'" value="'.$row['port'].'"></td>
				<td align="center"><input type="text" name="rtptimeout_'.$row['uniqueid'].'" value="'.$row['rtptimeout'].'"></td>
				<td align="center"><input type="radio" name="newDefaultLine" value="[SIP]'.$row['name'].'" '.((@$defaultLineName==$row['name'] && @$defaultLineType=='SIP')?'checked':'').'></td>
				<td align="center"><a href="#" onClick="if(confirm(\'Are you sure you want to delete this phone line?\'))self.location=\'index.php?section=phoneLines&dID='.$row['uniqueid'].'&name='.urlencode($row['name']).'&action=del&delTable=sip_buddies\'">Delete</a></td>
			</tr>
			';
		}

		$resLines=$astADO->Execute("SELECT * FROM iax_buddies WHERE type='peer' ORDER BY name ASC");
		$pos=0;
		$iaxLinesArray=array();
		while($row=$resLines->FetchRow()){
			$iaxLinesArray[]=$row['uniqueid'];
			$pos++;
			$out.='
			<input type="hidden" name="oldName_'.$row['uniqueid'].'" value="'.$row['name'].'">
			<tr bgcolor="'.(($pos%2==0)?'#EEEEEE':'#E5E5E5').'">
				<td><input type="text" name="name_'.$row['uniqueid'].'" value="'.$row['name'].'"></td>
				<td><select name="type_'.$row['uniqueid'].'">
					<option value="SIP">SIP</option>
					<option value="IAX2" selected>IAX2</option>
				</select></td>
				<td><input type="text" name="username_'.$row['uniqueid'].'" value="'.$row['username'].'"></td>
				<td><input type="text" name="host_'.$row['uniqueid'].'" value="'.$row['host'].'"></td>
				<td align="center"><input type="text" name="port_'.$row['uniqueid'].'" value="'.$row['port'].'"></td>
				<td align="center"><input type="text" name="rtptimeout_'.$row['uniqueid'].'" value="'.$row['rtptimeout'].'"></td>
				<td align="center"><input type="radio" name="newDefaultLine" value="[IAX2]'.$row['name'].'" '.((@$defaultLineName==$row['name'] && @$defaultLineType=='IAX2')?'checked':'').'></td>
				<td align="center"><a href="#" onClick="if(confirm(\'Are you sure you want to delete this phone line?\'))self.location=\'index.php?section=phoneLines&dID='.$row['uniqueid'].'&name='.urlencode($row['name']).'&action=del&delTable=iax_buddies\'">Delete</a></td>
			</tr>
			';
		}
		
		if(count($sipLinesArray)>0 || count($iaxLinesArray)>0){
			$out.='
				<tr>
					<td colspan="8" align="center"><input type="submit" class="button" name="update" value="Update"></td>
				</tr>
			';
		}
		$out.='
			<input type="hidden" name="sipLinesArray" value="'.join(',',$sipLinesArray).'">
			<input type="hidden" name="iaxLinesArray" value="'.join(',',$iaxLinesArray).'">
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
				<td><B>Name</B></td>
				<td><input type="text" name="name" value=""></td>
			</tr>		
			<tr>
				<td><B>Type</B></td>
				<td><select name="lineType">
					<option value="SIP">SIP</option>
					<option value="IAX2">IAX2</option>
				</select>
				</td>
			</tr>		
			<tr>
				<td><B>Authentification ID (username)</B></td>
				<td><input type="text" name="username" value=""></td>
			</tr>
			<tr>
				<td><B>Password</B></td>
				<td><input type="text" name="secret" value=""></td>
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
			$type=$_POST['lineType'];
			
			$phoneTable=(($type=='SIP')?'sip_buddies':'iax_buddies');
			
			$resNameExist=$astADO->Execute('SELECT * FROM '.$phoneTable.' WHERE name=?',$name);
			if($resNameExist->RecordCount()>0){
				header('Location: index.php?section=phoneLines&error=A phone line called '.$name.' exist.');
				exit();
			}
			
			$insertPhoneLine='INSERT INTO '.$phoneTable.' (username, secret, name, host, port, rtptimeout, type, context, accountcode, fromdomain, nat, fromuser) VALUES (?,?,?,?,?,?,?,?,?,?,?,?)';
			$insertAstConfig='INSERT INTO ast_config (filename,var_name,var_val,category) VALUES (?,?,?,?)';
			$astADO->Execute($insertAstConfig,array('sip.conf', 'register', $username.':'.$secret.'@'.$host.'/'.$phoneNumber,'general'));
			$astADO->Execute($insertPhoneLine,array($username,$secret,$name,$host,$port,$rtptimeout,'peer','registered-lines',$phoneNumber,$host,'Y',$phoneNumber));
			
			if(!isset($defaultLineID)){
				$astADO->Execute('INSERT INTO extensions_table (context, exten, priority, app, appdata) VALUES (?,?,?,?,?)',array('outgoing-extern-selectline','_XXXX.',2,'SetVar','DIALLINE='.$name));
				$astADO->Execute('INSERT INTO extensions_table (context, exten, priority, app, appdata) VALUES (?,?,?,?,?)',array('outgoing-extern-selectline','_XXXX.',1,'SetVar','DIALLINETYPE='.$type));
			}else{
				if($isDefault==1){
					$astADO->Execute('UPDATE extensions_table SET appdata=? WHERE id=?',array('DIALLINE='.$name,$defaultLineID));
					$astADO->Execute('UPDATE extensions_table SET appdata=? WHERE id=?',array('DIALLINETYPE='.$type,$defaultLineTypeID));
				}
			}
			$sync=exec('/usr/pluto/bin/SynchronizeAsterisk.sh');
			if($sync!=0){
				header("Location: index.php?section=phoneLines&error=Synchronisation failed.");
				exit();
			}
			header('Location: index.php?section=phoneLines&msg=The phone line was added.');
			exit();
		}

		if(isset($_REQUEST['dID'])){
			$uniqueID=(int)$_REQUEST['dID'];
			$name=$_REQUEST['name'];
			$delTable=$_REQUEST['delTable'];
			setRandomLineDefault($name,$astADO,$defaultLineID,$defaultLineTypeID);
			deleteLine($uniqueID,$name,$delTable,$astADO);
			
			header('Location: index.php?section=phoneLines&msg=The phone line was deleted.');
			exit();
		}

		if(isset($_POST['update'])){
			$newDefault=@$_POST['newDefaultLine'];
			
			if(substr($newDefault,0,5)=='[SIP]'){
				$newDefaultLineType='SIP';
				$newDefaultLine=substr($newDefault,5);
			}else{
				$newDefaultLineType='IAX2';
				$newDefaultLine=substr($newDefault,6);
			}
			if(@$defaultLineName!=$newDefaultLine || @$defaultLineType!=$newDefaultLineType){
				if(isset($defaultLineID)){
					$astADO->Execute('UPDATE extensions_table SET appdata=? WHERE id=?',array('DIALLINE='.@$newDefaultLine,@$defaultLineID));
				}else{
					$astADO->Execute('INSERT INTO extensions_table (context, exten, priority, app, appdata) VALUES (?,?,?,?,?)',array('outgoing-extern-selectline','_XXXX.',2,'SetVar','DIALLINE='.$newDefaultLine));
				}
					
			}

			if(@$defaultLineType!=$newDefaultLineType){
				if(isset($defaultLineType)){
					$astADO->Execute('UPDATE extensions_table SET appdata=? WHERE id=?',array('DIALLINETYPE='.@$newDefaultLineType,@$defaultLineTypeID));
				}else{
					$astADO->Execute('INSERT INTO extensions_table (context, exten, priority, app, appdata) VALUES (?,?,?,?,?)',array('outgoing-extern-selectline','_XXXX.',1,'SetVar','DIALLINETYPE='.$newDefaultLineType));
				}
			}
			
			if($_POST['sipLinesArray']!=''){
				$sipLinesArray=explode(',',$_POST['sipLinesArray']);
				foreach($sipLinesArray AS $lineID){
					$username=@$_POST['username_'.$lineID];
					$name=@$_POST['name_'.$lineID];
					$oldName=@$_POST['oldName_'.$lineID];
					$host=@$_POST['host_'.$lineID];
					$port=@$_POST['port_'.$lineID];
					$rtptimeout=@$_POST['rtptimeout_'.$lineID];
					$type=@$_POST['type_'.$lineID];
					
					if($type=='SIP'){
						$astADO->Execute('UPDATE sip_buddies SET username=?, name=?, host=?, port=?, rtptimeout=? WHERE uniqueid=?',array($username,$name,$host,$port,$rtptimeout,$lineID));
						if($name!=$oldName){
							$astADO->Execute('UPDATE extensions_table SET appdata=? WHERE appdata=?',array('DIALLINE='.$name,'DIALLINE='.$oldName));
						}
					}else{
						if($name==$defaultLineName && $type!=@$defaultLineType){
							$astADO->Execute('UPDATE extensions_table SET appdata=? WHERE id=?',array('DIALLINETYPE=IAX2',@$defaultLineTypeID));
						}
						$insertPhoneLine='
							INSERT INTO iax_buddies 
								(username, secret, name, host, port, rtptimeout, type, context, accountcode, fromdomain, nat, fromuser) 
							SELECT username, secret, name, host, port, rtptimeout, type, context, accountcode, fromdomain, nat, fromuser FROM sip_buddies WHERE uniqueid=?';
						$astADO->Execute($insertPhoneLine,$lineID);
						deleteLine($lineID,$name,'sip_buddies',$astADO,1);
					}
				}
			}
			
			if($_POST['iaxLinesArray']!=''){
				$iaxLinesArray=explode(',',$_POST['iaxLinesArray']);
				foreach($iaxLinesArray AS $lineID){
					$username=@$_POST['username_'.$lineID];
					$name=@$_POST['name_'.$lineID];
					$oldName=@$_POST['oldName_'.$lineID];
					$host=@$_POST['host_'.$lineID];
					$port=@$_POST['port_'.$lineID];
					$rtptimeout=@$_POST['rtptimeout_'.$lineID];
					$type=@$_POST['type_'.$lineID];
					
					if($type=='IAX2'){
						$astADO->Execute('UPDATE iax_buddies SET username=?, name=?, host=?, port=?, rtptimeout=? WHERE uniqueid=?',array($username,$name,$host,$port,$rtptimeout,$lineID));
						if($name!=$oldName){
							$astADO->Execute('UPDATE extensions_table SET appdata=? WHERE appdata=?',array('DIALLINE='.$name,'DIALLINE='.$oldName));
						}
					}else{
						if($name==$defaultLineName && $type!=@$defaultLineType){
							$astADO->Execute('UPDATE extensions_table SET appdata=? WHERE id=?',array('DIALLINETYPE=SIP',@$defaultLineTypeID));
						}
						$insertPhoneLine='
							INSERT INTO sip_buddies 
								(username, secret, name, host, port, rtptimeout, type, context, accountcode, fromdomain, nat, fromuser) 
							SELECT username, secret, name, host, port, rtptimeout, type, context, accountcode, fromdomain, nat, fromuser FROM iax_buddies WHERE uniqueid=?';
						$astADO->Execute($insertPhoneLine,$lineID);
						deleteLine($lineID,$name,'iax_buddies',$astADO,1);
					}
				}
			}			
			
			$sync=exec('/usr/pluto/bin/SynchronizeAsterisk.sh',$retArray);

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

function deleteLine($uniqueID,$name,$delTable,$astADO,$dontDelDefault=0)
{
	
	$resPL=$astADO->Execute('SELECT * FROM '.$delTable.' WHERE uniqueid=?',$uniqueID);
	if($resPL->RecordCount()>0){
		$rowPL=$resPL->FetchRow();
		$var_val=$rowPL['username'].':'.$rowPL['secret'].'@'.$rowPL['host'].'/'.$rowPL['fromuser'];
		$astADO->Execute('DELETE FROM ast_config WHERE var_val=? AND filename=? AND var_name=? AND category=?',array($var_val,'sip.conf', 'register','general'));
	}
	
	// check if the line is international or internal
	$resCode=$astADO->Execute('SELECT exten FROM extensions_table WHERE context=? AND appdata LIKE ?',array('outgoing-extern-selectline','INTERNATIONALCODE=%'));
	if($resCode->RecordCount()>0){
		$rowCode=$resCode->FetchRow();
		$fullCode=$rowCode['exten'];
	}else
		$fullCode='_9011.';

	$resIsInternational=$astADO->Execute('SELECT * FROM extensions_table WHERE exten=? AND context=? AND appdata = ?',array($fullCode,'outgoing-extern-selectline','DIALLINE='.$name));
	if($resIsInternational->RecordCount()>0){
		$astADO->Execute('DELETE FROM extensions_table WHERE exten=? AND appdata LIKE?',array($fullCode,'DIALLINETYPE=%'));
	}
	$resIsIntern=$astADO->Execute('SELECT * FROM extensions_table WHERE exten=? AND context=? AND appdata=?',array('_9XXX.','outgoing-extern-selectline','DIALLINE='.$name));
	if($resIsIntern->RecordCount()>0){
		$astADO->Execute('DELETE FROM extensions_table WHERE exten=? AND appdata LIKE ?',array('_9XXX.','DIALLINETYPE=%'));
	}

	$astADO->Execute('DELETE FROM '.$delTable.' WHERE uniqueid=?',$uniqueID);
	if($dontDelDefault!=1)
		$astADO->Execute('DELETE FROM extensions_table WHERE appdata=?','DIALLINE='.$name);
	
}

function setRandomLineDefault($name,$astADO,$defaultLineID,$defaultLineTypeID)
{
	$resFirstOtherLine=$astADO->Execute('SELECT name FROM sip_buddies WHERE name!=? AND type=? LIMIT 0,1',array($name,'peer'));
	if($resFirstOtherLine->RecordCount()!=0){
		$rowFirstOtherLine=$resFirstOtherLine->FetchRow();
		$astADO->Execute('UPDATE extensions_table SET appdata=? WHERE id=?',array('DIALLINE='.$rowFirstOtherLine['name'],$defaultLineID));
		$astADO->Execute('UPDATE extensions_table SET appdata=? WHERE id=?',array('DIALLINETYPE=SIP',$defaultLineTypeID));
	}else{
		$resFirstOtherLine=$astADO->Execute('SELECT name FROM iax_buddies WHERE name!=? AND type=? LIMIT 0,1',array($name,'peer'));
		if($resFirstOtherLine->RecordCount()!=0){
			$rowFirstOtherLine=$resFirstOtherLine->FetchRow();
			$astADO->Execute('UPDATE extensions_table SET appdata=? WHERE id=?',array('DIALLINE='.$rowFirstOtherLine['name'],$defaultLineID));
			$astADO->Execute('UPDATE extensions_table SET appdata=? WHERE id=?',array('DIALLINETYPE=IAX2',$defaultLineTypeID));
		}
	}
}
?>
