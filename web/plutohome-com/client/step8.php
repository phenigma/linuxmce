<?

if(!isset($_SESSION['deviceID'])){
	$queryDevice='
		SELECT Device.* 
		FROM Device
		INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
		WHERE FK_DeviceCategory=? AND FK_Installation=?
		';
	$resDevice=$dbADO->Execute($queryDevice,array($GLOBALS['rootCoreID'],$installationID));
	if($resDevice->RecordCount()!=0){
		$rowDevice=$resDevice->FetchRow();
		$_SESSION['deviceID']=$rowDevice['PK_Device'];
	}else{
		unset($_SESSION['deviceID']);
		unset($_SESSION['isCoreFirstTime']);
		unset($_SESSION['CoreDCERouter']);
		unset($_SESSION['coreHybridID']);
		unset($_SESSION['OrbiterHybridChild']);
		unset($_SESSION['isHybridFirstTime']);
		$FK_DeviceTemplate=0;
		$FK_Distro=0;
		header("Location: index.php?section=wizard&step=1");
	}
}

$queryDevice='SELECT * FROM Device WHERE PK_Device=?';
$resDevice=$dbADO->Execute($queryDevice,$_SESSION['deviceID']);
$rowDevice=$resDevice->FetchRow();
$FK_DeviceTemplate=$rowDevice['FK_DeviceTemplate'];

$queryDeviceDeviceData='SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device=? AND FK_DeviceData=?';
$resDeviceDeviceData=$dbADO->Execute($queryDeviceDeviceData,array($_SESSION['deviceID'],$GLOBALS['rootPK_Distro']));
$rowDeviceDeviceData=$resDeviceDeviceData->FetchRow();
$FK_Distro=$rowDeviceDeviceData['IK_DeviceData'];
$_SESSION['distro']=$FK_Distro;
switch($FK_Distro){
	case 7:
		$_SESSION['dedicated']=2;
	break;
	default:
		$_SESSION['dedicated']=3;
	break;
}


$resDDD=$dbADO->Execute($queryDeviceDeviceData,array($_SESSION['deviceID'],$GLOBALS['kickStartCD']));
if($resDDD->RecordCount()>0){
	$_SESSION['dedicated']=1;
}

$queryDistroOS='
	SELECT Distro.*, OperatingSystem.Description AS OS 
	FROM Distro
	INNER JOIN OperatingSystem ON FK_OperatingSystem=PK_OperatingSystem
	WHERE PK_Distro=?';
$resDistroOS=$dbADO->Execute($queryDistroOS,$FK_Distro);
$rowDistro=$resDistroOS->FetchRow();
$DistroNameOS=$rowDistro['Description'].' / '.$rowDistro['OS'];
$distroKickStartCD=$rowDistro['KickStartCD'];
$distroInstaller=$rowDistro['Installer'];
$InstallerURL=$rowDistro['InstallerURL'];
$autoInstallScript=$_SESSION['deviceID'].'-'.$_SESSION['ActivationCode'];


$getInstallation='SELECT * FROM Installation WHERE PK_Installation=?';
$resInstallation=$dbADO->Execute($getInstallation,$installationID);
$rowInstallation=$resInstallation->FetchRow();
$selectedRepositoryType=($rowInstallation['FK_RepositoryType_Source'])?$rowInstallation['FK_RepositoryType_Source']:4;
$selectedRepositoryType_Binaries=($rowInstallation['FK_RepositoryType_Binaries'])?$rowInstallation['FK_RepositoryType_Binaries']:0;

$_SESSION['sollutionType']=1;
$queryMediaDirectors='
	SELECT Device.*
	FROM Device
	WHERE FK_DeviceTemplate=? AND FK_Installation=?';
$resMediaDirectors=$dbADO->Execute($queryMediaDirectors,array($GLOBALS['rootGenericMediaDirector'],$installationID));
$_SESSION['coreName']='Core';
while ($row=$resMediaDirectors->FetchRow()){
	if(!is_null($row['FK_Device_ControlledVia'])){
		$_SESSION['coreName']='Hybrid';
		$_SESSION['sollutionType']=($row['Description']=='The core/hybrid')?2:3;
	}
}

if($action=='form'){

	// display form
	$out = '
				<script>
					function windowOpen(locationA,attributes) {
						window.open(locationA,\'\',attributes);
					}
				</script>
				<br>
				   <form action="index.php" method="POST" name="wizard">
					<input type="hidden" name="section" value="wizard">
					<input type="hidden" name="step" value="'.$step.'">
					<input type="hidden" name="action" value="add">	
			      		<table align="center" width="100%">
							<tr>
								<td align="left" class="normaltext"><img src="images/install_wizard.gif"></td>
								<td align="right" class="normaltext" width="150"><a href="index.php?section=wizard&step=7">&lt;&lt; Previous</a></td>
							</tr>
			      			<tr>
			      				<td align="center" colspan="2" bgcolor="#DADDE4"><b>Step 8 of 8: Download</b></td>
			      			</tr>
							<tr class="normaltext">
								<td colspan="2" align="left">You finished the wizard!  All the settings you made have been saved into a database so your system can install and configure itself automatically.  You can go back to a prior page to change options and return to this page at any time.  
								Once your system is up and running, you will make all future changes on the Pluto Admin web site that gets installed on your '.$_SESSION['coreName'].'.  The settings you chose here are only used when you are installing the software, and have no effect once the software is already installed.<br><br></td>
							</tr>
							<tr class="normaltext">
								<td colspan="2" align="left"><b>Your activation code is: '.$_SESSION['ActivationCode'].'</b> &nbsp;&nbsp;&nbsp; You will be asked for this during installation.<br><br>During installation you will be asked for the activation code, and the device ID listed below.  This will be used to lookup the configuration at plutohome.com.  Just be sure to correctly specify the device for each computer you use it on.</td>
							</tr>
							<tr class="normaltext">
								<td colspan="2" align="left">&nbsp;</td>
							</tr>
							<tr class="normaltext">
								<td colspan="3" align="left"><br>Here are the computers you will need software for.  Be sure to make note of the Device numbers and click the appropriate link to download the software.<br><br><B>Core:</B></td>
							</tr>				
							<tr class="normaltext" bgcolor="lightblue">
								<td valign="top"><b>'.$DistroNameOS.'</b></td>
								<td>Device #: <b>'.$_SESSION['deviceID'].'</b></td>
							</tr>
							<tr class="normaltext">
								<td valign="top" colspan="2">Below are your link(s) to the the software for this computer: &nbsp;&nbsp;&nbsp;<b>FYI - Advanced users:</b> can get a list of all the software packages <a href="#" onClick="javascript:windowOpen(\'checkDependancies.php?device='.$rowDevice['PK_Device'].'\',\'width=1024,height=768,toolbars=true,scrollbars=1\');">here</a></td>
							</tr>
							<tr class="normaltext">
								<td valign="top" colspan="2"><table width="75%" align="center">';
			switch($_SESSION['dedicated']){
				case 1:
					$out.='
									<tr class="normaltext" bgcolor="#DADDE4">
										<td width="30%" align="left">'.(($distroKickStartCD=='')?'<span style="color:#999999;">- Kick-start CD</span>':'- <a href="'.$distroKickStartCD.'" target="_blank">Kick-start CD</a>').'</td>
										<td>'.(($distroKickStartCD=='')?'<span style="color:#999999;">Not available for '.$DistroNameOS.'</span>':' An ISO image you can burn to a CD.  It is a self-booting CD for '.$DistroNameOS).'</td>
									</tr>';
					break;
					case 2:
						$out.='
									<tr class="normaltext" bgcolor="#DADDE4">
										<td width="30%" align="left">- <a href="#" onClick="javascript:windowOpen(\''.$InstallerURL.'?code='.$autoInstallScript.'&distro='.$distroInstaller.'&name='.urlencode($rowDevice['Description']).'\',\'width=640,height=480,toolbars=true,scrollbars=1\');">Auto install script</a></td>
										<td>A script for '.$DistroNameOS.' that will install all the software automatically.</td>
									</tr>';
					break;
					case 3:
						$out.='
									<tr class="normaltext" bgcolor="#DADDE4">
										<td width="30%" align="left">- <a href="#" onClick="javascript:windowOpen(\''.$InstallerURL.'?code='.$autoInstallScript.'&distro='.$distroInstaller.'&name='.urlencode($rowDevice['Description']).'\',\'width=640,height=480,toolbars=true,scrollbars=1\');">Auto install script</a></td>
										<td>A script for '.$DistroNameOS.' that will install all the software automatically.</td>
									</tr>
									<tr class="normaltext" bgcolor="#DADDE4">
										<td width="30%" align="left">- <a href="#" onClick="javascript:windowOpen(\'installWraper.php?code='.$autoInstallScript.'\',\'width=1024,height=768,toolbars=true,scrollbars=1\');">Custom Wrapper for '.$rowDevice['Description'].'</a></td>
										<td>For you to write your own install script for '.$DistroNameOS.'.  You can use our 50-line Debian bash script as a template.</td>
									</tr>';
					break;
			}
						$out.='
								</table>
				 				</td>
							</tr>
							<tr class="normaltext">
								<td valign="top">&nbsp;</td>
							</tr>
							<tr class="normaltext">
								<td valign="top"><b>Media Directors:</b></td>
								<td></td>
							<tr>
							<tr>
								<td colspan="2">';
	
	$queryMediaDirectors='
		SELECT 
			Device.*,
			DeviceTemplate.Description AS Type
		FROM Device 
		INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
		WHERE FK_Device_ControlledVia IS NULL AND FK_DeviceTemplate=? AND FK_Installation=?';
	$resMediaDirectors=$dbADO->Execute($queryMediaDirectors,array($GLOBALS['rootGenericMediaDirector'],$installationID));
	if($resMediaDirectors->RecordCount()==0){
		$out.='		<tr class="normaltext">
										<td colspan="2" align="center">No Media Directors</td>
									</tr>';
	}
	while($rowMediaDirectors=$resMediaDirectors->FetchRow()){
		$queryDeviceDeviceData='
							SELECT 
								IK_DeviceData,
								Distro.Description AS Platform,
								OperatingSystem.Description AS OS,
								Distro.*
							FROM Device_DeviceData 
								INNER JOIN Distro ON PK_Distro=IK_DeviceData
								INNER JOIN OperatingSystem ON FK_OperatingSystem=PK_OperatingSystem
							WHERE FK_Device=? AND FK_DeviceData=?';
		$resDeviceDeviceData=$dbADO->Execute($queryDeviceDeviceData,array($rowMediaDirectors['PK_Device'],$GLOBALS['rootPK_Distro']));
		$rowDDD=$resDeviceDeviceData->FetchRow();

		$queryDiskless='SELECT * FROM Device_DeviceData WHERE FK_Device=? AND FK_DeviceData=?';
		$resDiskless=$dbADO->Execute($queryDiskless,array($rowMediaDirectors['PK_Device'],$GLOBALS['rootDisklessBoot']));

		$DistroNameOS=$rowDDD['Description'].' / '.$rowDDD['OS'];
		$distroKickStartCD=$rowDDD['KickStartCD'];
		$autoInstallScript=$rowMediaDirectors['PK_Device'].'-'.$_SESSION['ActivationCode'];
		$distroInstaller=$rowDDD['Installer'];
		$InstallerURL=$rowDDD['InstallerURL'];

		$out.='
							<tr class="normaltext" bgcolor="lightblue">
								<td valign="top"><b>Name: '.$rowMediaDirectors['Description'].' Type: '.$rowMediaDirectors['Type'].' Platform: '.$rowDDD['Platform'].' '.$rowDDD['OS'].'</b></td>
								<td>Device #: <b>'.$rowMediaDirectors['PK_Device'].'</b></td>
							</tr>';
		if(($resDiskless->RecordCount()==0)){
			$out.='
							<tr class="normaltext">
								<td valign="top" colspan="2">Below are your link(s) to the the software for this computer: &nbsp;&nbsp;&nbsp;<b>FYI - Advanced users:</b> can get a list of all the software packages <a href="#" onClick="javascript:windowOpen(\'checkDependancies.php?device='.$rowMediaDirectors['PK_Device'].'\',\'width=1024,height=768,toolbars=true,scrollbars=1\');">here</a></td>
							</tr>
							<tr class="normaltext">
								<td valign="top" colspan="2"><table width="75%" align="center">';
				switch($_SESSION['dedicated']){
					case 1:
						$out.='
									<tr class="normaltext" bgcolor="#DADDE4">
										<td width="30%" align="left">'.(($rowDDD['KickStartCD']=='')?'<span style="color:#999999;">- Kick-start CD</span>':'- <a href="'.$rowDDD['KickStartCD'].'" target="_blank">Kick-start CD</a>').'</td>
										<td>'.(($rowDDD['KickStartCD']=='')?'<span style="color:#999999;">Not available for '.$DistroNameOS.'</span>':'An ISO image you can burn to a CD.  It is a self-booting CD for '.$DistroNameOS).'</td>
									</tr>';
					break;
					case 2:
						$out.='
									<tr class="normaltext" bgcolor="#DADDE4">
										<td width="30%" align="left">'.(($rowDDD['Installer']=='')?'<span style="color:#999999;">- Auto install script</span>':'- <a href="#" onClick="javascript:windowOpen(\''.$InstallerURL.'?code='.$autoInstallScript.'&distro='.$distroInstaller.'&name='.urlencode($rowMediaDirectors['Description']).'\',\'width=640,height=480,toolbars=true,scrollbars=1\');">Auto install script</a>').'</td>
										<td>'.(($rowDDD['Installer']=='')?'<span style="color:#999999;">Not available for '.$rowDDD['Platform'].'</span>':'A script for '.$DistroNameOS.' that will install all the software automatically.').'</td>
									</tr>';
					break;
					case 3:
						$out.='
									<tr class="normaltext" bgcolor="#DADDE4">
										<td width="30%" align="left">'.(($rowDDD['Installer']=='')?'<span style="color:#999999;">- Auto install script</span>':'- <a href="#" onClick="javascript:windowOpen(\''.$InstallerURL.'?code='.$autoInstallScript.'&distro='.$distroInstaller.'&name='.urlencode($rowMediaDirectors['Description']).'\',\'width=640,height=480,toolbars=true,scrollbars=1\');">Auto install script</a>').'</td>
										<td>'.(($rowDDD['Installer']=='')?'<span style="color:#999999;">Not available for '.$rowDDD['Platform'].'</span>':'A script for '.$DistroNameOS.' that will install all the software automatically.').'</td>
									</tr>
									<tr class="normaltext" bgcolor="#DADDE4">
										<td width="30%" align="left">- <a href="#" onClick="javascript:windowOpen(\'installWraper.php?code='.$autoInstallScript.'\',\'width=1024,height=768,toolbars=true,scrollbars=1\');">Custom Wrapper for '.$rowMediaDirectors['Description'].'</a></td>
										<td>For you to write your own install script for '.$DistroNameOS.'.  You can use our 50-line Debian bash script as a template.</td>
									</tr>';
						break;
				}
					$out.='
								</table>
				 				</td>
							</tr>
						';
		}else{
			$out.='
							<tr class="normaltext">
								<td valign="top" colspan="2">There\'s no software to install here since it will do a network boot off your '.$_SESSION['coreName'].'.</td>
							</tr>';
		}
	}
	$out.='
							<tr class="normaltext">
								<td valign="top"><br><b>Orbiters:</b></td>
								<td></td>
							<tr>
							<tr class="normaltext">
								<td valign="top" colspan="2">The software for your Orbiters will be put on the Core.  Once your Core is up and running, just open a web browser on the Orbiter and go the Pluto Admin web site, which is hosted on the Core.  There will be links to the software.  You can also get it from the download section of Pluto\'s <a href="support/index.php">support website</a></td>
							<tr>';
	$validOrbiters = getValidOrbitersArray($installationID,$dbADO);
	$queryOrbiters='
						SELECT 
							Device.*,
							DeviceTemplate.Description AS Type
					 	FROM Device
							INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
					 	WHERE FK_DeviceTemplate IN ('.join(',',$validOrbiters).') AND FK_Installation=? AND FK_Device_ControlledVia IS NULL';
	$resOrbiters=$dbADO->Execute($queryOrbiters,$installationID);
	if($resOrbiters->RecordCount()==0){
		$out.='		<tr class="normaltext">
										<td colspan="2" align="center">No Orbiters</td>
									</tr>';
	}
	while($rowOrbiters=$resOrbiters->FetchRow()){

		$queryDeviceDeviceData='
							SELECT 
								IK_DeviceData,
								Distro.Description AS Platform,
								OperatingSystem.Description AS OS,
								Distro.*,FK_DeviceCategory
							FROM Device_DeviceData 
								INNER JOIN Device ON FK_Device=PK_Device
								INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
								INNER JOIN Distro ON PK_Distro=IK_DeviceData
								INNER JOIN OperatingSystem ON Distro.FK_OperatingSystem=PK_OperatingSystem
							WHERE FK_Device=? AND FK_DeviceData=?';
		$resDeviceDeviceData=$dbADO->Execute($queryDeviceDeviceData,array($rowOrbiters['PK_Device'],$GLOBALS['rootPK_Distro']));
		$rowDDD=$resDeviceDeviceData->FetchRow();

		$autoInstallScript=$rowOrbiters['PK_Device'].'-'.$_SESSION['ActivationCode'];
		$distroInstaller=$rowDDD['Installer'];
		$InstallerURL=$rowDDD['InstallerURL'];
		$DistroNameOS=$rowDDD['Description'].' / '.$rowDDD['OS'];
		$distroKickStartCD=$rowDDD['KickStartCD'];

		$out.='
							<tr class="normaltext" bgcolor="lightblue">
								<td valign="top"><b>Name: '.$rowOrbiters['Description'].' Type: '.$rowOrbiters['Type'].' Platform: '.$rowDDD['Platform'].' '.$rowDDD['OS'].'</b></td>
								<td>Device #: <b>'.$rowOrbiters['PK_Device'].'</b></td>
							</tr>';
	}
	$out.='
						
						</table>
						<br><br>
					</form>
			      	';

}else{
	// process form

	header("Location: index.php?section=wizard&step=8");
}

?>
