<?php
 function get_pluto($output) {
		$out = '';
		
$out.='

<table width="100%" border="0" cellpadding="0" cellspacing="0" class="maintable">
  <tr>
    <td class="titletable"><img src="images/submenus/how_to_buy_txt.gif" width="82" height="13"></td>
  </tr>
  <tr>
    <td class="insidetable"><table border="0" align="left" cellpadding="0" cellspacing="0">
      <tr>
        <td width="154"><a href="index.php?section=get_pluto"><img src="images/submenus/How%20to%20buy/pluto_for_my_home_on.gif" width="200" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
      </tr>
      <tr>
        <td><a href="index.php?section=dealer"><img src="images/submenus/How%20to%20buy/a_dealer_off.gif" width="200" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
      </tr>
      <tr>
        <td><a href="index.php?section=manufacturer"><img src="images/submenus/How%20to%20buy/a_manufacturer_off.gif" width="200" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
      </tr>
      <tr>
        <td><a href="index.php?section=developer"><img src="images/submenus/How%20to%20buy/open_source_developer_off.gif" width="200" height="17" border="0"></a></td>
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
    <p><img src="images/submenus/pluto_for_my_home_txt.gif" width="165" height="13"><br>
      <p>At this moment Pluto 2 has not yet been released for retail distribution.  You are welcome to download a preview version of the Pluto software and build your own Pluto system using your existing personal computers. <a href="support/index.php?section=document&docID=1">Get the software</a>
</p>
      <p>When the retail version is released, you will be able to get Pluto three ways:</p><br>
      <p><strong>Dealers</strong> &ndash; Pluto dealers offer the Pluto system by itself, or as part of a custom-installed whole-house smart home solution.  <a href="index.php?section=locator">Dealer locator</a></p>
      <p><strong>Powered By Pluto</strong> &ndash; Pluto version 2 is available as an embedded solution for incorporating into other consumer electronics products. For example, Powered by Pluto TV&rsquo;s have all the functionality of a Pluto Media Director built-in. Imagine buying a TV, plugging in only 1 cable, and your mobile phone turns into the remote, the TV recognizes you, knowing all your favorite shows, can play your entire collection of music and movies, and works seamlessly with all the devices in the house&mdash;regardless of brand. Or an in-car entertainment systems that automatically synchronizes with your Pluto Core so your entire media collection is always available, and the phone book, navigation system, playlists, and other preferences are automatically synchronized. You even receive instant notifications on your mobile phone if the car&rsquo;s alarm is set off&mdash;just like your Pluto Home system. Sign up for our newsletter to be notified as these revolutionary products are released.</p>
      <p><strong>Pluto software-only</strong> &ndash; Download the same software powering our retail products for FREE, as open source, compatible with Linux and Windows.  &lt;click <a href="index.php?section=developer">here</a> for details&gt;</p>
      <p>Version 2 is expected to be released for retail in March, 2005. </p>
      <p><br>
      </p>
 <table height="285" class="insidetable">
  <form action="index.php?section=contact" method="POST">
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
  				
  				$out.='<option value="'.$row->PKID_LeadType.'">'.$row->Description.'</option>';
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
	

		
		$output->setImgName("support.jpg"); //img that needs to be changed
        $output->setPageID(5); //butonul selectat
 		$output->setNavigationMenu(array("Sample page"=>"index.php?section=samplePage")); //not used yet
 		$output->setTitle(APPLICATION_NAME."::Get Pluto for My Home");			//page title
 		
 		
 		
 		//don't touch them 		
		$output->setScriptCalendar('null');
		$output->setScriptTRColor('null');		
		$output->setBody($out);		
  		$output->output(); 
 }
?>
