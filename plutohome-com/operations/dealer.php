<?php
 function dealer($output) {
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
        <td><a href="index.php?section=dealer"><img src="images/submenus/How%20to%20buy/a_dealer_on.gif" width="200" height="17" border="0"></a></td>
        
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
      </tr>
      <tr>
        <td><a href="index.php?section=manufacturer"><img src="images/submenus/How%20to%20buy/a_manufacturer_off.gif" width="200" height="17" border="0"></a></td>
        
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
    <p><img src="images/submenus/im_a_dealer_txt.gif" width="86" height="13"><br>
      Pluto is by far the most advanced smart home solution, and the only product that allows you to offer your customers a complete, turnkey solution.  But Pluto also has a very unique distribution method that will particularly benefit dealers.</p>
    <p>Nearly all smart home/entertainment products in the market are using pc computers inside special cases, just like Pluto series 1.  For Series 2 Pluto has separated from the pack and offers a program
    which allows dealers to buy servers and media pc from local suppliers and turn them into Pluto systems using a kick-start CD.  The advantages are clear: You have a choice of hardware suppliers and can choose the
    one that offers the best price and fastest turnaround.  You will not need to send defective parts back to Pluto.  Customs and shipping costs are drastically reduced.</p>
    <p>Pluto is looking for 2 types of dealers: Pluto Pro dealers are high-end dealers who offer lighting, climate, security and hi-fi systems and can deliver a complete, custom-installed, turn-key solution.  Only Pluto Pro dealers will be authorized to sell the Pluto Pro line.</p>
    <p>Pluto\'s standard dealers do have the same requirements as Pluto Pro dealers.  These dealers can contract with a local hardware supplier, or use one of Pluto\'s outsourcers, to get 
    Pluto-compatible hardware: Cores, Media Directors, and Orbiters.
      <p>Both lines will share the same features and benefits and run the same software, but the Pro line will focus on high-quality, specialized hardware components made from the finest materials, available in a variety of finishes and colors to complement your clients&rsquo; d&eacute;cor, sold through an exclusive channel, while the standard Pluto line runs on inexpensive PC computers.</p>
      <p>This strategy has proven very successful in many other sectors. For example, in the automotive industry most of the world&rsquo;s flagship, exotic manufacturers are now owned by mass-market car companies, and share parts across the entire product line. This is a new approach for the smart home industry, however we are confident it will benefit this segment in the same way.</p>
      <p>The high-end Pro dealers continue to offer an exclusive product, but one that is even more widely supported thanks to the mainstream product.</p>
      <p>Pluto\'s open source strategy is a really unique benefit to dealers also.  Customers are often afraid to invest in this kind of technology for fear that it may be 
      unreliable, or the company may not be around for the long run.  By making Pluto open source, your customers are assured there are a huge number of
      users ensuring the platform is rock-solid and dependable--something open source is famous for.  The standards and protocols are open, so your customers do 
      worry about being tied to anything proprietary.  And the open source community covers every corner of the globe, actively developing new add-ons and enhancements and more
      language options.
      </p>
      <p>If you are interested in becoming a Pluto dealer, please fill out the form below and a rep will contact you shortly.</p>
      
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
 		$output->setTitle(APPLICATION_NAME."::I'm a Dealer");			//page title
 		
 		
 		
 		//don't touch them 		
		$output->setScriptCalendar('null');
		$output->setScriptTRColor('null');		
		$output->setBody($out);		
  		$output->output(); 
 }
?>