<?
function basicInfo($output,$dbADO) {
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	$out='';
	$action = (isset($_REQUEST['action']) && $_REQUEST['action']!='')?cleanString($_REQUEST['action']):'form';

	$countriesFileArray=file('/usr/share/zoneinfo/iso3166.tab');
	$zonesFileArray=file('/usr/share/zoneinfo/zone.tab');
	$continentsArray=array('Africa', 'America', 'Antarctica', 'Arctic [Ocean]', 'Asia', 'Atlantic [Ocean]', 'Australia', 'Europe', 'Indian [Ocean]', 'Pacific [Ocean]');
	$currentTimeZone=trim(implode('',file('/etc/timezone')));

	$countriesArray=array();
	foreach ($countriesFileArray AS $line){
		if($line[0]!='#'){
			$parts=explode("\t",$line);
			$countriesArray[$parts[0]]=trim($parts[1]);
		}
	}
	
	$zonesArray=array();
	foreach ($zonesFileArray AS $line){
		if($line[0]!='#'){
			$parts=explode("\t",$line);
			$tz=explode('/',$parts[2]);
			$zonesArray[$tz[0]][$countriesArray[$parts[0]]][]=array($parts[2],@$parts[3]);
			if(trim($parts[2])==$currentTimeZone){
				$selContinent=$tz[0];
				$selCountry=$countriesArray[$parts[0]];
				$oldTimeZone=array($currentTimeZone,(isset($parts[3])?$parts[3]:$currentTimeZone));
			}
		}
	}
	
	$selContinent=(isset($_POST['continent']))?$_POST['continent']:@$selContinent;	
	$selCountry=(isset($_POST['country']))?$_POST['country']:@$selCountry;
	
	if($action=='form'){
		$out.='
		<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
		</script>		
			<div align="center" class="err">'.@$_REQUEST['error'].'</div>
			<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
			<h3 align="center">Set timezone</h3>
			<form action="index.php" method="POST" name="basicInfo">
			<input type="hidden" name="section" value="basicInfo">
			<input type="hidden" name="action" value="update">
			<input type="hidden" name="timeZoneText" value="update">
		
		<table cellpadding="3" align="center">';
			if(isset($oldTimeZone)){
				
				$out.='
				<tr>
					<td colspan="2" align="center">Current timezone: <B>'.$oldTimeZone[1].'</B></td>
				</tr>
				<input type="hidden" name="oldTimeZone" value="'.$oldTimeZone[0].'">
				';
			}		
		$out.='
			<tr>
				<td><B>Continent</B></td>
				<td><select name="continent" onChange="document.basicInfo.action.value=\'form\';document.basicInfo.submit();">
					<option value="">- Please select -</option>';

		foreach ($continentsArray AS $continent){
			$cleanContinent=preg_replace('/ \[(\w+)\]/','',$continent);
			$out.='<option value="'.$cleanContinent.'" '.(($cleanContinent==$selContinent)?'selected':'').'>'.$continent.'</option>';
		}
		$out.='
				</select></td>
			</tr>';
		if($selContinent!=''){
			$out.='
			<tr>
				<td><B>Country</B></td>
				<td>';
			if(count(array_keys($zonesArray[$selContinent]))==1){
				$tmpArray=array_keys($zonesArray[$selContinent]);
				$out.=$tmpArray[0];
				$singleCountry=1;
				$selCountry=$tmpArray[0];
			}else{
				$out.='
				<select name="country" onChange="document.basicInfo.action.value=\'form\';document.basicInfo.submit();">
					<option value="">- Please select -</option>';
				foreach (array_keys($zonesArray[$selContinent]) AS $country){
					$out.='<option value="'.$country.'" '.(($country==@$selCountry)?'selected':'').'>'.$country.'</option>';
				}
				$out.='
					</select>';
			}
			$out.='
				</td>
			</tr>';
			if($selCountry!='' && isset($zonesArray[$selContinent][$selCountry])){
				$out.='
				<tr>
					<td><B>Zone</B></td>
					<td>';
				if(count($zonesArray[$selContinent][$selCountry])==1){
					$tmpArray=$zonesArray[$selContinent][$selCountry][0];
					$timeZoneLabel=((@$tmpArray[1]!='')?$tmpArray[1]:$tmpArray[0]);
					$out.=$timeZoneLabel;
					$timeZone=array($tmpArray[0],$timeZoneLabel);
				}else{
					$out.='
						<select name="zone" onChange="document.basicInfo.timeZoneText.value=escape(document.basicInfo.zone[document.basicInfo.zone.selectedIndex].text);document.basicInfo.action.value=\'form\';document.basicInfo.submit();">
							<option value="">- Please select -</option>';
					foreach ($zonesArray[$selContinent][$selCountry] AS $zone){
						$out.='<option value="'.$zone[0].'" '.(($zone[0]==@$_POST['zone'])?'selected':'').'>'.(($zone[1]!='')?$zone[1]:$zone[0]).'</option>';
					}
					$out.='
						</select>';
				}
				$out.='	
					</td>
				</tr>';
			}
			if(isset($_POST['zone']) && $_POST['zone']!=''){
				$timeZone=array($_POST['zone'],urldecode($_POST['timeZoneText']));
			} 
			if(isset($timeZone)){
				
				$out.='
				<tr>
					<td colspan="2" align="center">Timezone selected: <B>'.$timeZone[1].'</B></td>
				</tr>
				<tr>
					<td colspan="2" align="center"><input type="submit" class="button" name="setTimezone" value="Set TimeZone"></td>
				</tr>				
				<input type="hidden" name="newTimeZone" value="'.$timeZone[0].'">
				';
			}
			
		}
		$out.='
		</table>
		</form>';
	}else{
	// process area
	$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
	if (!$canModifyInstallation) {	
		header('Location: index.php?section=basicInfo&error=You are not allowed to change the installation');
		exit();
	}
		$newTimeZone=$_POST['newTimeZone'];
		$oldTimeZone=$_POST['oldTimeZone'];
		if($newTimeZone!=$oldTimeZone){
			$cmdToSend='sudo -u root /usr/pluto/bin/SetTimeZone.sh '.$newTimeZone;
			exec($cmdToSend);	
			header('Location: index.php?section=basicInfo&msg=Timezone was changed.');
			exit();
		}else{
			header('Location: index.php?section=basicInfo&error=No change for timezone.');
			exit();
		}
	}
	
	$output->setScriptCalendar('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}


?>