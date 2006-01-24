<?
function mythSettings($output,$dbADO) {
	// include language files
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/common.lang.php');
	include(APPROOT.'/languages/'.$GLOBALS['lang'].'/mythSettings.lang.php');
	
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
	<h3>'.$TEXT_MYTH_SETTINGS_CONST.'</h3>
		
<form method="post" action="index.php" name="mythSettings">
	<input type="hidden" name="section" value="mythSettings">
	<input type="hidden" name="action" value="update">
<table cellspacing="0" cellpadding="3">
	<tr bgcolor="lightblue">
		<td align="center"><B>'.$TEXT_INPUT_CONST.'</B></td>
		<td align="center"><B>'.$TEXT_PICK_THE_TUNING_DEVICE_CONST.'</B></td>
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
		<td colspan="2" align="center"><input type="submit" class="button" name="update" value="'.$TEXT_UPDATE_CONST.'"></td>
	</tr>
</table>
</form>';
	
	} else {
		
		//check if current user canModifyInstallation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$installationID,$dbADO);
		
		if (!$canModifyInstallation) {
			Header('Location: index.php?section=mythSettings&error='.$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST);
			exit();
		}
		
		// process update
		for ($i=0;$i<count($mythInputs);$i++){
			$newCommand=((int)@$_POST['input_'.$mythInputs['cardinputid'][$i]]!==0)?$externalCommand.(int)@$_POST['input_'.$mythInputs['cardinputid'][$i]]:NULL;
			$mythADO->Execute('UPDATE cardinput SET externalcommand=? WHERE cardinputid=?',array($newCommand,$mythInputs['cardinputid'][$i]));
		}
		// end process update

		Header('Location: index.php?section=mythSettings&msg='.$TEXT_MYTH_SETTINGS_UPDATED_CONST);
	}
	
	$output->setNavigationMenu(array($TEXT_MY_DEVICES_CONST=>'index.php?section=myDevices',$TEXT_MYTH_SETTINGS_CONST=>"index.php?section=mythSettings"));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_MYTH_SETTINGS_CONST);			
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
