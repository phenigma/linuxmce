<?
function mythSettings($output,$dbADO) {
/* @var $dbADO ADOConnection */
/* @var $mythADO ADOConnection */
/* @var $rs ADORecordSet */
	global 	$dbMythType, $dbMythUser, $dbMythPass, $dbMythServer, $dbMythDatabase;
	global $externalCommand;
	
	$externalCommand='/usr/pluto/bin/TuneToChannel.sh ';
	
  	$mythADO= &ADONewConnection('mysql');
  	$mythADO->NConnect($dbMythServer,urlencode($dbMythUser),urlencode($dbMythPass),urlencode($dbMythDatabase));

  	
	$out='';
	$installationID = (int)@$_SESSION['installationID'];
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$mythInputs=getMythInputs($mythADO);
	
	if ($action == 'form') {
		
		
	$out.=setLeftMenu($dbADO).'
	
	<div class="err">'.(isset($_GET['error'])?stripslashes($_GET['error']):'').'</div>
	<div class="confirm"><B>'.(isset($_GET['msg'])?stripslashes($_GET['msg']):'').'</B></div>
	<h3>Myth Settings</h3>
		
<form method="post" action="index.php" name="mythSettings">
	<input type="hidden" name="section" value="mythSettings">
	<input type="hidden" name="action" value="update">
<table cellspacing="0" cellpadding="3">
	<tr bgcolor="lightblue">
		<td align="center"><B>Input</B></td>
		<td align="center"><B>Pick the tuning device</B></td>
	</tr>';

	$tunningDevices=getDevices(array($GLOBALS['rootAVEquipment'],$GLOBALS['TVdevices'],$GLOBALS['SatelliteBoxes'],$GLOBALS['CableBoxes']),$dbADO);
	
	for ($i=0;$i<count($mythInputs);$i++){
		$altColor=($i%2==0)?'#F0F3F8':'#FFFFFF';
		$out.='
			<tr bgcolor="'.$altColor.'">
				<td>'.$mythInputs['name'][$i].'</td>
				<td>'.pulldownFromArray($tunningDevices,'input_'.$mythInputs['cardinputid'][$i],$mythInputs['device'][$i]).'</td>
			</tr>
		';
	}
$out.='	
	<tr>
		<td colspan="2" align="center"><input type="submit" class="button" name="update" value="Update"></td>
	</tr>
</table>
</form>';
	
	} else {
		
		//check if current user canModifyInstallation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$installationID,$dbADO);
		
		if (!$canModifyInstallation) {
			Header('Location: index.php?section=mythSettings&error=You are not allowed to modify installation.');
			exit();
		}
		
		// process update
		for ($i=0;$i<count($mythInputs);$i++){
			$newCommand=((int)@$_POST['input_'.$mythInputs['cardinputid'][$i]]!==0)?$externalCommand.(int)@$_POST['input_'.$mythInputs['cardinputid'][$i]]:NULL;
			$mythADO->Execute('UPDATE cardinput SET externalcommand=? WHERE cardinputid=?',array($newCommand,$mythInputs['cardinputid'][$i]));
		}
		// end process update

		Header('Location: index.php?section=mythSettings&msg=Myth settings updated.');
	}
	
	$output->setNavigationMenu(array("My Devices"=>'index.php?section=myDevices',"Myth settings"=>"index.php?section=mythSettings"));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();  		
}

function getMythInputs($mythADO)
{
	global $externalCommand;
	
	$inputs=array();
	$rs=$mythADO->Execute('SELECT cardinputid,inputname,externalcommand FROM cardinput ORDER BY inputname ASC');
	while($row=$rs->FetchRow()){
		$inputs['cardinputid'][]=$row['cardinputid'];
		$inputs['name'][]=$row['inputname'];
		$inputs['device'][]=str_replace($externalCommand,'',$row['externalcommand']);
	}
	return $inputs;
}

function getDevices($categories,$dbADO)
{
	$rs=$dbADO->Execute('
		SELECT PK_Device, Device.Description 
		FROM Device
		INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
		WHERE FK_DeviceCategory IN ('.join(',',$categories).')
		ORDER BY Description ASC');
	$devices=array();
	while($row=$rs->FetchRow()){
		$devices[$row['PK_Device']]=$row['Description'];
	}
	
	return $devices;
}
?>
