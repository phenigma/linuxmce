<?
	require('include/config/config.inc.php');
	include_once($GLOBALS['globalconfigPath'].'trackingDB.inc.php');
	
	$toSend=$tADO->Execute('SELECT * FROM Transmission WHERE TimeSent IS NULL');
	$sentMsg=0;
	while($rowTrans=$toSend->FetchRow()){
			
		$qid=$rowTrans['FK_Questionaire'];
		$rs=$tADO->Execute('SELECT * FROM Questionaire WHERE PK_Questionaire=?',$qid);
		if($rs->RecordCount()==0){
			die('Invalid questionaire');
		}
		$rq=$rs->FetchRow();
		
		$datetime=date('U');
		$form=nl2br($rq['EmailIntro']);
		$form.='Click <a href="'.$PlutoHomeHost.'qid.php?tID='.$rowTrans['PK_Transmission'].'&dt='.$datetime.'">here</a> for a short questionaire.';
		
		//sendEmail($rowTrans['Email'],$form);
		
		$tADO->Execute('UPDATE Transmission SET TimeSent=? WHERE PK_Transmission=?',array(date('Y-m-d H:i:s',$datetime),$rowTrans['PK_Transmission']));
		$sentMsg++;
	}
	
	// testing email, it could be removed
	sendEmail('vali.g@plutohome.com',@$form);
	
	print 'Succes: '.$sentMsg.' email(s) was sent.';
	
function sendEmail($to,$message)
{
	$headers = "From: Pluto Home<website@plutohome.com>\n";
	$headers .= "X-Sender: <website@plutohome.com>\n";
	$headers .= "X-Mailer: PHP\n"; //mailer
	$headers .= "X-Priority: 3\n"; //1 UrgentMessage, 3 Normal
	$headers .= "Return-Path: <website@plutohome.com>\n";
	$headers .= "Content-Type: text/html; charset=iso-8859-1\n";
	$headers .= "cc: \n";
	$headers .= "bcc: ";
	
	$subject = 'Plutohome Feedback Form';
	
	@mail($to,$subject,$message,$headers);	
}
?>