<?
function technical_support($output)
{
 
$out='';
$out.='<table width="100%" border="0" cellpadding="0" cellspacing="0" class="maintable"><td width="30%" valign="top" align="center" >
     <table border="0" align="left" cellpadding="0" cellspacing="0">
     <tr>
    <td class="titletable"><img src="images/submenus/support_txt.gif" width="60" height="13"></td>
    </tr>
       </table></td>
      <tr><td width="30%" valign="top" align="center" class="insidetable">
      <table border="0" align="left" cellpadding="0" cellspacing="0">
      <tr>
        <td width="154"><a href="index.php?section=installation"><img src="images/submenus/Support/installation_information_of.gif" width="207" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=project"><img src="images/submenus/Support/project_management_off.gif" width="207" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=warranty"><img src="images/submenus/Support/warranty_off.gif" width="207" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=download"><img src="images/submenus/Support/download_off.gif" width="207" height="17" border="0"></a></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=compatibility"><img src="images/submenus/Support/compatibility_off.gif" width="207" height="17" border="0"></a></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=technical_support"><img src="images/submenus/Support/technical_support_on.gif" width="207" height="17" border="0"></a></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=faq_presentation"><img src="images/submenus/Support/faqs_off.gif" width="207" height="17" border="0"></a></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td>&nbsp;</td>
        <td>&nbsp;</td>
      </tr>
    </table>
       </tr>
      </td>
      <td width="70%" class="insidetable">
<p>Pluto has many support options.  You can click "Live Chat" at the top of the page for instant help.  
Our support web site also has documentation, forums and bug reports both for Pluto in general, and for each of Pluto\'s modules.  
You may want to try these useful links to pages in our support site:<br>
      <br><a href="/support/index.php?section=document&docID=1&package=0">Main Support Page</a>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href="/support/index.php?section=document&docID=11&package=0">Installation Section</a>
      <br><a href="/support/index.php?section=document&docID=14&package=0">User\'s manual</a>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href="/support/index.php?section=document&docID=15&package=0">Introduction for programmers and developers</a>
</p>      
<br>
You can also use the form below to contact tech support, or call (877) pluto-4-u.
<br>
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
  			<select size="1" name="FKID_LeadType">
  <option value="5" selected>(select one)</option>';
        include("include/config/contactdatabase.inc.php");
        $sql1="select PKID_LeadType,Description from LeadType";
        $r1=mysql_query($sql1);
        while($row=mysql_fetch_object($r1))
          {
  				
  				$out.='<option value="'.$row->PKID_LeadType.'">'.$row->Description.'</option>';
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
if(isset($_POST['send']))
  {   
  $address=$_POST['Address1'].' '.$_POST['Address2'];
  $sql="insert into Lead(Name,CustomerID,Address1,Address2,City,State,Postal_Code,Country,Phone,Fax,Email,website,FKID_LeadType,Comments) values('".$_POST['Name']."','".$_POST['CustomerID']."','".$_POST['Address1']."','".$_POST['Address2']."','".$_POST['City'].
    "','".$_POST['State']."','".$_POST['Postal_Code']."','".$_POST['Country']."','".$_POST['Phone']."','".$_POST['Fax']."','".$_POST['Email']."','".$_POST['Website']."','".$_POST['FKID_LeadType']."','".$_POST['Comments']."')";
  $r=mysql_query($sql) or die("Can not insert into database". mysql_error());
   $message=$_POST['Comments'];
   $headers = 'From: website@plutohome.com';
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
