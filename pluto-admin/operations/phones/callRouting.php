<?
function callRouting($output,$dbADO) {
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];

	$intLines=array();
	$resIsInternational=$dbADO->Execute('SELECT * FROM extensions_table WHERE exten=? AND context=?',array('_9011.','outgoing-extern-selectline'));
	while($rowExternalLines=$resIsInternational->FetchRow()){
		$intLines[]=str_replace('DIALLINE=','',$rowExternalLines['appdata']);
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
			$linesArray[$row['uniqueid']]=$row['name'];
			$pos++;
			$out.='
			<tr bgcolor="'.(($pos%2==0)?'#EEEEEE':'#E5E5E5').'">
				<td><B>'.$row['name'].'</B></td>
				<td>'.$row['username'].'</td>
				<td>'.$row['ipaddr'].'</td>
				<td align="center">'.$row['port'].'</td>
				<td align="center">'.$row['rtptimeout'].'</td>
				<td align="center"><input type="radio" name="routing_'.$row['uniqueid'].'" value="int" '.((in_array($row['name'],$intLines))?'checked':'').'> International <input type="radio" name="routing_'.$row['uniqueid'].'" value="nat" '.((!in_array($row['name'],$intLines))?'checked':'').'> National</td>
			</tr>
			';
		}
		if(count($linesArray)>0){
			$out.='
				<tr>
					<td colspan="6" align="center"><input type="submit" class="button" name="update" value="Update"></td>
				</tr>
			';
		}
		$out.='
			<input type="hidden" name="linesArray" value="'.urlencode(serialize($linesArray)).'">
		</table>
		</form>
		<br>
		';
	}else{
	// process area
	
		if(isset($_POST['update'])){
			$linesArray=unserialize(urldecode($_POST['linesArray']));
			foreach ($linesArray AS $lineID=>$lineName){
				if($_POST['routing_'.$lineID]=='int' && !in_array($lineName,$intLines)){
					$dbADO->Execute('INSERT INTO extensions_table (context, exten, priority, app, appdata) VALUES (?,?,?,?,?)',array('outgoing-extern-selectline','_9011.',1,'SetVar','DIALLINE='.$lineName));
				}elseif($_POST['routing_'.$lineID]!='int' && in_array($lineName,$intLines)){
					$dbADO->Execute('DELETE FROM extensions_table WHERE exten=? AND appdata=?',array('_9011.','DIALLINE='.$lineName));
				}
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
