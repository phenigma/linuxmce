<?
function edit_dealer_profile($output,$conn,$dbADO) {
	/* @var $dbADO ADOConnection */
	global $dbDealerServer, $dbDealerUser, $dbDealerPass, $dbDealerDatabase;

	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';	
	$out='';
	
	$FK_Users=(isset($_SESSION['userID']))?(int)$_SESSION['userID']:0;
	$dealerADO=dbConnection($dbDealerServer,$dbDealerUser,$dbDealerPass,$dbDealerDatabase);
	$userDealer=getFieldsAsArray('Dealer_Users','Title,Validated,CanEditProfile,FK_Dealer',$dealerADO,' WHERE EK_Users='.$FK_Users);
	$dealerID=(int)@$userDealer['FK_Dealer'][0];
	$countriesArray=getAssocArray('Country','PK_Country','Description',$dbADO,'ORDER BY Description ASC');

	$confArray=array(0=>'N',1=>'Y');
	
	if($dealerID==0 || @$userDealer['Validated'][0]!=1){
		header('Location: index.php?section=dealer_signin');
		exit();
	}else{
		$dealerData=getFieldsAsArray('Dealer','Name, Address1, Address2, City, State, PostalCode, EK_Country, Phone, Phone_TollFree, Phone_Support, Fax, Email, IsProDealer, SellsCores, SellsMDs, SellsOrbiters, SellsAccessories, SellsAutomation, CustomInstallation, ListInDirectory',$dealerADO,'WHERE PK_Dealer='.$dealerID);
	}	

	
if($action=='form'){
	$out .= '
	<style>
	hr	{ height: 0px; border: solid #D1D7DC 0px; border-top-width: 1px;}
	</style>
	<form action="index.php" method="POST" Name="edit_dealer_profile">
  		<input type="hidden" Name="section" value="edit_dealer_profile">
		<input type="hidden" Name="action" value="add">
	<table width="100%"  border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td align="left" class="insidetable"><table width="563"  border="0" cellpadding="0" cellspacing="0">
      <tr>
        <td align="left"><img src="images/titles/dealer.gif"></td>
      </tr>
    </table></td>
  </tr>
	<tr>
		<td class="normaltext" align="left">&nbsp;<a href="index.php?section=dealer_area">&lt;&lt; Back</a></td>
	</tr>	
		<tr class="insidetable2">
			<td align="center" class="Title" height="30"><B>Edit Dealer</B></td>
		</tr>
		<tr>
			<td class="insidetable2" align="center"><B>Title:</B> <input type="text" Name="Title" value="'.$userDealer['Title'][0].'">
			</td>
		</tr>
		<tr>
			<td align="center" class="insidetable2">
			<table>
				<tr bgcolor="#DADDE4">
					<td colspan="6" align="center" class="normaltext"><B>Address</B></td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>Dealer Name:</B></td>
					<td colspan="5" align="left" class="normaltext"><input type="text" class="textfield" Name="Name" value="'.$dealerData['Name'][0].'"></td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>Primary Address:</B></td>
					<td colspan="5" align="left" class="normaltext"><input type="text" class="textfield" Name="Address1" value="'.$dealerData['Address1'][0].'" style="width:478px;"></td>
				</tr>
				<tr>	
					<td align="right" class="normaltext"><B>Secondary address</B></td>
					<td colspan="5" align="left" class="normaltext"><input type="text" class="textfield" Name="Address2" value="'.$dealerData['Address2'][0].'" style="width:478px;"></td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>City:</B></td>
					<td align="left" class="normaltext"><input type="text" class="textfield" Name="City" value="'.$dealerData['City'][0].'"></td>
					<td align="right" class="normaltext"><B>State:</B></td>
					<td align="left" class="normaltext"><input type="text" class="textfield" Name="State" value="'.$dealerData['State'][0].'"></td>
					<td align="right" class="normaltext"><B>Postal/ZIP:</B></td>
					<td align="left" class="normaltext"><input type="text" class="textfield" Name="PostalCode" value="'.$dealerData['PostalCode'][0].'"></td>
				</tr>
				<tr>	
					<td align="right" class="normaltext"><B>Country:</B></td>
					<td colspan="5" align="left" class="normaltext">'.htmlPulldown($countriesArray,'Country',$dealerData['EK_Country'][0]).'</td>
				</tr>
	
				<tr bgcolor="#DADDE4">
					<td colspan="6" align="center" class="normaltext"><B>Contact</B></td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>Phone:</B></td>
					<td align="left" class="normaltext" colspan="2"><input type="text" class="textfield" Name="Phone" value="'.$dealerData['Phone'][0].'"></td>
					<td align="right" class="normaltext"><B>TollFree Phone:</B></td>
					<td align="left" class="normaltext" colspan="2"><input type="text" class="textfield" Name="Phone_TollFree" value="'.$dealerData['Phone_TollFree'][0].'"></td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>Support Phone:</B></td>
					<td align="left" class="normaltext" colspan="2"><input type="text" class="textfield" Name="Phone_Support" value="'.$dealerData['Phone_Support'][0].'"></td>
					<td align="right" class="normaltext"><B>Fax:</B></td>
					<td align="left" class="normaltext" colspan="2"><input type="text" class="textfield" Name="Fax" value="'.$dealerData['Fax'][0].'"></td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>Email:</B></td>
					<td align="left" class="normaltext" colspan="2"><input type="text" class="textfield" Name="Email" value="'.$dealerData['Email'][0].'"></td>
					<td align="right" class="normaltext">&nbsp;</td>
					<td align="left" class="normaltext" colspan="2">&nbsp;</td>
				</tr>	
				<tr bgcolor="#DADDE4">
					<td colspan="6" align="center" class="normaltext"><B>Details</B></td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>Sells Cores:</B></td>
					<td align="left" class="normaltext" colspan="2"><input type="checkbox" Name="SellsCores" value="1" '.(($dealerData['SellsCores'][0]==1)?'checked':'').'></td>
					<td align="right" class="normaltext"><B>Sells Accessories:</B></td>
					<td align="left" class="normaltext" colspan="2"><input type="checkbox" Name="SellsAccessories" value="1" '.(($dealerData['SellsAccessories'][0]==1)?'checked':'').'></td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>Sells MDs:</B></td>
					<td align="left" class="normaltext" colspan="2"><input type="checkbox" Name="SellsMDs" value="1" '.(($dealerData['SellsMDs'][0]==1)?'checked':'').'></td>
					<td align="right" class="normaltext"><B>Sells Automation:</B></td>
					<td align="left" class="normaltext" colspan="2"><input type="checkbox" Name="SellsAutomation" value="1" '.(($dealerData['SellsAutomation'][0]==1)?'checked':'').'></td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>Sells Orbiters:</B></td>
					<td align="left" class="normaltext" colspan="2"><input type="checkbox" Name="SellsOrbiters" value="1" '.(($dealerData['SellsOrbiters'][0]==1)?'checked':'').'></td>
					<td align="right" class="normaltext">&nbsp;</td>
					<td align="left" class="normaltext" colspan="2">&nbsp;</td>
				</tr>	
				<tr>
					<td align="center" class="normaltext" colspan="6">
						<table>
							<tr>
								<td><B>Pro Dealer:</B></td>
								<td><input type="checkbox" Name="IsProDealer" value="1" '.(($dealerData['IsProDealer'][0]==1)?'checked':'').'></td>
								<td><B>Custom Installation:</B></td>
								<td><input type="checkbox" Name="CustomInstallation" value="1" '.(($dealerData['CustomInstallation'][0]==1)?'checked':'').'></td>
								<td><B>List In Directory:</B></td>
								<td><input type="checkbox" Name="ListInDirectory" value="1" '.(($dealerData['ListInDirectory'][0]==1)?'checked':'').'></td>
							<tr>
						</table>
					</td>
				</tr>	
				<tr>
					<td colspan="6" align="center" class="normaltext"><hr></td>
				</tr>
				<tr>
					<td colspan="6" align="center" class="normaltext"><input type="submit" Name="submitDealer" value="Submit"></td>
				</tr>							
				
			</table>
			</td>
		</tr>
	</table>
	</form>
';
}else{
	// process
	$Title=stripslashes($_POST['Title']);
	$Name=stripslashes($_POST['Name']);
	$Address1=stripslashes($_POST['Address1']);
	$Address2=stripslashes($_POST['Address2']);
	$City=stripslashes($_POST['City']);
	$State=stripslashes($_POST['State']);
	$PostalCode=stripslashes($_POST['PostalCode']);
	$EK_Country=((int)$_POST['Country']>0)?(int)$_POST['Country']:NULL;
	$Phone=stripslashes($_POST['Phone']);
	$Phone_TollFree=stripslashes($_POST['Phone_TollFree']);
	$Phone_Support=stripslashes($_POST['Phone_Support']);
	$Fax=stripslashes($_POST['Fax']);
	$Email=stripslashes($_POST['Email']);
	$SellsCores=(int)@$_POST['SellsCores'];
	$SellsMDs=(int)@$_POST['SellsMDs'];
	$SellsOrbiters=(int)@$_POST['SellsOrbiters'];
	$SellsAccessories=(int)@$_POST['SellsAccessories'];
	$SellsAutomation=(int)@$_POST['SellsAutomation'];
	$IsProDealer=(int)@$_POST['IsProDealer'];
	$CustomInstallation=(int)@$_POST['CustomInstallation'];
	$ListInDirectory=(int)@$_POST['ListInDirectory'];
	
	$emailBody='
		Hi,<br>
	User '.$_SESSION['username'].' has edited the dealer account #'.$dealerID.'.<br>
	Title '.(($Title!=$userDealer['Title'][0])?'<font color="red">'.$Title.'</font> ['.$userDealer['Title'][0].']':$userDealer['Title'][0]).'
			<table>
				<tr bgcolor="#DADDE4">
					<td colspan="6" align="center" class="normaltext"><B>Address</B></td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>Dealer Name:</B></td>
					<td colspan="5" align="left" class="normaltext">'.(($Name!=$dealerData['Name'][0])?'<font color="red">'.$Name.'</font> ['.$dealerData['Name'][0].']':$dealerData['Name'][0]).'</td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>Primary Address:</B></td>
					<td colspan="5" align="left" class="normaltext">'.(($Address1!=$dealerData['Address1'][0])?'<font color="red">'.$Address1.'</font> ['.$dealerData['Address1'][0].']':$dealerData['Address1'][0]).'</td>
				</tr>
				<tr>	
					<td align="right" class="normaltext"><B>Secondary address</B></td>
					<td colspan="5" align="left" class="normaltext">'.(($Address2!=$dealerData['Address2'][0])?'<font color="red">'.$Address2.'</font> ['.$dealerData['Address2'][0].']':$dealerData['Address2'][0]).'</td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>City:</B></td>
					<td align="left" class="normaltext">'.(($City!=$dealerData['City'][0])?'<font color="red">'.$City.'</font> ['.$dealerData['City'][0].']':$dealerData['City'][0]).'</td>
					<td align="right" class="normaltext"><B>State:</B></td>
					<td align="left" class="normaltext">'.(($State!=$dealerData['State'][0])?'<font color="red">'.$State.'</font> ['.$dealerData['State'][0].']':$dealerData['State'][0]).'</td>
					<td align="right" class="normaltext"><B>Postal/ZIP:</B></td>
					<td align="left" class="normaltext">'.(($PostalCode!=$dealerData['PostalCode'][0])?'<font color="red">'.$PostalCode.'</font> ['.$dealerData['PostalCode'][0].']':$dealerData['PostalCode'][0]).'</td>
				</tr>
				<tr>	
					<td align="right" class="normaltext"><B>Country:</B></td>
					<td colspan="5" align="left" class="normaltext">'.(($EK_Country!=$dealerData['EK_Country'][0])?'<font color="red">'.$countriesArray[$EK_Country].'</font> ['.$dealerData['EK_Country'][0].']':$dealerData['EK_Country'][0]).'</td>
				</tr>
	
				<tr bgcolor="#DADDE4">
					<td colspan="6" align="center" class="normaltext"><B>Contact</B></td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>Phone:</B></td>
					<td align="left" class="normaltext" colspan="2">'.(($Phone!=$dealerData['Phone'][0])?'<font color="red">'.$Phone.'</font> ['.$dealerData['Phone'][0].']':$dealerData['Phone'][0]).'</td>
					<td align="right" class="normaltext"><B>TollFree Phone:</B></td>
					<td align="left" class="normaltext" colspan="2">'.(($Phone_TollFree!=$dealerData['Phone_TollFree'][0])?'<font color="red">'.$Phone_TollFree.'</font> ['.$dealerData['Phone_TollFree'][0].']':$dealerData['Phone_TollFree'][0]).'</td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>Support Phone:</B></td>
					<td align="left" class="normaltext" colspan="2">'.(($Phone_Support!=$dealerData['Phone_Support'][0])?'<font color="red">'.$Phone_Support.'</font> ['.$dealerData['Phone_Support'][0].']':$dealerData['Phone_Support'][0]).'</td>
					<td align="right" class="normaltext"><B>Fax:</B></td>
					<td align="left" class="normaltext" colspan="2">'.(($Fax!=$dealerData['Fax'][0])?'<font color="red">'.$Fax.'</font> ['.$dealerData['Fax'][0].']':$dealerData['Fax'][0]).'</td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>Email:</B></td>
					<td align="left" class="normaltext" colspan="2">'.(($Email!=$dealerData['Email'][0])?'<font color="red">'.$Email.'</font> ['.$dealerData['Email'][0].']':$dealerData['Email'][0]).'</td>
					<td align="right" class="normaltext">&nbsp;</td>
					<td align="left" class="normaltext" colspan="2">&nbsp;</td>
				</tr>	
				<tr bgcolor="#DADDE4">
					<td colspan="6" align="center" class="normaltext"><B>Details</B></td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>Sells Cores:</B></td>
					<td align="left" class="normaltext" colspan="2">'.(($SellsCores!=$dealerData['SellsCores'][0])?'<font color="red">'.$confArray[$SellsCores].'</font> ['.$confArray[$dealerData['SellsCores'][0]].']':$confArray[$dealerData['SellsCores'][0]]).'</td>
					<td align="right" class="normaltext"><B>Sells Accessories:</B></td>
					<td align="left" class="normaltext" colspan="2">'.(($SellsAccessories!=$dealerData['SellsAccessories'][0])?'<font color="red">'.$confArray[$SellsAccessories].'</font> ['.$confArray[$dealerData['SellsAccessories'][0]].']':$confArray[$dealerData['SellsAccessories'][0]]).'</td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>Sells MDs:</B></td>
					<td align="left" class="normaltext" colspan="2">'.(($SellsMDs!=$dealerData['SellsMDs'][0])?'<font color="red">'.$confArray[$SellsMDs].'</font> ['.$confArray[$dealerData['SellsMDs'][0]].']':$confArray[$dealerData['SellsMDs'][0]]).'</td>
					<td align="right" class="normaltext"><B>Sells Automation:</B></td>
					<td align="left" class="normaltext" colspan="2">'.(($SellsAutomation!=$dealerData['SellsAutomation'][0])?'<font color="red">'.$confArray[$SellsAutomation].'</font> ['.$confArray[$dealerData['SellsAutomation'][0]].']':$confArray[$dealerData['SellsAutomation'][0]]).'</td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>Sells Orbiters:</B></td>
					<td align="left" class="normaltext" colspan="2">'.(($SellsOrbiters!=$dealerData['SellsOrbiters'][0])?'<font color="red">'.$confArray[$SellsOrbiters].'</font> ['.$confArray[$dealerData['SellsOrbiters'][0]].']':$confArray[$dealerData['SellsOrbiters'][0]]).'</td>
					<td align="right" class="normaltext">&nbsp;</td>
					<td align="left" class="normaltext" colspan="2">&nbsp;</td>
				</tr>	
				<tr>
					<td align="center" class="normaltext" colspan="6">
						<table>
							<tr>
								<td><B>Pro Dealer:</B></td>
								<td>'.(($IsProDealer!=$dealerData['IsProDealer'][0])?'<font color="red">'.$confArray[$IsProDealer].'</font> ['.$confArray[$dealerData['IsProDealer'][0]].']':$confArray[$dealerData['IsProDealer'][0]]).'</td>
								<td><B>Custom Installation:</B></td>
								<td>'.(($CustomInstallation!=$dealerData['CustomInstallation'][0])?'<font color="red">'.$confArray[$CustomInstallation].'</font> ['.$confArray[$dealerData['CustomInstallation'][0]].']':$confArray[$dealerData['CustomInstallation'][0]]).'</td>
								<td><B>List In Directory:</B></td>
								<td>'.(($ListInDirectory!=$dealerData['ListInDirectory'][0])?'<font color="red">'.$confArray[$ListInDirectory].'</font> ['.$confArray[$dealerData['ListInDirectory'][0]].']':$confArray[$dealerData['ListInDirectory'][0]]).'</td>
							<tr>
						</table>
					</td>
				</tr>	
				<tr>
					<td colspan="6" align="center" class="normaltext"><hr></td>
				</tr>
			</table>
* In square brackets are old values. No brackets mean that the user didn\'t made any changes to corresponding field.
	';

	$dealerADO->Execute('
		UPDATE Dealer 
		SET 
			Name=?,
			Address1=?, 
			Address2=?, 
			City=?, 
			State=?, 
			PostalCode=?, 
			EK_Country=?, 
			Phone=?, 
			Phone_TollFree=?, 
			Phone_Support=?, 
			Fax=?, 
			Email=?, 
			IsProDealer=?, 
			SellsCores=?, 
			SellsMDs=?, 
			SellsOrbiters=?, 
			SellsAccessories=?, 
			SellsAutomation=?, 
			CustomInstallation=?, 
			ListInDirectory=?
		WHERE PK_Dealer=?',
	array($Name,$Address1,$Address2,$City,$State,$PostalCode,$EK_Country,$Phone,$Phone_TollFree,$Phone_Support,$Fax,$Email,$IsProDealer,$SellsCores,$SellsMDs,$SellsOrbiters,$SellsAccessories,$SellsAutomation,$CustomInstallation,$ListInDirectory,$dealerID));
	if($Title!=$userDealer['Title'][0]){
		$dealerADO->Execute('UPDATE Dealer_Users SET Title=? WHERE FK_Dealer=? AND EK_Users=?',array($Title,$dealerID,$FK_Users));
	}
	
	$headers = "From: Pluto Home<website@plutohome.com>\n";
	$headers .= "X-Sender: <website@plutohome.com>\n";
	$headers .= "X-Mailer: PHP\n"; //mailer
	$headers .= "X-Priority: 3\n"; //1 UrgentMessage, 3 Normal
	$headers .= "Return-Path: <website@plutohome.com>\n";
	$headers .= "Content-Type: text/html; charset=iso-8859-1\n";
	$headers .= "cc: \n";
	$headers .= "bcc: ";

	$subject = 'Dealer profile updated';
	@mail($GLOBALS['sendNotificationsTo'],$subject,$emailBody,$headers);
	
		header('Location: index.php?section=edit_dealer_profile&msg=The dealer profile was updated.');
	}


	$output->setPressedButton(5);
	$output->setNavigationMenu(array('My Pluto'=>'index.php?section=myPluto','Dealer area'=>'index.php?section=dealer_area','Edit dealer'=>'index.php?section=edit_dealer_profile'));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME." :: Edit dealer profile");
	$output->output();
}
?>