<?php
function installationSettings($output,$dbADO) {
	//$dbADO->debug=true;
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$from = isset($_REQUEST['from'])?cleanString($_REQUEST['from']):'';
	
	$countriesFileArray=file('/usr/share/zoneinfo/iso3166.tab');
	$zonesFileArray=file('/usr/share/zoneinfo/zone.tab');
	$continentsArray=array('Africa', 'America', 'Antarctica', 'Arctic [Ocean]', 'Asia', 'Atlantic [Ocean]', 'Australia', 'Europe', 'Indian [Ocean]', 'Pacific [Ocean]');
	$currentTimeZone=trim(implode('',file('/etc/timezone')));
	$ripFormats=array('mp3'=>'mp3','ogg'=>'ogg', 'flac'=>'flac', 'wav'=>'wav');
	
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
	
	
	$installationID = cleanInteger($_SESSION['installationID']);
	$dceRouterID=getDeviceFromDT($installationID,$GLOBALS['rootDCERouter'],$dbADO);
	if($dceRouterID!==null){
		$dceRouterDD=getDD($dceRouterID,$GLOBALS['Language'].','.$GLOBALS['PK_City'],$dbADO);
		$defLanguage=$dceRouterDD[$GLOBALS['Language']];
		$selectedCity=(int)$dceRouterDD[$GLOBALS['PK_City']];
		if($selectedCity>0){
			$cityData=getFieldsAsArray('City','FK_Region,FK_Country',$dbADO,'WHERE PK_City='.$selectedCity);
			$selectedRegion=$cityData['FK_Region'][0];
		}
	}
	
	$eventPluginID=getDeviceFromDT($installationID,$GLOBALS['EventPlugIn'],$dbADO);
	if($eventPluginID!==null){
		$eventPluginDD=getDD($eventPluginID,$GLOBALS['Longitude'].','.$GLOBALS['Latitude'],$dbADO);
		$Longitude=$eventPluginDD[$GLOBALS['Longitude']];
		$Latitude=$eventPluginDD[$GLOBALS['Latitude']];
	}

	$mediaPluginID=getDeviceFromDT($installationID,$GLOBALS['rootMediaPlugin'],$dbADO);
	if($mediaPluginID!==null){
		$mediaPluginDD=getDD($mediaPluginID,$GLOBALS['RipFormat'],$dbADO);
		$selectedRipFormat=$mediaPluginDD[$GLOBALS['RipFormat']];
	}else{
		$selectedRipFormat='mp3';
	}
	
	$query = "
		SELECT Installation.*,Version.Description AS V_Desc FROM Installation 
			LEFT JOIN Version on FK_Version	= PK_Version 
		WHERE 
		PK_Installation = ?
	";
	$selectInstallation = $dbADO->Execute($query,array($installationID));
	
	if ($selectInstallation) {
		$rowInstallation = $selectInstallation->FetchRow();			
	}
	$selectedCountry=(isset($_POST['countryID']))?(int)$_POST['countryID']:$rowInstallation['FK_Country'];	
	
	if ($action=='form') {		
		$out.=setLeftMenu($dbADO).'
		<form action="index.php" method="post" name="installationSettings">
		<input type="hidden" name="section" value="installationSettings">
		<input type="hidden" name="action" value="add">		
		<input type="hidden" name="lastAction" value="">
		<input type="hidden" name="from" value="'.$from.'">
		<input type="hidden" name="timeZoneText" value="update">

		<div align="center" class="err">'.@$_REQUEST['error'].'</div>
		<div align="center" class="confirm"><B>'.@$_REQUEST['msg'].'</B></div>
			<table width="300">			
				<tr>
					<td colspan="2" align="center" bgcolor="lightblue"><B>Location</B>:</td>
				</tr>		
				<tr>
					<td width="100"><B>Description</B>:</td>
					<td><input type="text" size="30" name="Description" value="'.$rowInstallation['Description'].'"></td>
				</tr>
				<tr>
					<td><B>Name</B>:</td>
					<td><input type="text" size="30" name="Name" value="'.$rowInstallation['Name'].'"></td>
				</tr>
				<tr>
					<td><B>Address</B>:</td>
					<td><input type="text" size="30" name="Address" value="'.$rowInstallation['Address'].'"></td>
				</tr>
				<tr>
					<td colspan="2">This information is optional and presently not used. In the future some plug-ins, like security monitoring, may require address information.</td>
				</tr>
				<tr>
					<td><B>City</B>:</td>
					<td><input type="text" size="30" name="City" value="'.$rowInstallation['City'].'"></td>
				</tr>
				<tr>
					<td><B>State</B>:</td>
					<td><input type="text" size="30" name="State" value="'.$rowInstallation['State'].'"></td>
				</tr>
				<tr>
					<td><B>Country</B>:</td>
					<td>'.generatePullDown('countryID','Country','PK_Country','Description',$selectedCountry,$dbADO,'','onChange="document.installationSettings.action.value=\'form\';document.installationSettings.submit();"').'</td>
				</tr>';
				if($selectedCountry>0){
					$selectedRegion=(isset($_POST['region']))?(int)$_POST['region']:(int)@$selectedRegion;	
					$out.='
					<tr>
						<td><B>Region</B>:</td>
						<td>'.generatePullDown('region','Region','PK_Region','Region',@$selectedRegion,$dbADO,'WHERE FK_Country='.$selectedCountry,'onChange="document.installationSettings.action.value=\'form\';document.installationSettings.submit();"').'</td>
					</tr>';
					
					if($selectedRegion>0){
						$output->setScriptInHead(getCitiesCoordsArray($dbADO,' WHERE FK_Region='.$selectedRegion));
					$out.='
					<tr>
						<td><B>City</B>:</td>
						<td>'.generatePullDown('city_coords','City','PK_City','City',@$selectedCity,$dbADO,'WHERE FK_Region='.$selectedRegion,'onChange="setCoordinates()"').'</td>
					</tr>';
					
					}
				}
				$out.='		
				<tr>
					<td colspan="2">
						<B>Longitude</B>: <input type="text" size="5" name="Longitude" value="'.@$Longitude.'">
						<B>Latitude</B>: <input type="text" size="5" name="Latitude" value="'.@$Latitude.'">
					</td>
				</tr>
				<tr>
					<td><B>Zip/Postal Code</B>:</td>
					<td><input type="text" size="30" name="Zip" value="'.$rowInstallation['Zip'].'"></td>
				</tr>
				<tr>
					<td colspan="2">
					<table cellpadding="3" align="left" width="100%">';
			if(isset($oldTimeZone)){
				
				$out.='
				<tr>
					<td colspan="2" align="center" bgcolor="lightblue">Current timezone: <B>'.$oldTimeZone[1].'</B></td>
				</tr>
				<input type="hidden" name="oldTimeZone" value="'.$oldTimeZone[0].'">
				';
			}		
		$out.='
			<tr>
				<td><B>Continent</B></td>
				<td><select name="continent" onChange="document.installationSettings.action.value=\'form\';document.installationSettings.submit();">
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
				<select name="country" onChange="document.installationSettings.action.value=\'form\';document.installationSettings.submit();">
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
						<select name="zone" onChange="document.installationSettings.timeZoneText.value=escape(document.installationSettings.zone[document.installationSettings.zone.selectedIndex].text);document.installationSettings.action.value=\'form\';document.installationSettings.submit();">
							<option value="">- Please select -</option>';
					$orderedZonesArray=$zonesArray[$selContinent][$selCountry];
					asort($orderedZonesArray);
					foreach ($orderedZonesArray AS $zone){
						$out.='<option value="'.$zone[0].'" '.(($zone[0]==@$_POST['zone'])?'selected':'').'>'.str_replace($selContinent.'/','',$zone[0]).' - '.(($zone[1]!='')?$zone[1]:$zone[0]).'</option>';
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
					<td align="center"><B>Default Language</B></td>
					<td>'.generatePullDown('newLanguage','Language','PK_Language','Description',$defLanguage,$dbADO).'</td>
				</tr>				
				<input type="hidden" name="newTimeZone" value="'.$timeZone[0].'">
				';
			}
			
		}
		$out.='
					</table>
		
					</td>
				</tr>
				<tr>
					<td colspan="2" align="center" bgcolor="lightblue"><B>Miscelaneous</B>:</td>
				</tr>
				<tr>
					<td align="left" colspan="2"><B>Ripping format for cd\'s: </B>'.pulldownFromArray($ripFormats,'rip',$selectedRipFormat).'</td>
				</tr>				
				<tr>
					<td colspan="2" align="center"><input type="submit" class="button" name="submitX" value="Save"  ></td>
				</tr>
			</table>
		</form>
		
		<script>
		 	var frmvalidator = new formValidator("installationSettings");
 			frmvalidator.addValidation("Description","req","Please enter a description");
			//frmvalidator.addValidation("Name","req","Please enter a name");			
		</script>
		';
	} else {
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=installationSettings&error=You are not authorised to change the installation.");
			exit(0);
		}
		

		$description = cleanString(@$_POST['Description'],50);
		$name = cleanString(@$_POST['Name'],50);
		$address = cleanString(@$_POST['Address'],50);
		$city = cleanString(@$_POST['City'],50);
		$state = cleanString(@$_POST['State'],50);
		$zip = cleanString(@$_POST['Zip'],50);
		$country=((int)$_POST['countryID']!=0)?(int)$_POST['countryID']:NULL;
		$newLanguage=(int)$_POST['newLanguage'];
		
		$city_coords=(int)@$_POST['city_coords'];
		$Longitude=$_POST['Longitude'];
		$Latitude=$_POST['Latitude'];
		$updateDD='UPDATE Device_DeviceData SET IK_DeviceData=? WHERE FK_Device=? AND FK_DeviceData=?';
		$err='';
		if($dceRouterID!==null){
			$dbADO->Execute($updateDD,array($city_coords,$dceRouterID,$GLOBALS['PK_City']));
		}else{
			$err.='Error: unable to find DCE router device.<br>';
		}	
		if($eventPluginID!==null){
			$dbADO->Execute($updateDD,array($Longitude,$eventPluginID,$GLOBALS['Longitude']));
			$dbADO->Execute($updateDD,array($Latitude,$eventPluginID,$GLOBALS['Latitude']));
		}else{
			$err.='Error: unable to find Event plugin device.<br>';
		}

		$rip=$_POST['rip'];
		if($mediaPluginID!==null){
			$dbADO->Execute($updateDD,array($rip,$mediaPluginID,$GLOBALS['RipFormat']));
		}else{
			$err.='Error: unable to find Media plugin device.<br>';
		}

		
		if ($installationID!=0 && $description!='') {
			$queryUpdate = 'UPDATE Installation Set Description=?,Name=?,Address=?,City=?,State=?,Zip=?,FK_Country=? 
							WHERE PK_Installation = ?';
			$dbADO->Execute($queryUpdate,array($description,$name,$address,$city,$state,$zip,$country,$installationID));
			
			$newTimeZone=$_POST['newTimeZone'];
			$oldTimeZone=$_POST['oldTimeZone'];
			if($newTimeZone!=$oldTimeZone){
				$cmdToSend='sudo -u root /usr/pluto/bin/SetTimeZone.sh '.$newTimeZone;
				exec($cmdToSend);	
			}
			if($newLanguage!=$defLanguage){
				if(is_null($defLanguage)){
					$dbADO->Execute('INSERT INTO Device_DeviceData (IK_DeviceData,FK_Device,FK_DeviceData) VALUES (?,?,?)',array($newLanguage,$dceRouterID,$GLOBALS['Language']));
				}else{
					$dbADO->Execute('UPDATE Device_DeviceData SET IK_DeviceData=? WHERE FK_Device=? AND FK_DeviceData=?',array($newLanguage,$dceRouterID,$GLOBALS['Language']));
				}
			}
			
			$suffix=(isset($err))?'&error='.$err:'';
			header("Location: index.php?section=installationSettings&from=$from&msg=Installation was updated.$suffix");
			exit();
		} else {
			header("Location: index.php?section=installationSettings&error=Invalid ID or empty name");
		}
				
	}
	
	$output->setNavigationMenu(array("Installation Settings"=>"index.php?section=installationSettings"));
	
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);			
	$output->output();
}

function getDD($deviceID,$deviceDataValues,$dbADO)
{
	$ddArray=array();
	$fields=explode(',',$deviceDataValues);
	foreach ($fields AS $fieldID){
		$ddArray[$fieldID]=null;
	}
	
	$res=$dbADO->Execute('SELECT * FROM Device_DeviceData WHERE FK_Device=? AND FK_DeviceData IN ('.$deviceDataValues.')',array($deviceID));
	if($res->RecordCount()==0){
		return $ddArray;
	}else{
		while($row=$res->FetchRow()){
			$ddArray[$row['FK_DeviceData']]=$row['IK_DeviceData'];
		}
	}


	return $ddArray;
}

// return a javascript array with coordinates
function getCitiesCoordsArray($dbADO,$filter='')
{
	
	$out='
	<script>
	function setCoordinates()
	{
		currentCity=document.installationSettings.city_coords.value;
		try{
			document.installationSettings.Longitude.value=coords_long[currentCity];
			document.installationSettings.Latitude.value=coords_lat[currentCity];
		}catch(e){
			// do nothing
		}
	}
	';
	$res=$dbADO->Execute('SELECT * FROM City '.$filter);
	$out.='
		var coords_long=new Array('.$res->RecordCount().');
		var coords_lat=new Array('.$res->RecordCount().');
	';
	while($row=$res->FetchRow()){
		$out.='
		coords_long['.$row['PK_City'].']='.$row['Longitude'].';
		coords_lat['.$row['PK_City'].']='.$row['Latitude'].';';
	}
	$out.='
	</script>
	';

	return $out;
}
?>
