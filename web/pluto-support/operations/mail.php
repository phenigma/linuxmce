<?
$package=(isset($_SESSION['package']))?(int)$_SESSION['package']:0;	
$userID=(int)$_SESSION['userID'];

$out='<form name="mail" method="POST" action="right.php?section=operations/mail" target="_self">';

$mlSubscribed=array();
$packageML=array();
if($package!=0){
	$resML=$webADO->Execute('
		SELECT * 
		FROM MailingList
		INNER JOIN MailingList_Package ON MailingList_Package.FK_MailingList=PK_MailingList
		LEFT JOIN MailingList_MasterUsers ON MailingList_MasterUsers.FK_MailingList=PK_MailingList AND FK_MasterUsers=?
		WHERE MailingList_Package.FK_Package=?',array($userID,$package));
}else{
	$resML=$webADO->Execute('
		SELECT * 
		FROM MailingList
		LEFT JOIN MailingList_Package ON MailingList_Package.FK_MailingList=PK_MailingList
		LEFT JOIN MailingList_MasterUsers ON MailingList_MasterUsers.FK_MailingList=PK_MailingList AND FK_MasterUsers=?
		WHERE MailingList_Package.FK_Package IS NULL AND MailingList_Package.FK_MailingList IS NULL',$userID);
}
while($row=$resML->FetchRow()){
	$packageML[]=$row['PK_MailingList'];
	if($row['FK_MasterUsers']!=''){
		$mlSubscribed[]=$row['PK_MailingList'];
	}
	$out.='<input type="checkbox" name="ml_'.$row['PK_MailingList'].'" value="1" '.(($row['FK_MasterUsers']!='')?'checked':'').'> '.$row['Description'].' ('.$row['EmailField'].')<br>';
}
if(count($packageML)>0){
	$out.='<input type="submit" name="subscribe" value="Subscribe">
	<input type="hidden" name="mlSubscribed" value="'.join(',',$mlSubscribed).'">
	<input type="hidden" name="packageML" value="'.join(',',$packageML).'">';
}else{
	$out.='No mailing list available.';
}
$out.='
</form>';
if(isset($_POST['subscribe'])){
	$mlSubscribed=explode(',',$_POST['mlSubscribed']);
	$packageML=explode(',',$_POST['packageML']);
	foreach ($packageML AS $ml){
		if(isset($_POST['ml_'.$ml])){
			if(!in_array($ml,$mlSubscribed)){
				$webADO->Execute('INSERT INTO MailingList_MasterUsers (FK_MailingList,FK_MAsterUsers) VALUES (?,?)',array($ml,$userID));
				// TODO
				// call Radu's script for subscribe
			}
		}else{
			if(in_array($ml,$mlSubscribed)){
				$webADO->Execute('DELETE FROM MailingList_MasterUsers WHERE FK_MailingList=? AND FK_MAsterUsers=?',array($ml,$userID));
				// TODO
				// call Radu's script for unsubscribe
			}
		}
	}
	header('Location: right.php?section=operations/mail');
}

$output->setBody($out);
$output->setTitle(APPLICATION_NAME);			
$output->output();  		
?>