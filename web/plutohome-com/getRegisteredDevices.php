<?php
require('include/config/config.inc.php');
require('include/template.class.inc.php');
require('include/utils.inc.php');

$separator="\t";
$manufacturerADO=dbConnection($dbManufacturerServer,$dbManufacturerUser,$dbManufacturerPass,$dbManufacturerDatabase);

// todo: retrive manufacturer id
$manufacturerID=1;

$res=$manufacturerADO->Execute('
	SELECT RegisteredDevice_DeviceTemplate.*,RegisteredDevice_DeviceTemplate_DeviceData.*
	FROM RegisteredDevice_DeviceTemplate
	INNER JOIN RegisteredDevice ON FK_RegisteredDevice=PK_RegisteredDevice
	INNER JOIN RegisteredDevice_DeviceTemplate_DeviceData ON FK_RegisteredDevice_DeviceTemplate=PK_RegisteredDevice_DeviceTemplate
	WHERE FK_Manufacturer=?
',$manufacturerID);

// answer format
// [device template]$separator[mac address]$separator[EK_DeviceData]$separator[IK_DeviceData] ... \n
$answer='';
$pos=0;
while($row=$res->FetchRow()){
	if($row['PK_RegisteredDevice_DeviceTemplate']!=$pos){
		$answer.="\n".$row['EK_DeviceTemplate'].$separator.$row['MacAddress'];	
		$pos=$row['PK_RegisteredDevice_DeviceTemplate'];
	}
	$answer.=$separator.$row['IK_DeviceData']."\t".$row['IK_DeviceData']."";
}

print trim($answer);
?>
