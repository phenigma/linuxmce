<?
function remoteOrbiter($output,$dbADO){
	
	$orbiterID=(int)@$_REQUEST['orbiterID'];
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID = (int)@$_SESSION['installationID'];

	if(isset($_POST['go'])){
		$address=$_POST['host'];
		$port=$_POST['port'];
		$refresh=$_POST['refresh'];
		
		// TODO: replace image path
		$iframeSRC='index.php?section=proxySocket&address='.$address.'&port='.$port.'&refresh='.$refresh;
		$orbiterScreen=
			'<img id="screen" src="include/image.php?imagepath='.getcwd().'/security_images/orbiter_screen.png" width="640" height="480" onClick="sendTouch();">
				<iframe id="imageLoader" src="'.$iframeSRC.'&command=IMAGE"></iframe>';
		$jsFunction='
		<script src="scripts/connectionWizard/connectionWizard.js" type="text/javascript" language="JavaScript"></script>
		<script>
		function findPosX(obj)
		{
			var curleft = 0;
			if (obj.offsetParent)
			{
				while (obj.offsetParent)
				{
					curleft += obj.offsetLeft
					obj = obj.offsetParent;
				}
			}
			else if (obj.x)
				curleft += obj.x;
			return curleft;
		}
		
		function findPosY(obj)
		{
			var curtop = 0;
			if (obj.offsetParent)
			{
				while (obj.offsetParent)
				{
					curtop += obj.offsetTop
					obj = obj.offsetParent;
				}
			}
			else if (obj.y)
				curtop += obj.y;
			return curtop;
		}
				
		function sendTouch()
		{
			xRelative=xMousePos-findPosX(document.getElementById("screen"));
			yRelative=yMousePos-findPosY(document.getElementById("screen"));

			document.getElementById("imageLoader").src="'.$iframeSRC.'&command=TOUCH "+xRelative+"x"+yRelative;
		}
		</script>
		';	
	}
	
	if($action=='form'){
		$orbiters=getDevicesArrayFromCategory($GLOBALS['rootOrbiterID'],$dbADO);
		$out.='
		<div class="err">'.stripslashes(@$_GET['error']).'</div>
		<div class="confirm"><B>'.@stripslashes($_GET['msg']).'</B></div>
		<form action="index.php" method="POST" name="remoteOrbiter">
			<input type="hidden" name="section" value="remoteOrbiter">
			<input type="hidden" name="action" value="form">
		<table cellpadding="3" cellspacing="0">
			<tr>
				<td>Choose orbiter: </td>
				<td>'.pulldownFromArray($orbiters,'orbiterID',$orbiterID).'</td>
			</tr>
			<tr>
				<td>IP address: </td>
				<td><input type="text" name="host" value="10.0.0.79"></td>
			</tr>
			<tr>
				<td>Port: </td>
				<td><input type="text" name="port" value="3451"></td>
			</tr>
			<tr>
				<td>Refresh (s): </td>
				<td><input type="text" name="refresh" value="5"></td>
			</tr>
			<tr>
				<td colspan="2"><input type="submit" class="button" name="go" value="Submit"></td>
			</tr>
		</table>
		</form>
		'.@$orbiterScreen.'		
		'.@$jsFunction;
	}else{
		
	}	
	

	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();  		
}

?>
