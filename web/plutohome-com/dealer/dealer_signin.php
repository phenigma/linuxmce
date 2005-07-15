<?
if(!isset($_SESSION['userIsLogged'])){
	header('Location: index.php?section=login');
	exit();
}
function dealer_signin($output,$dbADO) {
	/* @var $dbADO ADOConnection */
	global $dbDealerServer, $dbDealerUser, $dbDealerPass, $dbDealerDatabase;
	
	if ($_SESSION['userIsLogged']!="yes") {
		header("Location: index.php");
	}
	$dealerADO=dbConnection($dbDealerServer,$dbDealerUser,$dbDealerPass,$dbDealerDatabase);
	
	$FK_Users=(isset($_SESSION['userID']))?(int)$_SESSION['userID']:0;
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';

if($action=='form'){

	$dealerUser=getFieldsAsArray('Dealer_Users','FK_Dealer,EK_Users,Validated',$dealerADO,'WHERE EK_Users='.$FK_Users);
	$content=(isset($dealerUser['Validated'][0]) && $dealerUser['Validated'][0]!=1)?'<B>Thank you</B><br>Your request has been recorded.<br>Please allow 24-48 hours for your request to be processed.':'Choose dealer: '.htmlPulldown(getAssocArray('Dealer','PK_Dealer','Name',$dealerADO,'','ORDER BY Name ASC'),'dealerID','None','Please select').' <input type="submit" name="submit" value="Submit">';

	$errMsg=(isset($_REQUEST['err']))?'<span class="err">'.stripslashes($_REQUEST['err']).'</span>':'';
	$out = '
		<form action="index.php" method="POST" name="dealer_signin">
			<input type="hidden" name="section" value="dealer_signin">
			<input type="hidden" name="action" value="add">		
	
      		<table width="100%">
	      		<tr>
      				<td align="left" colspan="2" class="insidetable"><img src="images/titles/my_pluto.gif" border="0"></td>
				</tr>
				<tr>
					<td class="normaltext">&nbsp;'.@$msg.@$errMsg.'</td>
      			</tr>	
      			<tr bgcolor="#DADDE4">
      				<td align="center" width="33%" class="normaltext"><b>Link my user account to a dealer</b></td>
      			</tr>
      			<tr>
      				<td width="33%" class="normaltext"></td>
      			</tr>
      			<tr>
      				<td width="33%" class="normaltext" align="center" height="200">'.$content.'</td>
      			</tr>	
			</table>
		</form>
  		<script>
		 	var frmvalidator = new formValidator("dealer_signin");
			frmvalidator.addValidation("dealerID","dontselect=0","Please select the dealer from the list.");
		</script>	
      	';
}else{
	$dealerID=(int)$_POST['dealerID'];
	if($dealerID==0){
		header('Location: index.php?section=dealer_area&err=You must select a dealer');
		exit();
	}else{
		$dealerADO->Execute('INSERT INTO Dealer_Users (FK_Dealer,EK_Users) VALUES (?,?)',array($dealerID,$FK_Users));
		
		$message='Hi,<br>
		User '.$_SESSION['username'].' has linked his account #'.$FK_Users.' to dealer account #'.$dealerID;
		
		$headers = "From: Pluto Home<website@plutohome.com>\n";
		$headers .= "X-Sender: <website@plutohome.com>\n";
		$headers .= "X-Mailer: PHP\n"; //mailer
		$headers .= "X-Priority: 3\n"; //1 UrgentMessage, 3 Normal
		$headers .= "Return-Path: <website@plutohome.com>\n";
		$headers .= "Content-Type: text/html; charset=iso-8859-1\n";
		$headers .= "cc: \n"; 
		$headers .= "bcc: "; 
	
		$subject = 'User request to link to a dealer account';
		$to = $GLOBALS['sendNotificationsTo'];
	
		@mail($to,$subject,$message,$headers);
	}
	
	header('Location: index.php?section=dealer_signin');
	exit();
}


	$output->setNavigationMenu(array("dealer_signin"=>"index.php?section=dealer_signin"));

	$output->setScriptCalendar('null');
	$output->setScriptTRColor('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME." :: Link my user account to a dealer");
	$output->output();
}
?>