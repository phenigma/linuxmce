<?
function fundRequestApplication($output,$conn) {
	/* @var $dbADO ADOConnection */
	$promoInfoArray=array(1=>'Print Advertising', 2=>'Telemarketing', 3=>'Broadcast Advertising', 4=>'Web Advertising', 5=>'Outdoor Advertising', 6=>'Promotional Item', 7=>'Direct Mail Program', 8=>'Trade Show/Seminar/Event');
	
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';	
	$out='';
	
if($action=='form'){
	$out .= '
	<style>
	hr	{ height: 0px; border: solid #D1D7DC 0px; border-top-width: 1px;}
	</style>
	<form action="index.php" method="POST" name="fundRequestApplication">
  		<input type="hidden" name="section" value="fundRequestApplication">
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
			<td align="center" class="title"><B>Pluto Marketing Funds Request Form</B></td>
		</tr>
		<tr>
			<td class="insidetable2">
				<I>Complete this form and submit with supporting documentation at least 4 weeks prior to the marketing activity to the Pluto Home Marketing Manager</I>
			</td>
		</tr>
		<tr>
			<td align="center" class="insidetable2">
			<table>
				<tr bgcolor="#DADDE4">
					<td colspan="6" align="center" class="normaltext"><B>Company Information</B></td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>Dealer Company Name:</B></td>
					<td colspan="5" align="left" class="normaltext"><input type="text" class="textfield" name="companyName" value=""></td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>Your name:</B></td>
					<td colspan="5" align="left" class="normaltext"><input type="text" class="textfield" name="individualName" value=""></td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>Phone:</B></td>
					<td align="left" class="normaltext"><input type="text" class="textfield" name="phone" value=""></td>
					<td align="right" class="normaltext"><B>Fax:</B></td>
					<td align="left" class="normaltext"><input type="text" class="textfield" name="fax" value=""></td>
					<td align="right" class="normaltext"><B>Email:</B></td>
					<td align="left" class="normaltext"><input type="text" class="textfield" name="email" value=""></td>
				</tr>
				<tr bgcolor="#DADDE4">
					<td colspan="6" align="center" class="normaltext"><B>Promotional  Information</B></td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>Check item below:</B></td>
					<td colspan="5" align="left" class="normaltext">';
				foreach($promoInfoArray AS $key=>$label){
					$out.='<input type="checkbox" name="promoInfo_'.$key.'" value="'.$key.'"> '.$label.(($label=='Other')?' <input type="text" class="textfield" name="otherBusiness" value="">':'').'<br>';
				}
				$out.='
						Other: (please specify) <input type="text" class="textfield" name="otherPromoInformation" value="">
					</td>
				</tr>	
				<tr>
					<td colspan="6" align="center" class="normaltext"><hr></td>
				</tr>
				<tr>
					<td align="right" class="normaltext">&nbsp;</td>
					<td colspan="5" align="left" class="normaltext"><B>Briefly Describe the object of your Marketing activity:</B><br><textarea rows="2" style="width: 380;" name="marketingActivity"></textarea></td>
				</tr>
				<tr>
					<td colspan="3" align="right" class="normaltext"><B>Dates For Activity:</B> <input type="text" class="textfield" name="activityDates" value=""></td>
					<td colspan="3" align="left" class="normaltext"><B>Target Audience:</B> <input type="text" class="textfield" name="targetAudience" value=""></td>
				</tr>
				<tr>
					<td align="right" class="normaltext">&nbsp;</td>
					<td colspan="5" align="left" class="normaltext"><B>Briefly Describe the expected results of your Marketing activity:</B><br><textarea rows="2" style="width: 380;" name="expectedResults"></textarea></td>
				</tr>
				<tr>
					<td colspan="6" align="center" class="normaltext"><hr></td>
				</tr>				
				<tr>
					<td align="right" class="normaltext">&nbsp;</td>
					<td colspan="5" align="left" class="normaltext"><B>Briefly Describe the follow up plan:</B><br><textarea rows="2" style="width: 380;" name="followUpPlan"></textarea></td>
				</tr>
				<tr bgcolor="#DADDE4">
					<td colspan="6" align="center" class="normaltext"><B>Activity Cost Information and Approval</B></td>
				</tr>				
				<tr>
					<td colspan="2" align="right" class="normaltext"><B>Please list all costs for Activity:</B></td>
					<td colspan="4" align="left" class="normaltext"><input type="text" class="textfield" name="listAllCosts" value=""></td>
				</tr>				
				<tr>
					<td colspan="2" align="right" class="normaltext"><B>Total of all cost for Activity:</B></td>
					<td colspan="4" align="left" class="normaltext"><input type="text" class="textfield" name="totalAllCosts" value=""></td>
				</tr>				
				<tr>
					<td colspan="2" align="right" class="normaltext"><B>Total Amount Requested for Reimbursement from Pluto Home:</B></td>
					<td colspan="4" align="left" class="normaltext"><input type="text" class="textfield" name="totalFromPluto" value=""></td>
				</tr>				
				<tr>
					<td colspan="6" align="center" class="normaltext"><hr><I>Based on the information in this proposal and pending appropriate documentation for reimbursement, this request for Pluto Home Marketing Support or Marketing Development Funds is approved.</I></td>
				</tr>							
				<tr>
					<td colspan="6" align="center" class="normaltext"><input type="submit" name="requestFunds" value="Submit"></td>
				</tr>							
				<tr>
					<td colspan="6" align="center" class="normaltext">&nbsp;</td>
				</tr>							
				<tr bgcolor="#DADDE4">
					<td colspan="6" align="center" class="normaltext"><B>Request Form Supporting Documentation</B></td>
				</tr>				
				<tr>
					<td colspan="6" align="left" class="normaltext">
						<p><B><I>Please include the following documentation with your Request Form, and submit 4 weeks prior to marketing activity.</I></B>
						<p><B><I>For print advertising</I></B>
						<br>A copy of the ad to be placed and a schedule of ad run dates and publications.
						<p><B><I>For broadcast advertising</I></B>
						<br>A copy of the proposed radio or television script (and story board for television) with a run schedule and/or list of stations you plan to use. As a general rule, the Pluto name should be mentioned at least twice in the ad copy. Please attach a schedule of stations and air times with the script.
						<p><B><I>For outdoors advertising</I></B>
						<br>A drawing or picture of the proposed billboard, transit sign, etc. is needed. The Pluto logo must be prominently displayed on the billboard in order for the project to be funded by Pluto Home 
						<p><B><I>For direct mail programs</I></B>
						<br>A copy of every piece that’s going in the mailing, plus an estimate or exact figure of the postal costs is needed. If the pieces are being developed by a marketing/advertising agency and Pluto is to be the featured Product, then it would be advisable to involve Pluto Home during the design stage. Creative and development costs and mailing list costs are not eligible for funding under the guidelines of the Pluto Home Co-op Marketing Program.
						<p><B><I>For trade shows, seminars, events</I></B>
						<br>A diagram of the layout of the booth or room indicating Pluto coverage, a detailed plan of the event including target audience, and samples of materials must accompany your request. Also, provide details of other vendors being represented.
						<p><B><I>For telemarketing</I></B>
						<br>A statement of the purpose of your telemarketing campaign along with who the target market is. A copy of the proposed script along with a completed call information sheet attached is needed. If an outside vendor is doing the telemarketing campaign, please attach copies of all documentation received from that vendor.
						<p><B><I>For web advertising</I></B>
						<br>A print out of the proposed ad or the URL that can be used for viewing the proposed ad is needed.
						<p><B><I>For promotional items/premiums </I></B>
						<br>A sketch of the proposed promotional item, indicating the placement of the Pluto logo and Dealer logo, and a sample of the proposed item is needed.
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
	if(isset($_POST['requestFunds'])){
		$companyName=cleanString($_POST['companyName']);
		$individualName=cleanString($_POST['individualName']);
		$phone=cleanString($_POST['phone']);
		$fax=cleanString($_POST['fax']);
		$email=cleanString($_POST['email']);
		$promoArray=array();
		foreach($promoInfoArray AS $key=>$label){
			if(isset($_POST['promoInfo_'.$key]))
				$promoArray[]=$label;
		}
		$promotionalInformation=join(', ',$promoArray);
		$otherPromoInformation=cleanString($_POST['otherPromoInformation']);
		$marketingActivityObject=stripslashes(cleanString($_POST['marketingActivity']));
		$activityDates=cleanString($_POST['activityDates']);
		$targetAudience=cleanString($_POST['targetAudience']);
		$expectedResults=stripslashes(cleanString($_POST['expectedResults']));
		$followUpPlan=stripslashes(cleanString($_POST['followUpPlan']));
		$listAllCosts=cleanString($_POST['listAllCosts']);
		$totalAllCosts=cleanInteger($_POST['totalAllCosts']);
		$totalFromPluto=cleanString($_POST['totalFromPluto']);

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
					<td colspan="6" align="center" class="titletable"><B>New Fund Request</B></td>
				</tr>
				<tr bgcolor="#DADDE4">
					<td colspan="6" align="center" class="normaltext"><B>Company Information</B></td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>Dealer Company Name:</B></td>
					<td colspan="5" align="left" class="normaltext">'.$companyName.'</td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>Your name:</B></td>
					<td colspan="5" align="left" class="normaltext">'.$individualName.'</td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>Phone:</B></td>
					<td align="left" class="normaltext">'.$phone.'</td>
					<td align="right" class="normaltext"><B>Fax:</B></td>
					<td align="left" class="normaltext">'.$email.'</td>
					<td align="right" class="normaltext"><B>Email:</B></td>
					<td align="left" class="normaltext">'.$email.'</td>
				</tr>
				<tr bgcolor="#DADDE4">
					<td colspan="6" align="center" class="normaltext"><B>Promotional  Information</B></td>
				</tr>
				<tr>
					<td colspan="6" align="left" class="normaltext">'.$promotionalInformation.(($otherPromoInformation!='')?', Other: '.$otherPromoInformation:'').'</td>
				</tr>	
				<tr>
					<td colspan="6" align="center" class="normaltext"><hr></td>
				</tr>
				<tr>
					<td align="right" class="normaltext">&nbsp;</td>
					<td colspan="5" align="left" class="normaltext"><B>The object of the Marketing activity:</B><br>'.nl2br(stripslashes($marketingActivityObject)).'</td>
				</tr>
				<tr>
					<td colspan="3" align="right" class="normaltext"><B>Dates For Activity:</B> '.$activityDates.'</td>
					<td colspan="3" align="left" class="normaltext"><B>Target Audience:</B> '.$targetAudience.'</td>
				</tr>
				<tr>
					<td align="right" class="normaltext">&nbsp;</td>
					<td colspan="5" align="left" class="normaltext"><B>Expected results of the Marketing activity:</B><br>'.nl2br(stripslashes($expectedResults)).'</td>
				</tr>
				<tr>
					<td colspan="6" align="center" class="normaltext"><hr></td>
				</tr>				
				<tr>
					<td align="right" class="normaltext">&nbsp;</td>
					<td colspan="5" align="left" class="normaltext"><B>The follow up plan:</B><br>'.nl2br($followUpPlan).'</td>
				</tr>
				<tr bgcolor="#DADDE4">
					<td colspan="6" align="center" class="normaltext"><B>Activity Cost Information and Approval</B></td>
				</tr>				
				<tr>
					<td colspan="2" align="right" class="normaltext"><B>Please list all costs for Activity:</B></td>
					<td colspan="4" align="left" class="normaltext">'.$listAllCosts.'</td>
				</tr>				
				<tr>
					<td colspan="2" align="right" class="normaltext"><B>Total of all cost for Activity:</B></td>
					<td colspan="4" align="left" class="normaltext">'.$totalAllCosts.'</td>
				</tr>				
				<tr>
					<td colspan="2" align="right" class="normaltext"><B>Total Amount Requested for Reimbursement from Pluto Home:</B></td>
					<td colspan="4" align="left" class="normaltext">'.$totalFromPluto.'</td>
				</tr>				
			</table>		
		';

		$insertFundRequest="
			INSERT INTO FundRequest
				(CompanyName, IndividualName, Phone, Fax, Email, PromotionalInformation, OtherPromoInformation, MarketingActivityObject, ActivityDates, TargetAudience, ExpectedResults, FollowUpPlan, ListAllCosts, TotalAllCosts, TotalFromPluto,Date)
			VALUES
				('$companyName', '$individualName', '$phone', '$fax', '$email', '$promotionalInformation', '$otherPromoInformation', '$marketingActivityObject', '$activityDates', '$targetAudience', '$expectedResults', '$followUpPlan', '$listAllCosts', '$totalAllCosts', '$totalFromPluto',NOW())";
		dbQuery($insertFundRequest,$conn);
		
		$headers = "From: Pluto Home<website@plutohome.com>\n";
		$headers .= "X-Sender: <website@plutohome.com>\n";
		$headers .= "X-Mailer: PHP\n"; //mailer
		$headers .= "X-Priority: 3\n"; //1 UrgentMessage, 3 Normal
		$headers .= "Return-Path: <website@plutohome.com>\n";
		$headers .= "Content-Type: text/html; charset=iso-8859-1\n";
		$headers .= "cc: \n"; 
		$headers .= "bcc: "; 
	
		$subject = 'New fund request';
	
		@mail($GLOBALS['sendNotificationsTo'],$subject,$emailBody,$headers);
	}
	$out='
	<table align="center">
		<tr>
			<td class="insidetable2" height="250">
				<p class="normaltext"><B>Thank you.</B>
				<p class="normaltext">Based on the information in this proposal and pending appropriate documentation for reimbursement, this request for Pluto Home Marketing Support or Marketing Development Funds is approved.
			</td>
		<tr>
	</table>';
}

	$output->setPressedButton(5);
	$output->setNavigationMenu(array('Get Pluto'=>'index.php?section=get_pluto','I\'m a dealer'=>'index.php?section=dealer','Dealer Program'=>'index.php?section=dealerProgram','Fund Request Form'=>'index.php?section=fundRequestApplication'));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME."::Client");
	$output->output();
}
?>