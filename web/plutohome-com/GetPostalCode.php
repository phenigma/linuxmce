<?
require('include/config/config.inc.php');
require('include/template.class.inc.php');
require('include/utils.inc.php');

/* @var $dbADO ADOConnection */
/* @var $res ADORecordSet */ 
$out='ERROR: no parameters specified.';

// GetPostalCode.php?PK_Country=1&PostalCode=’83883’  (again postal code is a like)
if(isset($_GET['PK_Country'])){
	$out=getPostalCodeByCountry((int)$_GET['PK_Country'],@$_GET['PostalCode'],$dbADO);
}

// GetPostalCode.php?PK_PostalCode=28383
if(isset($_GET['PK_PostalCode'])){
	$out=getPostalCodeById((int)$_GET['PK_PostalCode'],$dbADO);
}

print nl2br($out);


function getPostalCodeByCountry($country,$PostalCode,$dbADO){
	global $dbPlutoMainDatabase,$dbPlutoMainUser,$dbPlutoMainPass;
	
	if($country==0){
		return 'ERROR: country not specified';
	}
	
	if($PostalCode!=''){
		$filter=' AND PostalCode LIKE \'%'.$PostalCode.'%\'';
	}
	
	$res=$dbADO->Execute('SELECT psc_id FROM PostalCode WHERE FK_Country=?'.@$filter,$country);
	if($res->RecordCount()==0){
		return 'ERROR: Postal codes does not exist.';
	}
	$codes=array();
	while($row=$res->FetchRow()){
		$codes[]=$row['psc_id'];
	}
	$out=join(',',$codes)."\n";
	$dumpCmd='mysqldump '.$dbPlutoMainDatabase.' -u'.$dbPlutoMainUser.' -p'.$dbPlutoMainPass.' PostalCode --where "psc_id IN ('.join(',',$codes).')" --no-create-info --skip-comments';

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

function getPostalCodeById($PK_PostalCode,$dbADO){
	global $dbPlutoMainDatabase,$dbPlutoMainUser,$dbPlutoMainPass;
	
	if($PK_PostalCode==0){
		return 'ERROR: PostalCode not specified';
	}
	
	
	$res=$dbADO->Execute('SELECT psc_id FROM PostalCode WHERE PK_PostalCode=?',$PK_PostalCode);
	if($res->RecordCount()==0){
		return 'ERROR: Postal code does not exist.';
	}
	$codes=array();
	while($row=$res->FetchRow()){
		$codes[]=$row['psc_id'];
	}
	$out=join(',',$codes)."\n";
	$dumpCmd='mysqldump '.$dbPlutoMainDatabase.' -u'.$dbPlutoMainUser.' -p'.$dbPlutoMainPass.' PostalCode --where "psc_id IN ('.join(',',$codes).')" --no-create-info --skip-comments';

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
