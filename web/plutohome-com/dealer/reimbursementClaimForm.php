<?
function reimbursementClaimForm($output,$conn) {
	/* @var $dbADO ADOConnection */
	
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';	
	$out='';
	
if($action=='form'){
	$out .= '
	<style>
	hr	{ height: 0px; border: solid #D1D7DC 0px; border-top-width: 1px;}
	</style>
	<form action="index.php" method="POST" name="reimbursementClaimForm">
  		<input type="hidden" name="section" value="reimbursementClaimForm">
		<input type="hidden" name="action" value="add">
<table width="100%"  border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td align="center" class="insidetable"><table width="563"  border="0" cellpadding="0" cellspacing="0">
      <tr>
        <td align="left"><img src="images/titles/dealer.gif"></td>
      </tr>
    </table></td>
  </tr>
		<tr class="insidetable2">
			<td align="center" class="title"><B>Pluto Marketing Reimbursement Claim Form</B></td>
		</tr>
		<tr>
			<td class="insidetable2">
				<I>Complete this form following the marketing activity, and include copies of paid invoices. Mail to your Pluto Home Marketing Manager. See below for additional documentation required.</I>
			</td>
		</tr>
		<tr>
			<td align="center" class="insidetable2">
			<table>
				<tr bgcolor="#DADDE4">
					<td colspan="6" align="center" class="normaltext"><B>Company Information</B></td>
				</tr>
				<tr>
					<td colspan="2" align="right" class="normaltext"><B>Dealer Company Name:</B></td>
					<td colspan="4" align="left" class="normaltext"><input type="text" class="textfield" name="companyName" value=""></td>
				</tr>
				<tr>
					<td colspan="2" align="right" class="normaltext"><B>Your name:</B></td>
					<td colspan="4" align="left" class="normaltext"><input type="text" class="textfield" name="individualName" value=""></td>
				</tr>
				<tr>
					<td colspan="2" align="right" class="normaltext"><B>Approved Reimbursement Total:$:</B></td>
					<td colspan="4" align="left" class="normaltext"><input type="text" class="textfield" name="approvedReimbursement" value=""></td>
				</tr>	
				<tr>
					<td align="right" class="normaltext"><B>Phone:</B></td>
					<td align="left" class="normaltext"><input type="text" class="textfield" name="phone" value=""></td>
					<td align="right" class="normaltext"><B>Fax:</B></td>
					<td align="left" class="normaltext"><input type="text" class="textfield" name="fax" value=""></td>
					<td align="right" class="normaltext"><B>Assigned Activity#:</B></td>
					<td align="left" class="normaltext"><input type="text" class="textfield" name="asignedActivity" value=""></td>
				</tr>
				<tr>
					<td colspan="6" align="center" class="normaltext"><input type="submit" name="submit" value="Submit"></td>
				</tr>							
				<tr>
					<td colspan="6" align="center" class="normaltext">&nbsp;</td>
				</tr>							
				<tr>
					<td colspan="6" align="left" class="normaltext">
						<p><B><I>When submitting your Claim Form for reimbursement, please include copies of all paid invoices and the following documentation:</I></B>
						<p><B><I>For print advertising</I></B>
						<br>For newspaper and magazine advertising, an original full-page tear sheet with the ad and the date of the publication on it.
						<p><B><I>For broadcast advertising</I></B>
						<br>A copy of the script with notarized affidavit of performance from the radio or television station attached, along with the ad on audio or videotape. Your Sales Representative at the radio or television station can provide these items at your request.
						<p><B><I>For telemarketing</I></B>
						<br>Detailed call records
					</td>
				</tr>							
			</table>
			</td>
		</tr>
	</table>
	</form>
';
}else{
	// process
	if(isset($_POST['submit'])){
		$companyName=cleanString($_POST['companyName']);
		$individualName=cleanString($_POST['individualName']);
		$approvedReimbursement=cleanString($_POST['approvedReimbursement']);
		$phone=cleanString($_POST['phone']);
		$fax=cleanString($_POST['fax']);
		$assignedActivity=cleanInteger($_POST['asignedActivity']);

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
					<td colspan="6" align="center" class="titletable"><B>New Pluto Marketing Reimbursement Claim</B></td>
				</tr>
				<tr bgcolor="#DADDE4">
					<td colspan="6" align="center" class="normaltext"><B>Company Information</B></td>
				</tr>
				<tr>
					<td colspan="2" align="right" class="normaltext"><B>Dealer Company Name:</B></td>
					<td colspan="4" align="left" class="normaltext">'.$companyName.'</td>
				</tr>
				<tr>
					<td colspan="2" align="right" class="normaltext"><B>Your name:</B></td>
					<td colspan="4" align="left" class="normaltext">'.$individualName.'</td>
				</tr>
				<tr>
					<td colspan="2" align="right" class="normaltext"><B>Approved Reimbursement Total:$:</B></td>
					<td colspan="4" align="left" class="normaltext">'.$approvedReimbursement.'</td>
				</tr>	
				<tr>
					<td align="right" class="normaltext"><B>Phone:</B></td>
					<td align="left" class="normaltext">'.$phone.'</td>
					<td align="right" class="normaltext"><B>Fax:</B></td>
					<td align="left" class="normaltext">'.$fax.'</td>
					<td align="right" class="normaltext"><B>Assigned Activity#:</B></td>
					<td align="left" class="normaltext">'.$assignedActivity.'</td>
				</tr>
			</table>		
		';

		$insertFundRequest="
			INSERT INTO Reimbursement
				(CompanyName, IndividualName, ApprovedReimbursement, Phone, Fax, AsignedActivity,Date)
			VALUES
				('$companyName', '$individualName', '$approvedReimbursement','$phone', '$fax', '$assignedActivity',NOW())";
		dbQuery($insertFundRequest,$conn);
		
		$headers = "From: Pluto Home<website@plutohome.com>\n";
		$headers .= "X-Sender: <website@plutohome.com>\n";
		$headers .= "X-Mailer: PHP\n"; //mailer
		$headers .= "X-Priority: 3\n"; //1 UrgentMessage, 3 Normal
		$headers .= "Return-Path: <website@plutohome.com>\n";
		$headers .= "Content-Type: text/html; charset=iso-8859-1\n";
		$headers .= "cc: \n"; 
		$headers .= "bcc: "; 
	
		$subject = 'New Pluto Marketing Reimbursement Claim';
	
		@mail($GLOBALS['sendNotificationsTo'],$subject,$emailBody,$headers);
	}
	$out='
	<table align="center">
		<tr>
			<td class="insidetable2" height="250">
				<p class="normaltext"><B>Thank you.</B>
				<p class="normaltext">Pluto Home Marketing Manager will answer as soon as posible.
			</td>
		<tr>
	</table>';
}


	$output->setPressedButton(5);
	$output->setNavigationMenu(array('Get Pluto'=>'index.php?section=get_pluto','I\'m a dealer'=>'index.php?section=dealer','Dealer Program'=>'index.php?section=dealerProgram','Reimbursement Claim Form'=>'index.php?section=reimbursementClaimForm'));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME."::Client");
	$output->output();
}
?>