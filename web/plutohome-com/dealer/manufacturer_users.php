<?
if(!isset($_SESSION['userIsLogged'])){
	header('Location: index.php?section=login');
	exit();
}
function manufacturer_users($output,$dbADO) {
	/* @var $dbADO ADOConnection */
	global $dbManufacturerServer, $dbManufacturerUser, $dbManufacturerPass, $dbManufacturerDatabase;
	
	if ($_SESSION['userIsLogged']!="yes") {
		header("Location: index.php");
	}
	$manufacturerADO=dbConnection($dbManufacturerServer,$dbManufacturerUser,$dbManufacturerPass,$dbManufacturerDatabase);
	
	// get anufacturer ID from Users table
	$resUser=$dbADO->Execute('SELECT EK_Manufacturer FROM Users WHERE PK_Users=?',$_SESSION['userID']);
	if($resUser->RecordCount()==0){
		header('Location: index.php');
		exit();
	}
	$rowUser=$resUser->FetchRow();
	$manufacturerID=$rowUser['EK_Manufacturer'];
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';
	$usersArr=getAssocArray('Users','PK_Users','Username',$dbADO,'','ORDER BY Username ASC');
	
if($action=='form'){

	$manufacturerUser=getFieldsAsArray('Manufacturer_Users','EK_Users',$manufacturerADO,'WHERE EK_Manufacturer='.$manufacturerID);
	$content='
	<table cellpadding="3" cellspacing="0">
		<tr bgcolor="#DADDE4">
			<td><B>User ID</B></td>
			<td><B>User name</B></td>
			<td><B>Action</B></td>
		</tr>';
	
	$existingUsers=array();
	for($pos=0;$pos<count(@$manufacturerUser['EK_Users']);$pos++){
		$user=$manufacturerUser['EK_Users'][$pos];
		$existingUsers[$user]=$usersArr[$user];
		$color=($pos%2==0)?'#F0F3F8':'#FFFFFF';
		$content.='
		<tr bgcolor="'.$color.'">
			<td>'.$user.'</td>
			<td>'.$usersArr[$user].'</td>
			<td><a href="javascript:if(confirm(\'Are you sure you want to delete the user from manufacturer account?\'))self.location=\'index.php?section=manufacturer_users&action=del&du='.$user.'\'">Delete</a></td>
		</tr>		
		';
	}
	if(count(@$manufacturerUser['EK_Users'])==0){
		$content.='
		<tr>
			<td colspan="3">No users added to this manufacturer account</td>
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
		<form action="index.php" method="POST" name="manufacturer_users">
			<input type="hidden" name="section" value="manufacturer_users">
			<input type="hidden" name="action" value="add">		
	
      		<table width="100%">
	      		<tr>
      				<td align="left" colspan="2" class="insidetable"><img src="images/titles/my_pluto.gif" border="0"></td>
				</tr>
				<tr>
					<td class="normaltext">&nbsp;'.@$msg.@$errMsg.'</td>
      			</tr>	
      			<tr>
      				<td align="center" width="33%" class="normaltext"><b>Users added to manufacturer account</b></td>
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
		 	var frmvalidator = new formValidator("manufacturer_users");
			frmvalidator.addValidation("manufacturerID","dontselect=0","Please select the manufacturer from the list.");
		</script>	
      	';
}else{
	if(isset($_REQUEST['du']) && (int)$_REQUEST['du']>0){
		$manufacturerADO->Execute('DELETE FROM  Manufacturer_Users WHERE EK_Manufacturer=? AND EK_Users=?',array($manufacturerID,(int)$_REQUEST['du']));
		
		header('Location: index.php?section=manufacturer_users&msg=The user was deleted from your manufacturer account');
		exit();
	}
	
	$user=(int)$_POST['user'];
	if($user==0){
		header('Location: index.php?section=manufacturer_area&err=You must select a manufacturer');
		exit();
	}else{
		$deviceTemplatesArray=getAssocArray('DeviceTemplate','PK_DeviceTemplate','Description',$dbADO,'WHERE FK_Manufacturer='.$manufacturerID);
		if(count($deviceTemplatesArray)>0){
			foreach ($deviceTemplatesArray AS $dtID=>$descr){
				$manufacturerADO->Execute('INSERT INTO Manufacturer_DeviceTemplate (EK_Manufacturer,EK_DeviceTemplate) VALUES (?,?)',array($manufacturerID,$dtID));
			}
		}

		$existingManuf=getAssocArray('Manufacturer','EK_Manufacturer','Name',$manufacturerADO,'WHERE EK_Manufacturer='.$manufacturerID);
		if(count($existingManuf)==0){
			$manufArray=getAssocArray('Manufacturer','PK_Manufacturer','Description',$dbADO);
			$manufacturerADO->Execute('INSERT INTO Manufacturer (EK_Manufacturer,Name) VALUES (?,?)',array($manufacturerID,$manufArray[$manufacturerID]));
		}
		
		$manufacturerADO->Execute('INSERT INTO Manufacturer_Users (EK_Manufacturer,EK_Users) VALUES (?,?)',array($manufacturerID,$user));
		
		$message='Hi,<br>
		Manufacturer #'.$manufacturerID.' has added the user #'.$user.' to manufacturer account.';
		
		$headers = "From: Pluto Home<website@plutohome.com>\n";
		$headers .= "X-Sender: <website@plutohome.com>\n";
		$headers .= "X-Mailer: PHP\n"; //mailer
		$headers .= "X-Priority: 3\n"; //1 UrgentMessage, 3 Normal
		$headers .= "Return-Path: <website@plutohome.com>\n";
		$headers .= "Content-Type: text/html; charset=iso-8859-1\n";
		$headers .= "cc: \n"; 
		$headers .= "bcc: "; 
	
		$subject = 'Manufacturer #'.$manufacturerID.' has added the user #'.$user.' to manufacturer account';
		$to = $GLOBALS['sendNotificationsTo'];
	
		@mail($to,$subject,$message,$headers);
	}
	
	header('Location: index.php?section=manufacturer_users&msg=The user was added to your manufacturer account');
	exit();
}


	$output->setNavigationMenu(array("manufacturer_users"=>"index.php?section=manufacturer_users"));

	$output->setScriptCalendar('null');
	$output->setScriptTRColor('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME." :: Add/remove users to manufacturer account");
	$output->output();
}
?>