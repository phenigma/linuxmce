<?
session_name('Pluto-admin');
session_start();

require('include/config/config.inc.php');
require('include/utils.inc.php');

$sID=(int)@$_REQUEST['sID'];
if($sID==0){
	die('Icon not available');
}

$picArray=getAssocArray('Software','PK_Software','Iconstr',$dbADO,'WHERE PK_Software='.$sID);
if(count($picArray)==0){
	die('Icon not available');
}

Header( "Content-type:  image/png");
//send the binary data
echo $picArray[$sID];
?>