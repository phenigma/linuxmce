<?php
 function ref_comm($output) {
		$out = '';
		
$out.=<<<TEXT

<table width="100%" border="0" cellpadding="0" cellspacing="0" class="maintable">

  <tr>
    <td class="insidetable">
<p><b><u>Pluto Home Commission structure:</b></u></p>

<p><b>Pluto Home has 2 lines:</b></p>

<p><b>Pluto Pro</b> is sold exclusively by high-end dealers who also sell and install custom hi-fi gear, home cinemas, and home automation components, like lighting, climate and security.  Pluto maintains a very close relationship with the Pluto Pro dealers.  They must be trained and certified, maintain a stellar reputation for customer service, and prove they are capable of satisfying the high-end clients.  These dealers must also agree to provide a referral commission of <b>5% of the sales price</b> of the Pluto system, which is subsidized by Pluto corporate.  Since a typical sale is usually in the $10,000-30,000 range, the sales commission can be quite a tidy sum.</p>

<p>All other Pluto systems are licensed to third party hard goods manufacturers.  Pluto has no say in the product pricing, distribution or dealer network, and only receives a modest licensing fee.  Therefore, Pluto has no control over whether the manufacturer offers referral fees.  So we offer a generous portion of our licensing fee, paid directly from Pluto corporate to the referring user via U.S. Check, Paypal, E-Gold or Bank Wire.  The commission is US $20 for a Core/Server product (such as a Media Server), and $10 for a media director product, such as a smart television</p>

<p><b>Referrals for Licensing Deals:</b></p>

<p>If you refer a hard goods manufacturer that licenses the system you have the chance to make some <b>serious commissions</b> because Pluto offers a commission that is <b>minimum 3% of the entire licensing deal</b>.  Considering that such deals often involve very large quantities, this can be a really huge deal, depending on the type of licensing.</p>
<p>For miscellaneous DCE devices, like lighting system, garage doors, intercoms, sprinklers, etc., Pluto releases the DCE protocol as open source so the manufacturer does not need to pay a license.  However, manufacturers may still want to license the logo, trademark or other optional services, and the 3%+ commission applies to those fees.</p>
<p>However, for manufacturers who want to offer a complete smart home solution or a complete entertainment system, a license is required.  Although DCE is free under the GPL license, much of the core software that is totally unique to Pluto, such as our mobile orbiters with follow me and the Core media server are released under the Pluto License.  This is free, open source only when it is distributed in a pure software form.  If a manufacturer wants to embed Pluto in their hard goods to offer a turn-key appliance, then a separate license is required.  Examples are media servers, smart home solutions, in-vehicle entertainment systems, and so on.  Pluto’s licensing fees are quite modest, however the quantities can be very large and the 3%+ commission can amount to a really fantastic payback for just making a referral.</p>
</td>
  </tr>
</table>
	
TEXT;
		
		$output->setImgName("support.jpg"); //img that needs to be changed
        $output->setPageID(9); //butonul selectat
 		$output->setNavigationMenu(array("Sample page"=>"index.php?section=samplePage")); //not used yet
 		$output->setTitle(APPLICATION_NAME."::I'm a Open Source Developer");			//page title
 		
 		
 		
 		//don't touch them 		
		$output->setScriptCalendar('null');
		$output->setScriptTRColor('null');		
		$output->setBody($out);		
  		$output->output(); 
 }
?>
