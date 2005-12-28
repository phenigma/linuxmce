<?
function proxySocket($output,$dbADO){
	if(!isset($_REQUEST['address']) && !isset($_REQUEST['port'])){
		$phoneIPAddress=$_SERVER['REMOTE_ADDR'];
		$ProxyOrbiterInfo=getFieldsAsArray('Device','FK_Device_ControlledVia,IPAddress,DD1.IK_DeviceData AS PhoneIP,DD2.IK_DeviceData AS Port',$dbADO,'INNER JOIN Device_DeviceData DD1 ON DD1.FK_Device=PK_Device AND DD1.FK_DeviceData='.$GLOBALS['RemotePhoneIP'].' INNER JOIN Device_DeviceData DD2 ON DD2.FK_Device=PK_Device AND DD2.FK_DeviceData='.$GLOBALS['ListenPort'].' WHERE DD1.IK_DeviceData=\''.$phoneIPAddress.'\' AND FK_DeviceTemplate='.$GLOBALS['Cisco7970Orbiter']);
		if(count($ProxyOrbiterInfo)!=0){
			$address=$ProxyOrbiterInfo['IPAddress'][0];
			if($address=='' || is_null($address)){
				//$address=getDeviceIP($ProxyOrbiterInfo['FK_Device_ControlledVia'][0],$dbADO);
				$address=getCoreIP($dbADO);
				if(is_null($address)){
					$address='192.168.80.1';
					write_log("\n\nOrbiter proxy and his ancestors does not have IP address\n");
				}
			}
			$port=$ProxyOrbiterInfo['Port'][0];
		}else{
			$address=@$_REQUEST['address'];
			$port=@$_REQUEST['port'];
			xml_die($address,$port,$command,"\n\nOrbiter proxy not found for phone IP ".$phoneIPAddress."\n",'Invalid phone IP '.$phoneIPAddress);
		}

	}else{
		$address=@$_REQUEST['address'];
		$port=@$_REQUEST['port'];
	}
	$refresh=@$_REQUEST['refresh'];
	$command=$_REQUEST['command'];

	$out=$command.'<br>';
		
	/* Create a TCP/IP socket. */

	write_log("\n".date('d-m-Y H:i:s')."\nAttempting to create socket on host $address port $port ... ");
	$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
	if ($socket === false) {
		xml_die($address,$port,$command,"socket_create() failed: reason: " . socket_strerror($socket) . "\n",'Failed to connect');
	} else {
		write_log("OK.\n");
	}

	$result = @socket_connect($socket, $address, $port);

	if ($result !==true) {
		@socket_close($socket);
		xml_die($address,$port,$command,"socket_connect() failed.\nReason: (".socket_last_error().") " . socket_strerror(socket_last_error()) . "\n",'Failed to connect');
	} else {
		write_log("Connected on socket ... OK.\n");
	}

	write_log("Received parameters ".$_SERVER['QUERY_STRING']."\n");
	
	// get image
	if($command=='IMAGE'){
		$out.=getImage($socket,$refresh);
	}
	
	// send touch command
	if(strpos($command,'TOUCH')!==false){
		$out.=sendCommand($socket,$command,$refresh);
	}
	
	// send XML command
	if(strpos($command,'XML')!==false){
		
		$x=(int)@$_REQUEST['x'];
		$y=(int)@$_REQUEST['y'];
		if($x!=0 && $y!=0){
			$out.=sendCommand($socket,'TOUCH '.$x.'x'.$y,$refresh);
		}else{
			$key=@$_REQUEST['key'];
			if($key==''){
				getImage($socket);
			}else{
				$out.=sendCommand($socket,'PLUTO_KEY '.$key,$refresh);
			}
		}
		$XML=getXML($socket);
		socket_close($socket);
		$refreshURL="http://".$_SERVER['SERVER_ADDR']."/pluto-admin/index.php?section=proxySocket&address=$address&port=$port&command=XML";
		
		Header("Refresh: 5; url=$refreshURL");
		Header("Content-type: text/xml"); 
		write_log( "Redirecting to $refreshURL\n");
		die($XML);
	}	
	
	
	write_log("Closing socket ... ");
	@socket_close($socket);
	write_log( "OK.\n");
	

	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();  
}

function getImage($socket,$refresh=''){
	$in = "IMAGE ".rand(10000,11000)."\n";
	$out='';

	write_log("Sending command $in");
	$written=@socket_write($socket, $in, strlen($in));
	if($written===false){
		write_log("Failure: ".socket_strerror(socket_last_error($socket))."\n");
	}

	$outResponse= @socket_read($socket, 2048,PHP_NORMAL_READ);
	if($outResponse===false){
		write_log("Failed reading socket: ".socket_strerror(socket_last_error($socket))."\n");
	}else{
		write_log("Read: ".$outResponse."\n");
	}

	$imageSize=(int)trim(str_replace('IMAGE ','',$outResponse));
	if($imageSize>0){
		$remaining=$imageSize;
		$outImage='';
		while($remaining>0){
			$linesize=($remaining<2048)?$remaining:2048;
			$chunk=@socket_read($socket, $linesize,PHP_BINARY_READ);
			$outImage.= $chunk;
			$remaining-=strlen($chunk);
			write_log("Retrieved image chunk ".strlen($chunk).", remaining $remaining \n");
		}
		
		if(isset($outImage)){
			$isSaved=writeFile(getcwd().'/security_images/orbiter_screen.png',$outImage);
			write_log("Received image size $imageSize \n");
			
		}
	}
	if((int)$refresh>0){
		$out.=reloadImageJS();
	}
	
	return $out;
}

function getXML($socket){
	$in = "XML ".rand(10000,11000)."\n";
	$out='';
	
	write_log("Sending command $in");
	$written=@socket_write($socket, $in, strlen($in));
	if($written===false){
		write_log("Failure: ".socket_strerror(socket_last_error($socket))."\n");
	}

	$outResponse= @socket_read($socket, 2048,PHP_NORMAL_READ);
	if($outResponse===false){
		write_log("Failed reading socket: ".socket_strerror(socket_last_error($socket))."\n");
	}

	$xmlSize=(int)trim(str_replace('XML ','',$outResponse));
	if($xmlSize>0){
		$remaining=$xmlSize;
		$outXML='';
		while($remaining>0){
			$linesize=($remaining<2048)?$remaining:2048;
			$chunk=@socket_read($socket, $linesize,PHP_BINARY_READ);
			$outXML.= $chunk;
			$remaining-=strlen($chunk);
			write_log("Retrieved xml chunk ".strlen($chunk).", remaining $remaining \n");
		}
		
		if(isset($outXML)){
			write_log("Received xml size $xmlSize \n");
		}
	}
	$out.=@$outXML;
	
	
	return $out;
}



function sendCommand($socket,$command,$refresh){
	$in = $command."\n";

	$out='';

	write_log("Sending command $in");
	$written=@socket_write($socket, $in, strlen($in));
	if($written===false){
		write_log("Failure: ".socket_strerror(socket_last_error($socket))."\n");
	}
	
	$outResponse= socket_read($socket, 2048,PHP_NORMAL_READ);
	if($outResponse===false){
		write_log("Failed reading socket: ".socket_strerror(socket_last_error($socket))."\n");
	}else{
		write_log($outResponse);
	}
	$out.=$outResponse;

	$out.=getImage($socket,$refresh);
	
	return $out;
}

// reload the image "screen" from parent
function reloadImageJS(){
	$js='
		<script>
			parent.document.getElementById("screen").src="rooms/spacer.gif";
			parent.document.getElementById("screen").src="include/image.php?imagepath='.getcwd().'/security_images/orbiter_screen.png&randno='.rand(0,10000).'";
		</script>
	';
	
	return $js;
}

function write_log($log){
	writeFile(getcwd().'/security_images/socket.log',$log,'a+');
}

function getDeviceIP($deviceID,$dbADO){
	$arr=getFieldsAsArray('Device','IPAddress,FK_Device_ControlledVia',$dbADO,'WHERE PK_Device='.$deviceID);

	if($arr['IPAddress'][0]!=''){
		return $arr['IPAddress'][0];
	}

	if(is_null($arr['FK_Device_ControlledVia'][0])){
		return NULL;
	}
	
	return getDeviceIP($arr['FK_Device_ControlledVia'][0],$dbADO);
}

function getCoreIP($dbADO)
{
	$res=$dbADO->Execute('SELECT * FROM Device INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE FK_DeviceCategory=?',array($GLOBALS['CategoryCore']));
	$row=$res->FetchRow();
	return @$row['IPAddress'];
}

function xml_die($address,$port,$command,$message,$userFriendlyMessage=''){
	// the phone has 32 characters limit for values
	write_log(trim($message));
	$xmlMessage=($userFriendlyMessage!='')?$userFriendlyMessage:str_replace("\n","",trim($message));
	$xml='
<CiscoIPPhoneGraphicFileMenu>
	<Title>Pluto Orbiter</Title>
	<Prompt>'.substr($xmlMessage,0,31).'</Prompt>
	<LocationX>0</LocationX>
	<LocationY>0</LocationY>
	<URL>http://192.168.80.1/pluto-admin/security_images/generic_xml_error.png</URL>
	<MenuItem>
		<Name>Button</Name>
			<URL>http://192.168.80.1/pluto-admin/security_images/generic_xml_error.png</URL>
		<TouchArea X1="0" Y1="0" X2="0" Y2="0"/>
	</MenuItem>	
</CiscoIPPhoneGraphicFileMenu>';

	$refreshURL="http://".$_SERVER['SERVER_ADDR']."/pluto-admin/index.php?section=proxySocket&address=$address&port=$port&command=$command";
	Header("Refresh: 5; url=$refreshURL");
	Header("Content-type: text/xml"); 
	write_log("\nRedirecting to $refreshURL\n");
	die($xml);	
}
?>
