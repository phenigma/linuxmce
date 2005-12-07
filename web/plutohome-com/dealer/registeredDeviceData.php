<?
if(!isset($_SESSION['userIsLogged'])){
	header('Location: index.php?section=login');
	exit();
}
function registeredDeviceData($output,$dbADO) {
	/* @var $dbADO ADOConnection */
	global $dbManufacturerServer, $dbManufacturerUser, $dbManufacturerPass, $dbManufacturerDatabase;
	
	if ($_SESSION['userIsLogged']!="yes") {
		header("Location: index.php");
	}
	
	$FK_Users=(isset($_SESSION['userID']))?(int)$_SESSION['userID']:0;
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$rdDT=$_REQUEST['rdDT'];
	
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
	$currentValues=getAssocArray('RegisteredDevice_DeviceTemplate_DeviceData','EK_DeviceData','IK_DeviceData',$manufacturerADO,'WHERE FK_RegisteredDevice_DeviceTemplate='.$rdDT);
	if(count($currentValues)>0){	
		$ddArray=getAssocArray('DeviceData','PK_DeviceData','Description',$dbADO,'WHERE PK_DeviceData IN ('.join(',',array_keys($currentValues)).')','ORDER BY description ASC');
		
		$table='
		<br>
		<table border="1" cellpadding="3" style="border-collapse: collapse" bordercolor="#111111" align="center" class="insidetable">
			<tr bgcolor="lightblue">
				<td><B>Device data</B></td>
				<td><B>Value</B></td>
			</tr>';
		foreach ($currentValues AS $ddID=>$ddValue){
			$table.='
			<tr>
				<td>'.$ddArray[$ddID].'</td>
				<td><input type="text" name="dd_'.$ddID.'" value="'.$ddValue.'"></td>
			</tr>';
		}
		$table.='</table>';
		$table.='<div align="center"><input type="submit" name="update" value="Update"></div>
		<input type="hidden" name="ddArray" value="'.join(',',array_keys($currentValues)).'">';
	}else{
		$table='This device does not have any device data.';
	}
	
	$errMsg=(isset($_REQUEST['err']))?'<span class="err">'.stripslashes($_REQUEST['err']).'</span>':'';
	$msg=(isset($_REQUEST['msg']))?'<span class="confirm">'.stripslashes($_REQUEST['msg']).'</span>':'';
	
	$out = '
		<form action="index.php" method="POST" name="registeredDeviceData">
			<input type="hidden" name="section" value="registeredDeviceData">
			<input type="hidden" name="action" value="add">		
			<input type="hidden" name="rdDT" value="'.$rdDT.'">		
			
			<h3 align="center">Edit Device Data</h3>
			'.$table.'
		</form>
      	';
}else{

	$ddArray=explode(',',$_POST['ddArray']);
	foreach ($ddArray AS $ddID){
		$ddValue=str_replace("\t","",$_POST['dd_'.$ddID]);
		$manufacturerADO->Execute('UPDATE RegisteredDevice_DeviceTemplate_DeviceData SET IK_DeviceData=? WHERE FK_RegisteredDevice_DeviceTemplate=? AND EK_DeviceData=?',array($ddValue,$rdDT,$ddID));
	}
	$msg='The device data were updated.';
	
	$sufix=(@$err!='')?'&err='.$err:'&msg='.@$msg;
	header('Location: index.php?section=registeredDeviceData&rdDT='.$rdDT.$sufix);
	exit();
}


	$output->setNavigationMenu(array("My Pluto"=>"index.php?section=myPluto","Manufacturer Area"=>"index.php?section=manufacturer_area","Device Data for Registered device"=>"index.php?section=registeredDeviceData"));

	$output->setScriptCalendar('null');
	$output->setScriptTRColor('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME." :: Manufacturer area");
	$output->output();
}
?>