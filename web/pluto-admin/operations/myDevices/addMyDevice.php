<?
//se iau intai cele selectate, din baza de date si apoi se verifica array-ul din DeviceTemplate_DeviceParameter!
function addMyDevice($output,$dbADO) {
	global $dbPlutoMainDatabase;
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$userID = (int)@$_SESSION['userID'];
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID=(int)$_SESSION['installationID'];


	if ($action == 'form') {
		$parentID = (int)@$_SESSION['parentID'];
		$parentID = isset($_REQUEST['parentID'])?cleanInteger($_REQUEST['parentID']):$parentID;
		$out.='
	<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
	</script>
		
	<div class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
	<form action="index.php" method="POST" name="addMyDevice">
	<input type="hidden" name="section" value="addMyDevice">
	<input type="hidden" name="action" value="add">	
	<input type="hidden" name="parentID" value="'.$parentID.'">
		<h3>Add '.(($parentID==0)?'top level ':' child ').'device</h3>
		<table>			
				<tr>
					<td>Description:</td>
					<td><input type="text" size="15" name="Description" value=""></td>
				</tr>				
				<tr>
					<td>IP Address:</td>
					<td><textarea name="IPaddress"></textarea></td>
				</tr>
				<tr>
					<td>MACaddress:</td>
					<td><textarea name="MACaddress"></textarea></td>
				</tr>
				<tr>
					<td>Ignore On/Off:</td>
					<td>On:<input type="radio" value="1" name="IgnoreOnOff"> &nbsp; Off:<input type="radio" value="0" name="IgnoreOnOff" checked="checked"></td>
				</tr>
				<tr>
					<td>Device template</td>
					<td><input type="button" class="button" name="pick" value="Pick device template" onclick="document.addMyDevice.submit();windowOpen(\'index.php?section=deviceTemplatePicker&categoryID=0&from='.urlencode('addMyDevice&parentID='.$parentID).'\',\'width=800,height=600,toolbars=true,scrollbars=1,resizable=1\');"></td>
				</tr>
			</table>
	</form>
	';
	} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=addMyDevice&parentID=$parentID&error=You are not authorised to change the installation.");
			exit(0);
		}


		if (isset($_REQUEST['deviceTemplate']) && (int)$_REQUEST['deviceTemplate']!=0) {
			$deviceTemplate=(int)$_REQUEST['deviceTemplate'];
			$descriptionMyDevice = $_SESSION['Description'];
			$IPaddressMyDevice = $_SESSION['IPaddressMyDevice'];
			$MACaddressMyDevice = $_SESSION['MACaddressMyDevice'];
			$ignoreOnOff = $_SESSION['ignoreOnOff'];
			$parentID = $_SESSION['parentID'];
			
			$insertID=exec('/usr/pluto/bin/CreateDevice -h localhost -D '.$dbPlutoMainDatabase.' -d '.$deviceTemplate.' -i '.$installationID.(($IPaddressMyDevice!='')?' -I '.$IPaddressMyDevice:'').(($MACaddressMyDevice!='')?' -M '.$MACaddressMyDevice:'').(($parentID!=0)?' -C '.$parentID:''));
			if($descriptionMyDevice!='')
				$dbADO->Execute('UPDATE Device SET Description=?, IgnoreOnOff=? WHERE PK_Device=?',array($descriptionMyDevice,$ignoreOnOff,$insertID));
				
			setDCERouterNeedConfigure($_SESSION['installationID'],$dbADO);
			unset($_SESSION['Description']);
			unset($_SESSION['IPaddressMyDevice']);
			unset($_SESSION['MACaddressMyDevice']);
			unset($_SESSION['ignoreOnOff']);
			
			$out.="<script>
			top.frames['treeframe'].location='index.php?section=leftMenu&deviceID=$insertID';
			self.location.href=\"index.php?section=editDeviceParams&deviceID=$insertID&parentID=$parentID&msg=New device added.\";
		</script>";
		}else{
			$_SESSION['Description']=cleanString(@$_POST['Description']);
			$_SESSION['IPaddressMyDevice'] = cleanString(@$_POST['IPaddress']);
			$_SESSION['MACaddressMyDevice'] = cleanString(@$_POST['MACaddress']);
			$_SESSION['ignoreOnOff'] = cleanInteger(@$_POST['IgnoreOnOff']);
			$_SESSION['parentID']=(int)@$_POST['parentID'];
			$parentID = isset($_REQUEST['parentID'])?cleanInteger($_REQUEST['parentID']):0;			
			
			header("Location: index.php?section=addMyDevice&parentID=$parentID");
			exit();
		}

	}

	$output->setScriptCalendar('null');

	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}
