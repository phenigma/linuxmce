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
			      				<td align="center" colspan="2" bgcolor="#DADDE4"><b>Step 1: '.((!isset($_SESSION['installationDescription']))?'New Installation':'Update Installation').'</b></td>
			      			</tr>
							<tr>
								<td colspan="2" class="normaltext">Pluto is a whole-house solution.  You can have several computers and other devices all working together as a single, whole-house solution.  We refer to a collection of equipment at a given location as an "Installation".  An installation usually refers to a particular home, or location.  If you have multiple homes, you should create an installation for each home and then "join" them together later.  Please type a name to refer to this installation, or location, you are creating, such as: 123 Main Street, or Beach House, etc.</td>
							</tr>
			      	      	<tr>
			      				<td align="right" valign="top" class="normaltext"><B>Name</B></td>
								<td align="left" valign="top" class="normaltext"><input type="text" name="Description" value="'.@$_SESSION['installationDescription'].'"> <input type="submit" name="submitBtn" value="Next"></td>
			      			</tr>
							<tr>
								<td colspan="2" align="left" class="normaltext"><br>Following are the 3 pieces of computer equipment in a Pluto system which need software.  Please decide now what equipment you will use, since this wizard will need some information about your equipment.  The minimum is 1 computer that you can use as both your Core and Media Director.  However, Pluto is easier to use if you have Orbiters too.<br><br>
									<B>1. The Core</B>, or server, which is the main brains of the system.  This is where all the data and configuration for your home are stored.<br><br>
									<B>2. Any number of media directors</B>.  These are media pc’s hook up to your tv and stereo to provide media in a location.<br><br>
									NOTE:   You need only 1 computer since the core can also be a media director, which we call a ‘hybrid’.  However, for a big installation with lots of media directors and orbiters, you really should have a dedicated, fail-safe core with lots of storage for the whole house.<br>
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
				$description=cleanString($_POST['Description']);
				
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
			      				<td align="center" colspan="2" bgcolor="#DADDE4"><b>Step 2: Version</b></td>
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
								<td colspan="2" align="center" bgcolor="#DADDE4"><B>Step 3: Operating system</B></td>
							</tr>
							<tr>
								<td colspan="2" align="left" class="normaltext">Note: At present Linux is the only supported platform for media playback.<br><br>
								You have chosen the <b>'.$_SESSION['version'].'</b> version of the software.  Here are the operating systems that this version supports for your Core, Media Directors and Orbiters.  Note which installation options are available for each platform.  
								On the next page you will be asked to choose what operating system/distro you want to use.  If "Confirmed" is not checked, then it <b>should</b> work with minimal effort, but has not been tested:
								</td>
							</tr>									
							<tr>
								<td colspan="2" align="center">
									<table>
										<tr class="normaltext">
											<td>&nbsp;</td>
											<td colspan="4"><B>Installation options</B></td>
											<td colspan="3"><B>Can be used as</B></td>
											<td rowspan="2"><B>Confirmed</B></td>
										</tr>
										<tr class="normaltext">
											<td>&nbsp;</td>
											<td><B>Kick-Start</B></td>
											<td><B>Installer</B></td>
											<td><B>Pre-compiled Binaries</B></td>
											<td><B>Source code</B></td>
											<td><B>Core</B></td>
											<td><B>Media Director</B></td>
											<td><B>Orbiter</B></td>
										</tr>';
					$queryDistroOS='
						SELECT Distro.*, OperatingSystem.Description AS OS FROM Distro
							INNER JOIN OperatingSystem 
								ON FK_OperatingSystem=PK_OperatingSystem';
					$resDistroOS=$dbADO->Execute($queryDistroOS);
					while($row=$resDistroOS->FetchRow()){
						$out.='		<tr class="normaltext">
										<td><B>'.$row['Description'].' - '.$row['OS'].'</B></td>
										<td align="center">'.(($row['KickStartCD']!=0)?'x':'-').'</td>
										<td align="center">'.(($row['Installer']!='')?'x':'-').'</td>
										<td align="center">'.(($row['Binaries']!=0)?'x':'-').'</td>
										<td align="center">'.(($row['SourceCode']!=0)?'x':'-').'</td>
										<td align="center">'.(($row['Core']!=0)?'x':'-').'</td>
										<td align="center">'.(($row['MediaDirector']!=0)?'x':'-').'</td>
										<td align="center">'.(($row['Orbiter']!=0)?'x':'-').'</td>
										<td align="center">'.(($row['Confirmed']!=0)?'x':'-').'</td>
							  		</tr>';
					}
					$out.='		</table>
								</td>
							</tr>
							<tr class="normaltext">
								<td colspan="2" align="left">&nbsp;&nbsp;&nbsp;&nbsp;The kick-start CD is the easiest way to get Pluto, particularly on a clean computer.  Just put the disk in and boot.  It will give you the option of partitioning and formatting the drive, and auto-install all the software.  If you want to contribute a kick-start CD for a distribution we don’t yet support, we’ll be happy to host us.  <a href="index.php?section=kickStartCD">Learn more</a><br><br>
									&nbsp;&nbsp;&nbsp;&nbsp;The installer also makes it a snap to install Pluto.  The installer is actually a very simple script that downloads and installs packages and source code automatically.  The script is really simple since all the complicated logic is stored in a central database and works for every platform.  If you want to run Pluto on a platform that doesn’t have an installer and know how to write scripts, like bash or perl or batch files, it will probably be faster to write an install script for your platform than to download all the modules individually.  And then if you share it with us, others can use it too.  <a href="index.php?section=installer">Learn more</a><br><br>
									&nbsp;&nbsp;&nbsp;&nbsp;If there is no kick-start CD or installer for your choice, you can always use pre-compiled binaries or build everything from source code.<br><br>
									&nbsp;&nbsp;&nbsp;&nbsp;Click Next to configure your core, media directors and orbiters and choose what platform you want to use on each of them.
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
								<td colspan="2" align="center" bgcolor="#DADDE4"><B>Step 4: Pluto Core</B></td>
							</tr>
							<tr class="normaltext">
								<td colspan="2" align="left">Tell me about the computer you’ll use as the core.  Just choose "Generic PC" if we don’t yet have your specific model listed.  <b>Don\'t forget to check the \'Hybrid\' box if you will use this to play back media too!</b></td>
							</tr>
							<tr class="normaltext">
								<td>&nbsp;&nbsp;&nbsp;&nbsp;First, what type of computer is it?</td>
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
							}
					$out.='		</td>
							</tr>
							<tr>
								<td colspan="2">&nbsp;</td>
							</tr>
							<tr class="normaltext">
								<td colspan="2"><input type="checkbox" name="installSourceCode" value="1" '.((!isset($_SESSION['deviceID']))?'disabled':'').' '.(($_SESSION['installSourceCode']==1)?'checked':'').'><span style="color:'.((!isset($_SESSION['deviceID']))?'#CCCCCC':'').'"> Install Pluto’s source code too.  This will also add any development libraries that are needed to compile.</span></td>
							</tr>
							<tr class="normaltext">
								<td colspan="2" valign="top"><input type="checkbox" name="hybridCore" value="1" '.((!isset($_SESSION['deviceID']))?'disabled':'').' '.((isset($_SESSION['coreHybridID']))?'checked':'').'><span style="color:'.((!isset($_SESSION['deviceID']))?'#CCCCCC':'').'"> <b>Hybrid:</b>  My core will also be a media director (ie a "hybrid").  If checked, the media playback modules will be installed on the core so you can hook it up directly to your tv/stereo and don’t need a separate computer for your media playback.</span></td>
							</tr>
							<tr>	
								<td colspan="2" class="normaltext">'.((!isset($_SESSION['deviceID']))?'&nbsp;':getInstallWizardDeviceTemplates($step,$dbADO,'',$FK_Distro,$CoreOS)).'</td>
							</tr>	
							<tr>
								<td colspan="2" align="center"><input type="submit" name="continue" value="Next" '.((!isset($_SESSION['deviceID']))?'disabled':'').'></td>
							</tr>		
			      		</table>
					</form>
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
					}else{
						$insertDevice='INSERT INTO Device (Description, FK_DeviceTemplate, FK_Installation) VALUES (?,?,?)';
						$dbADO->Execute($insertDevice,array('CORE',$deviceTemplate,$installationID));
						$deviceID=$dbADO->Insert_ID();
						createChildsForControledViaDeviceTemplate($deviceTemplate,$installationID,$deviceID,$dbADO);
						createChildsForControledViaDeviceCategory($deviceTemplate,$installationID,$deviceID,$dbADO);
						
						$insertDeviceDeviceData='INSERT INTO Device_DeviceData (FK_Device, FK_DeviceData,IK_DeviceData) VALUES (?,?,?)';
						$dbADO->Execute($insertDeviceDeviceData,array($deviceID,$GLOBALS['rootPK_Distro'],$distro));
						$_SESSION['deviceID']=$deviceID;
					}
					header("Location: index.php?section=wizard&step=4");
				}else{
					$installSourceCode=(isset($_POST['installSourceCode'])?$_POST['installSourceCode']:0);
					$oldInstallSourceCode=(isset($_SESSION['installSourceCode'])?$_SESSION['installSourceCode']:0);
					
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
					$hybridCore=(isset($_POST['hybridCore'])?$_POST['hybridCore']:0);
					$coreHybridID=$_SESSION['coreHybridID'];
					if($hybridCore!=$oldHybridCore){
						if($hybridCore!=0){
							$insertDeviceMD='INSERT INTO Device (Description, FK_DeviceTemplate, FK_Installation, FK_Device_ControlledVia) VALUES (?,?,?,?)';
							$dbADO->Execute($insertDeviceMD,array('The core/hybrid',$GLOBALS['rootGenericMediaDirector'],$installationID,$_SESSION['deviceID']));
							$_SESSION['coreHybridID']=$dbADO->Insert_ID();

							$insertDeviceMDDeviceData='INSERT INTO Device_DeviceData (FK_Device, FK_DeviceData,IK_DeviceData) VALUES (?,?,?)';
							$dbADO->Execute($insertDeviceMDDeviceData,array($_SESSION['coreHybridID'],$GLOBALS['rootPK_Distro'],$distro));
						}else{
							$deleteDeviceMD='DELETE FROM Device WHERE PK_Device=?';
							$dbADO->Execute($deleteDeviceMD,$coreHybridID);
							
							$deleteDeviceMDDeviceData='DELETE FROM Device_DeviceData WHERE FK_Device=? AND FK_DeviceData=?';
							$dbADO->Execute($deleteDeviceMDDeviceData,array($coreHybridID,$GLOBALS['rootPK_Distro']));

							unset($_SESSION['coreHybridID']);
						}
					}
					header("Location: index.php?section=wizard&step=5");
				}
			}
		break;
		case 5:
				$installationID=(isset($_SESSION['installationID']))?$_SESSION['installationID']:0;
				
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
								<td colspan="2" align="center" bgcolor="#DADDE4"><B>Step 5: Media Directors</B></td>
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
										<td  align="center"><input type="checkbox" name="disklessBoot_'.$rowMediaDirectors['PK_Device'].'" '.(($resDiskless->RecordCount()>0)?'checked':'').' value="1"></td>
										<td class="normaltext">'.'<input type="submit" name="delete_'.$rowMediaDirectors['PK_Device'].'" value="Delete"></td>
									</tr>';
							}
							else {
								$out .='<td colspan="5" class="normaltext">This is your hybrid--the media director software will be running on the same computer as the core.  Choose the media director-related software modules you want on this hybrid.  To change the core-related software modules or other options, return to the <a href="index.php?section=wizard&step=4">Core page</a>.  You can add more stand-alone media directors below that will connect to this hybrid.</td>';
							}
							$out .='<tr '.$rowcolor.'>	
										<td colspan="6">'.getInstallWizardDeviceTemplates($step,$dbADO,'_'.$rowMediaDirectors['PK_Device'],$selectedDistro).'</td>
									</tr>';
					}
					$out.='<input type="hidden" name="displayedDevices" value="'.join(',',$displayedDevices).'">';
					$out.='		</table>
								</td>
							</tr>
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
								<td><input type="checkbox" name="disklessBoot" value="1" '.((@$_POST['disklessBoot']==1)?'checked':'').'></td>
							</tr>';
					if(isset($_POST['newPlatform']))
						$out.='<tr class="normaltext">
								<td colspan="2">'.getInstallWizardDeviceTemplates($step,$dbADO,'',@$_POST['newPlatform']).'</td>
							</tr>';
					$out.='	<tr>
								<td colspan="2" align="center">'.((isset($_POST['newPlatform']) && $_POST['newPlatform']!='0')?'<input type="submit" name="continue" value="Add&Continue">':'').' '.((isset($_POST['newPlatform']))?'<input type="button" name="next" value="Cancel" onClick="self.location=\'index.php?section=wizard&step=5\'">':'').' <input type="button" name="next" value="Next" onClick="javascript:document.wizard.action.value=\'update\';document.wizard.submit();"></td>
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
				// process form step 5
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
					
					$displayedInstallWizardArray=explode(',',$_POST['displayedInstallWizard']);
					foreach($displayedInstallWizardArray as $value){
						$insertChildDevice='INSERT INTO Device (Description, FK_DeviceTemplate, FK_Installation,FK_Device_ControlledVia) VALUES (?,?,?,?)';
						$dbADO->Execute($insertChildDevice,array($description." Child",$value,$_SESSION['installationID'],$insertID));
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
				}
			header("Location: index.php?section=wizard&step=6");
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
								<td colspan="2" align="center" bgcolor="#DADDE4"><B>Step 6: Orbiter</B></td>
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
						SELECT * FROM Device WHERE FK_DeviceTemplate IN ('.join(',',$validOrbiters).') AND FK_Installation=?';
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
										<td colspan="6">'.getInstallWizardDeviceTemplates($step,$dbADO,'_'.$rowOrbiters['PK_Device'],$selectedDistro).'</td>
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

					$displayedInstallWizardArray=explode(',',$_POST['displayedInstallWizard']);
					foreach($displayedInstallWizardArray as $value){
						$insertChildDevice='INSERT INTO Device (Description, FK_DeviceTemplate, FK_Installation,FK_Device_ControlledVia) VALUES (?,?,?,?)';
						$dbADO->Execute($insertChildDevice,array($description." Child",$value,$_SESSION['installationID'],$insertID));
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
			      				<td align="center" colspan="2" bgcolor="#DADDE4"><b>Step 7: Download</b></td>
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
										<input type="radio" name="method" value="6" '.(($selectedRepositoryType==6)?'checked':'').' onChange="document.wizard.submit();"> Download and uncompress an archive (tar, zip, etc.)&nbsp;&nbsp;&nbsp
										<input type="radio" name="method" value="1" '.(($selectedRepositoryType==1)?'checked':'').' onChange="document.wizard.submit();"> a package (deb, rpm, etc.)&nbsp;&nbsp;&nbsp
										<input type="radio" name="method" value="3" '.(($selectedRepositoryType==3)?'checked':'').' onChange="document.wizard.submit();"> cvs&nbsp;&nbsp;&nbsp
										<input type="radio" name="method" value="4" '.(($selectedRepositoryType==4)?'checked':'').' onChange="document.wizard.submit();"> svn&nbsp;&nbsp;&nbsp
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
								<td valign="top" colspan="2"><a href="#" onClick="javascript:windowOpen(\'checkDependancies.php?device='.$_SESSION['deviceID'].'\',\'width=1024,height=768,toolbars=true\');">Click here</a> to list all the software that is required for this computer.</td>
							</tr>
							<tr class="normaltext">
								<td valign="top">&nbsp;</td>
							</tr>
							<tr class="normaltext">
								<td valign="top" colspan="2">Here are the 4 ways you can install the software on this computer.  Choose the one you want to use:</td>
							</tr>
							<tr class="normaltext">
								<td valign="top" colspan="2"><table width="75%" align="center">
									<tr class="normaltext">
										<td width="30%" align="left"><span style="color:'.(($distroKickStartCD=='')?'#CCCCCC':'').'">- '.(($distroKickStartCD!='')?'<a href="'.$domain.$downloadDirectory.$distroKickStartCD.'">':'').'Kick-start CD'.(($distroKickStartCD!='')?'</a>':'').(($distroKickStartCD=='')?'<br>Not available for '.$DistroNameOS:'').'</span></td>
										<td> An ISO image you can burn to a CD.  It is a self-booting CD for '.$DistroNameOS.' that will also setup the Pluto software.  You can use the same CD for any computer with '.$DistroNameOS.'</td>
									</tr>
									<tr class="normaltext" bgcolor="#DADDE4">
										<td width="30%" align="left">- <a href="#" onClick="javascript:windowOpen(\'autoInstall.php?code='.$autoInstallScript.'&distro='.$distroInstaller.'&name='.urlencode($rowDevice['Description']).'\',\'width=640,height=480,toolbars=true\');">Auto install script</a></td>
										<td></td>
									</tr>
									<tr class="normaltext">
										<td width="30%" align="left">- <a href="#" onClick="javascript:windowOpen(\'installWraper.php?code='.$autoInstallScript.'\',\'width=1024,height=768,toolbars=true\');">Custom Wrapper for '.$rowDevice['Description'].'</a></td>
										<td></td>
									</tr>
									<tr class="normaltext" bgcolor="#DADDE4">
										<td width="30%" align="left">- <a href="support/index.php?section=mainDownload">Download page</a></td>
										<td>Just take me to the download page.  I’ll get everything manually.</td>
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
						WHERE FK_DeviceTemplate=? AND FK_Installation=?';
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
						
						$autoInstallScript=$rowMediaDirectors['PK_Device'].'-'.$_SESSION['ActivationCode'];
						$distroInstaller=$rowDDD['Installer'];
						
						$out.='
							<tr class="normaltext" bgcolor="lightblue">
								<td valign="top"><b>Name: '.$rowMediaDirectors['Description'].' Type: '.$rowMediaDirectors['Type'].' Platform: '.$rowDDD['Platform'].' '.$rowDDD['OS'].'</b></td>
								<td>Device #: <b>'.$rowMediaDirectors['PK_Device'].'</b></td>
							</tr>
							<tr class="normaltext">
								<td valign="top" colspan="2"><a href="#" onClick="javascript:windowOpen(\'checkDependancies.php?device='.$rowMediaDirectors['PK_Device'].'\',\'width=1024,height=768,toolbars=true\');">Click here</a> to list to list all the software that is required for this computer.</td>
							</tr>
							<tr class="normaltext">
								<td valign="top">&nbsp;</td>
							</tr>
							<tr class="normaltext">
								<td valign="top" colspan="2">Here are the 4 ways you can install the software on this computer.  Choose the one you want to use:</td>
							</tr>
							<tr class="normaltext">
								<td valign="top" colspan="2"><table width="75%" align="center">
									<tr class="normaltext">
										<td width="30%" align="left"><span style="color:'.(($rowDDD['distroKickStartCD']=='')?'#CCCCCC':'').'">- '.(($rowDDD['distroKickStartCD']!='')?'<a href="'.$domain.$downloadDirectory.$rowDDD['distroKickStartCD'].'">':'').'Kick-start CD'.(($rowDDD['distroKickStartCD']!='')?'</a>':'').(($rowDDD['distroKickStartCD']=='')?'<br>Autoinstall script: Not available for '.$rowDDD['Platform']:'').'</span></td>
										<td> An ISO image for you to burn to a CD.</td>
									</tr>
									<tr class="normaltext" bgcolor="#DADDE4">
										<td width="30%" align="left">- '.(($rowDDD['Installer']!='')?'<a href="#" onClick="javascript:windowOpen(\'autoInstall.php?code='.$autoInstallScript.'&distro='.$distroInstaller.'&name='.urlencode($rowMediaDirectors['Description']).'\',\'width=640,height=480,toolbars=true\');">Auto install script</a>':'<span  style="color:#CCCCCC">Not available for '.$rowDDD['Platform'].'</span>').'</td>
										<td></td>
									</tr>
									<tr class="normaltext">
										<td width="30%" align="left">- <a href="#" onClick="javascript:windowOpen(\'installWraper.php?code='.$autoInstallScript.'\',\'width=1024,height=768,toolbars=true\');">Custom Wrapper for '.$rowMediaDirectors['Description'].'</a></td>
										<td></td>
									</tr>
									<tr class="normaltext" bgcolor="#DADDE4">
										<td width="30%" align="left">- <a href="support/index.php?section=mainDownload">Download page</a></td>
										<td>Just take me to the download page.  I’ll get everything manually.</td>
									</tr>				
								</table>
				 				</td>
							</tr>

						';
					}
					$out.='
							<tr class="normaltext">
								<td valign="top" colspan="2">For each media director you have the same options as for the Core.  Refer to the Core’s download options above for a more detailed explanation.  No software is required for diskless media directors.  It will be installed on the Core automatically.</td>
							</tr>
							<tr>
								<td valign="top">&nbsp;</td>
							</tr>
							<tr class="normaltext">
								<td valign="top"><b>Orbiters:</b></td>
								<td></td>
							<tr>';
					$validOrbiters = getValidOrbitersArray($installationID,$dbADO);
					$queryOrbiters='
						SELECT 
							Device.*,
							DeviceTemplate.Description AS Type
					 	FROM Device
							INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
					 	WHERE FK_DeviceTemplate IN ('.join(',',$validOrbiters).') AND FK_Installation=?';
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
								Distro.*
							FROM Device_DeviceData 
								INNER JOIN Distro ON PK_Distro=IK_DeviceData
								INNER JOIN OperatingSystem ON FK_OperatingSystem=PK_OperatingSystem
							WHERE FK_Device=? AND FK_DeviceData=?';
						$resDeviceDeviceData=$dbADO->Execute($queryDeviceDeviceData,array($rowOrbiters['PK_Device'],$GLOBALS['rootPK_Distro']));
						$rowDDD=$resDeviceDeviceData->FetchRow();
						
						$autoInstallScript=$rowOrbiters['PK_Device'].'-'.$_SESSION['ActivationCode'];
						$distroInstaller=$rowDDD['Installer'];

						$out.='
							<tr class="normaltext" bgcolor="lightblue">
								<td valign="top"><b>Name: '.$rowOrbiters['Description'].' Type: '.$rowOrbiters['Type'].' Platform: '.$rowDDD['Platform'].' '.$rowDDD['OS'].'</b></td>
								<td>Device #: <b>'.$rowOrbiters['PK_Device'].'</b></td>
							</tr>
							<tr class="normaltext">
								<td valign="top" colspan="2"><a href="#" onClick="javascript:windowOpen(\'checkDependancies.php?device='.$rowOrbiters['PK_Device'].'\',\'width=1024,height=768,toolbars=true\');">Click here</a> to list all the software that is required for this computer.</td>
							</tr>
							<tr class="normaltext">
								<td valign="top">&nbsp;</td>
							</tr>
							<tr class="normaltext">
								<td valign="top" colspan="2">Here are the 4 ways you can install the software on this computer.  Choose the one you want to use:</td>
							</tr>
							<tr class="normaltext">
								<td valign="top" colspan="2"><table width="75%" align="center">
									<tr class="normaltext">
										<td width="30%" align="left"><span style="color:'.(($rowDDD['distroKickStartCD']=='')?'#CCCCCC':'').'">- '.(($rowDDD['distroKickStartCD']!='')?'<a href="'.$domain.$downloadDirectory.$rowDDD['distroKickStartCD'].'">':'').'Kick-start CD'.(($rowDDD['distroKickStartCD']!='')?'</a>':'').(($rowDDD['distroKickStartCD']=='')?'<br>Autoinstall script: Not available for '.$rowDDD['Platform']:'').'</span></td>
										<td> An ISO image for you to burn to a CD.</td>
									</tr>
									<tr class="normaltext" bgcolor="#DADDE4">
										<td width="30%" align="left">- '.(($rowDDD['Installer']!='')?'<a href="#" onClick="javascript:windowOpen(\'autoInstall.php?code='.$autoInstallScript.'&distro='.$distroInstaller.'&name='.urlencode($rowOrbiters['Description']).'\',\'width=640,height=480,toolbars=true\');">Auto install script</a>':'<span  style="color:#CCCCCC">Not available for '.$rowDDD['Platform'].'</span>').'</td>
										<td></td>
									</tr>
									<tr class="normaltext">
										<td width="30%" align="left">- <a href="#" onClick="javascript:windowOpen(\'installWraper.php?code='.$autoInstallScript.'\',\'width=1024,height=768,toolbars=true\');">Custom Wrapper for '.$rowOrbiters['Description'].'</a></td>
										<td></td>
									</tr>
									<tr class="normaltext" bgcolor="#DADDE4">
										<td width="30%" align="left">- <a href="support/index.php?section=mainDownload">Download page</a></td>
										<td>Just take me to the download page.  I’ll get everything manually.</td>
									</tr>				
								</table>
				 				</td>
							</tr>
';
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