<?php
 function dealer($output,$conn) {
		$out = '';
		
$out.='
<table width="100%"  border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td align="center" class="insidetable"><table width="563"  border="0" cellpadding="0" cellspacing="0">
      <tr>
        <td align="left"><img src="images/titles/dealer.gif"></td>
      </tr>
    </table></td>
  </tr>
  <tr>
    <td align="left" class="insidetable2">
	<div style="position:relative; top=0px; float:left; width=250px;">
	<table width="230" border="0" cellpadding="0" cellspacing="0">
	    <tr>
	        <td class="normaltext"><img src="images/bullet_1.gif" width="11" height="11" align="middle" /> <a href="index.php?section=dealerProgram"><B>Dealer Program</B></a></td>
	    </tr>
	    <tr>
	        <td class="normaltext"><img src="images/bullet_1.gif" width="11" height="11" align="middle" /> <a href="index.php?section=dealerApplication"><B>Dealer Application</B></a></td>
	    </tr>
	    <tr>
	        <td class="normaltext" height="30">&nbsp;</td>
	    </tr>
	</table>
	</div>
      <p class="title">Why should you carry Pluto?</p>

<p><strong>1.	Profit!</strong> – Enjoy bigger margins, while offering a lower price.  Consider:  Virtually all the smart home solutions, movie & music servers, and home automation components you sell are PC computers inside custom cases.  But the manufactures refuse to let you use any hardware but what they provide at several times over fair market value.  Try asking one of the manufacturers of, say DVD video servers, to let you use an off-the-shelf server instead of the one they provide.  Even though it could work and would cost much less, the manufacturers strictly forbid it and actively prevent it.  With Pluto, you have a choice.  You can offer a "Pluto Pro" product, which you buy from us with everything pre-installed, or you can purchase any of the servers, media PCs, or tablets that we have certified to be 100% compatible from any vendor you choose, and only pay us a very modest license fee.  We can even provide the custom case separately so you can offer your customers a product that looks custom, just like Pluto Pro.  This also allows you to...</p>
<p><strong>2.	Provide better service</strong> – As explained above, with our competitors\' products, if you need repairs or new parts, you are at their mercy as the sole supplier.  This can lead to unacceptable delays with shipping, customs, and other hassles.  With Pluto, you have the option of buying the hardware from a local supplier that offers a good price, fast service, and local inventory.</p>
<p><strong>3.	Aggressive marketing</strong> – Rather than carefully guarding our software as our competitors do, we offer the same software that powers the high-end Pluto Pro product for FREE to the open source community.  They can build their own Pluto systems.  Then we offer referral fees when they refer others who buy the retail product from a dealer.  Admittedly this is very unconventional, but consider…  As a dealer you know your customers.  They are well-off individuals who want the convenience of a dealer that will install and setup everything for them—that\'s why they come to you.  They are not open source techies who are going to download the software and build their own systems—there are already open source projects for that.  So, giving the software away for free does not take away from your customer base.  It just means that a huge number of techies will have Pluto.  And all of them have less technically-inclined friends and relatives who are in your customer base and want to buy the same turnkey product from a dealer.</p>
<p><strong>4.	Offer your customers the only all-in-one integrated solution</strong> – Without Pluto you\'d have to buy a hodge-podge of "stuff" from a variety of different vendors: a movie server, a whole-house audio solution, home automation controls and tablets, a phone system, media PCs, etc.  Only Pluto brings it all in one system.</p>
<p><strong>5.	The most reliable, stable solution</strong> - High-end smart home and hi-fi is still a relatively small, niche market.  Yet what makes a product rock-solid and reliable is exhaustive testing, both in-house, and from myriads upon myriads of end-users.  Open source has proven itself to be the best way to achieve this kind of dependability.  Pluto is the only commercial vendor that has been willing to sacrifice any revenue from software sales, releasing the software as royalty-free open source, so that massive numbers of users worldwide are able to enhance, modify, pummel and stress-test the very product that you will be offering your customers.</p>
<p><strong>6.	A product that will be around for the long haul</strong> – As a dealer you know that many customers shy away from cutting-edge smart home solutions because none of the manufacturers in this area have lengthy track records.  How could they, the product as we know it today didn\'t exist that long ago?  But if the manufacturer disappears, suddenly their high-priced smart home solution becomes worthless with no support for the software, and no replacement parts for the equipment.  You can reassure your customers that is not an issue with Pluto.  We designed our product to run on a variety of hardware from several manufacturers, and the software is open source, freely available to the public.</p>
<p><strong>7.	Support for more devices</strong> – Once your customers start to enjoy the lifestyle, they want everything in the house to work with the same level of ease.  Because Pluto is an open standard, and the source code is publicly available, anyone, anywhere can enhance it.  A translator in Thailand can make a new skin for the Orbiters run in their native language.  Or a dealer in Cape Town can hire a local programmer to write an interface for a type of pool control popular in the South African market.</p>

<p>Two levels of dealers</p>

<p>1.	<b>Pluto Pro</b> is for high-end dealers offer lighting, climate, security and hi-fi systems and can deliver a complete, custom-installed, turn-key solution. Only Pluto Pro dealers are authorized to sell the Pluto Pro line, which focuses on high-quality, specialized hardware made from the finest materials, available in a variety of finishes and colors to complement your clients\' decor.</p>
<p>2.	Pluto\'s standard dealers do not have the same requirements as Pluto Pro dealers. These dealers can contract with a local hardware supplier, or use one of Pluto\'s outsourcers, to get Pluto-compatible hardware: Cores, Media Directors, Orbiters and accessories.  They are not required to offer custom installation.</p>

      <p>If you are interested in becoming a Pluto dealer, please fill out the form below and a rep will contact you shortly.</p>

	</td>
  </tr>
</table>
   
';
	include('operations/contactForm.php');
	$out.=contactForm('dealer',$conn);

	$output->setPressedButton(5);
	$output->setNavigationMenu(array('Get Pluto'=>'index.php?section=get_pluto','I\'m a dealer'=>'index.php?section=dealer'));
	$output->setTitle(APPLICATION_NAME." :: I'm a Dealer");			
	$output->setBody($out);
	$output->output();
 }
?>