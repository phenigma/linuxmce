<?php
function learnCode($output,$dbADO) {
	//$dbADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$deviceID = (int)$_REQUEST['deviceID'];
	$commandID = (int)$_REQUEST['commandID'];
		

	if ($action=='form') {		
		$res=$dbADO->Execute('SELECT MAX(psc_mod) AS lastTime FROM InfraredGroup_Command');
		$row=$res->FetchRow();
		if($_SESSION['LastInfraredGroup_CommandTime']!=$row['lastTime']){
			$out='
			<script>
				alert(\'Infrared code learned!\');
				opener.location.reload();
				self.close();
			</script>';
		}
		else{

			$out.='
		<form action="index.php" method="post" name="learnCode">
		<input type="hidden" name="section" value="learnCode">
		<input type="hidden" name="action" value="add">
		<input type="hidden" name="deviceID" value="'.$deviceID.'">
		<input type="hidden" name="commandID" value="'.$commandID.'">
		<table width="100%">
			<tr>
				<td height="200" align="center">Please transmit the I/R Code.   Waiting for code.....<br><br>
				<input type="submit" name="cancel" value="Cancel">
				</td>
			</tr>
		</table>			
		</form>
		<script>
			var newInterval=setInterval("self.location.reload()",5000);
		</script>
		';
		}	
	} else {
		if(isset($_POST['cancel'])){
			$commandToSend='/usr/pluto/bin/MessageSend localhost 0 '.$deviceID.' 1 245 8 0';
			system($commandToSend);
			$out.='<script>
					self.close();
				</script>
			';
		}else{
			$resLastCommand=$dbADO->Execute('SELECT MAX(psc_mod) AS lastTime FROM InfraredGroup_Command WHERE FK_Device=?',$deviceID);
			$rowLastCommand=$resLastCommand->FetchRow();
			$_SESSION['LastInfraredGroup_CommandTime']=$rowLastCommand['lastTime'];
			
			$commandToSend='/usr/pluto/bin/MessageSend localhost 0 '.$deviceID.' 1 245 8 1 71 '.$commandID;
			system($commandToSend);
			
			header("Location: index.php?section=learnCode&deviceID=$deviceID&commandID=".$commandID);
		}
			
	}
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Learn IR Code');			
	$output->output();
}
?>
