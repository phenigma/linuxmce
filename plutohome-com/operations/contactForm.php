<?
function contactForm($section)
{
$out='
<table height="285" class="insidetable">
  <form action="index.php?section='.$section.'" method="POST">
  <input type="hidden" name="section" value="contact">
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
  				
  				$out.='<option value="'.$row->PKID_LeadType.'-'.$row->Description.'">'.$row->Description.'</option>';
          }
  			$out.='</select>
		</td>
	</tr>
	<tr>
		<td height="22">Country:</td>
		<td height="22"><input type="text" name="Country" size="25"></td>
		<td colspan="2"><input type="submit" name="send" value="send message via secure site"></td>
	</tr>
	<tr>
		<td height="19">Your message:</td>
	</tr>
	<tr>
		<td colspan="4" height="82">
  			<textarea rows="11" name="Comments" cols="60"></textarea>
		</td>
	</tr>
</table>
</form></td></table>'; 
	if(isset($_POST['send'])){
    
		$leadTypeDescr=explode('-',$_POST['FKID_LeadType']);
	  	$leadType=@$leadTypeDescr[0];
	  	$leadDescription=@$leadTypeDescr[1];
	
	  	$address=$_POST['Address1'].' '.$_POST['Address2'];
	  	$sql="
	  		INSERT INTO Lead
	  			(Name,CustomerID,Address1,Address2,City,State,Postal_Code,Country,Phone,Fax,Email,website,FKID_LeadType,Comments)
	  	 	VALUES('".$_POST['Name']."','".@$_POST['CustomerID']."','".$_POST['Address1']."','".$_POST['Address2']."','".$_POST['City']."','".$_POST['State']."','".$_POST['Postal_Code']."','".$_POST['Country']."','".$_POST['Phone']."','".$_POST['Fax']."','".$_POST['Email']."','".$_POST['Website']."','".$leadType."','".$_POST['Comments']."')";
		$r=mysql_query($sql) or die("Can not insert into database". mysql_error());
	  	
		$message='
	  		Name: <B>'.$_POST['Name'].'</B><br>
	  		ID: <B>'.@$_POST['CustomerID'].'</B><br>
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
	  		<hr><br><br>This email was sent from PlutoHome Website.';
	   	
	   	$headers = "From: Pluto Home<website@plutohome.com>\n";
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
	  	$out.='
	  		<table width="100%" border="0" cellpadding="0" cellspacing="0" class="maintable">
	  			<tr>
	  				<td width="20" height="200"></td>
	  				<td><br><p> 
	  			Your message was sent through our secure server.<br>
	  		<a href="index.php">Click here</a> to go back to home page. </p>
	  				</td>
	  			</tr>
	  		</table>'; 
  	}
  	return $out;
}
?>