<?
function testCode($output,$publicADO)
{

	/* @var $dbADO ADOConnection */
	global $PlutoHomeHost;
	$userID=(int)@$_SESSION['userID'];
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$sender=$_REQUEST['sender'];
	
	$irgcID=(int)$_REQUEST['irgcID'];

	if($irgcID==0){
		$alert='Invalid code specified.<br>';
	}else{
		$data=getFieldsAsArray('InfraredGroup_Command','InfraredGroup.Description AS Description,IRData,Command.Description AS Command',$publicADO,'LEFT JOIN InfraredGroup ON FK_InfraredGroup=PK_InfraredGroup INNER JOIN Command ON FK_Command=PK_Command WHERE PK_InfraredGroup_Command='.$irgcID);
		if(isset($_REQUEST['deviceID']) && (int)$_REQUEST['deviceID']!=0){
			if(@$data['IRData'][0]==''){
				$alert='ERROR: Specified command had empty content.';
			}else{

				$deviceID=$_REQUEST['deviceID'];
				$deviceInfo=getFieldsAsArray('Device','FK_Device_ControlledVia',$publicADO,'WHERE PK_Device='.$deviceID);
				if((int)@$deviceInfo['FK_Device_ControlledVia'][0]>0 || $sender=='rubyCodes'){
					$parentDevice=(int)@$deviceInfo['FK_Device_ControlledVia'][0];
					
					$deviceToReceive=($sender=='rubyCodes')?$deviceID:$parentDevice;
					$commandToTest='/usr/pluto/bin/MessageSend localhost 0 '.$deviceToReceive.' 1 191 9 "'.$data['IRData'][0].'"';

					exec($commandToTest);
					$alert='The command was sent to device #'.$deviceToReceive;		
				}else{
					$alert='ERROR: the device it\'s not controlled via infrared interface.';
				}
			}
		}else{
			$alert='ERROR: No device specified;';
		}
	}

	$out='
	<script>
		alert(\''.addslashes($alert).'\');
	</script>';
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}
?>