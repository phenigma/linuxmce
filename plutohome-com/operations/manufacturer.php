<?php
 function manufacturer($output) {
		$out = '';
		
$out.='

<table width="100%" border="0" cellpadding="0" cellspacing="0" class="maintable"><td width="10%" valign="top" align="left" >
  <tr>
    <td class="titletable"><img src="images/submenus/how_to_buy_txt.gif" width="82" height="13"></td>
  </tr>
  <tr>
    <td class="insidetable" valign="top"><table border="0" align="left" cellpadding="0" cellspacing="0">
      <tr>
        <td width="154"><a href="index.php?section=get_pluto"><img src="images/submenus/How%20to%20buy/pluto_for_my_home_off.gif" width="200" height="17" border="0"></a></td>
        
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
      </tr>
      <tr>
        <td><a href="index.php?section=dealer"><img src="images/submenus/How%20to%20buy/a_dealer_off.gif" width="200" height="17" border="0"></a></td>
       
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
      </tr>
      <tr>
        <td><a href="index.php?section=manufacturer"><img src="images/submenus/How%20to%20buy/a_manufacturer_on.gif" width="200" height="17" border="0"></a></td>
        
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
      </tr>
      <tr>
        <td><a href="index.php?section=developer"><img src="images/submenus/How%20to%20buy/open_source_developer_off.gif" width="200" height="17" border="0"></a></td>
        
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
      </tr>
      <tr>
        
      </tr>
    </table>
  </tr>
  </td>
   <td width="100%" class="insidetable" align="left">
    <p><img src="images/submenus/im_a_manufacturer_txt.gif" width="145" height="13"><br>
      Pluto&rsquo;s manufacturing clients fall in 2 categories: </p>
    <p>Powered By Pluto: For manufacturers that embed Pluto technology, such as the core, media director or orbiter, within their products, like televisions, in-vehicle entertainment systems, pc computers, mobile phones, web pads, etc.,</p>
    <p>Pluto-compatible accessories: For manufacturers of a variety of miscellaneous devices compatible with Pluto&rsquo;s DCE protocol. This includes everything from light switches, to pool controls and garage doors.</p>
    <p><strong>Powered by Pluto</strong></p>
    <p>For these manufacturers, Pluto offers custom software-only versions as well as a turnkey embedded solution. All the features described on this web site, including &quot;follow-me&quot;, run on a platform not much larger than a deck of cards. It&rsquo;s a perfect drop-in solution for consumer electronics and automotive: ISO 9001 certified, passively cooled (silent, no fan), 200,000 hour MTBF, with an operating range of -40 F to +185 F (-40 C to +85 C) and a renowned track-record of reliability in even the harshest environments, such as military applications.</p>
    <p>Products with &ldquo;Powered by Pluto&rdquo; inside offer your customers unprecedented convenience and benefits. For example, consider television Powered By Pluto. Thanks to Pluto`s self-configuring and follow-me technologies, a consumer can take the tv out of the box, plug in just 1 network cable, and without doing a thing, his mobile phone will beep, thanking him for the purchase, and he`ll have full-control. The TV will even know his viewing preferences, play all the media centrally stored on his Pluto Core, and work seamlessly with everything else in the home. Pluto is even developing a new version for in-vehicle entertainment systems that allows the car to synchronize media, contacts and other data with the home system, extending &ldquo;follow me&rdquo; to the car.</p>
    <p><strong>Pluto-compatible accessories</strong></p>
    <p>As technology makes our lives increasingly complex, customers yearn for simplicity, where everything just works as expected without any hassle and without having to read a user&rsquo;s manual. If your device supports Pluto&rsquo;s DCE protocol, your customers can take your product out of the box, plug it in, and know that it will work as expected, controllable from their mobile phone or orbiter, without any configuration. All you need to do is add an Ethernet port, and implement DCE, which is very light, cross-platform compatible, and runs on a variety of very inexpensive microprocessors. Best of all, starting with version 2, the DCE platform will be available as open source under a GPL license. This means it is completely free to use, and Pluto does not charge any royalties for the technology. Plus, as an open source standard, you are not tied to anything proprietary, and are sure that this is a plug-and-play protocol that will last and gain mainstream acceptance. Pluto&rsquo;s in-house engineers can help you add DCE compatibility with minimal cost.</p>
    <p>If you are interested in working with Pluto, please complete the following inquiry form and a sales rep will contact you promptly:</p>
   
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
 		$output->setTitle(APPLICATION_NAME."::I'm a Manufacturer");			//page title
 		
 		
 		
 		//don't touch them 		
		$output->setScriptCalendar('null');
		$output->setScriptTRColor('null');		
		$output->setBody($out);		
  		$output->output(); 
 }
?>