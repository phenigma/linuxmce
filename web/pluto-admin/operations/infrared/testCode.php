<?
function testCode($output,$dbADO) {
/* @var $dbADO ADOConnection */
/* @var $rs ADORecordSet */

	$installationID = (int)@$_SESSION['installationID'];
	$irg_c=(int)$_REQUEST['irg_c'];
	$deviceID=(int)$_REQUEST['deviceID'];
	
	if($irg_c>0){
		$codeArray=getFieldsAsArray('InfraredGroup_Command','IRData',$dbADO,'WHERE PK_InfraredGroup_Command='.$irg_c);
		if(!isset($codeArray['IRData'][0])){
			$_GET['error']='Code not found';
		}else{
			$commandToTest='/usr/pluto/bin/MessageSend localhost 0 '.$deviceID.' 1 191 70 "'.$codeArray['IRData'][0].'"';
			print $commandToTest;
			exec($commandToTest);
			$message='The command was sent.';
		}
	}else{
		$_GET['error']='Code not found';
	}



	$out='
	
	<div class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
	<div class="confirm"><B>'.(isset($_GET['msg'])?strip_tags($_GET['msg']):'').'</B></div>
	
	<b>'.$message.'</b><br><br>
	<input type="button" class="button" name="close" value="Close" onClick="javascript:self.close();">';
		
		
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();  		
}
?>
