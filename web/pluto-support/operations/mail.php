<?
$package=(isset($_SESSION['package']))?(int)$_SESSION['package']:0;	
$userID=(int)@$_SESSION['userID'];
$userEmail=@$_SESSION['Email'];

if($userID!=0){
	$out='
		<div class="err">'.strip_tags(@$_GET['error']).'</div>
		<form name="mail" method="POST" action="right.php?section=operations/mail" target="_self">';
	
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
	$mlNames=array();
	while($row=$resML->FetchRow()){
		$mlNames[$row['PK_MailingList']]=substr($row['EmailField'],0,strpos($row['EmailField'],'@'));
		
		// ask ML for emails assigned and compare with DB; 
		// database corrections if necessary 
		$emailsML=getEmailsFromML(substr($row['EmailField'],0,strpos($row['EmailField'],'@')));
		$emailsDB=getEmailsFromDatabase($row['Description'],$webADO);
		$emailsToDelete=array_diff($emailsDB,$emailsML);
		$dbADO->debug=true;
		if(count($emailsToDelete)>0){
			$webADO->Execute('
				DELETE MailingList_MasterUsers
				FROM MailingList_MasterUsers 
				INNER JOIN MasterUsers ON FK_MasterUsers=PK_MasterUsers
				WHERE FK_MailingList=? AND PK_MasterUsers IN ('.join(',',array_keys($emailsToDelete)).')',$row['PK_MailingList']);
		}
		
		$packageML[]=$row['PK_MailingList'];
		if($row['FK_MasterUsers']!=''){
			$mlSubscribed[]=$row['PK_MailingList'];
		}
		$out.='<input type="checkbox" name="ml_'.$row['PK_MailingList'].'" value="1" '.(($row['FK_MasterUsers']!='')?'checked':'').'> '.$row['Description'].' ('.$row['EmailField'].')<br>';
	}
	
	if(count($packageML)>0){
		$out.='<input type="submit" name="subscribe" value="Subscribe">
		<input type="hidden" name="mlNames" value="'.urlencode(serialize($mlNames)).'">
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
		$mlNames=unserialize(urldecode(stripslashes($_POST['mlNames'])));
		
		foreach ($packageML AS $ml){
			if(isset($_POST['ml_'.$ml])){
				if(!in_array($ml,$mlSubscribed)){
					$webADO->Execute('INSERT INTO MailingList_MasterUsers (FK_MailingList,FK_MAsterUsers) VALUES (?,?)',array($ml,$userID));
					subscribe($mlNames[$ml],$userEmail);
				}
			}else{
				if(in_array($ml,$mlSubscribed)){
					$webADO->Execute('DELETE FROM MailingList_MasterUsers WHERE FK_MailingList=? AND FK_MAsterUsers=?',array($ml,$userID));
					unsubscribe($mlNames[$ml],$userEmail);
				}
			}
		}
		header('Location: right.php?section=operations/mail');
	}
}else{
	$out='Please login to be able to subscribe/unsubscribe to mailing lists.';
}
	
$output->setBody($out);
$output->setTitle(APPLICATION_NAME);			
$output->output();  		

// return an array with the emails who appear in database as subscribed to $MLname
function getEmailsFromDatabase($MLname,$webADO){
	$emailsDBArray=array();
	$res=$webADO->Execute('
		SELECT MailingList_MasterUsers.*,MasterUsers.Email,PK_MasterUsers 
		FROM MailingList_MasterUsers 
		INNER JOIN MasterUsers ON FK_MasterUsers=PK_MasterUsers
		INNER JOIN MailingList ON FK_MailingList=PK_MailingList WHERE Description=?',$MLname);
	while($row=$res->FetchRow()){
		$emailsDBArray[$row['PK_MasterUsers']]=$row['Email'];
	}
	
	return $emailsDBArray;
}

// return an array with the emails who subscribed to mailing list given in $MLname
function getEmailsFromML($MLname){
	$emailsArray=array();
	exec("sudo -u root /usr/pluto/bin/maillists.sh view members '$MLname'", $emailsArray);
	return $emailsArray;
}

// subscribe an email address to a mailing list
// without output because it will break the page
function subscribe($MLname,$userEmail)
{
	// return errors using 
	// header('Location: right.php?section=operations/mail&error=ERROR_MESSAGE');

	exec("sudo -u root /usr/pluto/bin/maillists.sh subscribe '$MLname' '$userEmail'");
}

// unsubscribe an email address to a mailing list 
function unsubscribe($MLname,$userEmail)
{
	// return errors using 
	// header('Location: right.php?section=operations/mail&error=ERROR_MESSAGE');
	
	exec("sudo -u root /usr/pluto/bin/maillists.sh unsubscribe '$MLname' '$userEmail'");
}

?>
