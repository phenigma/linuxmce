<?
function proxySocket($output){
	$address=$_REQUEST['address'];
	$port=$_REQUEST['port'];
	$refresh=@$_REQUEST['refresh'];
	$command=$_REQUEST['command'];

	$out=$command.'<br>';
		
	/* Create a TCP/IP socket. */
	$socketLog="\n".date('d-m-Y H:i:s')."\n";
	$socketLog.="Attempting to create socket on host $address port $port\n";
	$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
	if ($socket < 0) {
		$socketLog.= "socket_create() failed: reason: " . socket_strerror($socket) . "\n";
	} else {
		$socketLog.= "OK.\n";
	}

	$socketLog.= "Attempting to connect to '$address' on port '$port'...\n";
	$result = @socket_connect($socket, $address, $port);

	if ($result !==true) {
		$socketLog.="socket_connect() failed.\nReason: ($result) " . socket_strerror($result) . "\n";
	} else {
		$socketLog.="OK.\n";
	}

	// get image
	if($command=='IMAGE'){
		getImage($socket);
	}
	
	// send touch command
	if(strpos($command,'TOUCH')!==false){
		$out.=sendTouchCommand($socket,$command,$refresh);
	}
	
	// send XML command
	if(strpos($command,'XML')!==false){
		$x=(int)@$_REQUEST['x'];
		$y=(int)@$_REQUEST['y'];
		if($x!=0 && $y!=0){
			$out.=sendTouchCommand($socket,' TOUCH '.$x.'x'.$y,$refresh);
		}else{
			getImage($socket);
		}
		$XML=getXML($socket);
		socket_close($socket);
		Header( "Content-type: text/xml"); 
		die($XML);
	}	
	
	
	$socketLog.= "Closing socket...\n";
	@socket_close($socket);
	$socketLog.= "OK.\n";

	@writeFile(getcwd().'/security_images/socket.log',$socketLog,'a+');
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();  
}

function getImage($socket,$refresh=''){
	$in = "IMAGE ".rand(10000,11000)."\n";
	$out='';

	$socketLog="Sending command $in";
	$written=@socket_write($socket, $in, strlen($in));
	if($written===false){
		$socketLog.="Failure: ".socket_strerror(socket_last_error($socket))."\n";
	}

	$outResponse= @socket_read($socket, 2048);
	if($outResponse===false){
		$socketLog.="Failed reading socket: ".socket_strerror(socket_last_error($socket))."\n";
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
		}
		
		if(isset($outImage)){
			$isSaved=writeFile(getcwd().'/security_images/orbiter_screen.png',$outImage);
			$socketLog.="Received image size $imageSize \n";
			
			if($refresh!=''){
				$out.=reloadImageJS();
			}
		}
	}

	@writeFile(getcwd().'/security_images/socket.log',$socketLog,'a+');
	
	return $out;
}

function getXML($socket){
	$in = "XML ".rand(10000,11000)."\n";
	$out='';
	
	$socketLog="Sending command $in";
	$written=@socket_write($socket, $in, strlen($in));
	if($written===false){
		$socketLog.="Failure: ".socket_strerror(socket_last_error($socket))."\n";
	}

	$outResponse= @socket_read($socket, 2048);
	if($outResponse===false){
		$socketLog.="Failed reading socket: ".socket_strerror(socket_last_error($socket))."\n";
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
		}
		
		if(isset($outXML)){
			$socketLog.="Received xml size $xmlSize \n";
		}
	}
	$out.=@$outXML;
	
	@writeFile(getcwd().'/security_images/socket.log',$socketLog,'a+');
	
	return $out;
}



function sendTouchCommand($socket,$command,$refresh){
	$in = $command."\n";

	$out='';
	$socketLog="Sending command $in";
	$written=@socket_write($socket, $in, strlen($in));
	if(!$written===false){
		$socketLog.="Failure: ".socket_strerror(socket_last_error($socket))."\n";
	}
	$outResponse= @socket_read($socket, 2048);
	if($outResponse===false){
		$socketLog.="Failed reading socket: ".socket_strerror(socket_last_error($socket))."\n";
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
?>
