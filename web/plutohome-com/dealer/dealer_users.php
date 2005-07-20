<?
if(!isset($_SESSION['userIsLogged'])){
	header('Location: index.php?section=login');
	exit();
}
function dealer_users($output,$dbADO) {
	/* @var $dbADO ADOConnection */
	global $dbDealerServer, $dbDealerUser, $dbDealerPass, $dbDealerDatabase;
	
	if ($_SESSION['userIsLogged']!="yes") {
		header("Location: index.php");
	}
	$dealerADO=dbConnection($dbDealerServer,$dbDealerUser,$dbDealerPass,$dbDealerDatabase);
	
	// get dealer ID from Users table
	$resUser=$dbADO->Execute('SELECT EK_Dealer FROM Users WHERE PK_Users=?',$_SESSION['userID']);
	if($resUser->RecordCount()==0){
		header('Location: index.php?section=dealerApplication');
		exit();
	}
	$rowUser=$resUser->FetchRow();
	$dealerID=$rowUser['EK_Dealer'];
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$usersArr=getAssocArray('Users','PK_Users','Username',$dbADO,'','ORDER BY Username ASC');
	
if($action=='form'){

	$dealerUser=getFieldsAsArray('Dealer_Users','EK_Users',$dealerADO,'WHERE FK_Dealer='.$dealerID);
	$content='
	<table cellpadding="3" cellspacing="0">
		<tr bgcolor="#DADDE4">
			<td><B>User ID</B></td>
			<td><B>User name</B></td>
			<td><B>Action</B></td>
		</tr>';
	
	$existingUsers=array();
	for($pos=0;$pos<count(@$dealerUser['EK_Users']);$pos++){
		$user=$dealerUser['EK_Users'][$pos];
		$existingUsers[$user]=$usersArr[$user];
		$color=($pos%2==0)?'#F0F3F8':'#FFFFFF';
		$content.='
		<tr bgcolor="'.$color.'">
			<td>'.$user.'</td>
			<td>'.$usersArr[$user].'</td>
			<td><a href="javascript:if(confirm(\'Are you sure you want to delete the user from dealer account?\'))self.location=\'index.php?section=dealer_users&action=del&du='.$user.'\'">Delete</a></td>
		</tr>		
		';
	}
	if(count(@$dealerUser['EK_Users'])==0){
		$content.='
		<tr>
			<td colspan="3">No users added to this dealer account</td>
		</tr>		
		';
	}
	$remainingUsers=array_diff($usersArr,$existingUsers);
	$content.='
		<tr>
			<td>&nbsp;</td>
			<td>&nbsp;</td>
			<td>&nbsp;</td>
		</tr>	
		<tr bgcolor="#DADDE4">
			<td colspan="3" align="center"><B>Add user</B></td>
		</tr>
		<tr>
			<td><B>Select user</B></td>
			<td colspan="2">'.htmlPulldown($remainingUsers,'user',0).'</td>
		</tr>	
		<tr>
			<td colspan="3" align="center"><input type="submit" name="add" value="Add user"></td>
		</tr>	
	</table>';

	$errMsg=(isset($_REQUEST['err']))?'<span class="err">'.stripslashes($_REQUEST['err']).'</span>':'';
	$msg='<div class="confirm" align="center">'.stripslashes(@$_REQUEST['msg']).'</div>';
	$out = '
		<form action="index.php" method="POST" name="dealer_users">
			<input type="hidden" name="section" value="dealer_users">
			<input type="hidden" name="action" value="add">		
	
      		<table width="100%">
	      		<tr>
      				<td align="left" colspan="2" class="insidetable"><img src="images/titles/my_pluto.gif" border="0"></td>
				</tr>
				<tr>
					<td class="normaltext">&nbsp;'.@$msg.@$errMsg.'</td>
      			</tr>	
      			<tr>
      				<td align="center" width="33%" class="normaltext"><b>Users added to dealer account</b></td>
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
		 	var frmvalidator = new formValidator("dealer_users");
			frmvalidator.addValidation("dealerID","dontselect=0","Please select the dealer from the list.");
		</script>	
      	';
}else{
	if(isset($_REQUEST['du']) && (int)$_REQUEST['du']>0){
		$dealerADO->Execute('DELETE FROM  Dealer_Users WHERE FK_Dealer=? AND EK_Users=?',array($dealerID,(int)$_REQUEST['du']));
		
		header('Location: index.php?section=dealer_users&msg=The user was deleted from your dealer account');
		exit();
	}
	
	$user=(int)$_POST['user'];
	if($user==0){
		header('Location: index.php?section=dealer_area&err=You must select a dealer');
		exit();
	}else{
		$dealerADO->Execute('INSERT INTO Dealer_Users (FK_Dealer,EK_Users) VALUES (?,?)',array($dealerID,$user));
		
		$message='Hi,<br>
		Dealer #'.$dealerID.' has added the user #'.$user.' to dealer account.';
		
		$headers = "From: Pluto Home<website@plutohome.com>\n";
		$headers .= "X-Sender: <website@plutohome.com>\n";
		$headers .= "X-Mailer: PHP\n"; //mailer
		$headers .= "X-Priority: 3\n"; //1 UrgentMessage, 3 Normal
		$headers .= "Return-Path: <website@plutohome.com>\n";
		$headers .= "Content-Type: text/html; charset=iso-8859-1\n";
		$headers .= "cc: \n"; 
		$headers .= "bcc: "; 
	
		$subject = 'Dealer #'.$dealerID.' has added the user #'.$user.' to dealer account';
		$to = $GLOBALS['sendNotificationsTo'];
	
		@mail($to,$subject,$message,$headers);
	}
	
	header('Location: index.php?section=dealer_users&msg=The user was added to your dealer account');
	exit();
}


	$output->setNavigationMenu(array("dealer_users"=>"index.php?section=dealer_users"));

	$output->setScriptCalendar('null');
	$output->setScriptTRColor('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME." :: Add/remove users to dealer account");
	$output->output();
}
?>