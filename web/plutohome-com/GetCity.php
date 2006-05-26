<?
require('include/config/config.inc.php');
require('include/template.class.inc.php');
require('include/utils.inc.php');

/* @var $dbADO ADOConnection */
/* @var $res ADORecordSet */ 
$out='ERROR: no parameters specified.';

// GetCity.php?PK_PostalCode=34
if(isset($_GET['PK_PostalCode'])){
	$out=getPostalCode((int)$_GET['PK_PostalCode'],$dbADO);
}

// GetCity.php?PK_City=88
if(isset($_GET['PK_City'])){
	$out=getCity((int)$_GET['PK_City'],$dbADO);
}

// GetCity.php?PK_Country=1&PK_Region=5&Name=’angeles’  (note, this uses a like and will return multiple records with all cities containing angeles in country 1, region 5)
if(isset($_GET['PK_Country']) || isset($_GET['PK_Region'])){
	$out=getCountry((int)@$_GET['PK_Country'],(int)@$_GET['PK_Region'],@$_GET['Name'],$dbADO);
}

// Create=1
if(isset($_GET['Create'])){
	$out=getCityCreate($dbADO);
}

print "-- Database import\n".$out."\n-- EOF\n";

function getPostalCode($postalCode,$dbADO){
	global $dbPlutoMainDatabase,$dbPlutoMainUser,$dbPlutoMainPass;
	
	if($postalCode==0){
		return 'ERROR: invalid ID';
	}
	
	$res=$dbADO->Execute('SELECT psc_id FROM PostalCode WHERE PK_PostalCode=?',$postalCode);
	if($res->RecordCount()==0){
		return 'ERROR: Postal code does not exist.';
	}
	$row=$res->FetchRow();
	$out="-- psc_id: ".$row['psc_id']."\n";
	$dumpCmd='mysqldump '.$dbPlutoMainDatabase.' -u'.$dbPlutoMainUser.' -p'.$dbPlutoMainPass.' PostalCode --where "psc_id = '.$row['psc_id'].'" --no-create-info --skip-comments';

	exec($dumpCmd,$retArray);
	$cleanInserts=array();
	foreach ($retArray As $line){
		if(trim($line)!=''){
			$cleanInserts[]=$line;
		}
	}
	$out.=join("\n",$cleanInserts);
	
	return "\n".$out;
}

function getCity($city,$dbADO){
	global $dbPlutoMainDatabase,$dbPlutoMainUser,$dbPlutoMainPass;
	
	if($city==0){
		return 'ERROR: invalid ID';
	}
	
	$res=$dbADO->Execute('SELECT psc_id FROM City WHERE PK_City=?',$city);
	if($res->RecordCount()==0){
		return 'ERROR: City does not exist.';
	}
	$row=$res->FetchRow();
	$out="-- psc_id: ".$row['psc_id']."\n";
	$dumpCmd='mysqldump '.$dbPlutoMainDatabase.' -u'.$dbPlutoMainUser.' -p'.$dbPlutoMainPass.' City --where "psc_id = '.$row['psc_id'].'" --no-create-info --skip-comments';

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

function getCountry($country,$region,$name,$dbADO){
	global $dbPlutoMainDatabase,$dbPlutoMainUser,$dbPlutoMainPass;
	
	if($country==0 && $region==0){
		return 'ERROR: not enough parameters';
	}
	
	$filter='WHERE 1=1';
	if($country!=0){
		$filter.=' AND FK_Country='.$country;
	}	
	
	if($region!=0){
		$filter.=' AND FK_Region='.$region;
	}
	
	if($name!=''){
		$filter.=' AND City LIKE \'%'.$name.'%\'';
	}

	$res=$dbADO->Execute('SELECT psc_id FROM City '.@$filter);
	if($res->RecordCount()==0){
		return 'No records';
	}
	$ids=array();
	while($row=$res->FetchRow()){
		if((int)$row['psc_id']>0)		
			$ids[]=$row['psc_id'];
	}
	$out=join(',',$ids)."\n";
	$dumpCmd='mysqldump '.$dbPlutoMainDatabase.' -u'.$dbPlutoMainUser.' -p'.$dbPlutoMainPass.' City --where "psc_id IN ('.join(',',$ids).')" --no-create-info --skip-comments';

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

function getCityCreate($dbADO){
	global $dbPlutoMainDatabase,$dbPlutoMainUser,$dbPlutoMainPass;
	
	$dumpCmd='mysqldump '.$dbPlutoMainDatabase.' -u'.$dbPlutoMainUser.' -p'.$dbPlutoMainPass.' City --no-data --skip-comments';
	exec($dumpCmd,$retArray);
	
	$cleanInserts=array();
	foreach ($retArray As $line){
		if(trim($line)!=''){
			$cleanInserts[]=$line;
		}
	}
	$out=join("\n",$cleanInserts);
	
	return "\n".$out;
}


?>
