<?
function mythSettings($output,$dbADO) {

	error_reporting(E_ALL );

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
	$sids = getMythQAM($mythADO);
	
	if ($action == 'form') {
		
		
		$out.=setLeftMenu($dbADO).'
		
		<div class="err">'.(isset($_GET['error'])?stripslashes($_GET['error']):'').'</div>
		<div class="confirm"><B>'.(isset($_GET['msg'])?stripslashes($_GET['msg']):'').'</B></div>
		<h3>'.$TEXT_MYTH_SETTINGS_CONST.'</h3>
			
	<form method="post" action="index.php" name="mythSettings">
		<input type="hidden" name="section" value="mythSettings">
		<input type="hidden" name="action" value="update">';
		/*
	$out.='<table cellspacing="0" cellpadding="3">
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
		$out.='<tr><td colspan="2" align="center"><input type="submit" class="button" name="update" value="'.$TEXT_UPDATE_CONST.'"></td></tr></table>';
		*/
		if (count($sids)>0)
		{
			$MDArray=getDevicesArrayFromCategory($GLOBALS['rootMediaDirectors'],$dbADO);
			reset($MDArray);
			list($mdk, $mdv) = each($MDArray);

			$out.='<br>'.$TEXT_MYTH_PICK_MD.'&nbsp;'.pulldownFromArray($MDArray,'OutputMD', $mdk).'<br>';	
			
			for($i=0;$i<count($sids);$i++)
			{
				$UnassignedChannels = getMythQAMUnassignedChannels($mythADO, $sids[$i]);
				$UnmappedStations = getMythQAMUnmappedEPGChannels($mythADO, $sids[$i]);
				if (count($UnassignedChannels))
				{
					if (count($UnmappedStations))
					{						
						$out.='<br>'.$TEXT_MYTH_UNASSIGNED_CABLE_CONST.'<table cellspacing="0" cellpadding="3"><tr bgcolor="lightblue"><td>'.$TEXT_MYTH_CHANNEL_CONST.'</td><td>'.$TEXT_MYTH_STATION_CONST.'</td><td></td>';
						for($j=0;$j<count($UnassignedChannels);$j++)
						{		
							$altColor=($j%2==0)?'#F0F3F8':'#FFFFFF';
							$out.='<tr bgcolor="'.$altColor.'"><td>'.$UnassignedChannels[$j].'</td><td>'.pulldownFromArray($UnmappedStations['name'],'stationpd_'.$UnassignedChannels[$j], '0').'</td>';
							$out.='<td>
							       <input type="submit" class="button" name="chanset_'.$sids[$i].'_'.$UnassignedChannels[$j].'" value="'.$TEXT_MYTH_SET_CONST.'">
							       <input type="submit" class="button" name="mythtune_'.$sids[$i].'_'.$UnassignedChannels[$j].'" value="'.$TEXT_MYTH_TUNE_CONST.'">
							       <input type="submit" class="button" name="remove_'.$sids[$i].'_'.$UnassignedChannels[$j].'" value="'.$TEXT_DELETE_CONST.'"></td>
							       </tr>';
							
						}
						$out.="</table>";				
					}
					else
					{
						$out.='<br>'.$TEXT_MYTH_NOSTATIONS_CONST;
					}
				}
				$AssignedChannels = getMythQAMAssignedChannels($mythADO, $sids[$i]);
				if (count($AssignedChannels))
				{
					$out.='<br>'.$TEXT_MYTH_ASSIGNED_CABLE_CONST.'<table cellspacing="0" cellpadding="3"><tr bgcolor="lightblue"><td>'.$TEXT_MYTH_CHANNEL_CONST.'</td><td>'.$TEXT_MYTH_STATION_CONST.'</td><td></td>';
					$j=0;
					foreach($AssignedChannels['channum'] AS $chanID=>$channum)
					{		
						$altColor=($j%2==0)?'#F0F3F8':'#FFFFFF';
						$j++;
						$out.='<tr bgcolor="'.$altColor.'"><td>'.$channum.'</td><td>'.$AssignedChannels['name'][$chanID].'</td>';
						$out.='<td>
						       <input type="submit" class="button" name="mythtune_'.$sids[$i].'_'.$channum.'" value="'.$TEXT_MYTH_TUNE_CONST.'">
						       <input type="submit" class="button" name="remove_'.$sids[$i].'_'.$channum.'" value="'.$TEXT_DELETE_CONST.'"></td>
						       </tr>';						
					}
					$out."</table>";					
					
				}
				
			}		
		}
		$out.="</form>";
	} 
	else 
	{
		
		//check if current user canModifyInstallation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$installationID,$dbADO);
		
		if (!$canModifyInstallation) {
			Header('Location: index.php?section=mythSettings&error='.$TEXT_NOT_AUTHORISED_TO_MODIFY_INSTALLATION_CONST);
			exit();
		}
		
		// process update
		for ($i=0;$i<count($mythInputs);$i++){
			$newCommand=((int)@$_POST['input_'.$mythInputs['cardinputid'][$i]]!==0)?$externalCommand.(int)@$_POST['input_'.$mythInputs['cardinputid'][$i]]:NULL;
			$mythADO->Execute('UPDATE cardinput SET externalcommand=? WHERE cardinputid=?',array($newCommand,@$mythInputs['cardinputid'][$i]));
		}
		if(isset($_POST['reboot_core'])){
			$command='sudo -u root reboot';
			exec($command);
		}
		for($i=0;$i<count($sids);$i++)
		{
			$UnassignedChannels = getMythQAMUnassignedChannels($mythADO, $sids[$i]);
			for($j=0;$j<count($UnassignedChannels);$j++)
			{
				if (isset($_POST['chanset_'.$sids[$i].'_'.$UnassignedChannels[$j]]))
				{
					$setto = $_POST['stationpd_'.$UnassignedChannels[$j]];
					assignChannel($mythADO, $UnassignedChannels[$j], $setto);
				} 
				else if (isset($_POST['remove_'.$sids[$i].'_'.$UnassignedChannels[$j]]))
				{
					$mythADO->Execute('DELETE from channel WHERE channum=\''.$UnassignedChannels[$j].'\'');
				}
				else if (isset($_POST['mythtune_'.$sids[$i].'_'.$UnassignedChannels[$j]]))
				{
					tuneChannel($mythADO, $dbADO, $UnassignedChannels[$j],$sids[$i]);
				}														
			}
			$AssignedChannels = getMythQAMAssignedChannels($mythADO, $sids[$i]);
			foreach($AssignedChannels['channum'] AS $chanID=>$channum)
			{
				if (isset($_POST['remove_'.$sids[$i].'_'.$channum]))
				{
					$mythADO->Execute('DELETE from channel WHERE channum=\''.$UnassignedChannels[$j].'\'');
				}
				else if (isset($_POST['mythtune_'.$sids[$i].'_'.$channum]))
				{
					tuneChannel($mythADO, $dbADO, $channum,$sids[$i]);
				}				
			}	
			// end process update
		}
		
		Header('Location: index.php?section=mythSettings&msg='.$TEXT_MYTH_SETTINGS_UPDATED_CONST.' '.@$command);
		exit();
	}
	
	
	
	$output->setNavigationMenu(array($TEXT_MY_DEVICES_CONST=>'index.php?section=myDevices',$TEXT_MYTH_SETTINGS_CONST=>"index.php?section=mythSettings"));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: '.$TEXT_MYTH_SETTINGS_CONST);			
	$output->output();  		
}

function tuneChannel($mythADO, $dbADO, $chan, $sid)
{
	$MythPlayer = getMythPlayerForMD($dbADO, $_POST['OutputMD']);
	if ($MythPlayer > 0)
	{
		$mythADO->Execute('UPDATE cardinput SET startchan=\''.$chan.'\' WHERE sourceid='.$sid);
		$command='/usr/pluto/bin/MessageSend localhost -targetType template 0 2 1 43 2 '.$MythPlayer.' 29 1';
		exec($command);
	}
	else
	{
		Header('Location: index.php?section=mythSettings&msg='.$TEXT_MYTH_NOMYTHPLAYER.' (MD='.$_POST['OutputMD'].')');
		exit();
	}	
}

function assignChannel($mythADO, $uachannum, $stationchanid)
{
	$err = '';
	
	// First grab the mplexID, serviceid, and atscrcid from the unassigned channel
	
	$rs=$mythADO->Execute('SELECT mplexid, serviceid, atscsrcid FROM channel WHERE channum=\''.$uachannum.'\'');
	if ($uarow=$rs->FetchRow())
	{
		$mythADO->Execute('UPDATE channel SET mplexid='.$uarow['mplexid'].',serviceid='.$uarow['serviceid'].',atscsrcid='.$uarow['atscsrcid'].' WHERE chanid=\''.$stationchanid.'\'');
		$mythADO->Execute('DELETE from channel WHERE channum=\''.$uachannum.'\'');
	}
	else
	{
		$err = 'Could not find channum '.$uachannum;		
	}
	return $err;
}

function getMythQAM($mythADO)
{
	$sids=array();
	$rs=$mythADO->Execute('SELECT DISTINCTROW cardinput.sourceid FROM capturecard JOIN cardinput ON (capturecard.cardid = cardinput.cardid) JOIN dtv_multiplex ON (cardinput.sourceid=dtv_multiplex.sourceid) WHERE LEFT(dtv_multiplex.modulation,3) = \'qam\';');
	while($row=$rs->FetchRow())
	{
		$sids[]=$row['sourceid'];
	}
	return $sids;
}

function getMythQAMUnassignedChannels($mythADO, $sid)
{
	$channels=array();
	$rs=$mythADO->Execute('SELECT channum FROM channel WHERE sourceid='.$sid.' AND xmltvid = \'\' AND mplexid <> 0 AND mplexid <> 32767');
	while($row=$rs->FetchRow()){
		$channels[]=$row['channum'];
	}
	return $channels;
}

function getMythQAMUnmappedEPGChannels($mythADO, $sid)
{
	$channels=array();
	$rs=$mythADO->Execute('SELECT chanid, channum, name FROM channel WHERE sourceid='.$sid.' AND xmltvid <> \'\' AND mplexid=32767');
	while($row=$rs->FetchRow()){
		$channels['chanid'][$row['chanid']]=$row['chanid'];
		$channels['channum'][$row['chanid']]=$row['channum'];
		$channels['name'][$row['chanid']]=$row['name'];
	}
	return $channels;
}

function getMythPlayerForMD($dbADO, $MD)
{
	$rs=$dbADO->Execute('SELECT Device.PK_Device FROM Device LEFT JOIN Device as Parent ON Device.FK_Device_ControlledVia=Parent.PK_Device LEFT JOIN Device as Parent2 ON Parent.FK_Device_ControlledVia=Parent2.PK_Device LEFT JOIN Device as Parent3 ON Parent2.FK_Device_ControlledVia=Parent3.PK_Device WHERE Device.FK_DeviceTemplate=35 AND (Device.FK_Device_ControlledVia='.$MD.' OR Parent.FK_Device_ControlledVia='.$MD.' OR Parent2.FK_Device_ControlledVia='.$MD.' OR Parent3.FK_Device_ControlledVia='.$MD.')');
	if($row=$rs->FetchRow())
		return $row['PK_Device'];
	
	return 0;
}

function getMythQAMAssignedChannels($mythADO, $sid)
{
	$channels=array();
	$rs=$mythADO->Execute('SELECT chanid, channum, name FROM channel WHERE sourceid='.$sid.' AND xmltvid <> \'\' AND mplexid<32767');
	while($row=$rs->FetchRow()){
		$channels['chanid'][$row['channum']]=$row['chanid'];
		$channels['channum'][$row['channum']]=$row['channum'];
		$channels['name'][$row['channum']]=$row['name'];
	}
	return $channels;
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
	while($row=$rs->FetchRow())
	{
		$devices[$row['PK_Device']]=$row['Description'];
	}
	
	return $devices;
}
?>
