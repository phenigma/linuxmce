<?
function technical_support($output)
{
 
$out = '';
 		$out.='<table width="100%" border="0" cellpadding="0" cellspacing="0" class="maintable">
       <td width="10%" valign="top" align="left" >
   <table border="0" align="left" cellpadding="0" cellspacing="0">
     <tr>
    <td class="titletable"><img src="images/meniuri/plutovip_support_txt.gif" width="131" height="13"></td>
    </tr>
       </table></td>
      <tr><td width="20%" valign="top" align="center" class="insidetable">
      <table border="0" align="left" cellpadding="0" cellspacing="0">
      <tr>
        <td width="154"><a href="index.php?section=download"><img src="images/submeniuri/download_off.jpg" width="138" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=compatibility"><img src="images/submeniuri/compatibility_off.jpg" width="138" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=technical_support"><img src="images/submeniuri/technical_support_on.jpg" width="138" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=faq_presentation"><img src="images/submeniuri/faqs_off.jpg" width="138" height="17" border="0"></a></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
        </tr>
      
    </table>
       </tr>
      </td>
      <td width="100%" align="left" class="insidetable">
      <p><img src="images/submeniuri/tech_support_txt.gif" width="139" height="13"><br>
<p>Technical support is available 24 hours a day, 7 days a week.  If you are having a problem with your Pluto system, please <a href="http://setup.plutohome.com/customer_setup/login.php">log in</a> and click on the support link to open a trouble ticket.  A technician will troubleshoot your system and respond to you within 12 hours.  If your system requires a replacement part, it will be sent to you via overnight courier the next business day.
<br>
<br>
If you are not yet a customer but have a technical question, or if you are not able to login, please complete the following,
or call (877) pluto-4-u.
<br><br>
For sales or general inquiries, <a href="index.php?section=contact">click here</a>.

<br><br>
<table height="285" class="insidetable">
  <form action="index.php?section=technical_support" name="tech" method="POST">
  <input type="hidden" name="section" value="technical_support">
	<tr>
		<td height="22">Name:</td>
		<td height="22"><input type="text" name="Name" size="25"></td>
      <td height="22">Phone:</td>
		<td height="22"><input type="text" name="Phone" size="25"></td>
	</tr>
	<tr>
		<td height="22">Address:</td>
		<td height="22"><input type="text" name="Address1" size="25"></td>
		<td height="22">Email:</td>
		<td height="22"><input type="text" name="Email" size="25"></td>
    </tr>
	<tr>
		<td height="22"></td>
		<td height="22"><input type="text" name="Address2" size="25"></td>
		<td height="22">Fax:</td>
		<td height="22"><input type="text" name="Fax" size="25"></td>
	</tr>
	<tr>
		<td height="22">City:</td>
		<td height="22"><input type="text" name="City" size="25"></td>
      <td height="22">Website:</td>
		<td height="22"><input type="text" name="Website" size="25"></td>
	</tr>
	<tr>
		<td>State:</td>
		<td><input type="text" name="State" size="10"> Zip: <input type="text" name="Postal_Code" size="10"></td>
		<td>Interest:</td>
		<td>
  			<select size="1" name="FKID_LEAD_LeadType">
  <option value="5" selected>(select one)</option>';
        include("include/config/contactdatabase.inc.php");
        $sql1="select PKID_LEAD_LeadType,Description from LEAD_LeadType";
        $r1=mysql_query($sql1);
        while($row=mysql_fetch_object($r1))
          {
  				
  				$out.='<option value="'.$row->PKID_LeadType.'-'.$row->Description.'">'.$row->Description.'</option>';
          }
  			$out.='</select>
		</td>
	</tr>
	<tr>
		<td height="22">Country:</td>
		<td height="22"><input type="text" name="Country" size="25"></td>
		<td colspan="2"><input type="submit" name="send" value="send message via secure site">
</td>
	</tr>
	<tr>
		<td height="19">Your message:</td>
	</tr>
	<tr>
		<td colspan="4" height="82">
  			<textarea rows="8" name="Comments" cols="60"></textarea>
		</td>
	</tr>
</table>
</form></td></table>'; 
if(isset($_POST['send'])){ 
  	$leadTypeDescr=explode('-',$_POST['FKID_LEAD_LeadType']);
  	$leadType=@$leadTypeDescr[0];
  	$leadDescription=@$leadTypeDescr[1];
    
  	$address=$_POST['Address1'].' '.$_POST['Address2'];
  	$sql="insert into LEAD_Lead(Name,CustomerID,Address1,Address2,City,State,Postal_Code,Country,Phone,Fax,Email,website,FKID_LEAD_LeadType,Comments) values('".$_POST['Name']."','".$_POST['CustomerID']."','".$_POST['Address1']."','".$_POST['Address2']."','".$_POST['City'].
    "','".$_POST['State']."','".$_POST['Postal_Code']."','".$_POST['Country']."','".$_POST['Phone']."','".$_POST['Fax']."','".$_POST['Email']."','".$_POST['Website']."','".$leadType."','".$_POST['Comments']."')";
  	$r=mysql_query($sql) or die("Can not insert into database". mysql_error());
  	$message='
  		Name: <B>'.$_POST['Name'].'</B><br>
  		ID: <B>'.$_POST['CustomerID'].'</B><br>
  		Address: '.$_POST['Address1'].'<br>
  		'.$_POST['Address2'].'<br>
  		City: '.$_POST['City'].'<br>
  		State: '.$_POST['State'].'<br>
  		Postal code: '.$_POST['Postal_Code'].'<br>
  		Country: '.$_POST['Country'].'<br>
  		Phone: '.$_POST['Phone'].'<br>
  		Fax: '.$_POST['Fax'].'<br>
  		Email: <B>'.$_POST['Email'].'</B><br>
  		Website: '.$_POST['Website'].'<br>
  		Interest: '.$leadDescription.'<br>
  		Comments: <hr>'.nl2br($_POST['Comments']).'
  		<hr><br><br>This email was sent from PlutoVIP Website.';
   	
   	$headers = "From: Pluto VIP<website@plutohome.com>\n";
	$headers .= "X-Sender: <website@plutohome.com>\n";
	$headers .= "X-Mailer: PHP\n"; //mailer
	$headers .= "X-Priority: 3\n"; //1 UrgentMessage, 3 Normal
	$headers .= "Return-Path: <website@plutohome.com>\n";
	$headers .= "Content-Type: text/html; charset=iso-8859-1\n";
	$headers .= "cc: \n"; 
	$headers .= "bcc: "; 
	
	$subject = 'Website Form';
	$to = 'info@plutohome.com';
	@mail($to,$subject,$message,$headers);
  $out='';
$out.='<table width="100%" border="0" cellpadding="0" cellspacing="0" class="maintable"><td width="20"></td><td>
<br><p> 
  Your message was sent through our secure server.<br>
  <a href="index.php">Click here</a> to go back to home page. </p>
  </td></table>'; 
  }
  	$output->setNavigationMenu(array("Sample page"=>"index.php?section=samplePage"));
  	$output->setScriptCalendar('null');
	$output->setScriptTRColor('null');		
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME."::Technical support");	
    $output->setPageID(4); 
  	$output->output();
}
?>
