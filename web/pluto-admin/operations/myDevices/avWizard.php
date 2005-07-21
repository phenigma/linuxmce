<?
function avWizard($output,$dbADO) {
	global $dbPlutoMainDatabase;
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */
	//	$dbADO->debug=true;
	$userID = (int)@$_SESSION['userID'];
	$out='';
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$type = isset($_REQUEST['type'])?cleanString($_REQUEST['type']):'avEquipment';
	$installationID = (int)@$_SESSION['installationID'];

$start_time=getmicrotime();	
	$pvrArray=getAssocArray('DeviceTemplate','PK_DeviceTemplate','Description',$dbADO,' WHERE FK_DeviceCategory='.$GLOBALS['PVRCaptureCards'],'ORDER BY Description ASC');
	$soundArray=getAssocArray('DeviceTemplate','PK_DeviceTemplate','Description',$dbADO,' WHERE FK_DeviceCategory='.$GLOBALS['SoundCards'],'ORDER BY Description ASC');
	$videoArray=getAssocArray('DeviceTemplate','PK_DeviceTemplate','Description',$dbADO,' WHERE FK_DeviceCategory='.$GLOBALS['VideoCards'],'ORDER BY Description ASC');

	$deviceCategory=$GLOBALS['rootAVEquipment'];
	$specificFloorplanType=$GLOBALS['AVEquipmentFlorplanType'];
	$title='A/V Equipment';
	$output->setHelpSrc('/support/index.php?section=document&docID=131');

	// get selected category Device Templates
	getDeviceCategoryChildsArray($deviceCategory,$dbADO);
	$GLOBALS['childsDeviceCategoryArray']=cleanArray($GLOBALS['childsDeviceCategoryArray']);
	$GLOBALS['childsDeviceCategoryArray'][]=$deviceCategory;

	$queryDeviceTemplate='
		SELECT * FROM DeviceTemplate 
			WHERE FK_DeviceCategory IN ('.join(',',$GLOBALS['childsDeviceCategoryArray']).')
		ORDER BY Description ASC';
	$resDeviceTemplate=$dbADO->Execute($queryDeviceTemplate);
	$DTArray=array();
	$DTIDArray=array();
	while($rowDeviceCategory=$resDeviceTemplate->FetchRow()){
		$DTArray[]=$rowDeviceCategory['Description'];
		$DTIDArray[]=$rowDeviceCategory['PK_DeviceTemplate'];
	}


	$queryRooms='SELECT * FROM Room WHERE FK_Installation=? ORDER BY Description ASC';
	$resRooms=$dbADO->Execute($queryRooms,$installationID);
	$roomIDArray=array();
	$roomArray=array();
	while($rowRoom=$resRooms->FetchRow()){
		$roomArray[]=$rowRoom['Description'];
		$roomIDArray[]=$rowRoom['PK_Room'];
	}

	if(isset($_REQUEST['lastAdded']) && (int)$_REQUEST['lastAdded']!=0){
		$rs=$dbADO->Execute('SELECT Comments FROM DeviceTemplate WHERE PK_DeviceTemplate=?',(int)$_REQUEST['lastAdded']);
		$row=$rs->FetchRow();
		if($row['Comments']!=''){
			$out.='<script>
				alert(\''.addslashes($row['Comments']).'\')
			</script>';
		}
	}


	if ($action == 'form') {
		$out.=setLeftMenu($dbADO).'
	<script>
			function windowOpen(locationA,attributes) {
				window.open(locationA,\'\',attributes);
			}
	</script>
	<div class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</div>
	<div class="confirm" align="center"><B>'.strip_tags(@$_GET['msg']).'</B></div>
	<form action="index.php" method="POST" name="avWizard">
	<input type="hidden" name="section" value="avWizard">
	<input type="hidden" name="type" value="'.$type.'">
	<input type="hidden" name="action" value="add">
	<input type="hidden" name="cmd" value="0">			
	<div align="center"><h3>'.((isset($title))?$title:strtoupper(str_replace('_',' ',$type))).'</h3></div>
	
	<div id="preloader" style="display:;">
		<table width="100%">
			<tr>
				<td align="center">Loading, please wait ...</td>
			</tr>
		</table>
	</div>
	<div id="content" style="display:none;">';
		$out.='<a href="index.php?section=connectionWizard">Connection Wizard</a>';
		$queryDevice='
			SELECT Device.* 
			FROM Device
			WHERE FK_DeviceTemplate=? AND Device.FK_Installation=?';
		$resDevice=$dbADO->Execute($queryDevice,array($GLOBALS['rootCoreID'],$installationID));
		if($resDevice->RecordCount()!=0){
			$rowDevice=$resDevice->FetchRow();
			$coreID=$rowDevice['PK_Device'];
		}
		if(isset($coreID)){
			$resDevice_StartupScript=$dbADO->Execute('SELECT * FROM Device_StartupScript WHERE FK_Device=? AND FK_StartupScript=?',array($coreID,$GLOBALS['ShareIRCodes']));
			$rowShare=$resDevice_StartupScript->FetchRow();
			$sharedWithOthers=($rowShare['Enabled']==1)?1:0;
		}

		$out.='<div align="center"><input type="checkbox" name="shareIRCodes" value="1" '.((@$sharedWithOthers>0)?'checked':'').' onClick="document.avWizard.submit();"> Share my I/R codes with other Pluto users.</div>';
		$out.='	<input type="hidden" name="coreID" value="'.$coreID.'">
				<input type="hidden" name="oldShareIRCodes" value="'.((@$sharedWithOthers>0)?'1':'0').'">';

		$infraredAndSpecialisedDevices=getDevicesFromCategories(array($GLOBALS['specialized'],$GLOBALS['InfraredInterface']),$dbADO);
		$specialisedAndComputerDevices=getDevicesFromCategories(array($GLOBALS['rootComputerID'],$GLOBALS['specialized']),$dbADO);

		$out.='
		<table align="center" border="0" cellpadding="2" cellspacing="0">
			<tr bgcolor="lightblue">
					<td align="center" rowspan="2"><B>Device</B></td>
					<td align="center" rowspan="2"><B>Room / Controlled by</B></td>
					<td align="center" colspan="4"><B>Pipes</B></td>
					<td align="center" rowspan="2"><B>Device Data</B></td>
					<td align="center" rowspan="2"><B>Actions</B></td>
				</tr>		
				<tr bgcolor="lightblue">
					<td align="center"><B>Output</B></td>
					<td align="center"><B>Connected to</B></td>
					<td align="center"><B>Input</B></td>
					<td align="center"><B>&nbsp;</B></td>
				</tr>
					';
		if(count($DTIDArray)==0)
			$DTIDArray[]=0;
		$displayedAVDevices=array();
		$displayedAVDevicesDescription=array();
		$queryDevice='
					SELECT Device.*
					FROM Device 
					WHERE Device.FK_DeviceTemplate IN ('.join(',',$DTIDArray).') AND FK_Installation=?';	
		$resDevice=$dbADO->Execute($queryDevice,$installationID);
		while($rowD=$resDevice->FetchRow()){
			$displayedAVDevices[]=$rowD['PK_Device'];
			$displayedAVDevicesDescription[]=$rowD['Description'];
		}
		$resDevice->Close();

		$queryConnectedToDevices='
			SELECT DISTINCT Device.*
			FROM Device 
			INNER JOIN DeviceTemplate ON Device.FK_DeviceTemplate=PK_DeviceTemplate
			INNER JOIN DeviceTemplate_Input ON DeviceTemplate_Input.FK_DeviceTemplate=Device.FK_DeviceTemplate 
			WHERE Device.FK_DeviceTemplate IN ('.join(',',$DTIDArray).') AND FK_Installation=?';	
		$resConnectedToDevices=$dbADO->Execute($queryConnectedToDevices,$installationID);
		$conD=array();
		while($rowConD=$resConnectedToDevices->FetchRow()){
			$conD[$rowConD['PK_Device']]=$rowConD['Description'];
		}
		$resConnectedToDevices->Close();

		$displayedDevices=array();
		$joinArray=$DTIDArray;
		$joinArray[]=0; 	// used only for query when there are no DT in selected category
		$orderFilter=($type!='media_directors')?'ORDER BY FK_Device_ControlledVia DESC, Device.Description ASC':'';
		$queryDevice='
			SELECT 
				PK_Device,
				Device.Description,
				IPaddress,
				MACaddress,
				FK_Device_ControlledVia,
				FK_Device_RouteTo,
				FK_Room,
				Device.FK_DeviceTemplate,
				DeviceTemplate.Description AS TemplateName, 
				DeviceCategory.Description AS CategoryName, 
				Manufacturer.Description AS ManufacturerName, 
				IsIPBased, DeviceTemplate_AV.UsesIR, 
				FK_DeviceCategory,
				DeviceData.Description AS dd_Description, 
				Device_DeviceData.FK_DeviceData,
				ParameterType.Description AS typeParam, 
				Device_DeviceData.IK_DeviceData,
				ShowInWizard,ShortDescription,
				AllowedToModify,
				DeviceTemplate_DeviceData.Description AS Tooltip 
			FROM DeviceData 
			INNER JOIN ParameterType ON FK_ParameterType = PK_ParameterType 
			INNER JOIN Device_DeviceData ON Device_DeviceData.FK_DeviceData=PK_DeviceData 
			INNER JOIN Device ON FK_Device=PK_Device
			LEFT JOIN DeviceTemplate_DeviceData ON DeviceTemplate_DeviceData.FK_DeviceData=Device_DeviceData.FK_DeviceData AND DeviceTemplate_DeviceData.FK_DeviceTemplate=Device.FK_DeviceTemplate
			INNER JOIN DeviceTemplate ON Device.FK_DeviceTemplate=PK_DeviceTemplate 
			LEFT JOIN DeviceTemplate_AV ON Device.FK_DeviceTemplate=DeviceTemplate_AV.FK_DeviceTemplate 
			INNER JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory 
			INNER JOIN Manufacturer ON FK_Manufacturer=PK_Manufacturer 			
			WHERE Device.FK_DeviceTemplate IN ('.join(',',$joinArray).') AND FK_Installation=? '.$orderFilter;
	
		$resDevice=$dbADO->Execute($queryDevice,$installationID);
		$childOf=array();
		$firstDevice=0;
		$deviceDataArray=array();
		while($rowD=$resDevice->FetchRow()){
			if($rowD['FK_Device_ControlledVia']==$rowD['FK_Device_RouteTo'])
				$childOf[$rowD['PK_Device']]=$rowD['FK_Device_ControlledVia'];
			$displayedDevices[$rowD['PK_Device']]=$rowD['Description'];
			
			if($rowD['PK_Device']!=$firstDevice){
				$firstDevice=$rowD['PK_Device'];
				$deviceDataArray[$firstDevice]=array();
			}else{
				$deviceDataArray[$firstDevice][]=$rowD;
			}
		}
		$joinArray=array_keys($displayedDevices);	// used only for query when there are no Devices in selected category
		if(count($joinArray)==0)
			$joinArray[]=0;
		$resDevice->MoveFirst();
		$pos=0;
		$embededRows=array();
		$GLOBALS['DeviceDataToDisplay']=array();
		$deviceDisplayed=0;
		while($rowD=$resDevice->FetchRow()){
			if($rowD['PK_Device']!=$deviceDisplayed){
				$deviceDisplayed=$rowD['PK_Device'];
				$pos++;
	
				$deviceName=(@$childOf[$rowD['PK_Device']]=='')?'<input type="text" name="description_'.$rowD['PK_Device'].'" value="'.$rowD['Description'].'">':'<input type="hidden" name="description_'.$rowD['PK_Device'].'" value="'.$rowD['Description'].'"><B>'.$rowD['Description'].'</B>';
				$deviceName.=' # '.$rowD['PK_Device'];
				$roomPulldown='<select name="room_'.$rowD['PK_Device'].'">
							<option value="0">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;- Select room -&nbsp;&nbsp;&nbsp;&nbsp;</option>';
				foreach($roomIDArray as $key => $value){
					$roomPulldown.='<option value="'.$value.'" '.(($rowD['FK_Room']==$value)?'selected':'').'>'.$roomArray[$key].'</option>';
				}
				$roomPulldown.='</select>';
	
				$buttons='
						<input type="button" class="button" name="btn" value="A/V Properties" onClick="windowOpen(\'index.php?section=editAVDevice&dtID='.$rowD['FK_DeviceTemplate'].'&deviceID='.$rowD['PK_Device'].'&from='.urlencode('avWizard&type='.$type).'\',\'width=1024,height=768,toolbars=true,scrollbars=1,resizable=1\');"><br>
						<input type="button" class="button" name="btn" value="IR/GSD Codes" onClick="windowOpen(\'index.php?section=irCodes&from=avWizard&deviceID='.$rowD['PK_Device'].'&dtID='.$rowD['FK_DeviceTemplate'].'&from='.urlencode('avWizard&type='.$type).'\',\'width=1024,height=768,toolbars=true,scrollbars=1,resizable=1\');"><br>
						<input type="submit" class="button" name="delete_'.$rowD['PK_Device'].'" value="Delete"  onclick="if(confirm(\'Are you sure you want to delete this device?\'))return true;else return false;"></td>';
	
				$controlledByPulldown=controlledViaPullDown('controlledBy_'.$rowD['PK_Device'],$rowD['PK_Device'],$rowD['FK_DeviceTemplate'],$rowD['FK_DeviceCategory'],$rowD['FK_Device_ControlledVia'],$dbADO);
				$devicePipes=getPipes($rowD['PK_Device'],$dbADO);
				
				unset($GLOBALS['DeviceIDControlledVia']);
				unset($GLOBALS['DeviceControlledVia']);
	
	
				if(@$childOf[$rowD['PK_Device']]==''){
					$out.='
					<tr>
						<td align="center" bgcolor="#F0F3F8"><a name="deviceLink_'.$rowD['PK_Device'].'"></a>'.$deviceName.'</td>
						<td  align="right">'.$roomPulldown.'</td>
						<td bgcolor="#F0F3F8">A: '.@$devicePipes['1']['output'].'</td>
						<td bgcolor="#F0F3F8">'.@$devicePipes['1']['to'].'</td>
						<td bgcolor="#F0F3F8">'.@$devicePipes['1']['input'].'</td>
						<td bgcolor="#F0F3F8" rowspan="2"><a href="javascript:windowOpen(\'index.php?section=editPipes&deviceID='.$rowD['PK_Device'].'&from=avWizard\',\'width=600,height=300,toolbars=true,scrollbars=1,resizable=1\');">Edit</a></td>
						<td rowspan="2" valign="top" align="right">'.formatDeviceData($rowD['PK_Device'],$deviceDataArray[$rowD['PK_Device']],$dbADO,$rowD['IsIPBased']).'</td>
						<td bgcolor="#F0F3F8" align="center" rowspan="2" valign="top">'.$buttons.'</td>
					</tr>
					<tr>			
						<td align="center" bgcolor="#F0F3F8" title="Category: '.$rowD['CategoryName'].', manufacturer: '.$rowD['ManufacturerName'].'">DT: '.$rowD['TemplateName'].'<br><input type="button" class="button" name="edit_'.$rowD['PK_Device'].'" value="Advanced"  onClick="self.location=\'index.php?section=editDeviceParams&deviceID='.$rowD['PK_Device'].'\';"></td>
						<td align="right">'.$controlledByPulldown.'</td>
						<td bgcolor="#F0F3F8">V: '.@$devicePipes['2']['output'].'</td>
						<td bgcolor="#F0F3F8">'.@$devicePipes['2']['to'].'</td>
						<td bgcolor="#F0F3F8">'.@$devicePipes['2']['input'].'</td>
					</tr>';
					if(isset($embededRows[$rowD['PK_Device']])){
						foreach($embededRows[$rowD['PK_Device']] as $tuner){
							$out.=$tuner;
						}
					}
					$out.='
					<tr>
						<td><hr></td>
						<td><hr></td>
						<td colspan="4"><hr></td>
						<td><hr></td>
						<td><hr></td>
					</tr>';						
				}else{
					$embededRows[$rowD['FK_Device_ControlledVia']][]='
					<tr>
						<td align="center" bgcolor="#F0F3F8"><a name="deviceLink_'.$rowD['PK_Device'].'"></a>'.$deviceName.'</td>
						<td align="center">- Embeded device -</td>
						<td bgcolor="#F0F3F8">A: '.@$devicePipes['1']['output'].'</td>
						<td bgcolor="#F0F3F8">'.@$devicePipes['1']['to'].'</td>
						<td bgcolor="#F0F3F8">'.@$devicePipes['1']['input'].'</td>
						<td bgcolor="#F0F3F8" rowspan="2"><a href="javascript:windowOpen(\'index.php?section=editPipes&deviceID='.$rowD['PK_Device'].'&from=avWizard\',\'width=600,height=300,toolbars=true,scrollbars=1,resizable=1\');">Edit</a></td>
						<td valign="top" align="center">- embedded device -</td>
						<td bgcolor="#F0F3F8" align="center" rowspan="2" valign="top">&nbsp;</td>
					</tr>
					<tr>			
						<td align="center" bgcolor="#F0F3F8" title="Category: '.$rowD['CategoryName'].', manufacturer: '.$rowD['ManufacturerName'].'">DT: '.$rowD['TemplateName'].'</td>
						<td>&nbsp;</td>
						<td bgcolor="#F0F3F8">V: '.@$devicePipes['2']['output'].'</td>
						<td bgcolor="#F0F3F8">'.@$devicePipes['2']['to'].'</td>
						<td bgcolor="#F0F3F8">'.@$devicePipes['2']['input'].'</td>
					</tr>';
				}
	
			}
		}
		$out.='
				<input type="hidden" name="displayedDevices" value="'.join(',',array_keys($displayedDevices)).'">';

		if($resDevice->RecordCount()!=0){
			$out.='
				<tr>
					<td colspan="8" align="center"><input type="submit" class="button" name="update" value="Update"></td>
				</tr>
				<tr>
					<td colspan="8" align="left">&nbsp;</td>
				</tr>';
		}
		$out.='
				<tr>
					<td colspan="8">* PK_FloorplanObjectType in red has no picture available</td>
				</tr>
				<tr>
					<td colspan="8" align="center"><input type="button" class="button" name="button" value="Add device" onClick="document.avWizard.action.value=\'externalSubmit\';document.avWizard.submit();windowOpen(\'index.php?section=deviceTemplatePicker&allowAdd=1&from='.urlencode('avWizard&type='.$type).'&categoryID='.$deviceCategory.'\',\'width=800,height=600,toolbars=true,scrollbars=1,resizable=1\');"></td>
				</tr>
			</table>
			<input type="hidden" name="DeviceDataToDisplay" value="'.join(',',$GLOBALS['DeviceDataToDisplay']).'">
		</form>
		<script>
		 	var frmvalidator = new formValidator("avWizard");
 //			frmvalidator.addValidation("Description","req","Please enter a device description");			
//	 		frmvalidator.addValidation("masterDevice","dontselect=0","Please select a Device Template!");			
		</script>
	
	</form>
	</div>
	';
	$end_time=getmicrotime();			

		$output->setScriptInBody('onLoad="document.getElementById(\'preloader\').style.display=\'none\';document.getElementById(\'content\').style.display=\'\';";');
	} else {
		$cmd=cleanInteger(@$_POST['cmd']);
		// check if the user has the right to modify installation
		$canModifyInstallation = getUserCanModifyInstallation($_SESSION['userID'],$_SESSION['installationID'],$dbADO);
		if (!$canModifyInstallation){
			header("Location: index.php?section=avWizard&type=$type&error=You are not authorised to change the installation.");
			exit(0);
		}


		$oldShareIRCodes=@(int)$_POST['oldShareIRCodes'];
		$coreID=@(int)$_POST['coreID'];
		$resShare=$dbADO->Execute('SELECT * FROM Device_StartupScript WHERE FK_Device=? AND FK_StartupScript=?',array($coreID,$GLOBALS['ShareIRCodes']));
		if(isset($_POST['shareIRCodes'])){
			if($oldShareIRCodes==0 && $coreID!=0){
				if($resShare->RecordCount()==0)
				$dbADO->Execute('INSERT INTO Device_StartupScript (FK_Device, FK_StartupScript, Enabled) VALUES (?,?,1)',array($coreID,$GLOBALS['ShareIRCodes']));
				else
				$dbADO->Execute('UPDATE Device_StartupScript SET Enabled=1 WHERE FK_Device=? AND FK_StartupScript=?',array($coreID,$GLOBALS['ShareIRCodes']));
			}
		}else{
			if($oldShareIRCodes!=0){
				$dbADO->Execute('UPDATE Device_StartupScript SET Enabled=0 WHERE FK_Device=? AND FK_StartupScript=?',array($coreID,$GLOBALS['ShareIRCodes']));
			}
		}

		$displayedDevicesArray=explode(',',@$_POST['displayedDevices']);
		foreach($displayedDevicesArray as $value){
			if(isset($_POST['delete_'.$value])){
				deleteDevice($value,$dbADO);
			}
		}

		if(isset($_POST['update']) || $cmd==1 || $action=='externalSubmit'){
			setDCERouterNeedConfigure($_SESSION['installationID'],$dbADO);
			$DeviceDataToDisplayArray=explode(',',@$_POST['DeviceDataToDisplay']);
			if(@$_POST['displayedDevices']!=''){
				foreach($displayedDevicesArray as $key => $value){
					$description=stripslashes(@$_POST['description_'.$value]);
					if(isset($_POST['ip_'.$value])){
						$ip=$_POST['ip_'.$value];
						$mac=$_POST['mac_'.$value];
						$updateMacIp=",IPaddress='$ip', MACaddress='$mac'";
					}

					$room=(@$_POST['room_'.$value]!=0)?(int)@$_POST['room_'.$value]:NULL;
					$controlledBy=(@$_POST['controlledBy_'.$value]!=0)?(int)@$_POST['controlledBy_'.$value]:NULL;

					if(isset($_POST['controlledBy_'.$value]) && $type!='media_directors'){
						$updateDevice='UPDATE Device SET Description=?, FK_Room=?, FK_Device_ControlledVia=? '.@$updateMacIp.' WHERE PK_Device=?';
						$dbADO->Execute($updateDevice,array($description,$room,$controlledBy,$value));
					}else{
						if($type=='media_directors'){
							$updateDevice='UPDATE Device SET Description=?, FK_Room=? '.@$updateMacIp.' WHERE PK_Device=?';
							$dbADO->Execute($updateDevice,array($description,$room,$value));
						}else{
							$updateDevice='UPDATE Device SET Description=? '.@$updateMacIp.' WHERE PK_Device=?';
							$dbADO->Execute($updateDevice,array($description,$value));
						}
					}
					foreach($DeviceDataToDisplayArray as $ddValue){
						$deviceData=(isset($_POST['deviceData_'.$value.'_'.$ddValue]))?$_POST['deviceData_'.$value.'_'.$ddValue]:0;
						$oldDeviceData=@$_POST['oldDeviceData_'.$value.'_'.$ddValue];
						if($oldDeviceData!=$deviceData){
							if($oldDeviceData=='NULL'){
								$insertDDD='
									INSERT INTO Device_DeviceData 
										(FK_Device, FK_DeviceData, IK_DeviceData)
									VALUES 
										(?,?,?)';
								$dbADO->Execute($insertDDD,array($value,$ddValue,$deviceData));
							}
							else{
								$updateDDD='
									UPDATE Device_DeviceData 
										SET IK_DeviceData=? 
									WHERE FK_Device=? AND FK_DeviceData=?';
								$dbADO->Execute($updateDDD,array($deviceData,$value,$ddValue));
							}
						}
					}


					processReceiver($value,$dbADO);
				}
			}

			$commandToSend='/usr/pluto/bin/UpdateEntArea -h localhost';
			exec($commandToSend);
		}
		processRemotes($dbADO);

		if(isset($_REQUEST['add'])){
			unset($_SESSION['from']);
			$deviceTemplate=(int)$_REQUEST['deviceTemplate'];
			if($deviceTemplate!=0){
				$insertID=exec('sudo -u root /usr/pluto/bin/CreateDevice -h localhost -D '.$dbPlutoMainDatabase.' -d '.$deviceTemplate.' -i '.$installationID,$ret);
				setDCERouterNeedConfigure($_SESSION['installationID'],$dbADO);
				$commandToSend='/usr/pluto/bin/UpdateEntArea -h localhost';
				exec($commandToSend);

				if(isInfrared($deviceTemplate,$dbADO)){
					// add video port
					$dbADO->Execute('INSERT INTO Device_DeviceData (FK_Device, FK_DeviceData) VALUES (?,?)',array($insertID,$GLOBALS['InfraredPort']));
				}
			}
			header("Location: index.php?section=avWizard&type=$type&lastAdded=$deviceTemplate#deviceLink_".@$insertID);
			exit();
		}


		header("Location: index.php?section=avWizard&msg=The devices was updated&type=$type".@$anchor);
	}

	$output->setScriptCalendar('null');

	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.((isset($title))?' :: '.$title:' :: '.strtoupper(str_replace('_',' ',$type))));
	$output->output();
}

function getSubDT($mdID,$categoryID,$dbADO)
{
	$res=$dbADO->Execute('
		SELECT FK_DeviceTemplate
		FROM Device 
		INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
		WHERE FK_DeviceCategory=? AND FK_Device_ControlledVia=?',array($categoryID,$mdID));
	if($res->RecordCount()>0){
		$row=$res->FetchRow();
		return $row['FK_DeviceTemplate'];
	}else{
		return 0;
	}
}

function getSubDevice($mdID,$categoryID,$dbADO)
{
	$res=$dbADO->Execute('
		SELECT PK_Device
		FROM Device 
		INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
		WHERE FK_DeviceCategory=? AND FK_Device_ControlledVia=?',array($categoryID,$mdID));
	if($res->RecordCount()>0){
		$row=$res->FetchRow();
		return $row['PK_Device'];
	}else{
		return 0;
	}
}

function recreateDevice($mdID,$categoryID,$dtID,$installationID,$roomID,$dbADO)
{
	$deviceID=getSubDevice($mdID,$categoryID,$dbADO);
	if($deviceID!=0){
		deleteDevice($deviceID,$dbADO);
	}
	if($dtID!=0){
		createDevice($dtID,$installationID,$mdID,$roomID,$dbADO,1);
	}
}
?>
