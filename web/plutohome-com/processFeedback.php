<?
	require('include/config/config.inc.php');
	require('include/template.class.inc.php');
	require('include/masterusers.inc.php');
	require('include/utils.inc.php');
	include_once($GLOBALS['globalconfigPath'].'trackingDB.inc.php');
	
	$out='';
	if(isset($_POST['sendFeedback'])){
		$transmission=(int)$_POST['PK_Transmission'];
		$comments=stripslashes($_POST['comments']);
		
		$tADO->Execute('INSERT INTO Reply (FK_Transmission,DateTime,Comment) VALUES (?,NULL,?)',array($transmission,$comments));		
		$replyID=$tADO->Insert_ID();
		
		$res=$tADO->Execute('
			SELECT FK_Question 
			FROM Questionaire_Question 
			INNER JOIN Transmission ON Transmission.FK_Questionaire=Questionaire_Question.FK_Questionaire
			WHERE PK_Transmission=?
			ORDER BY `Order` ASC',$transmission);
		$output='';
		while($row=$res->FetchRow()){
			$response=(int)$_POST['response_'.$row['FK_Question']];
			$tADO->Execute('INSERT INTO Reply_Response (FK_Reply, FK_Question, FK_Response) VALUES (?,?,?)',array($replyID,$row['FK_Question'],$response));
		}
	
		$out='<br><h3>Thank you for your feedback!</h3>';
	}

   	$output = new Template();	
   	$output->setConn($conn);
   	$output->setTemplateFileType('home');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Thank you for your feedback');			
	$output->output();  
?>