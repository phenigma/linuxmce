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
		
		
		$res=$tADO->Execute('
			SELECT * 
			FROM Question 
			INNER JOIN 	Questionaire_Question ON FK_Question=PK_Question
			WHERE FK_Questionaire=?
			ORDER BY `Order` ASC',$qid);
		$form=nl2br($rq['EmailIntro']).'
		<form name="feedback" method="POST" action="'.$PlutoHomeHost.'processFeedback.php">
			<input type="hidden" name="PK_Transmission" value="'.$rowTrans['PK_Transmission'].'">
		<table>';
		
		$pos=0;
		while($row=$res->FetchRow()){
			$pos++;
			
			$anw=$tADO->Execute('SELECT * FROM Response WHERE FK_Question=? ORDER BY `Order` ASC',$row['PK_Question']);
			if($anw->RecordCount()>0){
				$responses='<table>';
				$respCount=0;
				while($rr=$anw->FetchRow()){
					$responses.='
						<tr>
							<td><input type="radio" name="response_'.$row['FK_Question'].'" value="'.$rr['PK_Response'].'" '.(($respCount==0)?'checked':'').'></td>
							<td>'.$rr['Response'].'</td>
						</tr>';
					$respCount++;
				}
				$responses.='</table>';
			}else{
				$responses='&nbsp;';
			}
			$form.='
				<tr>
					<td>'.$pos.'. <B>'.$row['Question'].'</B></td>
				</tr>
				<tr>
					<td>'.$responses.'</td>
				</tr>';
		}
		$form.='
			<tr>
				<td>Comments:<br><textarea name="comments"></textarea></td>
			</tr>
			<tr>
				<td><input type="submit" name="sendFeedback" value="Submit"></td>
			</tr>				
		</table>';
		$form.='</form>';
		
		//sendEmail($rowTrans['Email'],$form);
		//$tADO->Execute('UPDATE Transmission SET TimeSent=NOW() WHERE PK_Transmission=?',$rowTrans['PK_Transmission']);
		$sentMsg++;
	}	
	sendEmail('vali.g@plutohome.com',$form);
	
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