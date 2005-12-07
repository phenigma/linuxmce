<?
if(!isset($_SESSION['userIsLogged'])){
	header('Location: index.php?section=login');
	exit();
}
function manufacturer_area($output,$dbADO) {
	/* @var $dbADO ADOConnection */
	global $dbManufacturerServer, $dbManufacturerUser, $dbManufacturerPass, $dbManufacturerDatabase;
	
	if ($_SESSION['userIsLogged']!="yes") {
		header("Location: index.php");
	}
	
	$FK_Users=(isset($_SESSION['userID']))?(int)$_SESSION['userID']:0;
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';

	$manufacturerADO=dbConnection($dbManufacturerServer,$dbManufacturerUser,$dbManufacturerPass,$dbManufacturerDatabase);
	$manufacturerUser=getFieldsAsArray('Manufacturer_Users','CanEditProfile,EK_Manufacturer,EK_Users',$manufacturerADO,'WHERE EK_Users='.$FK_Users);	
	
	if(count($manufacturerUser['EK_Manufacturer'][0])==0){
		header('Location: index.php');
		exit();
	}else{
		$manufacturerID=$manufacturerUser['EK_Manufacturer'][0];
	}	
	$dtArray=getAssocArray('DeviceTemplate','PK_DeviceTemplate','Description',$dbADO,'WHERE FK_Manufacturer='.$manufacturerID,'ORDER BY Description ASC'); 

if($action=='form'){
	$registeredDevices='';
	$rdArray=array();
	$res=$manufacturerADO->Execute('SELECT * FROM RegisteredDevice_DeviceTemplate INNER JOIN RegisteredDevice ON FK_RegisteredDevice=PK_RegisteredDevice WHERE FK_Manufacturer=?',$manufacturerID);
	$registered=array();
	while($row=$res->FetchRow()){
		$rdArray[$row['PK_RegisteredDevice']]['MainMacAddress']=$row['MacAddress'];
		$rdArray[$row['PK_RegisteredDevice']]['SerialNumber']=$row['SerialNumber'];
		$rdArray[$row['PK_RegisteredDevice']]['ModelNumber']=$row['ModelNumber'];
		$rdArray[$row['PK_RegisteredDevice']]['EK_DeviceTemplate'][]=$row['EK_DeviceTemplate'];
		$rdArray[$row['PK_RegisteredDevice']]['InheritMac'][]=$row['InheritMac'];
		$rdArray[$row['PK_RegisteredDevice']]['MacAddress'][]=$row['MacAddress'];
		$rdArray[$row['PK_RegisteredDevice']]['PK_RegisteredDevice_DeviceTemplate'][]=$row['PK_RegisteredDevice_DeviceTemplate'];
	}
	
	$registeredDevices='
		<table border="1" cellpadding="3" style="border-collapse: collapse" bordercolor="#111111">
			<tr bgcolor="lightblue">
				<td><B>MAC Address</B></td>
				<td><B>Serial Number</B></td>
				<td><B>Model Number</B></td>
				<td><B>Device Template</B></td>
				<td align="center"><B>Inherit MAC Address</B></td>
				<td align="center"><B>MAC Address</B></td>
				<td align="center"><B>Actions</B></td>
			</tr>';
	if(count($rdArray)==0){
		$registeredDevices.='
			<tr>
				<td colspan="7" align="center">No registered devices.</td>
			</tr>';
	}
	
	foreach ($rdArray AS $rdID=>$arr){
		$devicesPerLine=count($arr['EK_DeviceTemplate']);
		$registeredDevices.='
			<tr>
				<td rowspan="'.($devicesPerLine+1).'">'.$arr['MainMacAddress'].'</td>
				<td rowspan="'.($devicesPerLine+1).'">'.$arr['SerialNumber'].'</td>
				<td rowspan="'.($devicesPerLine+1).'">'.$arr['ModelNumber'].'</td>
				<td>'.$dtArray[$arr['EK_DeviceTemplate'][0]].'</td>
				<td align="center"><input type="radio" name="inherit_'.$rdID.'" value="'.$arr['PK_RegisteredDevice_DeviceTemplate'][0].'" '.(($arr['InheritMac'][0]==1)?'checked':'').'></td>
				<td>'.$arr['MacAddress'][0].'</td>
				<td align="center"><input type="button" name="dd" value="Device Data" onClick="self.location=\'index.php?section=registeredDeviceData&rdDT='.$arr['PK_RegisteredDevice_DeviceTemplate'][0].'\'"> <input type="button" name="del_'.$rdID.'" value="Delete" onclick="if(confirm(\'Are you sure you want to remove this device?\'))self.location=\'index.php?section=manufacturer_area&did='.$arr['PK_RegisteredDevice_DeviceTemplate'][0].'&action=del\'"></td>
			</tr>';
		for($i=1;$i<$devicesPerLine;$i++){
		$registeredDevices.='
			<tr>
				<td>'.$dtArray[$arr['EK_DeviceTemplate'][$i]].'</td>
				<td align="center"><input type="radio" name="inherit_'.$rdID.'" value="'.$arr['PK_RegisteredDevice_DeviceTemplate'][$i].'" '.(($arr['InheritMac'][$i]==1)?'checked':'').'></td>
				<td>'.$arr['MacAddress'][$i].'</td>
				<td align="center"><input type="button" name="dd" value="Device Data" onClick="self.location=\'index.php?section=registeredDeviceData&rdDT='.$arr['PK_RegisteredDevice_DeviceTemplate'][$i].'\'"> <input type="button" name="del_'.$rdID.'" value="Delete" onclick="if(confirm(\'Are you sure you want to remove this device?\'))self.location=\'index.php?section=manufacturer_area&did='.$arr['PK_RegisteredDevice_DeviceTemplate'][$i].'&action=del\'"></td>
			</tr>';
		}
		
		$registeredDevices.='
			<tr>
				<td>'.pulldownFromArray($dtArray,'deviceTemplate_new_'.$rdID,0,'style="width:100px;"').'</td>
				<td align="center"><input type="radio" name="inherit_'.$rdID.'" value="new"></td>
				<td><input type="text" name="macAddress_new_'.$rdID.'" value="" class="textfield"></td>
				<td><input type="submit" name="add_embedded_'.$rdID.'" value="Add device"></td>
			</tr>		
		';
	}
	$registeredDevices.='</table>
	<input type="hidden" name="registered" value="'.join(',',array_keys($rdArray)).'">
	';
	
	$errMsg=(isset($_REQUEST['err']))?'<span class="err">'.stripslashes($_REQUEST['err']).'</span>':'';
	$msg=(isset($_REQUEST['msg']))?'<span class="confirm">'.stripslashes($_REQUEST['msg']).'</span>':'';
	
	$out = '
		<form action="index.php" method="POST" name="manufacturer_area">
			<input type="hidden" name="section" value="manufacturer_area">
			<input type="hidden" name="action" value="add">		
	
      		<table width="100%" cellpadding="3" cellspacing="0">
	      		<tr>
      				<td align="left" colspan="2" class="insidetable"><B>Manufacturer area</B> </td>
				</tr>
				<tr>
					<td class="normaltext" align="right">&nbsp;'.(($manufacturerUser['CanEditProfile'][0]==1)?'<a href="index.php?section=edit_manufacturer_profile">Edit my profile</a>':'').'</td>
      			</tr>	
				<tr>
					<td class="normaltext" align="center">&nbsp;'.@$msg.@$errMsg.'</td>
      			</tr>	
      			<tr bgcolor="#DADDE4">
      				<td align="center" width="33%" class="normaltext"><b>Registered devices</b></td>
      			</tr>
      			<tr>
      				<td width="33%" class="normaltext" align="center">'.$registeredDevices.'</td>
      			</tr>
      			<tr>
      				<td width="33%" class="normaltext" align="center"></td>
      			</tr>	
      			<tr>
      				<td width="33%" class="normaltext" align="center">&nbsp;</td>
      			</tr>	
				<tr bgcolor="#DADDE4">
      				<td align="center" width="33%" class="normaltext"><b>Register device</b></td>
      			</tr>	
				<tr>
      				<td align="center" width="33%" class="normaltext"><table>
						<tr>
							<td><B>Device Template</B></td>
							<td>'.pulldownFromArray($dtArray,'deviceTemplate',0).'</td>
						</tr>
						<tr>
							<td><B>MAC Address</B></td>
							<td><input type="text" name="macAddress" value="" class="textfield"></td>
						</tr>
						<tr>
							<td><B>Serial number</B></td>
							<td><input type="text" name="serialNumber" value="" class="textfield"></td>
						</tr>
						<tr>
							<td><B>Model ID</B></td>
							<td><input type="text" name="modelID" value="" class="textfield"></td>
						</tr>
						<tr>
							<td colspan="2" align="center"><input type="submit" name="add" value="Register device"></td>
						</tr>
					</table>
					</td>
      			</tr>	
	
			</table>
		</form>
      	';
}else{
	
	if(isset($_POST['add'])){
		$dt=$_POST['deviceTemplate'];
		$macAddress=$_POST['macAddress'];
		$serialNumber=$_POST['serialNumber'];
		$modelID=$_POST['modelID'];
		
		$manufacturerADO->Execute('INSERT INTO RegisteredDevice (MacAddress,SerialNumber,ModelNumber,FK_Manufacturer) VALUES (?,?,?,?)',array($macAddress,$serialNumber,$modelID,$manufacturerID)); 		
		$rdID=$manufacturerADO->Insert_ID();
		$manufacturerADO->Execute('INSERT INTO RegisteredDevice_DeviceTemplate (FK_RegisteredDevice,InheritMac,MacAddress,EK_DeviceTemplate) VALUES (?,1,?,?)',array($rdID,$macAddress,$dt)); 		
		$rdDT=$manufacturerADO->Insert_ID();
		add_dt_devicedata($rdDT,$dt,$manufacturerADO,$dbADO);
		$msg='The MAC address was registered.';
	}

	if(isset($_POST['registered']) && $_POST['registered']!=''){
		$registered=explode(',',@$_POST['registered']);
		if(count($registered)!=0){
			foreach ($registered AS $rdID){
				$newMAC=$_POST['macAddress_new_'.$rdID];
				$inherit=($_POST['inherit_'.$rdID]=='new')?1:0;
				$dt=$_POST['deviceTemplate_new_'.$rdID];
				if($dt!=0){
					if($newMAC!=''){
						$manufacturerADO->Execute('INSERT INTO RegisteredDevice_DeviceTemplate (FK_RegisteredDevice,InheritMac,MacAddress,EK_DeviceTemplate) VALUES (?,?,?,?)',array($rdID,$inherit,$newMAC,$dt)); 		
						$msg='The embedded device was added to registered device.';
					}elseif($inherit==1){
						$manufacturerADO->Execute('UPDATE RegisteredDevice_DeviceTemplate SET InheritMac=0 WHERE FK_RegisteredDevice=?',array($rdID)); 		
						$manufacturerADO->Execute('INSERT INTO RegisteredDevice_DeviceTemplate (FK_RegisteredDevice,InheritMac,MacAddress,EK_DeviceTemplate) SELECT ?,1,MacAddress,? FROM RegisteredDevice WHERE PK_RegisteredDevice=?',array($rdID,$dt,$rdID)); 		
						$rdDT=$manufacturerADO->Insert_ID();
						add_dt_devicedata($rdDT,$dt,$manufacturerADO,$dbADO);
						$msg='The embedded device was added to registered device.';
						
					}else{
						$err='Please fill in the MAC address for the embedded device.';
					}
				}
			}
		}
	}
	
	if(isset($_REQUEST['did']) && (int)@$_REQUEST['did']!=0){
		$did=(int)@$_REQUEST['did'];
		$ownRecord=getAssocArray('RegisteredDevice_DeviceTemplate','PK_RegisteredDevice_DeviceTemplate','PK_RegisteredDevice',$manufacturerADO,'INNER JOIN RegisteredDevice ON FK_RegisteredDevice=PK_RegisteredDevice WHERE PK_RegisteredDevice_DeviceTemplate='.$did.' AND FK_Manufacturer='.$manufacturerID);
		if(count($ownRecord)==0){
			$err='Operation is not allowed';
		}else{
			$manufacturerADO->Execute('DELETE FROM RegisteredDevice_DeviceTemplate WHERE PK_RegisteredDevice_DeviceTemplate=?',$did);
			
			// check if it's the last device and remove record from RegisteredDevice if so
			$res=$manufacturerADO->Execute('SELECT * FROM RegisteredDevice_DeviceTemplate WHERE FK_RegisteredDevice=?',$ownRecord[$did]);
			if($res->RecordCount()==0){
				$manufacturerADO->Execute('DELETE FROM RegisteredDevice WHERE PK_RegisteredDevice=?',$ownRecord[$did]);
			}
			$msg='The device was removed.';
		}
	}
	
	$sufix=(@$err!='')?'&err='.$err:'&msg='.@$msg;
	header('Location: index.php?section=manufacturer_area'.$sufix);
	exit();
}


	$output->setNavigationMenu(array("My Pluto"=>"index.php?section=myPluto","Manufacturer Area"=>"index.php?section=manufacturer_area"));

	$output->setScriptCalendar('null');
	$output->setScriptTRColor('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME." :: Manufacturer area");
	$output->output();
}

function add_dt_devicedata($rdDT,$dtID,$manufacturerADO,$dbADO){
	$ddArray=getAssocArray('DeviceTemplate_DeviceData','FK_DeviceData','IK_DeviceData',$dbADO,'WHERE FK_DeviceTemplate='.$dtID);
	foreach ($ddArray AS $dd=>$ik){
		$manufacturerADO->Execute('INSERT INTO RegisteredDevice_DeviceTemplate_DeviceData (FK_RegisteredDevice_DeviceTemplate,EK_DeviceData,IK_DeviceData) VALUES (?,?,?)',array($rdDT,$dd,$ik));
	}
}
?>