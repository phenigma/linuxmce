<?
function dealerApplication($output,$conn) {
	/* @var $dbADO ADOConnection */
	$businessArray=array(1=>'New-Startup',2=>'Security', 3=>'Audio and Video', 4=>'Satellite', 5=>'Electrical', 6=>'Consumer Electronics',7=>'Home Networking', 8=>'Home Automation', 9=>'Other');
	$marketFocusArray=array(1=>'New Construction', 2=>'Residential Retrofit', 3=>'Small Business', 4=>'High-End Custom Homes',5=>'Other');
	$estimatedRevenueArray=array(1=>'$0-$50K', 2=>'$50K-$250K', 3=>'$250K-$500K', 4=>'$500K-$750K', 5=>'$750K-$1M', 6=>'1M+');		
	
	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';	
	$out='';
	
if($action=='form'){
	$out .= '
	<style>
	hr	{ height: 0px; border: solid #D1D7DC 0px; border-top-width: 1px;}
	</style>
	<form action="index.php" method="POST" name="dealerApplication">
  		<input type="hidden" name="section" value="dealerApplication">
		<input type="hidden" name="action" value="add">
	<table>
		<tr>
			<td align="center" class="titletable"><B>Pluto Authorized Dealer Program Application</B></td>
		</tr>
		<tr>
			<td class="insidetable">
				Pluto Home accepts applicants as Pluto Authorized Dealers in its sole discretion.  Applications will not be processed or considered unless all questions are answered.
<br><br>
Pluto Home will use reasonable efforts to review your application and contact you within 10 days regarding the status of your membership into the Pluto Authorized Dealer Program. Pluto Home reserves the right to make changes to, make exceptions to, or terminate the Pluto Authorized Dealer Program and/or any benefits, policies and procedures there under at any time at its sole discretion. Dealer’s membership in the Pluto Authorized Dealer Program may be terminated as set forth in the Pluto Authorized Dealer Program Terms.
			</td>
		</tr>
		<tr>
			<td align="center" class="insidetable">
			<table>
				<tr bgcolor="#DADDE4">
					<td colspan="6" align="center" class="normaltext"><B>Company Information</B></td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>Company Name:</B></td>
					<td colspan="5" align="left" class="normaltext"><input type="text" name="companyName" value=""></td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>Current Address:</B></td>
					<td colspan="5" align="left" class="normaltext"><input type="text" name="currentAddress" value=""></td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>City:</B></td>
					<td align="left" class="normaltext"><input type="text" name="city" value=""></td>
					<td align="right" class="normaltext"><B>State:</B></td>
					<td align="left" class="normaltext"><input type="text" name="state" value=""></td>
					<td align="right" class="normaltext"><B>Postal/ZIP:</B></td>
					<td align="left" class="normaltext"><input type="text" name="zip" value=""></td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>Phone:</B></td>
					<td align="left" class="normaltext"><input type="text" name="phone" value=""></td>
					<td align="right" class="normaltext"><B>Fax:</B></td>
					<td align="left" class="normaltext"><input type="text" name="fax" value=""></td>
					<td align="right" class="normaltext"><B>Country:</B></td>
					<td align="left" class="normaltext"><input type="text" name="country" value=""></td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>Number of Employees:</B></td>
					<td align="left" class="normaltext"><input type="text" name="employees" value=""></td>
					<td align="right" class="normaltext"><B>Store Front or Showroom:</B></td>
					<td align="left" class="normaltext" colspan="3"><input type="checkbox" name="storeFront" value="1"></td>
				</tr>
				<tr bgcolor="#DADDE4">
					<td colspan="6" align="center" class="normaltext"><B>Individual Applicant Information</B></td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>Your Name:</B></td>
					<td colspan="5" align="left" class="normaltext"><input type="text" name="individualName" value=""></td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>Title:</B></td>
					<td colspan="5" align="left" class="normaltext"><input type="text" name="title" value=""></td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>Phone:</B></td>
					<td align="left" class="normaltext"><input type="text" name="individualPhone" value=""></td>
					<td align="right" class="normaltext"><B>Email:</B></td>
					<td align="left" class="normaltext"><input type="text" name="individualEmail" value=""></td>
					<td align="right" class="normaltext"><B>Fax:</B></td>
					<td align="left" class="normaltext"><input type="text" name="individualFax" value=""></td>
				</tr>
				<tr bgcolor="#DADDE4">
					<td colspan="6" align="center" class="normaltext"><B>Business Information</B></td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>Existing Core Business:</B></td>
					<td colspan="2" align="left" class="normaltext">';
				foreach($businessArray AS $key=>$label){
					$out.='<input type="checkbox" name="existingBusiness_'.$key.'" value="'.$key.'"> '.$label.(($label=='Other')?' <input type="text" name="otherBusiness" value="">':'').'<br>';
				}
				$out.='
					</td>
					<td align="right" class="normaltext"><B>Market Focus:</B></td>
					<td align="left" class="normaltext" colspan="2">';
				foreach($marketFocusArray AS $key=>$label){
					$out.='<input type="checkbox" name="marketFocus_'.$key.'" value="'.$key.'"> '.$label.(($label=='Other')?' <input type="text" name="otherMarketFocus" value="">':'').'<br>';
				}
				$out.='
				</td>
				</tr>	
				<tr>
					<td colspan="6" align="center" class="normaltext"><hr></td>
				</tr>
				<tr>
					<td colspan="2" align="right" class="normaltext"><B>Other Products Your Company Represents:</B></td>
					<td colspan="4" align="left" class="normaltext"><input type="text" name="otherProducts" value=""></td>
				</tr>
				<tr>
					<td colspan="6" align="left" class="normaltext"><B>Estimated Current Annual Revenue in smart home products:</B></td>
				</tr>
				<tr>
					<td align="right" class="normaltext">&nbsp;</td>
					<td colspan="5" align="left" class="normaltext">';				
				foreach($estimatedRevenueArray AS $key=>$label){
					$out.=' <input type="radio" name="estimatedRevenue" value="'.$key.'" '.(($key==1)?'checked':'').'> '.$label;
				}
				$out.='
					</td>
				</tr>
				<tr bgcolor="#DADDE4">
					<td colspan="6" align="center" class="normaltext"><B>Trade References – Manufacturers you carry</B></td>
				</tr>
				<tr>
					<td colspan="6" align="left" class="normaltext"><B>Please Provide 3 SmartHome/Media vendors whose products you are authorized to sell:</B></td>
				</tr>
				<tr>
					<td colspan="2" align="center" class="normaltext"><B>Name:</B></td>
					<td colspan="2" align="center" class="normaltext"><B>Address:</B></td>
					<td colspan="2" align="center" class="normaltext"><B>Phone:</B></td>
				</tr>
				<tr>
					<td colspan="2" align="center" class="normaltext"><input type="text" name="referenceName_1" value=""></td>
					<td colspan="2" align="center" class="normaltext"><input type="text" name="referenceAddress_1" value=""></td>
					<td colspan="2" align="center" class="normaltext"><input type="text" name="referencePhone_1" value=""></td>
				</tr>
				<tr>
					<td colspan="2" align="center" class="normaltext"><input type="text" name="referenceName_2" value=""></td>
					<td colspan="2" align="center" class="normaltext"><input type="text" name="referenceAddress_2" value=""></td>
					<td colspan="2" align="center" class="normaltext"><input type="text" name="referencePhone_2" value=""></td>
				</tr>
				<tr>
					<td colspan="2" align="center" class="normaltext"><input type="text" name="referenceName_3" value=""></td>
					<td colspan="2" align="center" class="normaltext"><input type="text" name="referenceAddress_3" value=""></td>
					<td colspan="2" align="center" class="normaltext"><input type="text" name="referencePhone_3" value=""></td>
				</tr>
				<tr bgcolor="#DADDE4">
					<td colspan="6" align="left" class="normaltext"><B>Support & Return Policies:</B></td>
				</tr>			
				<tr>
					<td align="right" class="normaltext">&nbsp;</td>
					<td colspan="5" align="left" class="normaltext">Please explain your return policy and your abilities to provide post-sales support:<br><textarea rows="3" style="width: 480;" name="returnPolicy"></textarea></td>
				</tr>
				<tr>
					<td colspan="6" align="left" class="normaltext"><hr><input type="checkbox" name="agreedProgram" value="1"> I have read the <a href="index.php?section=programGuidelines" target="_blank">Program Guidelines</a> in <a href="index.php?section=schedule&sch=1" target="_blank">Schedule 1</a>, <a href="index.php?section=schedule&sch=2" target="_blank">Schedule 2</a>, and for Pluto Pro dealers only, <a href="index.php?section=schedule&sch=3" target="_blank">Schedule 3</a>.</td>
				</tr>							
				<tr>
					<td colspan="6" align="left" class="normaltext"><input type="checkbox" name="agreedTerms" value="1"> I have read and agree to the terms and conditions <a href="index.php?section=dealerTerms" target="_blank">listed here</a>.  Should any of the information change, I will inform Pluto Home promptly in within 20 days.</td>
				</tr>							
				<tr>
					<td colspan="6" align="center" class="normaltext">Please sign this application by typing your name here: <input type="text" name="sign" value=""> <input type="submit" name="submitDealer" value="Submit"></td>
				</tr>							
				
			</table>
			</td>
		</tr>
	</table>
	</form>
	<script>
		var frmvalidator = new formValidator("dealerApplication");
 		frmvalidator.addValidation("agreedProgram","selectchk","You must read the Program Guidelines and the schedules.");			
		frmvalidator.addValidation("agreedTerms","selectchk","You must agree the terms and conditions.");			
		frmvalidator.addValidation("sign","req","You must sign this application.");			
	</script>				
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
		$coreBusinessArray=array();
		foreach($businessArray AS $key=>$label){
			if(isset($_POST['existingBusiness_'.$key]))
				$coreBusinessArray[]=$label;
		}
		$coreBusiness=join(', ',$coreBusinessArray);
		$otherBusiness=cleanString($_POST['otherBusiness']);
		$marketArray=array();
		foreach($marketFocusArray AS $key=>$label){
			if(isset($_POST['marketFocus_'.$key]))
				$marketArray[]=$label;
		}
		$marketFocus=join(', ',$marketArray);
		$otherMarketFocus=cleanString($_POST['otherMarketFocus']);
		$otherProducts=cleanString($_POST['otherProducts']);
		$estimatedRevenue=$estimatedRevenueArray[(int)$_POST['estimatedRevenue']];
		$reference1='Name: '.cleanString($_POST['referenceName_1']).'; Address: '.cleanString($_POST['referenceAddress_1']).'; Phone: '.cleanString($_POST['referencePhone_1']);
		$reference2='Name: '.cleanString($_POST['referenceName_2']).'; Address: '.cleanString($_POST['referenceAddress_2']).'; Phone: '.cleanString($_POST['referencePhone_2']);
		$reference3='Name: '.cleanString($_POST['referenceName_3']).'; Address: '.cleanString($_POST['referenceAddress_3']).'; Phone: '.cleanString($_POST['referencePhone_3']);
		$returnPolicy=addslashes($_POST['returnPolicy']);
		$signedAs=cleanString($_POST['sign']);

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
					<td colspan="6" align="center" class="maintable"><B>New dealer application</B></td>
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
					<td colspan="6" align="center" class="normaltext"><B>Business Information</B></td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>Existing Core Business:</B></td>
					<td colspan="2" align="left" class="normaltext">'.$coreBusiness.(($otherBusiness!='')?': '.$otherBusiness:'').'</td>
					<td align="right" class="normaltext"><B>Market Focus:</B></td>
					<td align="left" class="normaltext" colspan="2">'.$marketFocus.(($otherMarketFocus!='')?': '.$otherMarketFocus:'').'</td>
				</tr>	
				<tr>
					<td colspan="6" align="center" class="normaltext"><hr></td>
				</tr>
				<tr>
					<td colspan="2" align="right" class="normaltext"><B>Other Products Your Company Represents:</B></td>
					<td colspan="4" align="left" class="normaltext">'.$otherProducts.'</td>
				</tr>
				<tr>
					<td colspan="6" align="left" class="normaltext"><B>Estimated Current Annual Revenue in smart home products:</B></td>
				</tr>
				<tr>
					<td align="right" class="normaltext">&nbsp;</td>
					<td colspan="5" align="left" class="normaltext">'.$estimatedRevenue.'</td>
				</tr>
				<tr bgcolor="#DADDE4">
					<td colspan="6" align="center" class="normaltext"><B>Trade References </B></td>
				</tr>
				<tr>
					<td colspan="6" align="left" class="normaltext"><B>SmartHome/Media vendors whose products applicant is authorized to sell:</B></td>
				</tr>
				<tr>
					<td colspan="6" align="left" class="normaltext">'.$reference1.'</td>
				</tr>
				<tr>
					<td colspan="6" align="left" class="normaltext">'.$reference2.'</td>
				</tr>
				<tr>
					<td colspan="6" align="left" class="normaltext">'.$reference3.'</td>
				</tr>
				<tr bgcolor="#DADDE4">
					<td colspan="6" align="left" class="normaltext"><B>Support & Return Policies:</B></td>
				</tr>			
				<tr>
					<td align="right" class="normaltext">&nbsp;</td>
					<td colspan="5" align="left" class="normaltext">'.nl2br($returnPolicy).'</td>
				</tr>
				<tr>
					<td colspan="6" align="center" class="normaltext">Sign in as: '.$signedAs.'</td>
				</tr>							
			</table>';

		$insertDealer="
			INSERT INTO Dealer
				(SubmitDate,CompanyName, CurrentAddress, City, State, Zip, Phone, Fax, Country, EmployeesNo, StoreFront, IndividualName, Title, IndividualPhone, IndividualEmail, IndividualFax,CoreBusiness, OtherBusiness, MarketFocus, OtherMarketFocus,OtherProducts, EstimatedRevenue, Reference1, Reference2, Reference3, ReturnPolicy,SignInAs)
			VALUES
				(NOW(), '$companyName', '$currentAddress', '$city', '$state', '$zip', '$phone', '$fax', '$country', '$employees', '$storeFront', '$individualName', '$title', '$individualPhone', '$individualEmail', '$individualFax', '$coreBusiness', '$otherBusiness', '$marketFocus', '$otherMarketFocus','$otherProducts', '$estimatedRevenue', '$reference1', '$reference2', '$reference3', '$returnPolicy','$signedAs')";
		dbQuery($insertDealer,$conn);
		
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
				<p class="normaltext"><B>Thank you for your application.</B>
				<p class="normaltext">Pluto Home will use reasonable efforts to review your application and contact you within 10 days regarding the status of your membership into the Pluto Authorized Dealer Program. Pluto Home reserves the right to make changes to, make exceptions to, or terminate the Pluto Authorized Dealer Program and/or any benefits, policies and procedures there under at any time at its sole discretion. Dealer’s membership in the Pluto Authorized Dealer Program may be terminated as set forth in the Pluto Authorized Dealer Program Terms.
	
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