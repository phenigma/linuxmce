<?
function callRouting($output,$dbADO) {
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];

	$resIsInternational=$dbADO->Execute('SELECT * FROM extensions_table WHERE exten=? AND context=? AND appdata LIKE ?',array('_9011.','outgoing-extern-selectline','DIALLINE=%'));
	if($resIsInternational->RecordCount()>0){
		$rowExternalLines=$resIsInternational->FetchRow();
		$internationalLine=str_replace('DIALLINE=','',$rowExternalLines['appdata']);
	}
	$resIsIntern=$dbADO->Execute('SELECT * FROM extensions_table WHERE exten=? AND context=? AND appdata LIKE ?',array('_9XXX.','outgoing-extern-selectline','DIALLINE=%'));
	if($resIsIntern->RecordCount()>0){
		$rowInternLines=$resIsIntern->FetchRow();
		$internLine=str_replace('DIALLINE=','',$rowInternLines['appdata']);
	}

	
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
				<td><B>Name</B></td>
				<td><B>Type</B></td>
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
			$linesArray[$row['name']]=$row['name'].' [SIP]';
			$pos++;
			$lineType='';
			$lineType=($row['name']==@$internationalLine)?'International':$lineType;
			$lineType=($row['name']==@$internLine)?$lineType.' National':$lineType;
			$lineType=($lineType=='')?'Not Assigned':$lineType;
			
			$out.='
			<tr bgcolor="'.(($pos%2==0)?'#EEEEEE':'#E5E5E5').'">
				<td><B>'.$row['name'].'</B></td>
				<td><B>SIP</B></td>
				<td>'.$row['username'].'</td>
				<td>'.$row['ipaddr'].'</td>
				<td align="center">'.$row['port'].'</td>
				<td align="center">'.$row['rtptimeout'].'</td>
				<td align="center">'.$lineType.'</td>
			</tr>
			';
		}
		
		$resLines=$dbADO->Execute("SELECT * FROM iax_buddies WHERE type='peer'");
		while($row=$resLines->FetchRow()){
			$linesArray[$row['name']]=$row['name'].' [IAX2]';
			$pos++;
			$lineType='';
			$lineType=($row['name']==@$internationalLine)?'International':$lineType;
			$lineType=($row['name']==@$internLine)?$lineType.' National':$lineType;
			$lineType=($lineType=='')?'Not Assigned':$lineType;
			
			$out.='
			<tr bgcolor="'.(($pos%2==0)?'#EEEEEE':'#E5E5E5').'">
				<td><B>'.$row['name'].'</B></td>
				<td><B>IAX2</B></td>
				<td>'.$row['username'].'</td>
				<td>'.$row['ipaddr'].'</td>
				<td align="center">'.$row['port'].'</td>
				<td align="center">'.$row['rtptimeout'].'</td>
				<td align="center">'.$lineType.'</td>
			</tr>
			';
		}
		
		$internationaPulldown='<select name="international">
			<option value="">- Not assigned -</option>';
		foreach ($linesArray AS $name=>$label){
			$internationaPulldown.='<option value="'.$label.'" '.(($name==@$internationalLine)?'selected':'').'>'.$label.'</option>';
		}
		$internationaPulldown.='</select>';
		
		$internPulldown='<select name="intern">
			<option value="">- Not assigned -</option>';
		foreach ($linesArray AS $name=>$label){
			$internPulldown.='<option value="'.$label.'" '.(($name==@$internLine)?'selected':'').'>'.$label.'</option>';
		}
		$internPulldown.='</select>';

		
		$out.='
			<tr>
				<td colspan="7" align="center"><table align="center">
					<tr>
						<td><B>International</B></td>
						<td>'.$internationaPulldown.'</td>
					</tr>
					<tr>
						<td><B>National</B></td>
						<td>'.$internPulldown.'</td>
					</tr>
				</table></td>
			</tr>
			<tr>
				<td colspan="7" align="center"><input type="submit" class="button" name="update" value="Update"></td>
			</tr>
			';
		$out.='
			<input type="hidden" name="internationalLine" value="'.@$internationalLine.'">
			<input type="hidden" name="internLine" value="'.@$internLine.'">
		</table>
		</form>
		<br>
		';
	}else{
	// process area
		if(isset($_POST['update'])){
			$oldInternationalLine=$_POST['internationalLine'];
			$oldInternLine=$_POST['internLine'];
			
			if(substr($_POST['international'],-6)==' [SIP]'){
				$newInternational=substr($_POST['international'],0,-6);
				$newInternationalType='SIP';
			}else{
				$newInternational=substr($_POST['international'],0,-7);
				$newInternationalType='IAX2';
			}
			if(substr($_POST['intern'],-6)==' [SIP]'){
				$newIntern=substr($_POST['intern'],0,-6);
				$newInternType='SIP';
			}else{
				$newIntern=substr($_POST['intern'],0,-7);
				$newInternType='IAX2';
			}
			
			if($oldInternationalLine=='' && $newInternational!=''){
				$dbADO->Execute('INSERT INTO extensions_table (context, exten, priority, app, appdata) VALUES (?,?,?,?,?)',array('outgoing-extern-selectline','_9011.',2,'SetVar','DIALLINE='.$newInternational));
				$dbADO->Execute('INSERT INTO extensions_table (context, exten, priority, app, appdata) VALUES (?,?,?,?,?)',array('outgoing-extern-selectline','_9011.',1,'SetVar','DIALLINETYPE='.$newInternationalType));
			}elseif($oldInternationalLine!='' && $newInternational==''){
				$dbADO->Execute('DELETE FROM extensions_table WHERE exten=? AND appdata=?',array('_9011.','DIALLINE='.$oldInternationalLine));
				$dbADO->Execute('DELETE FROM extensions_table WHERE exten=?',array('_9011.'));
			}else{
				$dbADO->Execute('UPDATE extensions_table SET appdata=? WHERE context=? AND exten=? AND app=? AND appdata=?',array('DIALLINE='.$newInternational,'outgoing-extern-selectline','_9011.','SetVar','DIALLINE='.$oldInternationalLine));
				$dbADO->Execute('UPDATE extensions_table SET appdata=? WHERE context=? AND exten=? AND app=? AND appdata LIKE ?',array('DIALLINETYPE='.$newInternationalType,'outgoing-extern-selectline','_9011.','SetVar','DIALLINETYPE=%'));
			}

			if($oldInternLine=='' && $newIntern!=''){
				$dbADO->Execute('INSERT INTO extensions_table (context, exten, priority, app, appdata) VALUES (?,?,?,?,?)',array('outgoing-extern-selectline','_9XXX.',2,'SetVar','DIALLINE='.$newIntern));
				$dbADO->Execute('INSERT INTO extensions_table (context, exten, priority, app, appdata) VALUES (?,?,?,?,?)',array('outgoing-extern-selectline','_9XXX.',1,'SetVar','DIALLINETYPE='.$newInternType));
			}elseif($oldInternLine!='' && $newIntern==''){
				$dbADO->Execute('DELETE FROM extensions_table WHERE exten=? AND appdata=?',array('_9XXX.','DIALLINE='.$oldInternLine));
				$dbADO->Execute('DELETE FROM extensions_table WHERE exten=? AND appdata LIKE ?',array('_9XXX.','DIALLINETYPE=%'));
			}else{
				$dbADO->Execute('UPDATE extensions_table SET appdata=? WHERE context=? AND exten=? AND app=? AND appdata=?',array('DIALLINE='.$newIntern,'outgoing-extern-selectline','_9XXX.','SetVar','DIALLINE='.$oldInternLine));
				$dbADO->Execute('UPDATE extensions_table SET appdata=? WHERE context=? AND exten=? AND app=? AND appdata LIKE ?',array('DIALLINETYPE='.$newInternType,'outgoing-extern-selectline','_9XXX.','SetVar','DIALLINETYPE=%'));
			}

			header('Location: index.php?section=callRouting&msg=Call routing options was updated.');
			exit();
		}
		
		
		
		header('Location: index.php?section=callRouting');
	}
	
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}

?>
