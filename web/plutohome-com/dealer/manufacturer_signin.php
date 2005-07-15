<?
if(!isset($_SESSION['userIsLogged'])){
	header('Location: index.php?section=login');
	exit();
}
function manufacturer_signin($output,$dbADO) {
	/* @var $dbADO ADOConnection */
	global $dbManufacturerServer, $dbManufacturerUser, $dbManufacturerPass, $dbManufacturerDatabase;
	
	if ($_SESSION['userIsLogged']!="yes") {
		header("Location: index.php");
	}
	$manufacturerADO=dbConnection($dbManufacturerServer,$dbManufacturerUser,$dbManufacturerPass,$dbManufacturerDatabase);
	
	$FK_Users=(isset($_SESSION['userID']))?(int)$_SESSION['userID']:0;
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';

if($action=='form'){	
	
	$manufacturerUser=getFieldsAsArray('Manufacturer_Users','EK_Manufacturer,EK_Users,Validated',$manufacturerADO,'WHERE EK_Users='.$FK_Users);
	$content=(isset($manufacturerUser['Validated'][0]) && $manufacturerUser['Validated'][0]!=1)?'Please allow 24-48 hours for your request to be processed.':'Choose manufacturer: '.htmlPulldown(getAssocArray('Manufacturer','PK_Manufacturer','Description',$dbADO,'','ORDER BY Description ASC'),'manufacturerID','None','Please select').' <input type="submit" name="submit" value="Submit">';
	
	$errMsg=(isset($_REQUEST['err']))?'<span class="err">'.stripslashes($_REQUEST['err']).'</span>':'';
	$out = '
		<form action="index.php" method="POST" name="manufacturer_signin">
			<input type="hidden" name="section" value="manufacturer_signin">
			<input type="hidden" name="action" value="add">		
	
      		<table width="100%">
	      		<tr>
      				<td align="left" colspan="2" class="insidetable"><img src="images/titles/my_pluto.gif" border="0"></td>
				</tr>
				<tr>
					<td class="normaltext">&nbsp;'.@$msg.@$errMsg.'</td>
      			</tr>	
      			<tr bgcolor="#DADDE4">
      				<td align="center" width="33%" class="normaltext"><b>Link my user account to a manufacturer</b></td>
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
		 	var frmvalidator = new formValidator("manufacturer_signin");
			frmvalidator.addValidation("manufacturerID","dontselect=0","Please select the manufacturer from the list.");
		</script>	
      	';
}else{
	$manufacturerID=(int)$_POST['manufacturerID'];
	if($manufacturerID==0){
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
		
		$manufacturerADO->Execute('INSERT INTO Manufacturer_Users (EK_Manufacturer,EK_Users) VALUES (?,?)',array($manufacturerID,$FK_Users));
		
		$message='Hi,<br>
		User '.$_SESSION['username'].' has linked his account #'.$FK_Users.' to manufacturer account #'.$manufacturerID;
		
		$headers = "From: Pluto Home<website@plutohome.com>\n";
		$headers .= "X-Sender: <website@plutohome.com>\n";
		$headers .= "X-Mailer: PHP\n"; //mailer
		$headers .= "X-Priority: 3\n"; //1 UrgentMessage, 3 Normal
		$headers .= "Return-Path: <website@plutohome.com>\n";
		$headers .= "Content-Type: text/html; charset=iso-8859-1\n";
		$headers .= "cc: \n"; 
		$headers .= "bcc: "; 
	
		$subject = 'User request to link to a manufacturer account';
		$to = $GLOBALS['sendNotificationsTo'];
	
		@mail($to,$subject,$message,$headers);
	}
	
	header('Location: index.php?section=manufacturer_signin');
	exit();
}


	$output->setNavigationMenu(array("My Pluto"=>"index.php?section=myPluto","Manufacturer signin"=>"index.php?section=manufacturer_signin"));

	$output->setScriptCalendar('null');
	$output->setScriptTRColor('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME." :: Link my user account to a manufacturer");
	$output->output();
}
?>