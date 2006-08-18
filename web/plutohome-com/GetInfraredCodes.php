<?
require('include/config/config.inc.php');
require('include/template.class.inc.php');
require('include/utils.inc.php');
/* @var $mainSqlCVSADO ADOConnection */
/* @var $res ADORecordSet */ 
$dsn = $dbPlutoMainType.'://'.urlencode($dbPlutoMainUser).':'.urlencode($dbPlutoMainPass).'@'.$dbPlutoMainServer.'/'.urlencode('main_sqlcvs'); 
$mainSqlCVSADO = &ADONewConnection($dsn);

$out='ERROR: no parameters specified.';
$GLOBALS['RestrictedCommandsArray']=array(194=>'Toggle power',192=>'On',193=>'Off',205=>'1',89=>'Vol Up',90=>'Vol down',63=>'Skip Fwd',64=>'Skip Back');

// e.g. GetInfraredCodes.php?PK_InfraredGroup=333&PK_DeviceTemplate=222
if(isset($_GET['PK_InfraredGroup'])){
	$out=getCodesByTemplate((int)$_GET['PK_InfraredGroup'],(int)@$_GET['PK_DeviceTemplate'],$mainSqlCVSADO);
}

// Create=1, return schema
if(isset($_GET['Create'])){
	$out=getInfraredGroupCommandCreate($mainSqlCVSADO);
}

// PK_Manufacturer=8&PK_DeviceCategory=10
// retrieve all codes with the same manufacturer and device category	
if(isset($_GET['PK_Manufacturer'])){
	$out=getCodesByManufacturer((int)$_GET['PK_Manufacturer'],(int)@$_GET['PK_DeviceCategory'],$mainSqlCVSADO);
}

if(isset($_GET['psc_ids'])){
	$out=getCodesByIDs($_GET['psc_ids'],$mainSqlCVSADO);
}

print "-- Database import\n".$out."\n-- EOF\n";


function getCodesByTemplate($PK_InfraredGroup,$PK_DeviceTemplate,$mainSqlCVSADO){
	global $dbPlutoMainDatabase,$dbPlutoMainUser,$dbPlutoMainPass;
	$restricted=(int)@$_REQUEST['Restricted'];
	
	$andConditions=array();
	if($restricted==1){
		$andConditions[]='FK_Command IN ('.join(',',array_keys($GLOBALS['RestrictedCommandsArray'])).')';
	}
	
	$orConditions=array();
	if($PK_InfraredGroup!=0){
		$orConditions[]='FK_InfraredGroup='.$PK_InfraredGroup;
	}
	
	if($PK_DeviceTemplate>0){
		$orConditions[]='FK_DeviceTemplate = '.$PK_DeviceTemplate;
	}
	$filter='WHERE 1=1 '.((count($andConditions)>0)?'AND '.join(' AND ',$andConditions):'').((count($orConditions)>0)?' AND ('.join(' OR ',$orConditions).')':'');
	

	$res=$mainSqlCVSADO->Execute('SELECT InfraredGroup_Command.psc_id FROM InfraredGroup_Command '.@$filter);

	$codes=array();
	while($row=$res->FetchRow()){
		if((int)$row['psc_id']>0)		
			$codes[]=$row['psc_id'];
	}
	$out="-- psc_id: ".join(',',$codes)."\n";
	$dumpCmd='mysqldump '.$dbPlutoMainDatabase.' -u'.$dbPlutoMainUser.' -p'.$dbPlutoMainPass.' InfraredGroup_Command --where "psc_id IN ('.join(',',$codes).')" --no-create-info --skip-comments --force';

	exec($dumpCmd,$retArray);
	$cleanInserts=array();
	foreach ($retArray As $line){
		if(trim($line)!=''){
			$cleanInserts[]=$line;
		}
	}
	$out.=join("\n",$cleanInserts);
	
	return $out;
}

function getInfraredGroupCommandCreate($mainSqlCVSADO){
	global $dbPlutoMainDatabase,$dbPlutoMainUser,$dbPlutoMainPass;
	
	$dumpCmd='mysqldump '.$dbPlutoMainDatabase.' -u'.$dbPlutoMainUser.' -p'.$dbPlutoMainPass.' InfraredGroup_Command --no-data --skip-comments';
	exec($dumpCmd,$retArray);
	
	$cleanInserts=array();
	foreach ($retArray As $line){
		if(trim($line)!=''){
			$cleanInserts[]=$line;
		}
	}
	$out=join("\n",$cleanInserts);
	
	return "-- psc_id: \n".$out;
}

function getCodesByManufacturer($PK_Manufacturer,$PK_DeviceCategory,$mainSqlCVSADO){
	global $dbPlutoMainDatabase,$dbPlutoMainUser,$dbPlutoMainPass;
	$restricted=(int)@$_REQUEST['Restricted'];
	
	if($PK_Manufacturer==0 && $PK_DeviceCategory==0){
		return 'ERROR: parameters not specified';
	}
	
	$andConditions=array();
	$orConditions=array();
	if($PK_Manufacturer!=0){
		$andConditions[]='FK_Manufacturer='.$PK_Manufacturer;
	}
	
	if($PK_DeviceCategory!=0){
		$andConditions[]='FK_DeviceCategory= '.$PK_DeviceCategory;
	}
	
	if($restricted==1){
		$andConditions[]='FK_Command IN ('.join(',',array_keys($GLOBALS['RestrictedCommandsArray'])).')';
	}
	$filter='WHERE 1=1 '.((count($andConditions)>0)?'AND '.join(' AND ',$andConditions):'').((count($orConditions)>0)?' AND ('.join(' OR ',$orConditions).')':'');
	
	$res=$mainSqlCVSADO->Execute('SELECT InfraredGroup_Command.psc_id FROM InfraredGroup_Command INNER JOIN InfraredGroup ON FK_InfraredGroup=PK_InfraredGroup '.@$filter);

	$codes=array();
	while($row=$res->FetchRow()){
		if((int)$row['psc_id']>0)
			$codes[]=$row['psc_id'];
	}
	$out="-- psc_id: ".join(',',$codes)."\n";
	$dumpCmd='mysqldump '.$dbPlutoMainDatabase.' -u'.$dbPlutoMainUser.' -p'.$dbPlutoMainPass.' InfraredGroup_Command --where "psc_id IN ('.join(',',$codes).')" --no-create-info --skip-comments --force';

	exec($dumpCmd,$retArray);
	$cleanInserts=array();
	foreach ($retArray As $line){
		if(trim($line)!=''){
			$cleanInserts[]=$line;
		}
	}
	$out.=join("\n",$cleanInserts);
	
	return $out;
}

function getCodesByIDs($IDs,$mainSqlCVSADO){
	global $dbPlutoMainDatabase,$dbPlutoMainUser,$dbPlutoMainPass;
	
	if($IDs==''){
		return 'ERROR: parameters not specified';
	}
	$arrayIDs=explode(',',$IDs);
	
	$filter='WHERE InfraredGroup_Command.psc_id IN ('.join(',',$arrayIDs).')';
	$res=$mainSqlCVSADO->Execute('SELECT InfraredGroup_Command.psc_id FROM InfraredGroup_Command INNER JOIN InfraredGroup ON FK_InfraredGroup=PK_InfraredGroup '.@$filter);

	$codes=array();
	while($row=$res->FetchRow()){
		if((int)$row['psc_id']>0)
			$codes[]=$row['psc_id'];
	}
	$out="-- psc_id: ".join(',',$codes)."\n";
	$dumpCmd='mysqldump '.$dbPlutoMainDatabase.' -u'.$dbPlutoMainUser.' -p'.$dbPlutoMainPass.' InfraredGroup_Command --where "psc_id IN ('.join(',',$codes).')" --no-create-info --skip-comments --force';

	exec($dumpCmd,$retArray);
	$cleanInserts=array();
	foreach ($retArray As $line){
		if(trim($line)!=''){
			$cleanInserts[]=$line;
		}
	}
	$out.=join("\n",$cleanInserts);
	
	return $out;
	
}
?>
