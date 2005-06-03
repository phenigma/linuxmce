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
		$defLanguage=getLanguage($dceRouterID,$dbADO);
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
	
	if ($action=='form') {		
		$out.=setLeftMenu($dbADO).'
		<div class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
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
					<td>'.generatePullDown('countryID','Country','PK_Country','Description',$rowInstallation['FK_Country'],$dbADO).'</td>
				</tr>		
				<tr>
					<td><B>Zip/Postal Code</B>:</td>
					<td><input type="text" size="30" name="Zip" value="'.$rowInstallation['Zip'].'"></td>
				</tr>
				<tr>
					<td colspan="2">
					<table cellpadding="3" align="left">';
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
			
			header("Location: index.php?section=installationSettings&from=$from&msg=Installation was updated.");
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

function getLanguage($DCERouterID,$dbADO)
{
	$res=$dbADO->Execute('SELECT * FROM Device_DeviceData WHERE FK_Device=? AND FK_DeviceData=?',array($DCERouterID,$GLOBALS['Language']));
	if($res->RecordCount()==0){
		return null;
	}else{
		$row=$res->FetchRow();
		return $row['IK_DeviceData'];
	}
}
?>
