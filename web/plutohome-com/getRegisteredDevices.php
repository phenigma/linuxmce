<?php
require('include/config/config.inc.php');
require('include/template.class.inc.php');
require('include/utils.inc.php');
$separator="\t";
$manufacturerADO=dbConnection($dbManufacturerServer,$dbManufacturerUser,$dbManufacturerPass,$dbManufacturerDatabase);

if(isset($_REQUEST['MAC']) && !isset($_REQUEST['PK'])){
	$mac=@$_REQUEST['MAC'];
	if($mac==''){
		die("FAIL\tMAC address not provided");
	}
	if(!isMacRange($mac)){
		die("FAIL\tInvalid MAC address");
	}
	
	$macInt=convertMac($mac);
	$res=$manufacturerADO->Execute('
		SELECT RegisteredDevice_DeviceTemplate.*,RegisteredDevice.*
		FROM RegisteredDevice_DeviceTemplate
		INNER JOIN RegisteredDevice ON FK_RegisteredDevice=PK_RegisteredDevice
		WHERE MacAddressFrom<=? AND MacAddressTo >=? AND InheritMac=1',array($macInt,$macInt));
	if($res->RecordCount()==0){
		die('FAIL\tMac address not found');
	}
	
	$row=$res->FetchRow();
	$dt=$row['EK_DeviceTemplate'];
	$pkRD=$row['PK_RegisteredDevice'];
	
	if($dt==0){
		die('FAIL\tInvalid device template');
	}
	$dtArray=getAssocArray('DeviceTemplate','PK_DeviceTemplate','Description',$dbADO,'WHERE Pk_DeviceTemplate='.$dt);
	if(!isset($dtArray[$dt])){
		die('Device template #'.$dt.' does not exist.');
	}
	
	$answer="$pkRD\t".$dtArray[$dt];
	
}elseif(isset($_REQUEST['MAC']) && isset($_REQUEST['PK'])){
	$pk=$_REQUEST['PK'];
	$mac=$_REQUEST['MAC'];
	
	if($pk==0){
		die('FAIL\tInvalid PK parameter.');
	}
	
	$res=$manufacturerADO->Execute('
		SELECT RegisteredDevice_DeviceTemplate.*,RegisteredDevice_DeviceTemplate_DeviceData.*,RegisteredDevice.*
		FROM RegisteredDevice_DeviceTemplate
		INNER JOIN RegisteredDevice ON FK_RegisteredDevice=PK_RegisteredDevice
		INNER JOIN RegisteredDevice_DeviceTemplate_DeviceData ON FK_RegisteredDevice_DeviceTemplate=PK_RegisteredDevice_DeviceTemplate
		WHERE PK_RegisteredDevice=?
		ORDER BY InheritMac DESC',array($pk));
	
	if($res->RecordCount()==0){
		die("FAIL\tNo records");
	}
	
	// answer format
	// [device template]$separator[mac address]$separator[EK_DeviceData]$separator[IK_DeviceData] ... \n
	$answer="OK\t";
	$pos=0;
	while($row=$res->FetchRow()){
		if($row['PK_RegisteredDevice_DeviceTemplate']!=$pos){
			$answer.=($row['InheritMac']==1)?(($pos!=0)?"\n":'').$row['SerialNumber'].$separator.$row['ModelNumber']:'';	
			$answer.="\n".$row['EK_DeviceTemplate'].$separator.(($row['InheritMac']==1)?$mac:$row['MacAddress']);	
			$pos=$row['PK_RegisteredDevice_DeviceTemplate'];
		}
		$answer.=$separator.$row['EK_DeviceData']."\t".$row['IK_DeviceData']."";
	}

}else{
	die('No parameters specified');
}


print trim($answer);
?>
