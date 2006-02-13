<?php
require('include/config/config.inc.php');
require('include/utils.inc.php');
$start_time=getmicrotime();

$path='/home/users/vali/work/web/pluto-admin/languages/en/';
$cmd="grep -H '^\\$' $path*.php";

// hard-coded
$PlutoAdminCategory=208;
$subCategories=array(''=>0,)+getAssocArray('TextCategory','Description','PK_TextCategory',$dbADO,'WHERE FK_TextCategory_Parent='.$PlutoAdminCategory);
$plutoAdminFilenames=getAssocArray('Text','Description','PK_Text',$dbADO,'WHERE FK_TextCategory IN ('.join(',',$subCategories).')');

exec($cmd,$retArr);
$len=strlen($path);
foreach ($retArr AS $line){
	$pattern='/'.str_replace('/','\/',$path).'(.*).lang.php:\$TEXT_(.*)_CONST=\'(.*)\';/';
	preg_match($pattern,$line,$matches);
	if(isset($matches) && count($matches)==4){
		$filename=$matches[1];
		$variable=$matches[2];
		$value=$matches[3];
		// add record in TextCategory if it does not exist
		if(!isset($subCategories[$filename])){
			$dbADO->Execute('INSERT INTO TextCategory (FK_TextCategory_Parent,Description) VALUES (?,?)',array($PlutoAdminCategory,$filename));
			$subCategories[$filename]=$dbADO->Insert_ID();
		}

		// add record in text table if does not exist
		if(!isset($plutoAdminFilenames[$variable])){
			$dbADO->Execute('INSERT INTO Text (FK_TextCategory,Description) VALUES (?,?)',array($subCategories[$filename],$variable));
			$plutoAdminFilenames[$variable]=$dbADO->Insert_ID();
		}
		$dbADO->Execute('REPLACE INTO Text_LS (FK_Text,FK_Language,Description) VALUES (?,?,?)',array($plutoAdminFilenames[$variable],1,$value));
	}else{
		echo $line.'<br>';
	}

}

$end_time=getmicrotime();
print '<span class="normaltext">Import finished in '.round(($end_time-$start_time),3).' s.</span>';

?>