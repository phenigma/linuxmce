<?
function wizard($output,$dbADO) {
	global $domain, $downloadDirectory;
	/* @var $dbADO ADOConnection */
	
	if ($_SESSION['userIsLogged']!="yes") {
		header("Location: index.php");
	}

	$FK_Users=(isset($_SESSION['userID']))?(int)$_SESSION['userID']:0;
	$step=(int)$_REQUEST['step'];
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$installationID=(isset($_SESSION['installationID']))?$_SESSION['installationID']:0;
	
	switch($step){
		case 1:
			if($action=='form'){
				$PK_Installation=(isset($_REQUEST['instid']))?(int)$_REQUEST['instid']:$installationID;
				if($PK_Installation!=0){
					$queryInstallations='
						SELECT * FROM Installation
							INNER JOIN Installation_Users ON FK_Installation=PK_Installation
						WHERE PK_Installation=? AND FK_Users=?';
					$resInstallations=$dbADO->Execute($queryInstallations,array($PK_Installation,$FK_Users));
					if($resInstallations){
						$rowInstallations=$resInstallations->FetchRow();
						$_SESSION['installationID']=$PK_Installation;
						$_SESSION['installationDescription']=$rowInstallations['Description'];
						$_SESSION['ActivationCode']=$rowInstallations['ActivationCode'];
					}
				}else{
					unset($_SESSION['installationID']);
					unset($_SESSION['installationDescription']);
					unset($_SESSION['ActivationCode']);
				}
				// display form
				$out = '<br>
				   <form action="index.php" method="POST" name="wizard">
					<input type="hidden" name="section" value="wizard">
					<input type="hidden" name="step" value="'.$step.'">
					<input type="hidden" name="action" value="add">	
			      		<table align="center">
							<tr>
								<td align="left" class="normaltext"><img src="images/install_wizard.gif"></td>
			      				<td align="right" class="normaltext"><a href="#" onClick="javascript:document.wizard.submit();">Next &gt;&gt;</a></td>
			      			</tr>
			      			<tr>
			      				<td align="center" colspan="2" bgcolor="#DADDE4"><b>Step 1 of 7: '.((!isset($_SESSION['installationDescription']))?'New Installation':'Update Installation').'</b></td>
			      			</tr>
							<tr>
								<td colspan="2" class="normaltext">Pluto is a whole-house solution.  You can have several computers and other devices all working together as a single, whole-house solution.  We refer to a collection of equipment at a given location as an "Installation".  An installation usually refers to a particular home, or location.  If you have multiple homes, you should create an installation for each home and then "join" them together later.  Please type a name to refer to this installation, or location, you are creating, such as: 123 Main Street, or Beach House, etc.</td>
							</tr>
			      	      	<tr>
			      				<td align="right" valign="top" class="normaltext"><B>Name</B></td>
								<td align="left" valign="top" class="normaltext"><input type="text" name="Description" value="'.@stripslashes($_SESSION['installationDescription']).'"> <input type="submit" name="submitBtn" value="Next"></td>
			      			</tr>
							<tr>
								<td colspan="2" align="left" class="normaltext"><br>Following are the 3 pieces of computer equipment in a Pluto system which need software.  Please decide now what equipment you will use, since this wizard will need some information about your equipment.  The minimum is 1 computer that you can use as both your Core and Media Director.  However, Pluto is easier to use if you have Orbiters too.<br><br>
									<B>1. The Core</B>, or server, which is the main brains of the system.  This is where all the data and configuration for your home are stored.<br><br>
									<B>2. Any number of media directors</B>.  These are media pc’s hook up to your tv and stereo to provide media in a location.<br><br>
									NOTE:   You need only 1 computer since the core can also be a media director, which we call a ‘hybrid’.  However, for a big installation with lots of media directors and orbiters, you really should have a dedicated, fail-safe core with lots of storage for the whole house.<br><br>
									<B>3. Orbiters</B> are the devices you carry around to control the system—Pluto’s version of a remote control.  You don’t actually need any Orbiters since you can just use the media director’s on-screen menu to control everything with a mouse and keyboard.  But it’s more comfortable to use Orbiters.  The Orbiters can be PDA’s, tablet-pc’s, or web pad’s that you carry around the house.  The most comfortable orbiter is a Bluetooth mobile phone, which works even when you’re not in the house.
								</td>
							</tr>
			      		</table>
					</form>
				 	<script>
		 				var frmvalidator = new formValidator("wizard");
  						frmvalidator.addValidation("Description","req","Please enter a description.");
					</script>
			      	';
				
			}else{
				// process form
				$description=@$_POST['Description'];
				
				if(isset($_POST['submitBtn']) || $action=='add'){
					if($installationID==0){
						$randNumber=rand(1000,9999);
						$insertInstallation='INSERT INTO Installation (Description, ActivationCode) VALUES (?,?)';
						$dbADO->Execute($insertInstallation,array($description,$randNumber));
						$installationID=$dbADO->Insert_ID();
						$_SESSION['installationID']=$installationID;
						$_SESSION['installationDescription']=$description;
						$_SESSION['ActivationCode']=$randNumber;
						
						$insertInstallationUsers='
							INSERT INTO Installation_Users 
								(FK_Installation, FK_Users, userCanModifyInstallation)
							VALUES (?,?,?)';
						$dbADO->Execute($insertInstallationUsers,array($installationID,$FK_Users,1));
						
						$updateUser='UPDATE Users SET FK_Installation_Main=? WHERE PK_Users=?';
						$dbADO->Execute($updateUser,array($installationID,$FK_Users));
					}else{
						$installationID=(isset($_SESSION['installationID']))?$_SESSION['installationID']:0;
						$updateInstallation='UPDATE Installation SET Description=? WHERE PK_Installation=?';
						$dbADO->Execute($updateInstallation,array($description,$installationID));
						$_SESSION['installationDescription']=$description;
					}
					header("Location: index.php?section=wizard&step=2");
				}
			}
		break;
		case 2:
				if($action=='form'){
				$version=isset($_SESSION['version'])?$_SESSION['version']:'Development';
				// display form
				$out = '<br>
				   <form action="index.php" method="POST" name="wizard">
					<input type="hidden" name="section" value="wizard">
					<input type="hidden" name="step" value="'.$step.'">
					<input type="hidden" name="action" value="add">	
			      		<table align="center">
			      			<tr>
								<td align="left" class="normaltext"><img src="images/install_wizard.gif"></td>
								<td align="right" class="normaltext"><a href="index.php?section=wizard&step=1">&lt;&lt; Previous</a> <a href="index.php?section=wizard&step=3">Next &gt;&gt;</a></td>
							</tr>
							<tr>
			      				<td align="center" colspan="2" bgcolor="#DADDE4"><b>Step 2 of 7: Version</b></td>
			      			</tr>
							<tr>
								<td colspan="2" align="left" class="normaltext"><br>There are 3 versions of the Pluto software:</td>
							</tr>
							<tr>
								<td valign="top" class="normaltext" align="right"><input type="radio" name="version" value="Development" '.(($version=='Development')?'checked':'').'></td>
								<td class="normaltext"><B>Development</B> – This is the active development version.  It may not be completely stable, but if you’re a developer and want to make your own DCE devices, or work with the source code, this is the version you should install.</td>
							</tr>
							<tr>
								<td valign="top" class="normaltext" align="right"><input type="radio" name="version" value="Testing" disabled '.(($version=='Testing')?'checked':'').'></td>
								<td class="normaltext"><span style="color:#CCCCCC"><B>Testing</B> – This version is frozen, and there is no active development.  The modules have passed our in-house tests and our ready for field testing.  This is the best version if you want the latest features and devices, but still want a stable version.</span></td>
							</tr>
							<tr>
								<td valign="top" class="normaltext" align="right"><input type="radio" name="version" value="Release" disabled '.(($version=='Release')?'checked':'').'></td>
								<td class="normaltext"><span style="color:#CCCCCC"><B>Release</B> – This is the retail, stable version that has gone through exhaustive in-house and field testing.  It is completely stable.</span><br><br></td>
							</tr>
							<tr>
								<td colspan="2" align="center"><input type="submit" name="continue" value="Next"></td>
							</tr>				
			      		</table>
					</form><br>
			      	';
				
			}else{
				// process form step 2
				if(isset($_POST['continue'])){
					$_SESSION['version']=$_POST['version'];
					header("Location: index.php?section=wizard&step=3");
				}
			}
		break;
		case 3:
				if($action=='form'){
				// display form
				$out = '<br>
				   <form action="index.php" method="POST" name="wizard">
					<input type="hidden" name="section" value="wizard">
					<input type="hidden" name="step" value="'.$step.'">
					<input type="hidden" name="action" value="add">	
			      		<table align="center">
							 <tr>
								<td align="left" class="normaltext"><img src="images/install_wizard.gif"></td>
								<td align="right" class="normaltext"><a href="index.php?section=wizard&step=2">&lt;&lt; Previous</a> <a href="index.php?section=wizard&step=4">Next &gt;&gt;</a></td>
							</tr>
							<tr>
								<td colspan="2" align="center" bgcolor="#DADDE4"><B>Step 3 of 7: Operating system</B></td>
							</tr>
							<tr>
								<td colspan="2" align="left" class="normaltext">Note: At present Linux is the only supported platform for media playback.<br><br>
								You have chosen the <b>'.$_SESSION['version'].'</b> version of the software.  Here are the operating systems that this version supports for your Core, Media Directors and Orbiters.  Note which installation options are available for each platform, as described below.
								On the next page you will be asked to choose what operating system/distro you want to use.  If "Confirmed" is not checked, then it <b>should</b> work with minimal effort, but has not been tested:
								</td>
							</tr>									
							<tr>
								<td colspan="2" align="center">
									<table>
										<tr class="normaltext">
											<td>&nbsp;</td>
											<td colspan="4"><B>Installation options</B></td>
											<td colspan="3" bgcolor="#DADDE4"><B>Can be used as</B></td>
											<td rowspan="2"><B>Confirmed</B></td>
										</tr>
										<tr class="normaltext">
											<td>&nbsp;</td>
											<td><B>Kick-Start</B></td>
											<td><B>Installer</B></td>
											<td><B>Download</B></td>
											<td><B>Source code</B></td>
											<td bgcolor="#DADDE4"><B>Core</B></td>
											<td bgcolor="#DADDE4"><B>Media Director</B></td>
											<td bgcolor="#DADDE4"><B>Orbiter</B></td>
										</tr>';
					$queryDistroOS='
						SELECT Distro.*, OperatingSystem.Description AS OS FROM Distro
							INNER JOIN OperatingSystem 
								ON FK_OperatingSystem=PK_OperatingSystem';
					$resDistroOS=$dbADO->Execute($queryDistroOS);
					while($row=$resDistroOS->FetchRow()){
						$out.='		<tr class="normaltext">
										<td><B>'.$row['OS'].' - '.$row['Description'].'</B></td>
										<td align="center">'.(($row['KickStartCD']!='')?'x':'-').'</td>
										<td align="center">'.(($row['Installer']!='')?'x':'-').'</td>
										<td align="center">'.(($row['Binaries']!='')?'x':'-').'</td>
										<td align="center">'.(($row['SourceCode']!='')?'x':'-').'</td>
										<td align="center" bgcolor="#DADDE4">'.(($row['Core']!=0)?'x':'-').'</td>
										<td align="center" bgcolor="#DADDE4">'.(($row['MediaDirector']!=0)?'x':'-').'</td>
										<td align="center"bgcolor="#DADDE4">'.(($row['Orbiter']!=0)?'x':'-').'</td>
										<td align="center">'.(($row['Confirmed']!=0)?'x':'-').'</td>
							  		</tr>';
					}
					$out.='		</table>
								</td>
							</tr>
							<tr class="normaltext">
							<td colspan="2" align="left"><b><u>Explanation of the 4 installation options:</b></u></td>
							</tr>
							<tr class="normaltext">
								<td colspan="2" align="left">
									<b>1.</b>&nbsp;&nbsp;&nbsp;&nbsp;The <b>kick-start CD</b> is the easiest way to get Pluto, particularly on a clean computer.  Just put the disk in and boot.  
									It will give you the option of partitioning and formatting the drive, and auto-install all the software.  If you want to contribute a kick-start CD for a distribution we don’t yet support, we’ll be happy to host it.<br><br>
									<b>2.</b>&nbsp;&nbsp;&nbsp;&nbsp;The <b>installer</b> also makes it a snap to install Pluto.  This is a pretty simple script that downloads and installs software and source code automatically.  
									This wizard will a "Wrapper" which will feed the installer with a list of all the software you will need for this installation.<br><br>
									<b>2b.</b>&nbsp;&nbsp;&nbsp;&nbsp;If there is no installer for your choice, we can still give you the custom <b>"Wrapper"</b> listing all the
									software you will need.  If you know how to write scripts, like bash or perl or batch files, it will probably be faster to write an 
									installer for your platform than to install all the software by hand.  And, if you share it, then everyone else using that platform
									can use it too.<br><br>
									<b>3.</b>&nbsp;&nbsp;&nbsp;&nbsp;After you have completed the wizard, we will give you a list of all the software you will need for this
									installation and links to the places where you can <b>download</b> the software manually.<br><br>
									<b>4.</b>&nbsp;&nbsp;&nbsp;&nbsp;Worst case you will have to build from <b>source</b>.  All the source code is available in svn, cvs, and as archives.  
									Our source code comes with both gcc-compatible Makefiles, and Microsoft Visual Studio .NET 2003 project files.  We also have an automated build
									and propagate program, MakeRelease, that does automated builds of all the software in the right order for both Linux and Windows, and optionally builds
									packages and archives of the output.  Contact one of our programmers for assistance.<br>
			
									
									<br><br>&nbsp;&nbsp;&nbsp;&nbsp;Click Next to configure your core, media directors and orbiters and choose what platform you want to use on each of them.
								</td>
							</tr>
							<tr>
								<td colspan="2" align="center"><input type="button" name="continue" value="Next" onClick="self.location=\'index.php?section=wizard&step=4\'"></td>
							</tr>				
			      		</table>
					</form>
			      	';
				
			}else{
				// process form step 3
				// nothing to process
			}

		break;
		case 4:
				if($action=='form'){
					$out='
					<script>
					function enableHybrid()
					{
						if(document.wizard.computerType.selectedIndex!=0 && document.wizard.platform.selectedIndex!=0)
						{
							document.wizard.action.value="addDevice";
							document.wizard.submit();
						}
					}
					function showPlatforms()
					{
						document.wizard.action.value="form";
						document.wizard.submit();
					}
					</script>
					';
				// display form
				$queryDevice='
					SELECT * 
						FROM Device
					INNER JOIN DeviceTemplate ON 
						FK_DeviceTemplate=PK_DeviceTemplate
					WHERE FK_DeviceCategory=? AND Device.FK_Installation=?
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
				}
				
				if(isset($_SESSION['deviceID'])){
					$queryDevice='SELECT * FROM Device WHERE PK_Device=?';
					$resDevice=$dbADO->Execute($queryDevice,$_SESSION['deviceID']);
					$rowDevice=$resDevice->FetchRow();
					$FK_DeviceTemplate=$rowDevice['FK_DeviceTemplate'];

					$queryDeviceDeviceData='SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device=? AND FK_DeviceData=?';
					$resDeviceDeviceData=$dbADO->Execute($queryDeviceDeviceData,array($_SESSION['deviceID'],$GLOBALS['rootPK_Distro']));
					$rowDeviceDeviceData=$resDeviceDeviceData->FetchRow();
					$FK_Distro=$rowDeviceDeviceData['IK_DeviceData'];

					$queryOS='SELECT FK_OperatingSystem FROM Distro WHERE PK_Distro=?';

					$resOS=$dbADO->Execute($queryOS,$FK_Distro);
					$rowOS=$resOS->FetchRow();
					$CoreOS=$rowOS['FK_OperatingSystem'];
					
					// query if get Pluto Core is checked								
					$queryDeviceDeviceData='SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device=? AND FK_DeviceData=?';
					$resDeviceDeviceData=$dbADO->Execute($queryDeviceDeviceData,array($_SESSION['deviceID'],$GLOBALS['rootDevelopment']));
					$_SESSION['installSourceCode']=($resDeviceDeviceData->RecordCount()>0)?1:0;
					
					$queryDeviceMD='SELECT * FROM Device WHERE FK_DeviceTemplate=? AND FK_Device_ControlledVia IS NOT NULL AND FK_Installation=?';
					$resDeviceMD=$dbADO->Execute($queryDeviceMD,array($GLOBALS['rootGenericMediaDirector'],$installationID));
					if($resDeviceMD->RecordCount()>0){
						$rowDeviceMD=$resDeviceMD->FetchRow();
						$_SESSION['coreHybridID']=$rowDeviceMD['PK_Device'];
					}
					
					$getDCERouter='SELECT * FROM Device WHERE FK_DeviceTemplate=? AND FK_Device_ControlledVia=?';
					$resDCERouter=$dbADO->Execute($getDCERouter,array($GLOBALS['rootDCERouter'],$_SESSION['deviceID']));
					if($resDCERouter->RecordCount()!=0){
						$rowDCERouter=$resDCERouter->FetchRow();
						$_SESSION['CoreDCERouter']=$rowDCERouter['PK_Device'];
					}
				}
				
				if(isset($_POST['computerType']) && $_POST['computerType']!='0'){
					$getRestrictions='SELECT * FROM DeviceTemplate WHERE PK_DeviceTemplate=?';
					$resRestrictions=$dbADO->Execute($getRestrictions,$_POST['computerType']);
					$rowRestrictions=$resRestrictions->FetchRow();
					$filterRestrictions=($rowRestrictions['FK_OperatingSystem']!='')?" AND FK_Operatingsystem='".$rowRestrictions['FK_OperatingSystem']."'":'';
					$filterRestrictions=($rowRestrictions['FK_Distro']!='')?" AND PK_Distro='".$rowRestrictions['FK_Distro']."'":$filterRestrictions;
				}

				$out .= '<br>
				   <form action="index.php" method="POST" name="wizard">
					<input type="hidden" name="section" value="wizard">
					<input type="hidden" name="step" value="'.$step.'">
					<input type="hidden" name="action" value="add">	
			      		<table align="left" border="0">
							 <tr>
								<td align="left" class="normaltext"><img src="images/install_wizard.gif"></td>
								<td align="right" class="normaltext"><a href="index.php?section=wizard&step=3">&lt;&lt; Previous</a> '.((isset($_SESSION['deviceID']))?'<a href="#" onClick="javascript:document.wizard.submit();">Next &gt;&gt;</a>':'').'</td>
							</tr>				
							<tr>
								<td colspan="2" align="center" bgcolor="#DADDE4"><B>Step 4 of 7: Pluto Core</B></td>
							</tr>
							<tr class="normaltext">
								<td colspan="2" align="left">Tell me about the computer you’ll use as the core.  Just choose "Generic PC" if we don’t yet have your specific model listed.  Pluto’s in-house developers and testers use Linux/Debian Sarge.  
								<b>At the moment, we have only tested with the single computer, hybrid configuration.  We recommend you check the \'Hybrid\' box 
								and just use the same computer for both the server and media play back for now.</b><br>&nbsp;<br></td>
							</tr>
							<tr class="normaltext">
								<td width="350">&nbsp;&nbsp;&nbsp;&nbsp;First, what type of computer is it?</td>
								<td><select name="computerType" onChange="showPlatforms()">
									<option value="0">- Please select -</option>';
							$queryCoreDeviceTemplates='SELECT * FROM DeviceTemplate WHERE FK_DeviceCategory=?';
							$resCoreDeviceTemplates=$dbADO->Execute($queryCoreDeviceTemplates,$GLOBALS['rootCoreID']);
							while($rowCoreDeviceTemplates=$resCoreDeviceTemplates->FetchRow()){
								$out.='<option value="'.$rowCoreDeviceTemplates['PK_DeviceTemplate'].'" '.(($rowCoreDeviceTemplates['PK_DeviceTemplate']==$FK_DeviceTemplate || $rowCoreDeviceTemplates['PK_DeviceTemplate']==$_POST['computerType'])?'selected':'').'>'.$rowCoreDeviceTemplates['Description'].'</option>';
							}
					$out.='		</td>
							</tr>				
							<tr class="normaltext">
								<td>&nbsp;&nbsp;&nbsp;&nbsp;Next, what platform will you use?</td>
								<td><select name="platform" onChange="enableHybrid()">
									<option value="0">-'.((isset($_POST['computerType']) && $_POST['computerType']!='0')?'Please select':'Select computer type first').'-</option>';
							if((isset($_POST['computerType']) && $_POST['computerType']!='0') || $FK_Distro!=''){
								$queryDistroOS='
									SELECT Distro.*, OperatingSystem.Description AS OS FROM Distro
										INNER JOIN OperatingSystem 
											ON FK_OperatingSystem=PK_OperatingSystem
									WHERE Distro.Core=1 '.@$filterRestrictions;
								$resDistroOS=$dbADO->Execute($queryDistroOS);
								while($row=$resDistroOS->FetchRow()){
									$out.='<option value="'.$row['PK_Distro'].'" '.(($row['PK_Distro']==$FK_Distro)?'selected':'').'>'.$row['Description'].' / '.$row['OS'].'</option>';
								}
								$out.='</select>';
							}
							if(isset($_SESSION['deviceID'])){
								$queryDHCP='SELECT * FROM Device_DeviceData WHERE FK_Device=? AND FK_DeviceData=?';
								$resDHCP=$dbADO->Execute($queryDHCP,array($_SESSION['deviceID'],$GLOBALS['DHCPDeviceData']));
								if($resDHCP->RecordCount()!=0){
									$_SESSION['EnableDHCP']=1;
									$rowDHCP=$resDHCP->FetchRow();
									$ipArray=explode('-',$rowDHCP['IK_DeviceData']);
									if(count($ipArray)==3)
										$_SESSION['plutoOnlyIP']=0;
									else
										$_SESSION['plutoOnlyIP']=1;
									$ipPartsArray=explode('.',$ipArray[0]);
								}else{ 	
									$_SESSION['plutoOnlyIP']=1;
									$_SESSION['EnableDHCP']=0;
								}

								$out.='<input type="hidden" name="oldDHCP" value="'.@$rowDHCP['IK_DeviceData'].'">';
							}
					$out.='		</td>
							</tr>
							<tr>
								<td colspan="2">&nbsp;</td>
							</tr>
							<tr class="normaltext">
								<td colspan="2"><input type="checkbox" name="installSourceCode" value="1" '.((!isset($_SESSION['deviceID']))?'disabled':'').' '.(($_SESSION['installSourceCode']==1)?'checked':'').'><span style="color:'.((!isset($_SESSION['deviceID']))?'#CCCCCC':'').'"> <b>Install Pluto’s source code</b> too.  This will also add any development libraries that are needed to compile.</span></td>
							</tr>
							<tr class="normaltext">
								<td colspan="2"><input type="checkbox" name="enableDHCP" value="1" '.((!isset($_SESSION['deviceID']))?'disabled':'').' '.(($_SESSION['EnableDHCP']==1)?'checked':'').' onClick="enableDHCPElements();"><span style="color:'.((!isset($_SESSION['deviceID']))?'#CCCCCC':'').'"> <b>Enable DHCP.  Base IP Address</b> <input type="text" name="ip_1" size="3" value="'.((isset($ipPartsArray[0]))?$ipPartsArray[0]:'192').'" '.(($_SESSION['EnableDHCP']!=1)?'disabled':'').'>.<input type="text" name="ip_2" size="3" value="'.((isset($ipPartsArray[1]))?$ipPartsArray[1]:'168').'" '.(($_SESSION['EnableDHCP']!=1)?'disabled':'').'>.<input type="text" name="ip_3" size="3" value="'.((isset($ipPartsArray[2]))?$ipPartsArray[2]:'1').'" '.(($_SESSION['EnableDHCP']!=1)?'disabled':'').'>.x</span>  
								<br><b>Note:</b>  If you want to use diskless media directors (step 5), or if you want Pluto to auto-configure your devices, you will need to check this box to enable DHCP.  Your Pluto Core can co-exist with your existing DHCP server if you leave the next option checked.  <a href="/support/index.php?section=document&docID=138" target="_blank">explain this</a>  All your Pluto devices will be given an IP address that starts with the 3 numbers above.  The Core will end with .1</td>
							</tr>					
							<tr class="normaltext">
								<td colspan="2"><input type="checkbox" name="plutoOnlyIP" value="1" '.((!isset($_SESSION['deviceID']) || @$_SESSION['EnableDHCP']!=1)?'disabled':'').' '.(($_SESSION['plutoOnlyIP']==1)?'checked':'').'><span style="color:'.((!isset($_SESSION['deviceID']))?'#CCCCCC':'').'"> <b>Provide IP\'s only to Pluto equipment</b></span>  
								This means your Pluto Core will only provide IP addresses to the media directors and other Pluto devices with a MAC Address in your Core\'s database.  
								If you already have a router that is giving out IP addresses (ie a DHCP server), leave this option checked and the Pluto Core will not interfere with it, or any of your existing computer.
								If you want the Pluto Core to be your primary DHCP server, uncheck the box. <a href="/support/index.php?section=document&docID=138" target="_blank">networking issues explained</a></td>
							</tr>					
							<tr class="normaltext">
								<td colspan="2" valign="top"><input type="checkbox" name="hybridCore" value="1" '.((!isset($_SESSION['deviceID']))?'disabled':'').' '.((isset($_SESSION['coreHybridID']) || isset($_SESSION['recreateHybrid']))?'checked':'').'><span style="color:'.((!isset($_SESSION['deviceID']))?'#CCCCCC':'').'"> <b>Hybrid:</b>  My core will also be a media director (ie a "hybrid").  If checked, the media playback modules will be installed on the core so you can hook it up directly to your tv/stereo and don’t need a separate computer for your media playback.</span></td>
							</tr>
							<tr>	
								<td colspan="2" class="normaltext">'.((!isset($_SESSION['deviceID']))?'&nbsp;':getInstallWizardDeviceTemplates($step,$dbADO,$_SESSION['CoreDCERouter'],$FK_Distro,$CoreOS)).'</td>
							</tr>	
							<tr>
								<td colspan="2" align="center"><input type="submit" name="continue" value="Next" '.((!isset($_SESSION['deviceID']))?'disabled':'').'></td>
							</tr>		
			      		</table>
					</form>
					<script>
					function enableDHCPElements()
					{
						if(document.wizard.enableDHCP.checked){
							document.wizard.plutoOnlyIP.disabled=false;
							document.wizard.ip_1.disabled=false;
							document.wizard.ip_2.disabled=false;
							document.wizard.ip_3.disabled=false;
						}else{
							document.wizard.plutoOnlyIP.disabled=true;
							document.wizard.ip_1.disabled=true;
							document.wizard.ip_2.disabled=true;
							document.wizard.ip_3.disabled=true;
						}
					}
					</script>
			      	';
			}else{
				// process form step 4 subsection 1
				$deviceTemplate=$_POST['computerType'];
				$distro=$_POST['platform'];
								
				if($action=='addDevice'){
					if(isset($_SESSION['deviceID'])){
						$updateDevice='UPDATE Device SET FK_DeviceTemplate=? WHERE PK_Device=?';
						$dbADO->Execute($updateDevice,array($deviceTemplate,$_SESSION['deviceID']));
						$updateDeviceDeviceData='UPDATE Device_DeviceData SET IK_DeviceData=? WHERE FK_Device=? AND FK_DeviceData=?';
						$dbADO->Execute($updateDeviceDeviceData,array($distro,$_SESSION['deviceID'],$GLOBALS['rootPK_Distro']));
						
						// install options - delete or insert devices
						$installOptionsArray=explode(',',$_POST['displayedTemplatesRequired_'.$_SESSION['CoreDCERouter']]);
						foreach($installOptionsArray AS $elem){
							$oldDevice=$_POST['device_'.$_SESSION['CoreDCERouter'].'_requiredTemplate_'.$elem];
							$dbADO->Execute("DELETE FROM Device WHERE PK_Device='".$oldDevice."'");
						}
						// the template was changed, I delete the hybrid, it will be recreated when click on "Next"
						if(isset($_SESSION['coreHybridID'])){
							deleteDevice($_SESSION['coreHybridID'],$dbADO);
							$_SESSION['recreateHybrid']=1;
							unset($_SESSION['coreHybridID']);
							unset($_SESSION['isHybridFirstTime']);
							unset($_SESSION['OrbiterHybridChild']);
						}
					}else{
						$insertDevice='INSERT INTO Device (Description, FK_DeviceTemplate, FK_Installation) VALUES (?,?,?)';
						$dbADO->Execute($insertDevice,array('CORE',$deviceTemplate,$installationID));
						$deviceID=$dbADO->Insert_ID();
						createChildsForControledViaDeviceTemplate($deviceTemplate,$installationID,$deviceID,$dbADO);
						createChildsForControledViaDeviceCategory($deviceTemplate,$installationID,$deviceID,$dbADO);
						
						$getDCERouter='SELECT * FROM Device WHERE FK_DeviceTemplate=? AND FK_Device_ControlledVia=?';
						$resDCERouter=$dbADO->Execute($getDCERouter,array($GLOBALS['rootDCERouter'],$deviceID));
						if($resDCERouter->RecordCount()!=0){
							$rowDCERouter=$resDCERouter->FetchRow();
							$_SESSION['CoreDCERouter']=$rowDCERouter['PK_Device'];
						}
						
						$insertDeviceDeviceData='INSERT INTO Device_DeviceData (FK_Device, FK_DeviceData,IK_DeviceData) VALUES (?,?,?)';
						$dbADO->Execute($insertDeviceDeviceData,array($deviceID,$GLOBALS['rootPK_Distro'],$distro));
						$_SESSION['deviceID']=$deviceID;
						$_SESSION['isCoreFirstTime']=1;
			
					}
					header("Location: index.php?section=wizard&step=4");
				}else{
					$installSourceCode=(isset($_POST['installSourceCode'])?$_POST['installSourceCode']:0);
					$oldInstallSourceCode=(isset($_SESSION['installSourceCode'])?$_SESSION['installSourceCode']:0);
					
					$enableDHCP=$_POST['enableDHCP'];
					$plutoOnlyIP=@$_POST['plutoOnlyIP'];
					$oldDHCP=(isset($_POST['oldDHCP']))?$_POST['oldDHCP']:'';
					if($enableDHCP==1){
						$ip_1=$_POST['ip_1'];
						$ip_2=$_POST['ip_2'];
						$ip_3=$_POST['ip_3'];
					}
					
					$IPtoDeviceDeviceData=($plutoOnlyIP==1)?"$ip_1.$ip_2.$ip_3.2-$ip_1.$ip_2.$ip_3.254":"$ip_1.$ip_2.$ip_3.2-$ip_1.$ip_2.$ip_3.128,$ip_1.$ip_2.$ip_3.129-$ip_1.$ip_2.$ip_3.254";
					if($enableDHCP==1){
						if($IPtoDeviceDeviceData!=$oldDHCP || $oldDHCP==''){
							$insertDHCP='INSERT INTO Device_DeviceData (FK_Device, FK_DeviceData,IK_DeviceData) VALUES (?,?,?)';
							$dbADO->Execute($insertDHCP,array($_SESSION['deviceID'],$GLOBALS['DHCPDeviceData'],$IPtoDeviceDeviceData));
						}else{
							$updateDHCP='UPDATE Device_DeviceData SET IK_DeviceData=? WHERE FK_Device=? AND FK_DeviceData=?';
							$dbADO->Execute($updateDHCP,array($IPtoDeviceDeviceData,$_SESSION['deviceID'],$GLOBALS['DHCPDeviceData']));
						}
						$updateDevice='UPDATE Device SET IPaddress=? WHERE PK_Device=?';
						$dbADO->Execute($updateDevice,array("$ip_1.$ip_2.$ip_3.1",$_SESSION['deviceID']));
					}else{
						$deleteDeviceDeviceData='DELETE FROM Device_DeviceData WHERE FK_Device=? AND FK_DeviceData=?';
						$dbADO->Execute($deleteDeviceDeviceData,array($_SESSION['deviceID'],$GLOBALS['DHCPDeviceData']));
					}
						
					
					if($oldInstallSourceCode!=$installSourceCode){
						$_SESSION['installSourceCode']=$installSourceCode;
						if($installSourceCode!=0){
							$insertDeviceDeviceData='INSERT INTO Device_DeviceData (FK_Device, FK_DeviceData,IK_DeviceData) VALUES (?,?,?)';
							$dbADO->Execute($insertDeviceDeviceData,array($_SESSION['deviceID'],$GLOBALS['rootDevelopment'],'1'));
						}else{
							$deleteDeviceDeviceData='DELETE FROM Device_DeviceData WHERE FK_Device=? AND FK_DeviceData=?';
							$dbADO->Execute($deleteDeviceDeviceData,array($_SESSION['deviceID'],$GLOBALS['rootDevelopment']));
						}
					}
					
					
					$oldHybridCore=(isset($_SESSION['coreHybridID'])?1:0);
					$hybridCore=(isset($_POST['hybridCore'])?1:0);
					$coreHybridID=$_SESSION['coreHybridID'];
					if($oldHybridCore==0 || @$_SESSION['recreateHybrid']==1){
						if($hybridCore!=0 || @$_SESSION['recreateHybrid']==1){
							$insertDeviceMD='INSERT INTO Device (Description, FK_DeviceTemplate, FK_Installation, FK_Device_ControlledVia) VALUES (?,?,?,?)';
							$dbADO->Execute($insertDeviceMD,array('The core/hybrid',$GLOBALS['rootGenericMediaDirector'],$installationID,$_SESSION['deviceID']));
							$_SESSION['coreHybridID']=$dbADO->Insert_ID();
							
							// inherit DeviceData for hybrid MD
							InheritDeviceData($FK_DeviceTemplate,$insertID,$dbADO);
							createChildsForControledViaDeviceTemplate($GLOBALS['rootGenericMediaDirector'],$_SESSION['installationID'],$_SESSION['coreHybridID'],$dbADO);
							createChildsForControledViaDeviceCategory($GLOBALS['rootGenericMediaDirector'],$_SESSION['installationID'],$_SESSION['coreHybridID'],$dbADO);

							// get PK_Device for Orbiter child to Hybrid
							$getOrbiterHybridChild='SELECT * FROM Device WHERE FK_DeviceTemplate=? AND FK_Device_ControlledVia=?';
							$resOrbiterHybridChild=$dbADO->Execute($getOrbiterHybridChild,array($GLOBALS['deviceTemplateOrbiter'],$_SESSION['coreHybridID']));
							if($resOrbiterHybridChild->RecordCount()!=0){
								$rowOrbiterHybridChild=$resOrbiterHybridChild->FetchRow();
								$_SESSION['OrbiterHybridChild']=$rowOrbiterHybridChild['PK_Device'];
							}
							
							$insertDeviceMDDeviceData='INSERT INTO Device_DeviceData (FK_Device, FK_DeviceData,IK_DeviceData) VALUES (?,?,?)';
							$dbADO->Execute($insertDeviceMDDeviceData,array($_SESSION['coreHybridID'],$GLOBALS['rootPK_Distro'],$distro));
							$_SESSION['isHybridFirstTime']=1;
							unset($_SESSION['recreateHybrid']);
						}
					}elseif($hybridCore==0){
						deleteDevice($coreHybridID,$dbADO);
						unset($_SESSION['coreHybridID']);
						unset($_SESSION['isHybridFirstTime']);
						unset($_SESSION['OrbiterHybridChild']);
					}

					// install options - delete or insert devices
					$installOptionsArray=explode(',',$_POST['displayedTemplatesRequired_'.$_SESSION['CoreDCERouter']]);
					foreach($installOptionsArray AS $elem){
						$optionalDevice=(isset($_POST['device_'.$_SESSION['CoreDCERouter'].'_requiredTemplate_'.$elem]))?$_POST['device_'.$_SESSION['CoreDCERouter'].'_requiredTemplate_'.$elem]:0;
						$oldDevice=$_POST['oldDevice_'.$_SESSION['CoreDCERouter'].'_requiredTemplate_'.$elem];
						if($optionalDevice!=0){
							$OptionalDeviceName=cleanString(@$_POST['templateName_'.$elem]);
							$insertDevice='
									INSERT INTO Device 
										(Description, FK_DeviceTemplate, FK_Installation, FK_Device_ControlledVia) 
									VALUES (?,?,?,?)';
							if($oldDevice=='')
								$dbADO->Execute($insertDevice,array($OptionalDeviceName,$elem,$installationID,$_SESSION['CoreDCERouter']));
						}else{
							$dbADO->Execute("DELETE FROM Device WHERE PK_Device='".$oldDevice."'");
						}
					}
					
					header("Location: index.php?section=wizard&step=5");
				}
			}
		break;
		case 5:
				$queryCoreDeviceTemplates='SELECT * FROM DeviceTemplate WHERE FK_DeviceCategory=?';
				$resCoreDeviceTemplates=$dbADO->Execute($queryCoreDeviceTemplates,$GLOBALS['rootMediaDirectorID']);
				$deviceTemplateIdArray=array();
				$deviceTemplateDescriptionArray=array();
				while($rowCoreDeviceTemplates=$resCoreDeviceTemplates->FetchRow()){
					$deviceTemplateDescriptionArray[]=$rowCoreDeviceTemplates['Description'];
					$deviceTemplateIdArray[]=$rowCoreDeviceTemplates['PK_DeviceTemplate'];
				}
				
				if(isset($_POST['newType']) && $_POST['newType']!='0'){
					$getRestrictions='SELECT * FROM DeviceTemplate WHERE PK_DeviceTemplate=?';
					$resRestrictions=$dbADO->Execute($getRestrictions,$_POST['newType']);
					$rowRestrictions=$resRestrictions->FetchRow();
					$filterRestrictions=($rowRestrictions['FK_OperatingSystem']!='')?" AND FK_Operatingsystem='".$rowRestrictions['FK_OperatingSystem']."'":'';
					$filterRestrictions=($rowRestrictions['FK_Distro']!='')?" AND PK_Distro='".$rowRestrictions['FK_Distro']."'":$filterRestrictions;
				}

				$queryDistroOS='
					SELECT Distro.*, OperatingSystem.Description AS OS FROM Distro
						INNER JOIN OperatingSystem 
							ON FK_OperatingSystem=PK_OperatingSystem
					WHERE Distro.MediaDirector=1 '.$filterRestrictions;
				$resDistroOS=$dbADO->Execute($queryDistroOS);
				$distroIdArray=array();
				$distroDescriptionArray=array();
				while($row=$resDistroOS->FetchRow()){
					$distroDescriptionArray[]=$row['Description']." / ".$row['OS'];
					$distroIdArray[]=$row['PK_Distro'];
				}
				$displayedDevicesArray=explode(',',@$_POST['displayedDevices']);
				
				if($action=='form'){
				// display form
			
			
				foreach($displayedDevicesArray as $value){
					if(isset($_POST['delete_'.$value])){
						getDeviceChildsArray($value,$dbADO);
						$toDelete = cleanArray($GLOBALS['childsArray']);
						$toDelete[]=$value;
						if (!is_array($toDelete)) {
							$toDelete=array();
						}
						foreach ($toDelete as $elem) {
							$queryDelData = 'DELETE FROM Device_DeviceData  WHERE FK_Device = '.$elem;
							$queryDelDevice = 'DELETE FROM Device WHERE PK_Device = '.$elem;
							$dbADO->Execute($queryDelData);
							$dbADO->Execute($queryDelDevice);
						}
					}
				}
				
				$action=(isset($_POST['newPlatform']) && isset($_POST['newPlatform'])!='0')?'add':$action;
				$out = '
				<script>
				function showOptions()
				{
					if(document.wizard.newType.selectedIndex!=0 && document.wizard.newPlatform.selectedIndex!=0)
					{
						document.wizard.action.value="form";
						document.wizard.submit();
					}
				}
				function showPlatforms()
				{
					document.wizard.action.value="form";
					document.wizard.submit();
				}
				</script>
					<br>
				   <form action="index.php" method="POST" name="wizard">
					<input type="hidden" name="section" value="wizard">
					<input type="hidden" name="step" value="'.$step.'">
					<input type="hidden" name="action" value="'.$action.'">	
			      		<table align="center" width="100%">
							 <tr>
								<td align="left" class="normaltext"><img src="images/install_wizard.gif"></td>
								<td align="right" class="normaltext"><a href="index.php?section=wizard&step=4">&lt;&lt; Previous</a> <a href="#" onClick="javascript:document.wizard.action.value=\'update\';document.wizard.submit();">Next &gt;&gt;</a></td>
							</tr>				
							<tr>
								<td colspan="2" align="center" bgcolor="#DADDE4"><B>Step 5 of 7: Media Directors</B></td>
							</tr>
							<tr>
								<td colspan="2" align="left" class="normaltext">Here are the media directors in this installation that will connect to your Core.  You can add more by filling in the \'Add a Media Director\' section.  Be sure to give them descriptive names like \'Master Bedroom\'.</td>
							</tr>
							<tr>
								<td colspan="2">
								<table align="center">
									<tr bgcolor="#DADDE4" class="normaltext">
										<td><B>Name</B></td>
										<td><B>Type</B></td>
										<td><B>Platform</B></td>
										<td><B>Install source code</B></td>
										<td><B>Diskless boot</B></td>
										<td>&nbsp;</td>
									</tr>';
					$queryMediaDirectors='
						SELECT * FROM Device WHERE FK_DeviceTemplate=? AND FK_Installation=?';
					$resMediaDirectors=$dbADO->Execute($queryMediaDirectors,array($GLOBALS['rootGenericMediaDirector'],$installationID));
					$displayedDevices=array();
					if($resMediaDirectors->RecordCount()==0){
						$out.='		<tr class="normaltext">
										<td colspan="6" align="center">No Media Directors</td>
									</tr>';
					}
					$counter=0;
					while($rowMediaDirectors=$resMediaDirectors->FetchRow()){
						$displayedDevices[]=$rowMediaDirectors['PK_Device'];
						$counter++;
						if( $counter % 2==1 )
							$rowcolor = 'bgcolor="#F0F3F8"';
						else
							$rowcolor = '';
							
						$out.='
									<tr '.$rowcolor.'>
										<td><input type="text" name="description_'.$rowMediaDirectors['PK_Device'].'" value="'.$rowMediaDirectors['Description'].'"></td>';
										
								$queryDeviceDeviceData='SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device=? AND FK_DeviceData=?';
								$resDeviceDeviceData=$dbADO->Execute($queryDeviceDeviceData,array($rowMediaDirectors['PK_Device'],$GLOBALS['rootPK_Distro']));
								if($resDeviceDeviceData->RecordCount()>0){
									$row=$resDeviceDeviceData->FetchRow();
									$selectedDistro=$row['IK_DeviceData'];
								}

						if( $rowMediaDirectors['FK_Device_ControlledVia']=='' ) {
								$out.='<td><select name="deviceTemplate_'.$rowMediaDirectors['PK_Device'].'">';
								foreach ($deviceTemplateIdArray as $key => $value){
									$out.='<option value="'.$value.'" '.(($rowMediaDirectors['FK_DeviceTemplate']==$value)?'selected':'').'>'.$deviceTemplateDescriptionArray[$key].'</option>';
								}
								$out.='	</select>
										</td>
										<td><select name="platform_'.$rowMediaDirectors['PK_Device'].'">';
								foreach ($distroIdArray as $key => $value){
									$out.='<option value="'.$value.'" '.(($value==@$selectedDistro)?'selected':'').'>'.$distroDescriptionArray[$key].'</option>';
								}
								$queryInstallSourceCode='SELECT * FROM Device_DeviceData WHERE FK_Device=? AND FK_DeviceData=?';
								$resInstallSourceCode=$dbADO->Execute($queryInstallSourceCode,array($rowMediaDirectors['PK_Device'],$GLOBALS['rootDevelopment']));
								
								$queryDiskless='SELECT * FROM Device_DeviceData WHERE FK_Device=? AND FK_DeviceData=?';
								$resDiskless=$dbADO->Execute($queryDiskless,array($rowMediaDirectors['PK_Device'],$GLOBALS['rootDisklessBoot']));
				
								$out.='</select>';
									$out .='<input type="hidden" name="oldInstallSourceCode_'.$rowMediaDirectors['PK_Device'].'" value="'.(($resInstallSourceCode->RecordCount()>0)?'1':'0').'">
										<input type="hidden" name="oldDisklessBoot_'.$rowMediaDirectors['PK_Device'].'" value="'.(($resDiskless->RecordCount()>0)?'1':'0').'">
										</td>
										<td align="center"><input type="checkbox" name="installSourceCode_'.$rowMediaDirectors['PK_Device'].'" '.(($resInstallSourceCode->RecordCount()>0)?'checked':'').' value="1"></td>
										<td  align="center"><input type="checkbox" name="disklessBoot_'.$rowMediaDirectors['PK_Device'].'" '.(($resDiskless->RecordCount()>0)?'checked':'').' value="1" onClick="showMAC(\''.$rowMediaDirectors['PK_Device'].'\');"><span id="MACBox_'.$rowMediaDirectors['PK_Device'].'" name="MACBox_'.$rowMediaDirectors['PK_Device'].'" style="display:'.(($resDiskless->RecordCount()>0)?'':'none').'" class="normaltext">MAC Address: <input type="text" name="mdMAC_'.$rowMediaDirectors['PK_Device'].'" value="'.$rowMediaDirectors['MACaddress'].'"></span></td>
										<td class="normaltext">'.'<input type="submit" name="delete_'.$rowMediaDirectors['PK_Device'].'" value="Delete"></td>
									</tr>';
							}
							else {
								$out .='<input type="hidden" name="platform_'.$rowMediaDirectors['PK_Device'].'" value="'.@$selectedDistro.'">
										<input type="hidden" name="deviceTemplate_'.$rowMediaDirectors['PK_Device'].'" value="'.$GLOBALS['rootGenericMediaDirector'].'">
									<td colspan="5" class="normaltext">This is your hybrid--the media director software will be running on the same computer as the core.  Choose the media director-related software modules you want on this hybrid.  To change the core-related software modules or other options, return to the <a href="index.php?section=wizard&step=4">Core page</a>.  You can add more stand-alone media directors below that will connect to this hybrid.</td>
								</tr>';
							}
							$orbiterMDChild=getMediaDirectorOrbiterChild($rowMediaDirectors['PK_Device'],$dbADO);
							if($orbiterMDChild){
								$out.='
									<tr '.$rowcolor.'>	
										<td colspan="6">'.getInstallWizardDeviceTemplates($step,$dbADO,$orbiterMDChild,$selectedDistro).'</td>
									</tr>';
							}
					}
					$out.='<input type="hidden" name="displayedDevices" value="'.join(',',$displayedDevices).'">';
					$out.='		</table>
								</td>
							</tr>';
					if(count($displayedDevices)>0){
						$out.='
							<tr class="normaltext">
								<td colspan="2" align="center"><input type="button" name="continue" value="Save changes" onClick="javascript:document.wizard.action.value=\'updateOnly\';document.wizard.submit();"></td>
							</tr>';
					}
					$out.='
							<tr class="normaltext">
								<td colspan="2"><B>Add a Media Director</B></td>
							</tr>
							<tr class="normaltext">
								<td>Name</td>
								<td><input type="text" name="Description" value="'.@$_POST['Description'].'"></td>
							</tr>
							<tr class="normaltext">
								<td>Type</td>
								<td><select name="newType" onChange="showPlatforms();">
									<option value="0">-Please select-</option>';
								foreach ($deviceTemplateIdArray as $key => $value){
									$out.='<option value="'.$value.'" '.(($value==@$_POST['newType'])?'selected':'').'>'.$deviceTemplateDescriptionArray[$key].'</option>';
								}
								$out.='	</select></td>
							</tr>
							<tr class="normaltext">
								<td>Platform</td>
								<td><select name="newPlatform" onChange="showOptions();">
									<option value="0">-'.((isset($_POST['newType']))?'Please select':'Select type first').'-</option>';
								if(isset($_POST['newType'])){
									foreach ($distroIdArray as $key => $value){
										$out.='<option value="'.$value.'" '.(($value==@$_POST['newPlatform'])?'selected':'').'>'.$distroDescriptionArray[$key].'</option>';
									}
								}
								$out.='</select></td>
							</tr>
							<tr class="normaltext">
								<td>Install Source Code</td>
								<td><input type="checkbox" name="installSourceCode" value="1" '.((@$_POST['installSourceCode']==1)?'checked':'').'></td>
							</tr>
							<tr class="normaltext">
								<td>Diskless boot</td>
								<td><input type="checkbox" name="disklessBoot_" value="1" '.((@$_POST['disklessBoot_']==1)?'checked':'').' onClick="showMAC(\'\');"> <span id="MACBox_" name="MACBox_" style="display:'.((@$_POST['disklessBoot_']==1)?'':'none').'">MAC Address: <input type="text" name="mdMAC" value=""></span></td>
							</tr>
							<tr class="normaltext">
								<td colspan="2">Diskless, or network boot, means the media director does not need a hard drive.  It will boot off an image stored on the core.  
								You can have a hard drive in it anyway and use it to boot another operating system.  
								Then just press a button on the Pluto Orbiter to switch a media director between a network boot as a Pluto system, 
								and a normal boot on the hard drive.  <b>Note</b>:  To do a network boot, you must (a) have a computer that supports it, (b) enable network boot in the BIOS settings,
								 and (c) specify the MAC Address in the box above in the format aa:bb:cc:dd:ee:ff  <a href="/support/index.php?section=document&docID=138" target="_blank">explain this</a></td>
							</tr>';
					if(isset($_POST['newPlatform']))
						$out.='<tr class="normaltext">
								<td colspan="2">'.getInstallWizardDeviceTemplates($step,$dbADO,'',@$_POST['newPlatform']).'</td>
							</tr>';
					$out.='	<tr>
								<td colspan="2" align="center">'.((isset($_POST['newPlatform']) && $_POST['newPlatform']!='0')?'<input type="submit" name="continue" value="Add&Continue">':'').' '.((isset($_POST['newPlatform']) && $_POST['newPlatform']!='0')?'<input type="button" name="cancel" value="Cancel" onClick="self.location=\'index.php?section=wizard&step=5\'">':'<input type="button" name="next" value="Next" onClick="javascript:document.wizard.action.value=\'update\';document.wizard.submit();">').'</td>
							</tr>
			      		</table>
					</form>
					<script>
					function showMAC(deviceID)
					{
						isDiskless=eval("document.wizard.disklessBoot_"+deviceID+".checked");
						if(isDiskless)
							eval("document.getElementById(\'MACBox_"+deviceID+"\').style.display=\'\';")
						else
							eval("document.getElementById(\'MACBox_"+deviceID+"\').style.display=\'none\';")
					}
					</script>
					';
					if(isset($_POST['newPlatform']) && $_POST['newPlatform']!='0'){
						$out.='
				 	<script>
		 				var frmvalidator = new formValidator("wizard");
 						frmvalidator.addValidation("Description","req","Please enter a description.");
					</script>';
					}
				
			}elseif($action=='add'){
				// process form step 5
				if(isset($_POST['continue'])){
					$description=$_POST['Description'];
					$FK_DeviceTemplate=$_POST['newType'];
					$FK_Distro=$_POST['newPlatform'];
					$installSourceCode=(isset($_POST['installSourceCode']))?$_POST['installSourceCode']:0;
					$diskless=(isset($_POST['disklessBoot_']))?$_POST['disklessBoot_']:0;
					$macAddress=($diskless!=0)?$_POST['mdMAC']:NULL;

					$insertDevice='INSERT INTO Device (Description, FK_DeviceTemplate, FK_Installation,MACaddress) VALUES (?,?,?,?)';
					$dbADO->Execute($insertDevice,array($description,$FK_DeviceTemplate,$_SESSION['installationID'],$macAddress));
					$insertID=$dbADO->Insert_ID();
					InheritDeviceData($FK_DeviceTemplate,$insertID,$dbADO);
					createChildsForControledViaDeviceTemplate($FK_DeviceTemplate,$_SESSION['installationID'],$insertID,$dbADO);
					createChildsForControledViaDeviceCategory($FK_DeviceTemplate,$_SESSION['installationID'],$insertID,$dbADO);
					
					// install options 
					$orbiterMDChild=getMediaDirectorOrbiterChild($insertID,$dbADO);
					$installOptionsArray=explode(',',$_POST['displayedTemplatesRequired_']);
					foreach($installOptionsArray AS $elem){
						$optionalDevice=(isset($_POST['device__requiredTemplate_'.$elem]))?$_POST['device__requiredTemplate_'.$elem]:0;
						if($optionalDevice!=0){
							$OptionalDeviceName=cleanString(@$_POST['templateName_'.$elem]);
							$insertDevice='
									INSERT INTO Device 
										(Description, FK_DeviceTemplate, FK_Installation, FK_Device_ControlledVia) 
									VALUES (?,?,?,?)';
							$dbADO->Execute($insertDevice,array($OptionalDeviceName,$elem,$installationID,$orbiterMDChild));
						}
					}
										
					$insertDeviceDeviceData='INSERT INTO Device_DeviceData (FK_Device,FK_DeviceData,IK_DeviceData) VALUES (?,?,?)';
					$dbADO->Execute($insertDeviceDeviceData,array($insertID,$GLOBALS['rootPK_Distro'],$FK_Distro));
					
					if($installSourceCode!=0){
						$insertDeviceDeviceData='INSERT INTO Device_DeviceData (FK_Device,FK_DeviceData,IK_DeviceData) VALUES (?,?,?)';
						$dbADO->Execute($insertDeviceDeviceData,array($insertID,$GLOBALS['rootDevelopment'],1));
					}

					if($diskless!=0){
						$insertDeviceDeviceData='INSERT INTO Device_DeviceData (FK_Device,FK_DeviceData,IK_DeviceData) VALUES (?,?,?)';
						$dbADO->Execute($insertDeviceDeviceData,array($insertID,$GLOBALS['rootDisklessBoot'],1));
					}
				}
				header("Location: index.php?section=wizard&step=5");
			}else{
			// action =update and jump to next step

				foreach($displayedDevicesArray as $value){
					$description=@$_POST['description_'.$value];
					$FK_DeviceTemplate=@$_POST['deviceTemplate_'.$value];
					$FK_Distro=@$_POST['platform_'.$value];
					$oldInstallSourceCode=@$_POST['oldInstallSourceCode_'.$value];
					$oldDisklessBoot=@$_POST['oldDisklessBoot_'.$value];
					$installSourceCode=(isset($_POST['installSourceCode_'.$value]))?$_POST['installSourceCode_'.$value]:0;
					$diskless=(isset($_POST['disklessBoot_'.$value]))?$_POST['disklessBoot_'.$value]:0;
					$macAddress=($diskless!=0)?$_POST['mdMAC_'.$value]:NULL;
	
					$updateDevice='UPDATE Device SET Description=?, FK_DeviceTemplate=?,MACaddress=? WHERE PK_Device=?';
					$dbADO->Execute($updateDevice,array($description,$FK_DeviceTemplate,$macAddress,$value));
	
					$updateDeviceDeviceData='UPDATE Device_DeviceData SET IK_DeviceData=? WHERE FK_Device=? AND FK_DeviceData=?';
					$dbADO->Execute($updateDeviceDeviceData,array($FK_Distro,$value,$GLOBALS['rootPK_Distro']));
					if($installSourceCode!=$oldInstallSourceCode){
						if($installSourceCode==1){
							$insertDeviceDeviceData='INSERT INTO Device_DeviceData (FK_Device,FK_DeviceData,IK_DeviceData) VALUES (?,?,?)';
							$dbADO->Execute($insertDeviceDeviceData,array($value,$GLOBALS['rootDevelopment'],1));
						}else{
							$deleteDeviceDeviceData='DELETE FROM Device_DeviceData WHERE FK_Device=? AND FK_DeviceData=?';
							$dbADO->Execute($deleteDeviceDeviceData,array($value,$GLOBALS['rootDevelopment']));
						}
					}
	
					if($oldDisklessBoot!=$diskless){
						if($diskless==1){
							$insertDeviceDeviceData='INSERT INTO Device_DeviceData (FK_Device,FK_DeviceData,IK_DeviceData) VALUES (?,?,?)';
							$dbADO->Execute($insertDeviceDeviceData,array($value,$GLOBALS['rootDisklessBoot'],1));
						}else{
							$deleteDeviceDeviceData='DELETE FROM Device_DeviceData WHERE FK_Device=? AND FK_DeviceData=?';
							$dbADO->Execute($deleteDeviceDeviceData,array($value,$GLOBALS['rootDisklessBoot']));
						}
					}
					
					// install options - delete or insert devices
					$orbiterMDChild=getMediaDirectorOrbiterChild($value,$dbADO);
					$installOptionsArray=explode(',',$_POST['displayedTemplatesRequired_'.$orbiterMDChild]);
					foreach($installOptionsArray AS $elem){
						$oldDevice=$_POST['oldDevice_'.$orbiterMDChild.'_requiredTemplate_'.$elem];
						$optionalDevice=(isset($_POST['device_'.$orbiterMDChild.'_requiredTemplate_'.$elem]))?$_POST['device_'.$orbiterMDChild.'_requiredTemplate_'.$elem]:0;
						if($optionalDevice!=0){
							$OptionalDeviceName=cleanString(@$_POST['templateName_'.$elem]);
							$insertDevice='
									INSERT INTO Device 
										(Description, FK_DeviceTemplate, FK_Installation, FK_Device_ControlledVia) 
									VALUES (?,?,?,?)';
							if($oldDevice=='')
								$dbADO->Execute($insertDevice,array($OptionalDeviceName,$elem,$installationID,$orbiterMDChild));
						}else{
							$dbADO->Execute("DELETE FROM Device WHERE PK_Device='".$oldDevice."'");
						}
					}
				}
			header("Location: index.php?section=wizard&step=".(($action=='updateOnly')?'5':'6'));
			}
		break;
		case 6:
				$installationID=(isset($_SESSION['installationID']))?$_SESSION['installationID']:0;
				
				$validOrbiters = getValidOrbitersArray($installationID,$dbADO);
				$queryOrbiters='SELECT * FROM DeviceTemplate WHERE PK_DeviceTemplate IN ('.join(',',$validOrbiters).')';
				$resOrbiters=$dbADO->Execute($queryOrbiters);
				$deviceTemplateIdArray=array();
				$deviceTemplateDescriptionArray=array();
				while($rowOrbiters=$resOrbiters->FetchRow()){
					$deviceTemplateDescriptionArray[]=$rowOrbiters['Description'];
					$deviceTemplateIdArray[]=$rowOrbiters['PK_DeviceTemplate'];
				}

				if(isset($_POST['newType']) && $_POST['newType']!='0'){
					$getRestrictions='SELECT * FROM DeviceTemplate WHERE PK_DeviceTemplate=?';
					$resRestrictions=$dbADO->Execute($getRestrictions,$_POST['newType']);
					$rowRestrictions=$resRestrictions->FetchRow();
					$filterRestrictions=($rowRestrictions['FK_OperatingSystem']!='')?" AND FK_Operatingsystem='".$rowRestrictions['FK_OperatingSystem']."'":'';
					$filterRestrictions=($rowRestrictions['FK_Distro']!='')?" AND PK_Distro='".$rowRestrictions['FK_Distro']."'":$filterRestrictions;
				}
				
				$queryDistroOS='
					SELECT Distro.*, OperatingSystem.Description AS OS FROM Distro
						INNER JOIN OperatingSystem 
							ON FK_OperatingSystem=PK_OperatingSystem
					WHERE Distro.Orbiter=1 '.$filterRestrictions;
				$resDistroOS=$dbADO->Execute($queryDistroOS);
				$distroIdArray=array();
				$distroDescriptionArray=array();
				while($row=$resDistroOS->FetchRow()){
					$distroDescriptionArray[]=$row['Description']." / ".$row['OS'];
					$distroIdArray[]=$row['PK_Distro'];
				}

				
				$displayedDevicesArray=explode(',',@$_POST['displayedDevices']);
				
		
				foreach($displayedDevicesArray as $value){
					if(isset($_POST['delete_'.$value])){
						getDeviceChildsArray($value,$dbADO);
						$toDelete = cleanArray($GLOBALS['childsArray']);
						$toDelete[]=$value;
						if (!is_array($toDelete)) {
							$toDelete=array();
						}
						foreach ($toDelete as $elem) {
							$queryDelData = 'DELETE FROM Device_DeviceData  WHERE FK_Device = '.$elem;
							$queryDelDevice = 'DELETE FROM Device WHERE PK_Device = '.$elem;
							$dbADO->Execute($queryDelData);
							$dbADO->Execute($queryDelDevice);
						}
					}
				}
				
				if($action=='form'){
				// display form
				$out = '
				<script>
				function showOptions()
				{
					if(document.wizard.newType.selectedIndex!=0 && document.wizard.newPlatform.selectedIndex!=0)
					{
						document.wizard.action.value="form";
						document.wizard.submit();
					}
				}
				function showPlatforms()
				{
					document.wizard.action.value="form";
					document.wizard.submit();
				}
				</script>';
				$action=(isset($_POST['newPlatform']) && isset($_POST['newPlatform'])!='')?'add':$action;
				$out.='
					<br>
				   <form action="index.php" method="POST" name="wizard">
					<input type="hidden" name="section" value="wizard">
					<input type="hidden" name="step" value="'.$step.'">
					<input type="hidden" name="action" value="'.$action.'">	
			      		<table align="center" width="100%">
							 <tr>
								<td align="left" class="normaltext"><img src="images/install_wizard.gif"></td>
								<td align="right" class="normaltext"><a href="index.php?section=wizard&step=5">&lt;&lt; Previous</a> <a href="#" onClick="javascript:document.wizard.action.value=\'update\';document.wizard.submit();">Next &gt;&gt;</a></td>
							</tr>				
							<tr>
								<td colspan="2" align="center" bgcolor="#DADDE4"><B>Step 6 of 7: Orbiter</B></td>
							</tr>
							<tr class="normaltext">
								<td colspan="2" align="left">Here are your orbiters.  The Media Directors will have an Orbiter on the screen already to control the system.  But you can add more like Web Pads, PDAs, mobile phones, etc.  For the type just choose "Orbiter" unless you see another type that more closely describes your hardware.</td>
							</tr>
							<tr>
								<td colspan="2">
								<table align="center">
									<tr class="normaltext" bgcolor="#DADDE4">
										<td><B>Name</B></td>
										<td><B>Type</B></td>
										<td><B>Platform</B></td>
										<td><B>Install source code</B></td>
										<td>&nbsp;</td>
									</tr>';
					$queryOrbiters='
						SELECT * FROM Device WHERE FK_DeviceTemplate IN ('.join(',',$validOrbiters).') AND FK_Installation=?  AND FK_Device_ControlledVia IS NULL';
					$resOrbiters=$dbADO->Execute($queryOrbiters,$installationID);
					$displayedDevices=array();
					if($resOrbiters->RecordCount()==0){
						$out.='		<tr class="normaltext">
										<td colspan="6" align="center">No Orbiters</td>
									</tr>';
					}					
					while($rowOrbiters=$resOrbiters->FetchRow()){
						$displayedDevices[]=$rowOrbiters['PK_Device'];
						
						$out.='
									<tr>
										<td><input type="text" name="description_'.$rowOrbiters['PK_Device'].'" value="'.$rowOrbiters['Description'].'"></td>
										<td><select name="deviceTemplate_'.$rowOrbiters['PK_Device'].'">';
								foreach ($deviceTemplateIdArray as $key => $value){
									$out.='<option value="'.$value.'" '.(($rowOrbiters['FK_DeviceTemplate']==$value)?'selected':'').'>'.$deviceTemplateDescriptionArray[$key].'</option>';
								}
								$out.='	</select>
										</td>
										<td><select name="platform_'.$rowOrbiters['PK_Device'].'">
												<option value="0"></option>';
								$queryDeviceDeviceData='SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device=? AND FK_DeviceData=?';
								
								$resDeviceDeviceData=$dbADO->Execute($queryDeviceDeviceData,array($rowOrbiters['PK_Device'],$GLOBALS['rootPK_Distro']));
								
								if($resDeviceDeviceData->RecordCount()>0){
									$row=$resDeviceDeviceData->FetchRow();
									$selectedDistro=$row['IK_DeviceData'];
								}
								foreach ($distroIdArray as $key => $value){
									$out.='<option value="'.$value.'" '.(($value==@$selectedDistro)?'selected':'').'>'.$distroDescriptionArray[$key].'</option>';
								}
								$queryInstallSourceCode='SELECT * FROM Device_DeviceData WHERE FK_Device=? AND FK_DeviceData=?';
								$resInstallSourceCode=$dbADO->Execute($queryInstallSourceCode,array($rowOrbiters['PK_Device'],$GLOBALS['rootDevelopment']));
								
								$out.='</select>
										<input type="hidden" name="oldInstallSourceCode_'.$rowOrbiters['PK_Device'].'" value="'.(($resInstallSourceCode->RecordCount()>0)?'1':'0').'">
										</td>
										<td align="center"><input type="checkbox" name="installSourceCode_'.$rowOrbiters['PK_Device'].'" '.(($resInstallSourceCode->RecordCount()>0)?'checked':'').' value="1"></td>
										<td><input type="submit" name="delete_'.$rowOrbiters['PK_Device'].'" value="Delete"></td>
									</tr>
									<tr>	
										<td colspan="6">'.getInstallWizardDeviceTemplates($step,$dbADO,$rowOrbiters['PK_Device'],$selectedDistro).'</td>
									</tr>';
					}
					$out.='<input type="hidden" name="displayedDevices" value="'.join(',',$displayedDevices).'">';
					$out.='		</table>
								</td>
							</tr>
							<tr>
								<td colspan="2" class="normaltext"><B>Add an Orbiter</B></td>
							</tr>
							<tr class="normaltext">
								<td>Name</td>
								<td><input type="text" name="Description" value="'.@$_POST['Description'].'"></td>
							</tr>
							<tr class="normaltext">
								<td>Type</td>
								<td><select name="newType" onChange="showPlatforms();">
									<option value="0">-Please select-</option>';
								foreach ($deviceTemplateIdArray as $key => $value){
									$out.='<option value="'.$value.'" '.(($value==@$_POST['newType'])?'selected':'').'>'.$deviceTemplateDescriptionArray[$key].'</option>';
								}
								$out.='	</select></td>
							</tr>
							<tr class="normaltext">
								<td>Platform</td>
								<td><select name="newPlatform" onChange="showOptions();">
									<option value="0">-'.((isset($_POST['newType']))?'Please select':'Select type first').'-</option>';
								foreach ($distroIdArray as $key => $value){
									$out.='<option value="'.$value.'" '.(($value==@$_POST['newPlatform'])?'selected':'').'>'.$distroDescriptionArray[$key].'</option>';
								}
								$out.='</select></td>
							</tr>
							<tr class="normaltext">
								<td>Install Source Code</td>
								<td><input type="checkbox" name="installSourceCode" value="1" '.((@$_POST['installSourceCode']==1)?'checked':'').'></td>
							</tr>';
					if(isset($_POST['newPlatform']) && $_POST['newPlatform']!='0')
						$out.='<tr>
								<td colspan="2">'.getInstallWizardDeviceTemplates($step,$dbADO,'',@$_POST['newPlatform']).'</td>
							</tr>';
					$out.='	<tr>
								<td colspan="2" align="center">'.((isset($_POST['newPlatform']) && $_POST['newPlatform']!='0')?'<input type="submit" name="continue" value="Add&Continue"> <input type="button" name="cancel" value="Cancel" onClick="self.location=\'index.php?section=wizard&step=6\'">':'').'</td>
							</tr>
							<tr>
								<td colspan="2" align="center" class="normaltext">If you don’t need another orbiter, just click next.<br><input type="button" name="next" value="Next" onClick="javascript:document.wizard.action.value=\'update\';document.wizard.submit();"></td>
							</tr>				
			      		</table>
					</form>';
					if(isset($_POST['newPlatform']) && $_POST['newPlatform']!='0'){
						$out.='
				 	<script>
		 				var frmvalidator = new formValidator("wizard");
 						frmvalidator.addValidation("Description","req","Please enter a description.");
					</script>';
					}
			
			}elseif($action=='add'){
				// process form step 6
				if(isset($_POST['continue'])){
					$description=$_POST['Description'];
					$FK_DeviceTemplate=$_POST['newType'];
					$FK_Distro=$_POST['newPlatform'];
					$installSourceCode=(isset($_POST['installSourceCode']))?$_POST['installSourceCode']:0;
					$diskless=(isset($_POST['disklessBoot']))?$_POST['disklessBoot']:0;

					$insertDevice='INSERT INTO Device (Description, FK_DeviceTemplate, FK_Installation) VALUES (?,?,?)';
					$dbADO->Execute($insertDevice,array($description,$FK_DeviceTemplate,$_SESSION['installationID']));
					$insertID=$dbADO->Insert_ID();
					InheritDeviceData($FK_DeviceTemplate,$insertID,$dbADO);
					createChildsForControledViaDeviceTemplate($FK_DeviceTemplate,$_SESSION['installationID'],$insertID,$dbADO);
					createChildsForControledViaDeviceCategory($FK_DeviceTemplate,$_SESSION['installationID'],$insertID,$dbADO);

					// install options
					$installOptionsArray=explode(',',$_POST['displayedTemplatesRequired_']);
					foreach($installOptionsArray AS $elem){
						$optionalDevice=(isset($_POST['requiredTemplate_'.$elem]))?$_POST['requiredTemplate_'.$elem]:0;
						if($optionalDevice!=0){
							$OptionalDeviceName=cleanString(@$_POST['templateName_'.$elem]);
							$insertDevice='
								INSERT INTO Device 
									(Description, FK_DeviceTemplate, FK_Installation, FK_Device_ControlledVia) 
								VALUES (?,?,?,?)';
							$dbADO->Execute($insertDevice,array($OptionalDeviceName,$elem,$installationID,$insertID));
						}
					}

					$insertDeviceDeviceData='INSERT INTO Device_DeviceData (FK_Device,FK_DeviceData,IK_DeviceData) VALUES (?,?,?)';
					$dbADO->Execute($insertDeviceDeviceData,array($insertID,$GLOBALS['rootPK_Distro'],$FK_Distro));
					
					if($installSourceCode!=0){
						$insertDeviceDeviceData='INSERT INTO Device_DeviceData (FK_Device,FK_DeviceData,IK_DeviceData) VALUES (?,?,?)';
						$dbADO->Execute($insertDeviceDeviceData,array($insertID,$GLOBALS['rootDevelopment'],1));
					}

					if($diskless!=0){
						$insertDeviceDeviceData='INSERT INTO Device_DeviceData (FK_Device,FK_DeviceData,IK_DeviceData) VALUES (?,?,?)';
						$dbADO->Execute($insertDeviceDeviceData,array($insertID,$GLOBALS['rootDisklessBoot'],1));
					}
				}
				header("Location: index.php?section=wizard&step=6");

			}else {
				foreach($displayedDevicesArray as $value){
					$description=@$_POST['description_'.$value];
					$FK_DeviceTemplate=@$_POST['deviceTemplate_'.$value];
					$FK_Distro=@$_POST['platform_'.$value];
					$oldInstallSourceCode=@$_POST['oldInstallSourceCode_'.$value];
					$oldDisklessBoot=@$_POST['oldDisklessBoot_'.$value];
					$installSourceCode=(isset($_POST['installSourceCode_'.$value]))?$_POST['installSourceCode_'.$value]:0;
					$diskless=(isset($_POST['disklessBoot_'.$value]))?$_POST['disklessBoot_'.$value]:0;
					$updateDevice='UPDATE Device SET Description=?, FK_DeviceTemplate=? WHERE PK_Device=?';
					$dbADO->Execute($updateDevice,array($description,$FK_DeviceTemplate,$value));

					$updateDeviceDeviceData='UPDATE Device_DeviceData SET IK_DeviceData=? WHERE FK_Device=? AND FK_DeviceData=?';
					$dbADO->Execute($updateDeviceDeviceData,array($FK_Distro,$value,$GLOBALS['rootPK_Distro']));
					if($installSourceCode!=$oldInstallSourceCode){
						if($installSourceCode==1){
							$insertDeviceDeviceData='INSERT INTO Device_DeviceData (FK_Device,FK_DeviceData,IK_DeviceData) VALUES (?,?,?)';
							$dbADO->Execute($insertDeviceDeviceData,array($value,$GLOBALS['rootDevelopment'],1));
						}else{
							$deleteDeviceDeviceData='DELETE FROM Device_DeviceData WHERE FK_Device=? AND FK_DeviceData=?';
							$dbADO->Execute($deleteDeviceDeviceData,array($value,$GLOBALS['rootDevelopment']));
						}
					}

					if($oldDisklessBoot!=$diskless){
						if($diskless==1){
							$insertDeviceDeviceData='INSERT INTO Device_DeviceData (FK_Device,FK_DeviceData,IK_DeviceData) VALUES (?,?,?)';
							$dbADO->Execute($insertDeviceDeviceData,array($value,$GLOBALS['rootDisklessBoot'],1));
						}else{
							$deleteDeviceDeviceData='DELETE FROM Device_DeviceData WHERE FK_Device=? AND FK_DeviceData=?';
							$dbADO->Execute($deleteDeviceDeviceData,array($value,$GLOBALS['rootDisklessBoot']));
						}
					}
					
					// install options - delete or insert devices
					$installOptionsArray=explode(',',$_POST['displayedTemplatesRequired_'.$value]);
					foreach($installOptionsArray AS $elem){
						$oldDevice=$_POST['oldDevice_'.$value.'_requiredTemplate_'.$elem];
						$optionalDevice=(isset($_POST['device_'.$value.'_requiredTemplate_'.$elem]))?$_POST['device_'.$value.'_requiredTemplate_'.$elem]:0;
						if($optionalDevice!=0){
							$OptionalDeviceName=cleanString(@$_POST['templateName_'.$elem]);
							$insertDevice='
									INSERT INTO Device 
										(Description, FK_DeviceTemplate, FK_Installation, FK_Device_ControlledVia) 
									VALUES (?,?,?,?)';
							if($oldDevice=='')
								$dbADO->Execute($insertDevice,array($OptionalDeviceName,$elem,$installationID,$value));
						}else{
							$dbADO->Execute("DELETE FROM Device WHERE PK_Device='".$oldDevice."'");
						}
					}
			}
			
			header("Location: index.php?section=wizard&step=7");
		}
		
		break; 
		case 7:
				if(!isset($_SESSION['deviceID'])){
					$queryDevice='
						SELECT Device.* 
							FROM Device
						INNER JOIN DeviceTemplate ON 
							FK_DeviceTemplate=PK_DeviceTemplate
						WHERE FK_DeviceCategory=? AND FK_Installation=?
					';
					$resDevice=$dbADO->Execute($queryDevice,array($GLOBALS['rootCoreID'],$installationID));
					if($resDevice->RecordCount()!=0){
						$rowDevice=$resDevice->FetchRow();
						$_SESSION['deviceID']=$rowDevice['PK_Device'];
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

				$queryDistroOS='
						SELECT Distro.*, OperatingSystem.Description AS OS FROM Distro
							INNER JOIN OperatingSystem 
								ON FK_OperatingSystem=PK_OperatingSystem
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
				$selectedRepositoryType=($rowInstallation['FK_RepositoryType_Source'])?$rowInstallation['FK_RepositoryType_Source']:6;
											

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
								<td align="right" class="normaltext" width="150"><a href="index.php?section=wizard&step=6">&lt;&lt; Previous</a></td>
							</tr>
			      			<tr>
			      				<td align="center" colspan="2" bgcolor="#DADDE4"><b>Step 7 of 7: Download</b></td>
			      			</tr>
							<tr class="normaltext">
								<td colspan="2" align="left">You finished the wizard!  All the settings you made have been saved into a database so your system can install and configure itself automatically.  You can go back to a prior page to change options and return to this page at any time.<br><br></td>
							</tr>
							<tr class="normaltext">
								<td colspan="2" align="left"><b>Your activation code is: '.$_SESSION['ActivationCode'].'</b><br>If you install from the kick-start CD you will be asked to enter the activation code and device number of the computer you are installing.  This will be used to lookup at plutohome.com the configuration settings you just made so the CD can configure your system automatically.  You can use the same Kick-start CD on different computers.  Just be sure to correctly specify the device for each computer you use it on.</td>
							</tr>
							<tr class="normaltext">
								<td colspan="2" align="left">&nbsp;</td>
							</tr>
							<tr class="normaltext" bgcolor="DADDE4">
								<td colspan="2" align="left">
								<table>
								<tr class="normaltext" bgcolor="DADDE4">
									<td>Ignore the following question if you don’t understand it:<br>If you get source code, what is the preferred method that the install script should try first before trying other methods?</td>
								</tr>
								<tr class="normaltext" bgcolor="DADDE4">
									<td align="center">
										<input type="radio" name="method" value="6" '.(($selectedRepositoryType==6)?'checked':'').' onClick="document.wizard.submit();"> Download and uncompress an archive (tar, zip, etc.)&nbsp;&nbsp;&nbsp
										<input type="radio" name="method" value="1" '.(($selectedRepositoryType==1)?'checked':'').' onClick="document.wizard.submit();"> a package (deb, rpm, etc.)&nbsp;&nbsp;&nbsp
										<input type="radio" name="method" value="3" '.(($selectedRepositoryType==3)?'checked':'').' onClick="document.wizard.submit();"> cvs&nbsp;&nbsp;&nbsp
										<input type="radio" name="method" value="4" '.(($selectedRepositoryType==4)?'checked':'').' onClick="document.wizard.submit();"> svn&nbsp;&nbsp;&nbsp
									</td>
								</tr>
								</tr>
								</table>
								</td>
							<tr class="normaltext">
							<td colspan="2" align="left"><br>Here are the computers you will need software for.  Be sure to make note of the Device numbers:<br><br><B>Core:</B></td>
							</tr>				
							<tr class="normaltext" bgcolor="lightblue">
								<td valign="top"><b>'.$DistroNameOS.'</b></td>
								<td>Device #: <b>'.$_SESSION['deviceID'].'</b></td>
							</tr>
							<tr class="normaltext">
								<td valign="top" colspan="2">Here are your options for installing the software on this computer: <a href="#" onClick="javascript:windowOpen(\'checkDependancies.php?device='.$rowDevice['PK_Device'].'\',\'width=1024,height=768,toolbars=true,scrollbars=1\');">List of all the software this device will need.</a></td>
							</tr>
							<tr class="normaltext">
								<td valign="top" colspan="2"><table width="75%" align="center">
									<tr class="normaltext" bgcolor="#DADDE4">
										<td width="30%" align="left">'.(($distroKickStartCD=='')?'<span style="color:#999999;">- Kick-start CD</span>':'- <a href="'.$distroKickStartCD.'" target="_blank">Kick-start CD</a>').'</td>
										<td>'.(($distroKickStartCD=='')?'<span style="color:#999999;">Not available for '.$DistroNameOS.'</span>':' An ISO image you can burn to a CD.  It is a self-booting CD for '.$DistroNameOS).'</td>
									</tr>
									<tr class="normaltext" bgcolor="#DADDE4">
										<td width="30%" align="left">- <a href="#" onClick="javascript:windowOpen(\''.$InstallerURL.'?code='.$autoInstallScript.'&distro='.$distroInstaller.'&name='.urlencode($rowDevice['Description']).'\',\'width=640,height=480,toolbars=true,scrollbars=1\');">Auto install script</a></td>
										<td>A script for '.$DistroNameOS.' that will install all the software automatically.</td>
									</tr>
									<tr class="normaltext" bgcolor="#DADDE4">
										<td width="30%" align="left">- <a href="#" onClick="javascript:windowOpen(\'installWraper.php?code='.$autoInstallScript.'\',\'width=1024,height=768,toolbars=true,scrollbars=1\');">Custom Wrapper for '.$rowDevice['Description'].'</a></td>
										<td>For you to write your own install script for '.$DistroNameOS.'.  You can use our 50-line Debian bash script as a template.</td>
									</tr>
									<tr class="normaltext" bgcolor="#DADDE4">
										<td width="30%" align="left">- <a href="support/index.php?section=mainDownload" target="_blank">Download page</a></td>
										<td>Just take me to Pluto’s main download page.  I’ll get everything in the list manually.</td>
									</tr>				
									<tr class="normaltext" bgcolor="#DADDE4">
										<td width="30%" align="left">- <a href="/support/index.php?section=document&docID=101" target="_blank">Source Code</a></td>
										<td>Tell me how to get the source code.</td>
									</tr>				
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
								<td valign="top" colspan="2">Here are your options for installing the software on this computer: <a href="#" onClick="javascript:windowOpen(\'checkDependancies.php?device='.$rowMediaDirectors['PK_Device'].'\',\'width=1024,height=768,toolbars=true,scrollbars=1\');">List of all the software this device will need.</a></td>
							</tr>
							<tr class="normaltext">
								<td valign="top" colspan="2"><table width="75%" align="center">
									<tr class="normaltext" bgcolor="#DADDE4">
										<td width="30%" align="left">'.(($rowDDD['KickStartCD']=='')?'<span style="color:#999999;">- Kick-start CD</span>':'- <a href="'.$rowDDD['KickStartCD'].'" target="_blank">Kick-start CD</a>').'</td>
										<td>'.(($rowDDD['KickStartCD']=='')?'<span style="color:#999999;">Not available for '.$DistroNameOS.'</span>':'An ISO image you can burn to a CD.  It is a self-booting CD for '.$DistroNameOS).'</td>
									</tr>
									<tr class="normaltext" bgcolor="#DADDE4">
										<td width="30%" align="left">'.(($rowDDD['Installer']=='')?'<span style="color:#999999;">- Auto install script</span>':'- <a href="#" onClick="javascript:windowOpen(\''.$InstallerURL.'?code='.$autoInstallScript.'&distro='.$distroInstaller.'&name='.urlencode($rowMediaDirectors['Description']).'\',\'width=640,height=480,toolbars=true,scrollbars=1\');">Auto install script</a>').'</td>
										<td>'.(($rowDDD['Installer']=='')?'<span style="color:#999999;">Not available for '.$rowDDD['Platform'].'</span>':'A script for '.$DistroNameOS.' that will install all the software automatically.').'</td>
									</tr>
									<tr class="normaltext" bgcolor="#DADDE4">
										<td width="30%" align="left">- <a href="#" onClick="javascript:windowOpen(\'installWraper.php?code='.$autoInstallScript.'\',\'width=1024,height=768,toolbars=true,scrollbars=1\');">Custom Wrapper for '.$rowMediaDirectors['Description'].'</a></td>
										<td>For you to write your own install script for '.$DistroNameOS.'.  You can use our 50-line Debian bash script as a template.</td>
									</tr>
									<tr class="normaltext" bgcolor="#DADDE4">
										<td width="30%" align="left">- <a href="support/index.php?section=mainDownload" target="_blank">Download page</a></td>
										<td>Just take me to Pluto’s main download page.  I’ll get everything in the list manually.</td>
									</tr>				
									<tr class="normaltext" bgcolor="#DADDE4">
										<td width="30%" align="left">- <a href="/support/index.php?section=document&docID=101" target="_blank">Source Code</a></td>
										<td>Tell me how to get the source code.</td>
									</tr>				
								</table>
				 				</td>
							</tr>
						';
						}else{
						$out.='							
							<tr class="normaltext">
								<td valign="top" colspan="2">The software will be installed on the Core automatically for this diskless media director.</td>
							</tr>';
						}
					}
					$out.='
							<tr class="normaltext">
								<td valign="top"><br><b>Orbiters:</b></td>
								<td></td>
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
						if( $rowDDD['FK_DeviceCategory']==2 )
						{
							$out .= '<tr><td>The software for the mobile orbiters is installed automatically.</td></tr>';
						}
						else
						{
							$out .= '
							<tr class="normaltext">
								<td valign="top" colspan="2">Here are your options for installing the software on this computer: <a href="#" onClick="javascript:windowOpen(\'checkDependancies.php?device='.$rowMediaDirectors['PK_Device'].'\',\'width=1024,height=768,toolbars=true,scrollbars=1\');">List of all the software this device will need.</a></td>
							</tr>
							<tr class="normaltext">
								<td valign="top" colspan="2"><table width="75%" align="center">
									<tr class="normaltext" bgcolor="#DADDE4">
										<td width="30%" align="left">'.(($rowDDD['KickStartCD']=='')?'<span style="color:#999999;">- Kick-start CD</span>':'- <a href="'.$rowDDD['KickStartCD'].'" target="_blank">Kick-start CD</a>').'</td>
										<td>'.(($rowDDD['KickStartCD']=='')?'<span style="color:#999999;">Not available for '.$DistroNameOS.'</span>':'An ISO image you can burn to a CD.  It is a self-booting CD for '.$DistroNameOS).'</td>
									</tr>
									<tr class="normaltext" bgcolor="#DADDE4">
										<td width="30%" align="left">'.(($rowDDD['Installer']=='')?'<span style="color:#999999;">- Auto install script</span>':'- <a href="#" onClick="javascript:windowOpen(\''.$InstallerURL.'?code='.$autoInstallScript.'&distro='.$distroInstaller.'&name='.urlencode($rowMediaDirectors['Description']).'\',\'width=640,height=480,toolbars=true,scrollbars=1\');">Auto install script</a>').'</td>
										<td>'.(($rowDDD['Installer']=='')?'<span style="color:#999999;">Not available for '.$rowDDD['Platform'].'</span>':'A script for '.$DistroNameOS.' that will install all the software automatically.').'</td>
									</tr>
									<tr class="normaltext" bgcolor="#DADDE4">
										<td width="30%" align="left">- <a href="#" onClick="javascript:windowOpen(\'installWraper.php?code='.$autoInstallScript.'\',\'width=1024,height=768,toolbars=true,scrollbars=1\');">Custom Wrapper for '.$rowMediaDirectors['Description'].'</a></td>
										<td>For you to write your own install script for '.$DistroNameOS.'.  You can use our 50-line Debian bash script as a template.</td>
									</tr>
									<tr class="normaltext" bgcolor="#DADDE4">
										<td width="30%" align="left">- <a href="support/index.php?section=mainDownload" target="_blank">Download page</a></td>
										<td>Just take me to Pluto’s main download page.  I’ll get everything in the list manually.</td>
									</tr>				
									<tr class="normaltext" bgcolor="#DADDE4">
										<td width="30%" align="left">- <a href="/support/index.php?section=document&docID=101" target="_blank">Source Code</a></td>
										<td>Tell me how to get the source code.</td>
									</tr>				
								</table>
				 				</td>
							</tr>
								';
						}
					}
					$out.='
							
						</table>
					</form>
			      	';
				
			}else{
				// process form step 7
				$updateInstallation='UPDATE Installation SET FK_RepositoryType_Source=? WHERE PK_Installation=?';
				$dbADO->Execute($updateInstallation,array($_POST['method'],$installationID));

				header("Location: index.php?section=wizard&step=7");
			}

		break;
		default:
			header("Location: index.php");
	}


	$output->setNavigationMenu(array("Client home"=>"index.php?section=userHome"));

	$output->setLeftMenu($leftMenu);

	$output->setScriptCalendar('null');
	$output->setScriptTRColor('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME."::Client");
	$output->output();
}
?>