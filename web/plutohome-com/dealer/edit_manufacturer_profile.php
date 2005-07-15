<?
function edit_manufacturer_profile($output,$conn,$dbADO) {
	/* @var $dbADO ADOConnection */

	$action = isset($_REQUEST['action'])?cleanString($_REQUEST['action']):'form';	
	$out='';
	
	$FK_Users=(isset($_SESSION['userID']))?(int)$_SESSION['userID']:0;
	global $dbManufacturerServer, $dbManufacturerUser, $dbManufacturerPass, $dbManufacturerDatabase;
	$manufacturerADO=dbConnection($dbManufacturerServer,$dbManufacturerUser,$dbManufacturerPass,$dbManufacturerDatabase);
	$manufacturerUser=getFieldsAsArray('Manufacturer_Users','CanEditProfile,EK_Manufacturer,EK_Users,Validated',$manufacturerADO,'WHERE EK_Users='.$FK_Users);	
	$countriesArray=getAssocArray('Country','PK_Country','Description',$dbADO,'ORDER BY Description ASC');

	$confArray=array(0=>'N',1=>'Y');
	
	if(isset($manufacturerUser['Validated'][0]) && $manufacturerUser['Validated'][0]!=1){
		header('Location: index.php?section=manufacturer_signin');
		exit();
	}else{
		$manufacturerID=$manufacturerUser['EK_Manufacturer'][0];
		$manufData=getFieldsAsArray('Manufacturer','Name, Address1, Address2, City, State, PostalCode, EK_Country, Phone, Phone_TollFree, Phone_Support, Fax, Email, SellsCores, SellsMDs, SellsOrbiters, SellsAccessories, SellsAutomation',$manufacturerADO,'WHERE EK_Manufacturer='.$manufacturerID);
	}
		
	
if($action=='form'){
	$errMsg=(isset($_REQUEST['err']))?'<span class="err">'.stripslashes($_REQUEST['err']).'</span>':'';
	$msg=(isset($_REQUEST['msg']))?'<span class="confirm">'.stripslashes($_REQUEST['msg']).'</span>':'';
	
	$out .= '
	<style>
	hr	{ height: 0px; border: solid #D1D7DC 0px; border-top-width: 1px;}
	</style>
	<form action="index.php" method="POST" Name="edit_manufacturer_profile">
  		<input type="hidden" Name="section" value="edit_manufacturer_profile">
		<input type="hidden" Name="action" value="add">
	<table width="100%"  border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td align="left" class="insidetable"><table width="563"  border="0" cellpadding="0" cellspacing="0">
      <tr>
        <td align="left"><B>Edit Manufacturer</B></td>
      </tr>
    </table></td>
  </tr>
	<tr>
		<td class="normaltext" align="left">&nbsp;<a href="index.php?section=manufacturer_area">&lt;&lt; Back</a></td>
	</tr>	
		<tr>
			<td align="center" class="insidetable2">
	'.$errMsg.' '.$msg.'
			<table>
				<tr bgcolor="#DADDE4">
					<td colspan="6" align="center" class="normaltext"><B>Address</B></td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>Manufacturer Name:</B></td>
					<td colspan="5" align="left" class="normaltext"><input type="text" class="textfield" Name="Name" value="'.$manufData['Name'][0].'"></td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>Primary Address:</B></td>
					<td colspan="5" align="left" class="normaltext"><input type="text" class="textfield" Name="Address1" value="'.$manufData['Address1'][0].'" style="width:478px;"></td>
				</tr>
				<tr>	
					<td align="right" class="normaltext"><B>Secondary address</B></td>
					<td colspan="5" align="left" class="normaltext"><input type="text" class="textfield" Name="Address2" value="'.$manufData['Address2'][0].'" style="width:478px;"></td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>City:</B></td>
					<td align="left" class="normaltext"><input type="text" class="textfield" Name="City" value="'.$manufData['City'][0].'"></td>
					<td align="right" class="normaltext"><B>State:</B></td>
					<td align="left" class="normaltext"><input type="text" class="textfield" Name="State" value="'.$manufData['State'][0].'"></td>
					<td align="right" class="normaltext"><B>Postal/ZIP:</B></td>
					<td align="left" class="normaltext"><input type="text" class="textfield" Name="PostalCode" value="'.$manufData['PostalCode'][0].'"></td>
				</tr>
				<tr>	
					<td align="right" class="normaltext"><B>Country:</B></td>
					<td colspan="5" align="left" class="normaltext">'.htmlPulldown($countriesArray,'Country',$manufData['EK_Country'][0]).'</td>
				</tr>
	
				<tr bgcolor="#DADDE4">
					<td colspan="6" align="center" class="normaltext"><B>Contact</B></td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>Phone:</B></td>
					<td align="left" class="normaltext" colspan="2"><input type="text" class="textfield" Name="Phone" value="'.$manufData['Phone'][0].'"></td>
					<td align="right" class="normaltext"><B>TollFree Phone:</B></td>
					<td align="left" class="normaltext" colspan="2"><input type="text" class="textfield" Name="Phone_TollFree" value="'.$manufData['Phone_TollFree'][0].'"></td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>Support Phone:</B></td>
					<td align="left" class="normaltext" colspan="2"><input type="text" class="textfield" Name="Phone_Support" value="'.$manufData['Phone_Support'][0].'"></td>
					<td align="right" class="normaltext"><B>Fax:</B></td>
					<td align="left" class="normaltext" colspan="2"><input type="text" class="textfield" Name="Fax" value="'.$manufData['Fax'][0].'"></td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>Email:</B></td>
					<td align="left" class="normaltext" colspan="2"><input type="text" class="textfield" Name="Email" value="'.$manufData['Email'][0].'"></td>
					<td align="right" class="normaltext">&nbsp;</td>
					<td align="left" class="normaltext" colspan="2">&nbsp;</td>
				</tr>	
				<tr bgcolor="#DADDE4">
					<td colspan="6" align="center" class="normaltext"><B>Details</B></td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>Sells Cores:</B></td>
					<td align="left" class="normaltext" colspan="2"><input type="checkbox" Name="SellsCores" value="1" '.(($manufData['SellsCores'][0]==1)?'checked':'').'></td>
					<td align="right" class="normaltext"><B>Sells Accessories:</B></td>
					<td align="left" class="normaltext" colspan="2"><input type="checkbox" Name="SellsAccessories" value="1" '.(($manufData['SellsAccessories'][0]==1)?'checked':'').'></td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>Sells MDs:</B></td>
					<td align="left" class="normaltext" colspan="2"><input type="checkbox" Name="SellsMDs" value="1" '.(($manufData['SellsMDs'][0]==1)?'checked':'').'></td>
					<td align="right" class="normaltext"><B>Sells Automation:</B></td>
					<td align="left" class="normaltext" colspan="2"><input type="checkbox" Name="SellsAutomation" value="1" '.(($manufData['SellsAutomation'][0]==1)?'checked':'').'></td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>Sells Orbiters:</B></td>
					<td align="left" class="normaltext" colspan="2"><input type="checkbox" Name="SellsOrbiters" value="1" '.(($manufData['SellsOrbiters'][0]==1)?'checked':'').'></td>
					<td align="right" class="normaltext">&nbsp;</td>
					<td align="left" class="normaltext" colspan="2">&nbsp;</td>
				</tr>	
				<tr>
					<td colspan="6" align="center" class="normaltext"><hr></td>
				</tr>
				<tr>
					<td colspan="6" align="center" class="normaltext"><input type="submit" Name="submitManufacturer" value="Submit"></td>
				</tr>							
				
			</table>
			</td>
		</tr>
	</table>
	</form>
';
}else{
	// process
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
	
	$emailBody='
		Hi,<br>
	User '.$_SESSION['username'].' has edited the manufacturer account #'.$manufacturerID.'.<br>
			<table>
				<tr bgcolor="#DADDE4">
					<td colspan="6" align="center" class="normaltext"><B>Address</B></td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>Manufacturer Name:</B></td>
					<td colspan="5" align="left" class="normaltext">'.(($Name!=$manufData['Name'][0])?'<font color="red">'.$Name.'</font> ['.$manufData['Name'][0].']':$manufData['Name'][0]).'</td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>Primary Address:</B></td>
					<td colspan="5" align="left" class="normaltext">'.(($Address1!=$manufData['Address1'][0])?'<font color="red">'.$Address1.'</font> ['.$manufData['Address1'][0].']':$manufData['Address1'][0]).'</td>
				</tr>
				<tr>	
					<td align="right" class="normaltext"><B>Secondary address</B></td>
					<td colspan="5" align="left" class="normaltext">'.(($Address2!=$manufData['Address2'][0])?'<font color="red">'.$Address2.'</font> ['.$manufData['Address2'][0].']':$manufData['Address2'][0]).'</td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>City:</B></td>
					<td align="left" class="normaltext">'.(($City!=$manufData['City'][0])?'<font color="red">'.$City.'</font> ['.$manufData['City'][0].']':$manufData['City'][0]).'</td>
					<td align="right" class="normaltext"><B>State:</B></td>
					<td align="left" class="normaltext">'.(($State!=$manufData['State'][0])?'<font color="red">'.$State.'</font> ['.$manufData['State'][0].']':$manufData['State'][0]).'</td>
					<td align="right" class="normaltext"><B>Postal/ZIP:</B></td>
					<td align="left" class="normaltext">'.(($PostalCode!=$manufData['PostalCode'][0])?'<font color="red">'.$PostalCode.'</font> ['.$manufData['PostalCode'][0].']':$manufData['PostalCode'][0]).'</td>
				</tr>
				<tr>	
					<td align="right" class="normaltext"><B>Country:</B></td>
					<td colspan="5" align="left" class="normaltext">'.(($EK_Country!=$manufData['EK_Country'][0])?'<font color="red">'.$countriesArray[$EK_Country].'</font> ['.@$countriesArray[$manufData['EK_Country'][0]].']':$manufData['EK_Country'][0]).'</td>
				</tr>
	
				<tr bgcolor="#DADDE4">
					<td colspan="6" align="center" class="normaltext"><B>Contact</B></td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>Phone:</B></td>
					<td align="left" class="normaltext" colspan="2">'.(($Phone!=$manufData['Phone'][0])?'<font color="red">'.$Phone.'</font> ['.$manufData['Phone'][0].']':$manufData['Phone'][0]).'</td>
					<td align="right" class="normaltext"><B>TollFree Phone:</B></td>
					<td align="left" class="normaltext" colspan="2">'.(($Phone_TollFree!=$manufData['Phone_TollFree'][0])?'<font color="red">'.$Phone_TollFree.'</font> ['.$manufData['Phone_TollFree'][0].']':$manufData['Phone_TollFree'][0]).'</td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>Support Phone:</B></td>
					<td align="left" class="normaltext" colspan="2">'.(($Phone_Support!=$manufData['Phone_Support'][0])?'<font color="red">'.$Phone_Support.'</font> ['.$manufData['Phone_Support'][0].']':$manufData['Phone_Support'][0]).'</td>
					<td align="right" class="normaltext"><B>Fax:</B></td>
					<td align="left" class="normaltext" colspan="2">'.(($Fax!=$manufData['Fax'][0])?'<font color="red">'.$Fax.'</font> ['.$manufData['Fax'][0].']':$manufData['Fax'][0]).'</td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>Email:</B></td>
					<td align="left" class="normaltext" colspan="2">'.(($Email!=$manufData['Email'][0])?'<font color="red">'.$Email.'</font> ['.$manufData['Email'][0].']':$manufData['Email'][0]).'</td>
					<td align="right" class="normaltext">&nbsp;</td>
					<td align="left" class="normaltext" colspan="2">&nbsp;</td>
				</tr>	
				<tr bgcolor="#DADDE4">
					<td colspan="6" align="center" class="normaltext"><B>Details</B></td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>Sells Cores:</B></td>
					<td align="left" class="normaltext" colspan="2">'.(($SellsCores!=$manufData['SellsCores'][0])?'<font color="red">'.$confArray[$SellsCores].'</font> ['.$confArray[$manufData['SellsCores'][0]].']':$confArray[$manufData['SellsCores'][0]]).'</td>
					<td align="right" class="normaltext"><B>Sells Accessories:</B></td>
					<td align="left" class="normaltext" colspan="2">'.(($SellsAccessories!=$manufData['SellsAccessories'][0])?'<font color="red">'.$confArray[$SellsAccessories].'</font> ['.$confArray[$manufData['SellsAccessories'][0]].']':$confArray[$manufData['SellsAccessories'][0]]).'</td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>Sells MDs:</B></td>
					<td align="left" class="normaltext" colspan="2">'.(($SellsMDs!=$manufData['SellsMDs'][0])?'<font color="red">'.$confArray[$SellsMDs].'</font> ['.$confArray[$manufData['SellsMDs'][0]].']':$confArray[$manufData['SellsMDs'][0]]).'</td>
					<td align="right" class="normaltext"><B>Sells Automation:</B></td>
					<td align="left" class="normaltext" colspan="2">'.(($SellsAutomation!=$manufData['SellsAutomation'][0])?'<font color="red">'.$confArray[$SellsAutomation].'</font> ['.$confArray[$manufData['SellsAutomation'][0]].']':$confArray[$manufData['SellsAutomation'][0]]).'</td>
				</tr>
				<tr>
					<td align="right" class="normaltext"><B>Sells Orbiters:</B></td>
					<td align="left" class="normaltext" colspan="2">'.(($SellsOrbiters!=$manufData['SellsOrbiters'][0])?'<font color="red">'.$confArray[$SellsOrbiters].'</font> ['.$confArray[$manufData['SellsOrbiters'][0]].']':$confArray[$manufData['SellsOrbiters'][0]]).'</td>
					<td align="right" class="normaltext">&nbsp;</td>
					<td align="left" class="normaltext" colspan="2">&nbsp;</td>
				</tr>	
				<tr>
					<td colspan="6" align="center" class="normaltext"><hr></td>
				</tr>
			</table>
* In square brackets are old values. No brackets mean that the user didn\'t made any changes to corresponding field.
	';

	$manufacturerADO->Execute('
		UPDATE Manufacturer 
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
			SellsCores=?, 
			SellsMDs=?, 
			SellsOrbiters=?, 
			SellsAccessories=?, 
			SellsAutomation=?
		WHERE EK_Manufacturer=?',
	array($Name,$Address1,$Address2,$City,$State,$PostalCode,$EK_Country,$Phone,$Phone_TollFree,$Phone_Support,$Fax,$Email,$SellsCores,$SellsMDs,$SellsOrbiters,$SellsAccessories,$SellsAutomation,$manufacturerID));
	
	$headers = "From: Pluto Home<website@plutohome.com>\n";
	$headers .= "X-Sender: <website@plutohome.com>\n";
	$headers .= "X-Mailer: PHP\n"; //mailer
	$headers .= "X-Priority: 3\n"; //1 UrgentMessage, 3 Normal
	$headers .= "Return-Path: <website@plutohome.com>\n";
	$headers .= "Content-Type: text/html; charset=iso-8859-1\n";
	$headers .= "cc: \n";
	$headers .= "bcc: ";

	$subject = 'Manufacturer profile updated';
	@mail($GLOBALS['sendNotificationsTo'],$subject,$emailBody,$headers);
	
		header('Location: index.php?section=edit_manufacturer_profile&msg=The manufacturer profile was updated.');
	}


	$output->setPressedButton(5);
	$output->setNavigationMenu(array('My Pluto'=>'index.php?section=myPluto','Manufacturer area'=>'index.php?section=manufacturer_area','Edit manufacturer'=>'index.php?section=edit_manufacturer_profile'));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME." :: Edit manufacturer profile");
	$output->output();
}
?>