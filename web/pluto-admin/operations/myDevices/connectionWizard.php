<?
function connectionWizard($output,$dbADO) {
	global $dbPlutoMainDatabase;
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
//	$dbADO->debug=true;
	$userID = (int)@$_SESSION['userID'];
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$type = isset($_REQUEST['type'])?cleanString($_REQUEST['type']):'avEquipment';
	$installationID = (int)@$_SESSION['installationID'];

	$deviceCategory=$GLOBALS['rootAVEquipment'];
	$output->setHelpSrc('/support/index.php?section=document&docID=131');
	
	$_SESSION['AVentertainArea']=(isset($_REQUEST['entertainArea']) && (int)$_REQUEST['entertainArea']!=0)?(int)$_REQUEST['entertainArea']:@$_SESSION['AVentertainArea'];
	$entertainArea=@$_SESSION['AVentertainArea'];

	// read existing coordinates from cookie 
	if(isset($_COOKIE['PlutoAdminConnectionWizard_'.$_SESSION['installationID']])){
		$cookieArray=unserialize(stripslashes($_COOKIE['PlutoAdminConnectionWizard_'.$_SESSION['installationID']]));
		
		// coords for current entertain area
		if(isset($cookieArray[$entertainArea])){
			// grab coordinates for devices and pipes
			$positionsArray=explode(';',substr($cookieArray[$entertainArea],1));
			$oldDevice=array();
			$jsDrawPipes='';
			
			if($positionsArray[0]!=''){
				for($i=0;$i<count($positionsArray);$i=$i+7){
					$oldDevice[$positionsArray[$i]]['deviceX']=$positionsArray[$i+1];
					$oldDevice[$positionsArray[$i]]['deviceY']=$positionsArray[$i+2];
					if($positionsArray[$i+3]!='none'){
						$parts=explode(':',$positionsArray[$i+3]);
						$jsDrawPipes.='
							audioPipe['.$positionsArray[$i].']=new Array();
							audioPipe['.$positionsArray[$i].'][\'to\']='.$parts[0].';
							audioPipe['.$positionsArray[$i].'][\'coords\']=\''.$parts[1].'\';
							drawPipe(\''.$positionsArray[$i].'\',\'audio\','.$parts[1].');
						';
					}
					if($positionsArray[$i+4]!='none'){
						$parts=explode(':',$positionsArray[$i+4]);
						$jsDrawPipes.='
							videoPipe['.$positionsArray[$i].']=new Array();
							videoPipe['.$positionsArray[$i].'][\'to\']='.$parts[0].';
							videoPipe['.$positionsArray[$i].'][\'coords\']=\''.$parts[1].'\';
							drawPipe(\''.$positionsArray[$i].'\',\'video\','.$parts[1].');
						';
					}
					
					if($positionsArray[$i+5]!='none'){
						$parts=explode(':',$positionsArray[$i+5]);
						$jsDrawPipes.='
							audioLivePipe['.$positionsArray[$i].']=new Array();
							audioLivePipe['.$positionsArray[$i].'][\'to\']='.$parts[0].';
							audioLivePipe['.$positionsArray[$i].'][\'coords\']=\''.$parts[1].'\';
							drawPipe(\''.$positionsArray[$i].'\',\'audioLive\','.$parts[1].');
						';
					}
					if($positionsArray[$i+6]!='none'){
						$parts=explode(':',$positionsArray[$i+6]);
						$jsDrawPipes.='
							videoLivePipe['.$positionsArray[$i].']=new Array();
							videoLivePipe['.$positionsArray[$i].'][\'to\']='.$parts[0].';
							videoLivePipe['.$positionsArray[$i].'][\'coords\']=\''.$parts[1].'\';
							drawPipe(\''.$positionsArray[$i].'\',\'videoLive\','.$parts[1].');
						';
					}
					
					// TODO: check if pipes from database match those from cookie
					
				}
			}
		}
	}

	
	$connectorsArray=array(0=>'other.gif',1=>'composite.gif',2=>'svideo.gif',3=>'component.gif',4=>'dvi.gif',5=>'vga.gif',6=>'scart.gif');
	
if ($action == 'form') {
	$output->setScriptAnotherJS('scripts/connectionWizard/connectionWizard.js');
	$out.='
	<script type="text/javascript" src="scripts/connectionWizard/wz_jsgraphics.js"></script>
	<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
	</script>
	<div class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
	<div class="confirm" align="center"><B>'.strip_tags(@$_GET['msg']).'</B></div>
	<form action="index.php" method="POST" name="connectionWizard">
	<input type="hidden" name="section" value="connectionWizard">
	<input type="hidden" name="type" value="'.$type.'">
	<input type="hidden" name="action" value="add">
	<input type="hidden" name="devicesCoords" value="">
	<input type="hidden" name="oldEntertainArea" value="'.$entertainArea.'">
	
	<div style="position:absolute;top:25px;Z-INDEX:2;"><br><br><a href="index.php?section=avWizard">Advanced mode</a>
	<br>	<a href="javascript:savePositions();">Save</a></div>
	<div align="center"><h3>A/V equipment connection wizard</h3></div>
	Edit devices in:'.generatePullDown('entertainArea','EntertainArea','PK_EntertainArea','Description',$_SESSION['AVentertainArea'],$dbADO,' INNER JOIN Room ON FK_Room=PK_Room WHERE FK_Installation='.(int)$_SESSION['installationID'],'onChange="savePositions();"').'<br>';
	$devicesList=array();
	if($entertainArea!=0){		
		$out.='
			<input type="button" class="button" name="button" value="Add device" onClick="document.connectionWizard.action.value=\'externalSubmit\';document.connectionWizard.submit();windowOpen(\'index.php?section=deviceTemplatePicker&allowAdd=1&from=connectionWizard&categoryID='.$deviceCategory.'\',\'width=800,height=600,toolbars=true,scrollbars=1,resizable=1\');">
			<div style="position:absolute;top:25px;">
			<div style="position:relative; top:0px; width:360px;background:#DDDDDD;float:right;Z-INDEX:1;">
			<table align="right">
				<tr>
					<td><B>From</B>: </td>
					<td bgcolor="#EEEEEE" width="150"> <span id="fromMessage"></span></td>
				</tr>
				<tr>
					<td><B>To</B>: </td>
					<td bgcolor="#EEEEEE"> <span id="toMessage"></span></td>

				</tr>
				<tr>
					<td></td>
					<td></td>
				</tr>
				<tr>
					<td><B>Messages</B>:</td>
					<td bgcolor="#EEEEEE"> <span id="generalMessage"></span></td>
				</tr>
				<tr>
					<td><B>Pipe type</B>:</td>
					<td bgcolor="#EEEEEE"><input type="radio" name="pipeType" value="audio" checked onclick="setPipeType();"> <font color="red">Audio</font> <input type="radio" name="pipeType" value="video" onclick="setPipeType();"> <font color="blue">Video</font> <input type="radio" name="pipeType" value="audioLive" onclick="setPipeType();"> <font color="green">Audio Live</font> <input type="radio" name="pipeType" value="videoLive" onclick="setPipeType();"> <font color="magenta">Video Live</font></td>
				</tr>
			</table>
		</div>
		</div>
		';
		
		$avDTsArray=getDeviceTemplatesFromCategory($deviceCategory,$dbADO);
		if(count($avDTsArray)==0)
			$avDTsArray[]=0;
		$queryDevice='
			SELECT Device.*
			FROM Device 
			INNER JOIN Device_EntertainArea ON FK_Device=PK_Device
			WHERE FK_DeviceTemplate IN ('.join(',',$avDTsArray).') AND FK_Installation=? AND FK_EntertainArea=?';	
		$resDevice=$dbADO->Execute($queryDevice,array((int)$_SESSION['installationID'],$entertainArea));
		$topPos=230;
		
		while($rowD=$resDevice->FetchRow()){
			$devicesList[$rowD['PK_Device']]=$rowD['Description'];
			$inputsForDevice=array();
			$queryInput='
				SELECT Command.Description,PK_Command,FK_ConnectorType
				FROM DeviceTemplate_Input
				INNER JOIN Command ON FK_Command=PK_Command
				WHERE FK_DeviceTemplate=?';
			$resInputs=$dbADO->Execute($queryInput,$rowD['FK_DeviceTemplate']);
			$inputHeight=20;
			while($rowInputs=$resInputs->FetchRow()){
				$img=($rowInputs['FK_ConnectorType']=='')?$connectorsArray[0]:$connectorsArray[$rowInputs['FK_ConnectorType']];
				$inputsForDevice[]='<div id="in_'.$rowD['PK_Device'].'_cmd_'.$rowInputs['PK_Command'].'" style="position:absolute;Z-INDEX: 1000;background-image:url(include/images/'.$img.');width:30px;height:20px;left:5;top:'.$inputHeight.';" title="'.$rowInputs['Description'].'" onClick="setPipe(\''.$rowD['PK_Device'].'\',\''.$rowInputs['PK_Command'].'\',\'in_\',\''.$rowInputs['Description'].' ('.str_replace('.gif','',$img).') on '.$rowD['Description'].'\');"></div>';
				$inputHeight+=25;
			}
			
			$outputsForDevice=array();
			$queryOutput='
				SELECT Command.Description,PK_Command,FK_ConnectorType
				FROM DeviceTemplate_Output
				INNER JOIN Command ON FK_Command=PK_Command
				WHERE FK_DeviceTemplate=?';
			$resOutput=$dbADO->Execute($queryOutput,$rowD['FK_DeviceTemplate']);
			$outHeight=20;
			while($rowOutputs=$resOutput->FetchRow()){
				$img=($rowOutputs['FK_ConnectorType']=='')?$connectorsArray[0]:$connectorsArray[$rowOutputs['FK_ConnectorType']];
				$outputsForDevice[]='<div id="out_'.$rowD['PK_Device'].'_cmd_'.$rowOutputs['PK_Command'].'" style="position:absolute;Z-INDEX: 1000;background-image:url(include/images/'.$img.');width:30px;height:20px;left:215;top:'.$outHeight.';" title="'.$rowOutputs['Description'].'" onClick="setPipe(\''.$rowD['PK_Device'].'\',\''.$rowOutputs['PK_Command'].'\',\'out_\',\''.$rowOutputs['Description'].' ('.str_replace('.gif','',$img).') on '.$rowD['Description'].'\');"></div>';
				$outHeight+=25;
			}
			$height=($inputHeight>$outHeight)?$inputHeight:$outHeight;
			$height=(($height>150)?$height:150);
			
			$out.='
<div id="device_'.$rowD['PK_Device'].'_pipe_1" style="position:absolute;Z-INDEX: 100;left:0;top:0;"></div>
<div id="device_'.$rowD['PK_Device'].'_pipe_2" style="position:absolute;Z-INDEX: 100;left:0;top:0;"></div>
<div id="device_'.$rowD['PK_Device'].'_pipe_3" style="position:absolute;Z-INDEX: 100;left:0;top:0;"></div>
<div id="device_'.$rowD['PK_Device'].'_pipe_4" style="position:absolute;Z-INDEX: 100;left:0;top:0;"></div>	
<script>
	var audio_'.$rowD['PK_Device'].' = new jsGraphics("device_'.$rowD['PK_Device'].'_pipe_1");
	var video_'.$rowD['PK_Device'].' = new jsGraphics("device_'.$rowD['PK_Device'].'_pipe_2");			
	var audioLive_'.$rowD['PK_Device'].' = new jsGraphics("device_'.$rowD['PK_Device'].'_pipe_3");
	var videoLive_'.$rowD['PK_Device'].' = new jsGraphics("device_'.$rowD['PK_Device'].'_pipe_4");
</script>
<DIV id="device_'.$rowD['PK_Device'].'" style="BORDER-RIGHT: 2px outset; BORDER-TOP: 2px outset; DISPLAY: ; Z-INDEX: 1; BORDER-LEFT: 2px outset; WIDTH: 250px; BORDER-BOTTOM: 2px outset; POSITION: absolute; '.((isset($oldDevice[$rowD['PK_Device']])?'LEFT: '.$oldDevice[$rowD['PK_Device']]['deviceX'].'; TOP: '.$oldDevice[$rowD['PK_Device']]['deviceY'].';':'LEFT: 100px; TOP: '.$topPos.'px;')).' HEIGHT: '.$height.'px" onclick="bring_to_front(\'device_'.$rowD['PK_Device'].'\')" onmousedown="bring_to_front(\'device_'.$rowD['PK_Device'].'\')">
  <TABLE height="100%" cellSpacing=0 cellPadding=0 width="100%" bgColor=#EEEEEE border=0>
      <TBODY>
        <TR onmouseup="end_drag(\'device_'.$rowD['PK_Device'].'\')" onmousedown="start_drag(\'device_'.$rowD['PK_Device'].'\')" id="head_'.$rowD['PK_Device'].'"> 
          <TD align="center" width=196 bgColor=lightblue height="15" valign="middle"><B>'.$rowD['Description'].'</B></TD>
        </TR>
        <TR> 
          <TD align=middle><table width="100%" border="0">
			<tr>
				<td align="center" width="40"><div position:relative;>'.join('',$inputsForDevice).'</div></td>
				<td align="center" width="100%">Device <br>'.$rowD['PK_Device'].'</td>
				<td align="center" width="40"><div position:relative;>'.join('',$outputsForDevice).'</div></td>
			</tr>	
		</table></TD>
        </TR>
  </TABLE>
</DIV>';
			$topPos+=(15+$height);
		}
	}
	
	$out.='	
	</form>';
	if(count(@$devicesList)>1){
		$out.='<script>var layersArray = new Array('.join(',',array_keys($devicesList)).');</script>';
	}else{
		$out.='<script>
			var layersArray = new Array();';
		if(count(@$devicesList)!=0){
			$out.='
			layersArray[0]='.join('',array_keys(@$devicesList)).';';
		}
		$out.='	
		</script>';
	}
	$out.='<script>
	'.@$jsDrawPipes.'
	</script>';

	$output->SetScriptInBody('onmousemove="drag_layer();"');
} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=connectionWizard&error=You are not authorised to change the installation.");
			exit();
		}
		$entertainArea=(int)$_REQUEST['entertainArea'];
		$oldEntertainArea=(int)$_REQUEST['oldEntertainArea'];


		if($entertainArea!=0){
			$cookieArray[$oldEntertainArea]=$_REQUEST['devicesCoords'];
			
			// update database
			foreach ($cookieArray AS $entArea=>$params){
				if($entArea!=0){
					$positionsArray=explode(';',substr($cookieArray[$entArea],1));
					for($i=0;$i<count($positionsArray);$i=$i+7){
						addDeletePipe($positionsArray[$i+3],$positionsArray[$i],1,$dbADO);
						addDeletePipe($positionsArray[$i+4],$positionsArray[$i],2,$dbADO);
						addDeletePipe($positionsArray[$i+5],$positionsArray[$i],3,$dbADO);
						addDeletePipe($positionsArray[$i+6],$positionsArray[$i],4,$dbADO);
					}
				}
			}
			
			$cookieData=serialize($cookieArray);
			setcookie("PlutoAdminConnectionWizard_".$_SESSION['installationID'],$cookieData,time()+31536000,'/',false);
		}
				
		header("Location: index.php?section=connectionWizard&msg=The devices was updated&entertainArea=$entertainArea");		
	}

	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: A/V devices connection wizard');
	$output->output();
}

function addDeletePipe($operation,$deviceFrom,$pipe,$dbADO)
{
	if($operation=='none'){
		$dbADO->Execute('DELETE FROM Device_Device_Pipe WHERE FK_Device_From=? AND FK_Pipe=?',array($deviceFrom,$pipe));
	}else{
		$arr=explode(':',$operation);
		$deviceTo=$arr[0];
		$dbADO->Execute('INSERT IGNORE INTO Device_Device_Pipe (FK_Device_From,FK_Device_To,FK_Pipe,FK_Command_Input,FK_Command_Output) VALUES (?,?,?,?,?)',array($deviceFrom,$deviceTo,$pipe,$arr[2],$arr[3]));
	}
}

?>
