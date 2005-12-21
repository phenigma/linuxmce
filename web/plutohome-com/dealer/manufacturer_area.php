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
	$dtArray=getAssocArray('DeviceTemplate','PK_DeviceTemplate','Description',$dbADO,'','ORDER BY Description ASC'); 

if($action=='form'){
	$registeredDevices='';
	$rdArray=array();

	$res=$manufacturerADO->Execute('
		SELECT RegisteredDevice_DeviceTemplate.*, PK_RegisteredDevice,SerialNumber,ModelNumber,MacAddressFrom,MacAddressTo
		FROM RegisteredDevice_DeviceTemplate 
		INNER JOIN RegisteredDevice ON FK_RegisteredDevice=PK_RegisteredDevice 
		WHERE FK_Manufacturer=?',$manufacturerID);
	$registered=array();
	$rdDTArray=array();
	while($row=$res->FetchRow()){
		$rdArray[$row['PK_RegisteredDevice']]['MacAddressFrom']=$row['MacAddressFrom'];
		$rdArray[$row['PK_RegisteredDevice']]['MacAddressTo']=$row['MacAddressTo'];
		$rdArray[$row['PK_RegisteredDevice']]['SerialNumber']=$row['SerialNumber'];
		$rdArray[$row['PK_RegisteredDevice']]['ModelNumber']=$row['ModelNumber'];
		$rdArray[$row['PK_RegisteredDevice']]['EK_DeviceTemplate'][]=$row['EK_DeviceTemplate'];
		$rdArray[$row['PK_RegisteredDevice']]['InheritMac'][]=$row['InheritMac'];
		$rdArray[$row['PK_RegisteredDevice']]['MacAddress'][]=$row['MacAddress'];
		$rdArray[$row['PK_RegisteredDevice']]['PK_RegisteredDevice_DeviceTemplate'][]=$row['PK_RegisteredDevice_DeviceTemplate'];
		$rdDTArray[]=$row['PK_RegisteredDevice_DeviceTemplate'];
	}
	
	$registeredDevices='
		<table cellpadding="3" cellspacing="0" style="border-style: solid;border-color: #CCCCCC;border-width: 1px;">
			<tr bgcolor="lightblue">
				<td align="center"><B>MAC Address</B></td>
				<td align="center"><B>SN / Model No.</B></td>
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
	
	$line=0;
	foreach ($rdArray AS $rdID=>$arr){
		$devicesPerLine=count($arr['EK_DeviceTemplate']);
		$color=($line%2==0)?'#F6F6F6':'#FFFFFF';
		$line++;
		$registeredDevices.='
			<tr bgcolor="'.$color.'">
				<td rowspan="'.($devicesPerLine+1).'" align="right">
					From: <input type="text" name="macAddressFrom_'.$rdID.'" value="'.IntToMac($arr['MacAddressFrom']).'" class="textfield"><br>
					To: <input type="text" name="macAddressTo_'.$rdID.'" value="'.IntToMac($arr['MacAddressTo']).'" class="textfield"></td>
				<td rowspan="'.($devicesPerLine+1).'" align="right">
					SN: <input type="text" name="serialNumber_'.$rdID.'" value="'.$arr['SerialNumber'].'" class="textfield"><br>
					MN: <input type="text" name="modelNumber_'.$rdID.'" value="'.$arr['ModelNumber'].'" class="textfield"></td>
				<td>'.pulldownFromArray($dtArray,'deviceTemplate_'.$arr['PK_RegisteredDevice_DeviceTemplate'][0],$arr['EK_DeviceTemplate'][0],'style="width:100px;"').' 
					<input type="hidden" name="old_dt_'.$arr['PK_RegisteredDevice_DeviceTemplate'][0].'" value="'.$arr['EK_DeviceTemplate'][0].'"> 
					<input type="hidden" name="rd_'.$arr['PK_RegisteredDevice_DeviceTemplate'][0].'" value="'.$rdID.'">
				</td>
				<td align="center"><input type="radio" name="inherit_'.$rdID.'" value="'.$arr['PK_RegisteredDevice_DeviceTemplate'][0].'" '.(($arr['InheritMac'][0]==1)?'checked':'').'></td>
				<td><input type="text" name="macAddress_'.$arr['PK_RegisteredDevice_DeviceTemplate'][0].'" value="'.$arr['MacAddress'][0].'" class="textfield"></td>
				<td align="center"><input type="button" name="dd" value="Device Data" onClick="self.location=\'index.php?section=registeredDeviceData&rdDT='.$arr['PK_RegisteredDevice_DeviceTemplate'][0].'\'"> <input type="button" name="del_'.$rdID.'" value="Delete" onclick="if(confirm(\'Are you sure you want to remove this device?\'))self.location=\'index.php?section=manufacturer_area&did='.$arr['PK_RegisteredDevice_DeviceTemplate'][0].'&action=del\'"></td>
			</tr>';
		for($i=1;$i<$devicesPerLine;$i++){
		$registeredDevices.='
			<tr bgcolor="'.$color.'">
				<td>'.pulldownFromArray($dtArray,'deviceTemplate_'.$arr['PK_RegisteredDevice_DeviceTemplate'][$i],$arr['EK_DeviceTemplate'][$i],'style="width:100px;"').' 
					<input type="hidden" name="old_dt_'.$arr['PK_RegisteredDevice_DeviceTemplate'][$i].'" value="'.$arr['EK_DeviceTemplate'][$i].'">
					<input type="hidden" name="rd_'.$arr['PK_RegisteredDevice_DeviceTemplate'][$i].'" value="'.$rdID.'">
				</td>
				<td align="center"><input type="radio" name="inherit_'.$rdID.'" value="'.$arr['PK_RegisteredDevice_DeviceTemplate'][$i].'" '.(($arr['InheritMac'][$i]==1)?'checked':'').'></td>
				<td><input type="text" name="macAddress_'.$arr['PK_RegisteredDevice_DeviceTemplate'][$i].'" value="'.$arr['MacAddress'][$i].'" class="textfield"></td>
				<td align="center"><input type="button" name="dd" value="Device Data" onClick="self.location=\'index.php?section=registeredDeviceData&rdDT='.$arr['PK_RegisteredDevice_DeviceTemplate'][$i].'\'"> <input type="button" name="del_'.$rdID.'" value="Delete" onclick="if(confirm(\'Are you sure you want to remove this device?\'))self.location=\'index.php?section=manufacturer_area&did='.$arr['PK_RegisteredDevice_DeviceTemplate'][$i].'&action=del\'"></td>
			</tr>';
		}
		
		$registeredDevices.='
			<tr bgcolor="'.$color.'"s>
				<td>'.pulldownFromArray($dtArray,'deviceTemplate_new_'.$rdID,0,'style="width:100px;"').'</td>
				<td align="center"><input type="radio" name="inherit_'.$rdID.'" value="new"></td>
				<td><input type="text" name="macAddress_new_'.$rdID.'" value="" class="textfield"></td>
				<td align="center"><input type="submit" name="add_embedded_'.$rdID.'" value="Add device"></td>
			</tr>		
		';
	}
	$submitBtn=(count($rdArray)>0)?'<input type="submit" name="update" value="Update">':'';
	
	$registeredDevices.='</table><div align="center">'.$submitBtn.'</div>
	<input type="hidden" name="registered" value="'.join(',',array_keys($rdArray)).'">
	<input type="hidden" name="rdDTArray" value="'.join(',',$rdDTArray).'">
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

		if($dt==0){
			header('Location: index.php?section=manufacturer_area&err=You must select a device template.');
			exit();
		}
		
		if(!isMacRange($macAddress)){
			header('Location: index.php?section=manufacturer_area&err=The mac address or range is invalid.');
			exit();
		}

		if(macExist($macAddress,$manufacturerADO)){
			header('Location: index.php?section=manufacturer_area&err=The mac address or range is already in database.');
			exit();
		}
		
		if(strpos($macAddress,'-')!==false){
			$parts=explode('-',$macAddress);
			$macAddressFrom=convertMac(trim($parts[0]));
			$macAddressTo=convertMac(trim($parts[1]));
			if($macAddressFrom>$macAddressTo){
				$tmp=$macAddressFrom;
				$macAddressFrom=$macAddressTo;
				$macAddressTo=$tmp;
			}
		}else{
			$macAddressFrom=convertMac($macAddress);
			$macAddressTo=convertMac($macAddress);
		}
		
		$manufacturerADO->Execute('INSERT INTO RegisteredDevice (MacAddressFrom,MacAddressTo,SerialNumber,ModelNumber,FK_Manufacturer) VALUES (?,?,?,?,?)',array($macAddressFrom,$macAddressTo,$serialNumber,$modelID,$manufacturerID)); 		
		$rdID=$manufacturerADO->Insert_ID();
		$manufacturerADO->Execute('INSERT INTO RegisteredDevice_DeviceTemplate (FK_RegisteredDevice,InheritMac,MacAddress,EK_DeviceTemplate) VALUES (?,1,NULL,?)',array($rdID,$dt)); 		
		$rdDT=$manufacturerADO->Insert_ID();
		add_dt_devicedata($rdDT,$dt,$manufacturerADO,$dbADO);
		$msg='The MAC address was registered.';
	}

	if(isset($_POST['registered']) && $_POST['registered']!=''){
		$registered=explode(',',@$_POST['registered']);
		if(count($registered)!=0){
			foreach ($registered AS $rdID){
				// update existing records
				$serialNumber=$_POST['serialNumber_'.$rdID];
				$modelNumber=$_POST['modelNumber_'.$rdID];
				$macAddressFrom=trim($_REQUEST['macAddressFrom_'.$rdID]);
				$macAddressTo=trim($_REQUEST['macAddressTo_'.$rdID]);
				if($macAddressFrom!=$macAddressTo){
					$newMac=$macAddressFrom.'-'.$macAddressTo;
				}else{
					$newMac=$macAddressFrom;
				}
				if(!isMacRange($newMac)){
					header('Location: index.php?section=manufacturer_area&err=The mac address or range is invalid.');
					exit();
				}

				if(macExist($newMac,$manufacturerADO,$rdID)){
					header('Location: index.php?section=manufacturer_area&err=The mac address or range is already in database.');
					exit();
				}

				$macAddressFrom=convertMac($macAddressFrom);
				$macAddressTo=convertMac($macAddressTo);
				if($macAddressFrom>$macAddressTo){
					$tmp=$macAddressFrom;
					$macAddressFrom=$macAddressTo;
					$macAddressTo=$tmp;
				}
				
				$manufacturerADO->Execute('UPDATE RegisteredDevice SET MacAddressFrom=?, MacAddressTo=?,SerialNumber=?,ModelNumber=? WHERE PK_RegisteredDevice=?',array($macAddressFrom,$macAddressTo,$serialNumber,$modelNumber,$rdID));
				$msg='The registered devices were updated.';
				
				// add new device template
				$newMAC=($_POST['macAddress_new_'.$rdID]!='')?$_POST['macAddress_new_'.$rdID]:NULL;
				$inherit=($_POST['inherit_'.$rdID]=='new')?1:0;
				$dt=$_POST['deviceTemplate_new_'.$rdID];
				if($dt!=0){
					if($inherit==1){
						$manufacturerADO->Execute('UPDATE RegisteredDevice_DeviceTemplate SET InheritMac=0 WHERE FK_RegisteredDevice=?',array($rdID));
					}
					if(!is_null($newMAC) && !checkMAC($newMAC)){
						header('Location: index.php?section=manufacturer_area&err=Invalid MAC address.');
						exit();
					}
					$manufacturerADO->Execute('INSERT INTO RegisteredDevice_DeviceTemplate (FK_RegisteredDevice,InheritMac,MacAddress,EK_DeviceTemplate) VALUES (?,?,?,?)',array($rdID,$inherit,$newMAC,$dt)); 		
					$rdDT=$manufacturerADO->Insert_ID();
					add_dt_devicedata($rdDT,$dt,$manufacturerADO,$dbADO);
					$msg='The embedded device was added to registered devices.';
				}
			}

			$rdDTArray=explode(',',$_POST['rdDTArray']);
			foreach ($rdDTArray AS $rdDT){
				$newMAC=($_POST['macAddress_'.$rdDT]!='')?$_POST['macAddress_'.$rdDT]:NULL;
				$dt=$_POST['deviceTemplate_'.$rdDT];
				$old_dt=$_POST['old_dt_'.$rdDT];
				$rdID=$_POST['rd_'.$rdDT];
				$inherit=($_POST['inherit_'.$rdID]==$rdDT)?1:0;

				if(!is_null($newMAC) && !checkMAC($newMAC)){
					header('Location: index.php?section=manufacturer_area&err=Invalid MAC address.');
					exit();
				}			
				$manufacturerADO->Execute('UPDATE RegisteredDevice_DeviceTemplate SET EK_DeviceTemplate=?,MacAddress=?,InheritMac=? WHERE PK_RegisteredDevice_DeviceTemplate=?',array($dt,$newMAC,$inherit,$rdDT));
				if($dt!=$old_dt){
					$manufacturerADO->Execute('DELETE FROM RegisteredDevice_DeviceTemplate_DeviceData WHERE FK_RegisteredDevice_DeviceTemplate=?',array($rdDT));
					add_dt_devicedata($rdDT,$dt,$manufacturerADO,$dbADO);
					$msg='Updated.';
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


function macExist($macAddress,$manufacturerADO,$rdID=0){
	// todo later
	return false;
	$rdFilter=($rdID!=0)?' AND PK_RegisteredDevice!='.$rdID:'';
	$macArr=getAssocArray('RegisteredDevice','MacAddress','PK_RegisteredDevice',$manufacturerADO,'WHERE MacAddress=\''.$macAddress.'\''.$rdFilter);
	if(count($macArr)>0){
		return true;
	}
	
	return false;
}

?>