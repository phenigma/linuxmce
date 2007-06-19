<?
session_name('Web-admin');
session_start();

require('include/config/config.inc.php');
require('include/utils.inc.php');

$sID=(int)@$_REQUEST['sID'];
if($sID==0){
	$pic=join('',file('include/images/spacer.png'));
}else{
	$picArray=getAssocArray('Software_Source','PK_Software_Source','Iconstr',$dbADO,'WHERE PK_Software_Source='.$sID);
	if(count($picArray)==0){
		$pic=join('',file('include/images/spacer.png'));
	}else{
		$pic=($picArray[$sID]=='')?join('',file('include/images/spacer.png')):$picArray[$sID];
	}
	
}

Header( "Content-type:  image/png");
//send the binary data
echo $pic;
?>