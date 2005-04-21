<?
require('include/config/config.inc.php');
require('include/utils.inc.php');
require('include/masterusers.inc.php');

$ScenariosArray=getAssocArray('Template','PK_Template','Description',$dbADO,'','');
$ScenariosArray['']='Undefined';

if(isset($_REQUEST['cgid'])){
	$cgid=(int)$_REQUEST['cgid'];
	if($cgid>0){
		$res=$dbADO->Execute('SELECT Description,FK_Template FROM CommandGroup WHERE PK_CommandGroup=?',$cgid);
		if($res->RecordCount()>0){
			$row=$res->FetchRow();
			
			$commandToSend='/usr/pluto/bin/MessageSend localhost 0 0 10 '.$cgid;
			exec($commandToSend);
			$msg=$ScenariosArray[$row['FK_Template']].': Scenario <B>'.$row['Description'].'</B> was executed.';
		}else {
			$msg='Invalid scenario.';
		}
	}else{
		$msg='Invalid scenario ID.';
	}
	
	print $msg;
}
?>