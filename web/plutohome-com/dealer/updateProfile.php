<?
function updateProfile($output,$conn,$dbADO) {
	/* @var $dbADO ADOConnection */
	$businessArray=array(1=>'New-Startup',2=>'Security', 3=>'Audio and Video', 4=>'Satellite', 5=>'Electrical', 6=>'Consumer Electronics',7=>'Home Networking', 8=>'Home Automation', 9=>'Other');
	$marketFocusArray=array(1=>'New Construction', 2=>'Residential Retrofit', 3=>'Small Business', 4=>'High-End Custom Homes',5=>'Other');
	$estimatedRevenueArray=array(1=>'$0-$50K', 2=>'$50K-$250K', 3=>'$250K-$500K', 4=>'$500K-$750K', 5=>'$750K-$1M', 6=>'1M+');		
	
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';	
	
	// get dealer ID from Users table
	$resUser=$dbADO->Execute('SELECT FK_Dealer FROM Users WHERE PK_Users=?',$_SESSION['userID']);
	if($resUser->RecordCount()==0){
		header('Location: index.php?section=dealerApplication');
		exit();
	}
	$rowUser=$resUser->FetchRow();
	$dealerID=$rowUser['FK_Dealer'];
	
	// get dealer data from Dealer table
	$resDealer=dbQuery("SELECT * FROM Dealer WHERE PK_Dealer='$dealerID'",$conn);
	if(mysql_num_rows($resDealer)==0){
		header('Location: index.php?section=dealerApplication');
		exit();
	}
	$rowDealer=mysql_fetch_assoc($resDealer);
	
	$out='';

if($action=='form'){
	$out .= '
	<style>
	hr	{ height: 0px; border: solid #D1D7DC 0px; border-top-width: 1px;}
	</style>
	<form action="index.php" method="POST" name="updateProfile">
  		<input type="hidden" name="section" value="updateProfile">
		<input type="hidden" name="action" value="add">
	<table>
		<tr>
			<td align="center" class="titletable"><B>Update profile</B></td>
		</tr>
		<tr>
			<td align="center" class="insidetable">
			<table>
				<tr bgcolor="#DADDE4">
					<td colspan="6" align="center" class="normaltext"><B>Company Information</B></td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>Company Name:</B></td>
					<td colspan="5" align="left" class="normaltext"><input type="text" name="companyName" value="'.$rowDealer['CompanyName'].'"></td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>Current Address:</B></td>
					<td colspan="5" align="left" class="normaltext"><input type="text" name="currentAddress" value="'.$rowDealer['CurrentAddress'].'"></td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>City:</B></td>
					<td align="left" class="normaltext"><input type="text" name="city" value="'.$rowDealer['City'].'"></td>
					<td align="right" class="normaltext"><B>State:</B></td>
					<td align="left" class="normaltext"><input type="text" name="state" value="'.$rowDealer['State'].'"></td>
					<td align="right" class="normaltext"><B>Postal/ZIP:</B></td>
					<td align="left" class="normaltext"><input type="text" name="zip" value="'.$rowDealer['Zip'].'"></td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>Phone:</B></td>
					<td align="left" class="normaltext"><input type="text" name="phone" value="'.$rowDealer['Phone'].'"></td>
					<td align="right" class="normaltext"><B>Fax:</B></td>
					<td align="left" class="normaltext"><input type="text" name="fax" value="'.$rowDealer['Fax'].'"></td>
					<td align="right" class="normaltext"><B>Country:</B></td>
					<td align="left" class="normaltext"><input type="text" name="country" value="'.$rowDealer['Country'].'"></td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>Number of Employees:</B></td>
					<td align="left" class="normaltext"><input type="text" name="employees" value="'.$rowDealer['EmployeesNo'].'"></td>
					<td align="right" class="normaltext"><B>Store Front or Showroom:</B></td>
					<td align="left" class="normaltext" colspan="3"><input type="checkbox" name="storeFront" value="1" '.(($rowDealer['StoreFront']==1)?'checked':'').'></td>
				</tr>
				<tr bgcolor="#DADDE4">
					<td colspan="6" align="center" class="normaltext"><B>Individual Applicant Information</B></td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>Your Name:</B></td>
					<td colspan="5" align="left" class="normaltext"><input type="text" name="individualName" value="'.$rowDealer['IndividualName'].'"></td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>Title:</B></td>
					<td colspan="5" align="left" class="normaltext"><input type="text" name="title" value="'.$rowDealer['Title'].'"></td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>Phone:</B></td>
					<td align="left" class="normaltext"><input type="text" name="individualPhone" value="'.$rowDealer['IndividualPhone'].'"></td>
					<td align="right" class="normaltext"><B>Email:</B></td>
					<td align="left" class="normaltext"><input type="text" name="individualEmail" value="'.$rowDealer['IndividualEmail'].'"></td>
					<td align="right" class="normaltext"><B>Fax:</B></td>
					<td align="left" class="normaltext"><input type="text" name="individualFax" value="'.$rowDealer['IndividualFax'].'"></td>
				</tr>
				<tr bgcolor="#DADDE4">
					<td colspan="6" align="left" class="normaltext"><B>Support & Return Policies:</B></td>
				</tr>			
				<tr>
					<td align="right" class="normaltext">&nbsp;</td>
					<td colspan="5" align="left" class="normaltext">Please explain your return policy and your abilities to provide post-sales support:<br><textarea rows="3" style="width: 480;" name="returnPolicy">'.stripslashes($rowDealer['ReturnPolicy']).'</textarea></td>
				</tr>
				<tr>
					<td colspan="6" align="center" class="normaltext"><input type="submit" name="submitDealer" value="Update"></td>
				</tr>							
			</table>
			</td>
		</tr>
	</table>
	</form>
';
}else{
	// process
	if(isset($_POST['submitDealer'])){
		$companyName=cleanString($_POST['companyName']);
		$currentAddress=cleanString($_POST['currentAddress']);
		$city=cleanString($_POST['city']);
		$state=cleanString($_POST['state']);
		$zip=cleanString($_POST['zip']);
		$phone=cleanString($_POST['phone']);
		$fax=cleanString($_POST['fax']);
		$country=cleanString($_POST['country']);
		$employees=cleanInteger($_POST['employees']);
		$storeFront=isset($_POST['storeFront'])?$_POST['storeFront']:0;
		$individualName=cleanString($_POST['individualName']);
		$title=cleanString($_POST['title']);
		$individualPhone=cleanString($_POST['individualPhone']);
		$individualEmail=cleanString($_POST['individualEmail']);
		$individualFax=cleanString($_POST['individualFax']);
		$returnPolicy=$_POST['returnPolicy'];

		$emailBody='
				<style>
				.normaltext {
				   font-family: Arial, Verdana, Helvetica, sans-serif;
				   font-size: 12px;
				   color: #36456E;
				}
				</style>
				<table>
				<tr>
					<td colspan="6" align="center" class="maintable"><B>Dealer aplication updated</B></td>
				</tr>
				<tr bgcolor="#DADDE4">
					<td colspan="6" align="center" class="normaltext"><B>Company Information</B></td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>Company Name:</B></td>
					<td align="left" class="normaltext">'.$companyName.'</td>
					<td align="right" class="normaltext"><B>Current Address:</B></td>
					<td colspan="3" align="left" class="normaltext">'.$currentAddress.'</td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>City:</B></td>
					<td align="left" class="normaltext">'.$city.'</td>
					<td align="right" class="normaltext"><B>State:</B></td>
					<td align="left" class="normaltext">'.$state.'</td>
					<td align="right" class="normaltext"><B>Postal/ZIP:</B></td>
					<td align="left" class="normaltext">'.$zip.'</td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>Phone:</B></td>
					<td align="left" class="normaltext">'.$phone.'</td>
					<td align="right" class="normaltext"><B>Fax:</B></td>
					<td align="left" class="normaltext">'.$fax.'</td>
					<td align="right" class="normaltext"><B>Country:</B></td>
					<td align="left" class="normaltext">'.$country.'</td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>Number of Employees:</B></td>
					<td align="left" class="normaltext">'.$employees.'</td>
					<td align="right" class="normaltext"><B>Store Front or Showroom:</B></td>
					<td align="left" class="normaltext" colspan="3">'.(($storeFront==1)?'Yes':'No').'</td>
				</tr>
				<tr bgcolor="#DADDE4">
					<td colspan="6" align="center" class="normaltext"><B>Individual Applicant Information</B></td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>Your Name:</B></td>
					<td colspan="5" align="left" class="normaltext">'.$individualName.'</td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>Title:</B></td>
					<td colspan="5" align="left" class="normaltext">'.$title.'</td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>Phone:</B></td>
					<td align="left" class="normaltext">'.$individualPhone.'</td>
					<td align="right" class="normaltext"><B>Email:</B></td>
					<td align="left" class="normaltext">'.$individualEmail.'</td>
					<td align="right" class="normaltext"><B>Fax:</B></td>
					<td align="left" class="normaltext">'.$individualFax.'</td>
				</tr>
				<tr bgcolor="#DADDE4">
					<td colspan="6" align="left" class="normaltext"><B>Support & Return Policies:</B></td>
				</tr>			
				<tr>
					<td align="right" class="normaltext">&nbsp;</td>
					<td colspan="5" align="left" class="normaltext">'.nl2br(stripslashes($returnPolicy)).'</td>
				</tr>
			</table>';

		$updateDealer="
			UPDATE Dealer SET 
				CompanyName='$companyName', 
				CurrentAddress='$currentAddress', 
				City='$city', 
				State='$state', 
				Zip='$zip', 
				Phone='$phone', 
				Fax='$fax', 
				Country='$country', 
				EmployeesNo='$employees', 
				StoreFront='$storeFront', 
				IndividualName='$individualName', 
				Title='$title', 
				IndividualPhone='$individualPhone', 
				IndividualEmail='$individualEmail', 
				IndividualFax='$individualFax', 
				ReturnPolicy='$returnPolicy'
			WHERE PK_Dealer='$dealerID'";
		dbQuery($updateDealer,$conn);
		
		$headers = "From: Pluto Home<website@plutohome.com>\n";
		$headers .= "X-Sender: <website@plutohome.com>\n";
		$headers .= "X-Mailer: PHP\n"; //mailer
		$headers .= "X-Priority: 3\n"; //1 UrgentMessage, 3 Normal
		$headers .= "Return-Path: <website@plutohome.com>\n";
		$headers .= "Content-Type: text/html; charset=iso-8859-1\n";
		$headers .= "cc: \n"; 
		$headers .= "bcc: "; 
	
		$subject = 'New dealer application';
	
		@mail($GLOBALS['sendNotificationsTo'],$subject,$emailBody,$headers);
	}
	$out='
	<table align="center">
		<tr>
			<td class="insidetable" height="250">
				<p class="normaltext"><B>Your profile was updated.</B>
				<p class="normaltext">
	
			</td>
		<tr>
	</table>';
}


	$output->setScriptCalendar('null');
	$output->setScriptTRColor('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME."::Client");
	$output->output();
}
?>