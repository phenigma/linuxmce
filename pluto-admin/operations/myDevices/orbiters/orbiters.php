<?
function orbiters($output,$dbADO) {
/* @var $dbADO ADOConnection */
/* @var $rs ADORecordSet */
/* @var $resUserTypes ADORecordSet */
$out='';
$locationGoTo='';
//$dbADO->debug=false;
$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
$lastAction = isset($_REQUEST['lastAction'])?cleanString($_REQUEST['lastAction']):'';
$deviceID = (int)@$_REQUEST['deviceID']; //the device that is an orbiter

if (@$_SESSION['userLoggedIn']!=true) {
	header("Location: index.php?section=login&from=users");
}

$installationID = cleanInteger($_SESSION['installationID']);

$out.='<h3>List of Orbiters</h3>';
// grab the valid orbiters
		$validOrbiters = getValidOrbitersArray($installationID,$dbADO);
		
//clean-up
$queryDeleteBadOrbiters = "DELETE FROM Orbiter WHERE PK_Orbiter NOT IN (".(join(",",$validOrbiters)).")";
$queryInsertGoodOrbiters = "INSERT INTO Orbiter(PK_Orbiter) SELECT PK_Device FROM Device LEFT JOIN Orbiter ON PK_Device = PK_Orbiter WHERE PK_Orbiter IS NULL AND PK_Device IN (".join(",",$validOrbiters).")";
$res=$dbADO->_Execute($queryDeleteBadOrbiters);
$res=$dbADO->_Execute($queryInsertGoodOrbiters);

if ($action=='form') {
$queryOrbiters = '
		SELECT Orbiter.*,Device.Description
				FROM Orbiter 
					INNER JOIN Device ON PK_Device = PK_Orbiter
					LEFT JOIN EntertainArea ON FK_EntertainArea = PK_EntertainArea
					INNER JOIN System ON FK_System = PK_System
					INNER JOIN Skin ON FK_Skin = PK_Skin
					INNER JOIN Size ON FK_Size = PK_Size
					INNER JOIN Version ON FK_Version = PK_Version
					INNER JOIN Language ON FK_Language = PK_Language				
		WHERE PK_Orbiter IN ('.join(",",$validOrbiters).')';

$resOrbiters = $dbADO->_Execute($queryOrbiters);
//echo $resOrbiters->sql;


	if ($resOrbiters) {
		$out.="
	
		<table border='0'>
			<form action='index.php' method='post' name='orbiters'>
			<input type='hidden' name='section' value='orbiters'>
			<input type='hidden' name='action' value='add'>
			<input type='hidden' name='deviceID' value='{$deviceID}'>
			<input type='hidden' name='lastAction' value=''>
		";
		
		$orbitersFormValidation='';
		$out.='<tr bgcolor="#AAAAAA"><td>Description</td><td>Entertain Area</td><td>System</td><td>Skin</td>
					<td>Size</td><td>Version</td><td>Language</td>
					<td>No Effects</td>					
				</tr>';
		$displayedOrbiters=array();
		$i=0;
		
		$entertainAreaTxt = '';		
		$selectEntertainAreas = 'SELECT EntertainArea.* FROM 
									EntertainArea INNER JOIN Room ON PK_Room = FK_Room 						
								 WHERE FK_Installation = '.$installationID.'
							ORDER BY Description ASC';
		$resEntertainAreas = $dbADO->_Execute($selectEntertainAreas);
		
		$systemsTxt = '';		
		$selectSystems = 'SELECT * FROM System ORDER BY Description ASC';
		$resSystems = $dbADO->_Execute($selectSystems);
		
		$skinsTxt = '';		
		$selectSkins = 'SELECT * FROM Skin ORDER BY Description ASC';
		$resSkins = $dbADO->_Execute($selectSkins);
		
		$sizesTxt = '';		
		$selectSizes = 'SELECT * FROM Size ORDER BY Description ASC';
		$resSizes = $dbADO->_Execute($selectSizes);
		
		$versionsTxt = '';		
		$selectVersions = 'SELECT * FROM Version ORDER BY Description ASC';
		$resVersions = $dbADO->_Execute($selectVersions);
		
		$languagesTxt = '';		
		$selectLanguages = 'SELECT * FROM Language ORDER BY Description ASC';
		$resLanguages = $dbADO->_Execute($selectLanguages);
		
		
		
		while ($rowOrbiter = $resOrbiters->FetchRow()) {
			$displayedOrbiters[]=$rowOrbiter['PK_Orbiter'];
			$color=($i%2==1?"999999":"EEEEEE");
			$out.='<tr valign="top" bgcolor="#'.$color.'">
						<td>
							'.$rowOrbiter['Description'].'
						</td>
			';

			if ($resEntertainAreas) {
					$resEntertainAreas->MoveFirst();
					$entertainAreaTxt='';
					while ($rowEntertainArea=$resEntertainAreas->fetchRow()) {
						$entertainAreaTxt.='<option  '.($rowEntertainArea['PK_EntertainArea']==$rowOrbiter['FK_EntertainArea']?' selected="selected" ':'').' value="'.$rowEntertainArea['PK_EntertainArea'].'">'.$rowEntertainArea['Description'].'</option>';
					}
			}
			$out.='		<td>
							<select name="orbiterEntertainArea_'.$rowOrbiter['PK_Orbiter'].'">
							<option value="0">-please select-</option>
							'.$entertainAreaTxt.'
							</select>
						</td>
			';

			if ($resSystems) {
					$resSystems->MoveFirst();
					$systemsTxt='';
					while ($rowSystem=$resSystems->fetchRow()) {
						$systemsTxt.='<option  '.($rowSystem['PK_System']==$rowOrbiter['FK_System']?' selected="selected" ':'').' value="'.$rowSystem['PK_System'].'">'.$rowSystem['Description'].'</option>';
					}
			}
			$out.='		<td>
							<select name="orbiterSystem_'.$rowOrbiter['PK_Orbiter'].'">
							'.$systemsTxt.'
							</select>
						</td>
			';

			if ($resSkins) {
					$resSkins->MoveFirst();
					$skinsTxt='';
					while ($rowSkin=$resSkins->fetchRow()) {
						$skinsTxt.='<option  '.($rowSkin['PK_Skin']==$rowOrbiter['FK_Skin']?' selected="selected" ':'').' value="'.$rowSkin['PK_Skin'].'">'.$rowSkin['Description'].'</option>';
					}
			}
			$out.='		<td>
							<select name="orbiterSkin_'.$rowOrbiter['PK_Orbiter'].'">
							'.$skinsTxt.'
							</select>
						</td>
			';
			
			if ($resSizes) {
					$resSizes->MoveFirst();
					$sizesTxt='';
					while ($rowSize=$resSizes->fetchRow()) {
						$sizesTxt.='<option  '.($rowSize['PK_Size']==$rowOrbiter['FK_Size']?' selected="selected" ':'').' value="'.$rowSize['PK_Size'].'">'.$rowSize['Description'].'</option>';
					}
			}
			$out.='		<td>
							<select name="orbiterSize_'.$rowOrbiter['PK_Orbiter'].'">
							'.$sizesTxt.'
							</select>
						</td>
			';
			
			if ($resVersions) {
					$resVersions->MoveFirst();
					$versionsTxt='';
					while ($rowVersion=$resVersions->fetchRow()) {
						$versionsTxt.='<option  '.($rowVersion['PK_Version']==$rowOrbiter['FK_Version']?' selected="selected" ':'').' value="'.$rowVersion['PK_Version'].'">'.$rowVersion['Description'].'</option>';
					}
			}
			$out.='		<td>
							<select name="orbiterVersion_'.$rowOrbiter['PK_Orbiter'].'">
							'.$versionsTxt.'
							</select>
						</td>
			';
			
			
			if ($resLanguages) {
					$resLanguages->MoveFirst();
					$languagesTxt='';
					while ($rowLanguages=$resLanguages->fetchRow()) {
						$languagesTxt.='<option value="'.$rowLanguages['PK_Language'].'" '.($rowOrbiter['FK_Language']==$rowLanguages['PK_Language']?"selected = 'selected'":"").'>'.$rowLanguages['Description'].'</option>';
					}
			}
			$out.='		<td>
							<select name="orbiterLanguage_'.$rowOrbiter['PK_Orbiter'].'">
							'.$languagesTxt.'
							</select>
						</td>

			
			<td><input type="checkbox" name="orbiterUseEffects_'.$rowOrbiter['PK_Orbiter'].'" value="1" '.((int)$rowOrbiter['NoEffects']==1?" checked='checked' ":'').'></td>';
			
			$out.='</tr>';
			/*$orbitersFormValidation.='
				frmvalidator.addValidation("userUserName_'.$rowUser['PK_Users'].'","req","Please enter a description");
			';*/
			$i++;
		}
		
		
		
		$out.='
			
			<tr><td colspan="2"><input type="submit" name="submitX" value="Submit">'.(isset($_GET['msg'])?"<br/><b>".strip_tags($_GET['msg']).'</b>':'').'</td></tr>
				<input type="hidden" name="displayedOrbiters" value="'.join(",",$displayedOrbiters).'">
			</form>

		<script>
		 	var frmvalidator = new formValidator("orbiters"); 		
			'.$orbitersFormValidation.'
		</script>
			
		</table>
		
		
		';
		if ($deviceID!=0) {
			$out.='<a href="index.php?section=editDeviceParams&deviceID='.$deviceID.'">Back to Devices</a>';	
		} else {
			$out.='<a href="index.php?section=myDevices&action=showBasicInfo">Back to My Devices</a>';	
		}
	}
} else {
	
	//check if current user canModifyInstallation
	$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$installationID,$dbADO);
	
	if ($canModifyInstallation) {		
			//process			
			
			$displayedOrbiters = cleanString($_POST['displayedOrbiters']);	
			$displayedOrbitersArray = explode(",",$displayedOrbiters);
			
			
			
			if (!is_array($displayedOrbitersArray) || $displayedOrbitersArray===array()) {
				$displayedOrbitersArray=array();
			}
			
			
			foreach ($displayedOrbitersArray as $orbiter) {

			
				$orbiterEntertainArea= ((int)($_POST['orbiterEntertainArea_'.$orbiter])==0?NULL:cleanInteger($_POST['orbiterEntertainArea_'.$orbiter]));
				$orbiterSystem = cleanInteger(@$_POST['orbiterSystem_'.$orbiter]);
				$orbiterSkin = cleanInteger(@$_POST['orbiterSkin_'.$orbiter]);
				$orbiterSize = cleanInteger(@$_POST['orbiterSize_'.$orbiter]);
				$orbiterVersion = cleanInteger(@$_POST['orbiterVersion_'.$orbiter]);
				$orbiterLanguage = cleanInteger(@$_POST['orbiterLanguage_'.$orbiter]);
				$orbiterUseEffects = cleanInteger(@$_POST['orbiterUseEffects_'.$orbiter]);


				$query = 'UPDATE Orbiter set
									FK_EntertainArea = ?,
									FK_System =?,
									FK_Skin =?,
									FK_Size =?,
									FK_Version=?,
									FK_Language=?,
									NoEffects=?									
							WHERE PK_Orbiter = ?';

				if (is_null($orbiterEntertainArea) || $orbiterEntertainArea==0) {
					$resUpdOrbiter = $dbADO->Execute($query,array(NULL,$orbiterSystem,$orbiterSkin,
					$orbiterSize,$orbiterVersion,$orbiterLanguage,$orbiterUseEffects,$orbiter
					));
				} else {
					$resUpdOrbiter = $dbADO->Execute($query,array($orbiterEntertainArea,$orbiterSystem,$orbiterSkin,
					$orbiterSize,$orbiterVersion,$orbiterLanguage,$orbiterUseEffects,$orbiter
					));
				}

				//$locationGoTo = "orbiterEntertainArea_".$orbiter;


			}
			
			if (strstr($locationGoTo,"#")) {
				header("Location: index.php?section=orbiters&deviceID={$deviceID}&msg=Saved!".$locationGoTo);
			} else {
				header("Location: index.php?section=orbiters&deviceID={$deviceID}&msg=Saved!&lastAction=".$locationGoTo);
			}
	} else {
			header("Location: index.php?section=orbiters&deviceID={$deviceID}&msg=Not allowed!&lastAction=".$locationGoTo);
	}
	
}

	$onLoad='';
	if ($lastAction!=''?$onLoad.="if (document.forms.users.{$lastAction}) {document.forms.users.{$lastAction}.focus();} ":$onLoad.="")
	if (strlen($onLoad)>2) {
		$output->setScriptInBody("onLoad=\"javascript:eval('$onLoad');\"");
	}
	
	$output->setNavigationMenu(array("My Devices"=>'index.php?section=myDevices&action=showBasicInfo',"Orbiters"=>'index.php?section=orbiters'));
	
$output->setBody($out);
$output->setTitle(APPLICATION_NAME);			
$output->output();  		
}

?>