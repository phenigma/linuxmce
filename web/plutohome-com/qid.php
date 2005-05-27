<?
	require('include/config/config.inc.php');
	require('include/template.class.inc.php');
	require('include/masterusers.inc.php');
	require('include/utils.inc.php');
	include_once($GLOBALS['globalconfigPath'].'trackingDB.inc.php');
	
$tID=(int)$_REQUEST['tID'];
$dateTime=date('Y-m-d H:i:s',$_REQUEST['dt']);

$toShow=$tADO->Execute('SELECT * FROM Transmission WHERE PK_Transmission=? AND TimeSent=?',array($tID,$dateTime));
if($toShow->RecordCount()==0){
	$out='Invalid parameters.';
}else{
	$rowTrans=$toShow->FetchRow();
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
	$out='<p class="normaltext">'.nl2br($rq['EmailIntro']).'
		<form name="feedback" method="POST" action="'.$PlutoHomeHost.'processFeedback.php">
			<input type="hidden" name="PK_Transmission" value="'.$rowTrans['PK_Transmission'].'">
		<table class="normaltext">';

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
		$out.='
				<tr>
					<td>'.$pos.'. <B>'.$row['Question'].'</B></td>
				</tr>
				<tr>
					<td>'.$responses.'</td>
				</tr>';
	}
	$out.='
			<tr>
				<td>Comments:<br><textarea name="comments"></textarea></td>
			</tr>
			<tr>
				<td><input type="submit" name="sendFeedback" value="Submit"></td>
			</tr>				
		</table>';
	$out.='</form>';
}
   	$output = new Template();	
   	$output->setConn($conn);
   	$output->setTemplateFileType('home');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Questionaire');			
	$output->output(); 

?>