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
      The Pluto system featured on this site, version 1, is sold exclusively through high-end, full-service dealers.  With the upcoming release of our version 2, the current line of equipment will be renamed &ldquo;Pluto Pro&rdquo;, still sold exclusively through this existing dealer network. </p>
      <p> </p>
      <p>Version 2 will be available four different ways:</p>
      <p><strong>Pluto Pro</strong> &ndash; will be sold exclusively through our existing dealers. The Pluto Pro line is made from the finest materials, available in a variety of finishes and colors to complement any d&eacute;cor. Pluto Pro dealers also sell compatible lighting control systems, security, climate and pool controls. Most also sell high-end audio/video equipment, and work with contractors to offer built-in solutions, such as in-wall Orbiters. Your Pluto Pro dealer offers a complete, custom installed, turn-key solution.</p>
      <p><strong>Pluto</strong> &ndash; The standard Pluto system runs on computer PC&rsquo;s, and will be available through a variety of traditional retailers. It has the same features as Pluto Pro and works the same way, but in a lower cost package.</p>
      <p><strong>Powered By Pluto</strong> &ndash; Pluto version 2 is available as an embedded solution for incorporating into other consumer electronics products. For example, Powered by Pluto TV&rsquo;s have all the functionality of a Pluto Media Director built-in. Imagine buying a TV, plugging in only 1 cable, and your mobile phone turns into the remote, the TV recognizes you, knowing all your favorite shows, can play your entire collection of music and movies, and works seamlessly with all the devices in the house&mdash;regardless of brand. Or an in-car entertainment systems that automatically synchronizes with your Pluto Core so your entire media collection is always available, and the phone book, navigation system, playlists, and other preferences are automatically synchronized. You even receive instant notifications on your mobile phone if the car&rsquo;s alarm is set off&mdash;just like your Pluto Home system. Sign up for our newsletter to be notified as these revolutionary products are released.</p>
      <p><strong>Pluto software-only</strong> &ndash; With version 2.0, you will be able to receive the Pluto software for Linux or Windows free for use on your existing computer PC. And, all the source code will be available for open source developers. Imagine being able to enjoy all the benefits Pluto offers for free! &lt;click <a href="open_source_letter.pdf" target="_blank">here</a> for details&gt;</p>
      <p>Version 2 is expected to be released in February, 2005, with a beta version released in late October, 2004. As always, Pluto is self-updating and when version 2 is available, all existing customers will be automatically upgraded at no charge.</p>
      <p>If you would like to purchase a Pluto series 1 system through one of our Pro dealers, please complete the form below and we will put you in contact with a local dealer.</p>
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
  			<select size="1" name="FKID_LEAD_LeadType">
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
    "','".$_POST['State']."','".$_POST['Postal_Code']."','".$_POST['Country']."','".$_POST['Phone']."','".$_POST['Fax']."','".$_POST['Email']."','".$_POST['Website']."','".$_POST['FKID_LEAD_LeadType']."','".$_POST['Comments']."')";
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
 		$output->setTitle(APPLICATION_NAME."::Get Pluto");			//page title
 		
 		
 		
 		//don't touch them 		
		$output->setScriptCalendar('null');
		$output->setScriptTRColor('null');		
		$output->setBody($out);		
  		$output->output(); 
 }
?>
