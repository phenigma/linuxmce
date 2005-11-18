<?
//se iau intai cele selectate, din baza de date si apoi se verifica array-ul din DeviceTemplate_DeviceParameter!
function deleteMyDevice($output,$dbADO) {
/* @var $dbADO ADOConnection */
/* @var $rs ADORecordSet */
$out='';

$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
$deviceID = isset($_REQUEST['deviceID'])?cleanInteger($_REQUEST['deviceID']):0;

// check if the user has the right to modify installation
$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
if (!$canModifyInstallation){
	header("Location: index.php?section=editDeviceParams&deviceID=$deviceID&error=You are not authorised to change the installation.");
	exit(0);
}
	$fields=getFieldsAsArray('Device','FK_DeviceTemplate',$dbADO,'WHERE PK_Device='.$deviceID);
	$isPhone=isPhone($fields['FK_DeviceTemplate'][0],$dbADO);

	deleteDevice($deviceID,$dbADO);
	if($isPhone){
		$cmd='sudo -u root /usr/pluto/bin/sync_pluto2amp.pl '.$deviceID;
		exec($cmd);
	}
	setDCERouterNeedConfigure($_SESSION['installationID'],$dbADO);
	
	$out.='
	<script>
		alert("Device(s) deleted!");
		top.frames["treeframe"].location="index.php?section=leftMenu";
		document.location.href="index.php?section=myDevices&action=showBasicInfo";
	</script>
	<noscript>
		Device deleted! Please <a href="index.php?section=myDevices&action=showBasicInfo">click here</a> to go to main area!
	</noscript>
	';
	
	
$output->setScriptCalendar('null');

$output->setBody($out);
$output->setTitle(APPLICATION_NAME);			
$output->output();  	
}