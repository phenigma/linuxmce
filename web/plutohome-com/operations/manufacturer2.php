<?php
 function manufacturer($output,$conn) {
$out = '';
		
$out.='
<table width="100%"  border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td align="center" class="insidetable"><table width="563"  border="0" cellpadding="0" cellspacing="0">
      <tr>
        <td align="left"><img src="images/titles/manufacturer.gif"></td>
      </tr>
    </table></td>
  </tr>
  <tr>
    <td align="left" class="insidetable2">      Pluto&rsquo;s manufacturing clients fall in 2 categories: </p>
    <p>Powered By Pluto: For manufacturers that embed Pluto technology, such as the core, media director or orbiter, within their products, like televisions, in-vehicle entertainment systems, PC computers, mobile phones, web pads, etc.,</p>
    <p>Pluto-compatible accessories: For manufacturers of a variety of miscellaneous devices compatible with Pluto&rsquo;s DCE protocol. This includes everything from light switches, to pool controls and garage doors.</p>
    <p><strong>Powered by Pluto</strong></p>
    <p>For these manufacturers, Pluto offers custom software-only versions as well as a turnkey embedded solution. All the features described on this web site, including &quot;follow-me&quot;, run on a platform not much larger than a deck of cards. It&rsquo;s a perfect drop-in solution for consumer electronics and automotive: ISO 9001 certified, passively cooled (silent, no fan), 200,000 hour MTBF, with an operating range of -40 F to +185 F (-40 C to +85 C) and a renowned track-record of reliability in even the harshest environments, such as military applications.</p>
    <p>Products with &ldquo;Powered by Pluto&rdquo; inside offer your customers unprecedented convenience and benefits. For example, consider television Powered By Pluto. Thanks to Pluto`s self-configuring and follow-me technologies, a consumer can take the TV out of the box, plug in just 1 network cable, and without doing a thing, his mobile phone will beep, thanking him for the purchase, and he`ll have full-control. The TV will even know his viewing preferences, play all the media centrally stored on his Pluto Core, and work seamlessly with everything else in the home. Pluto is even developing a new version for in-vehicle entertainment systems that allows the car to synchronize media, contacts and other data with the home system, extending &ldquo;follow me&rdquo; to the car.</p>
	<p>Pricing: See the <a href="index.php?section=mf_psa">License Fees</a> for manufacturers</p>
    <p><strong>Pluto-compatible accessories</strong></p>
    <p>As technology makes our lives increasingly complex, customers yearn for simplicity, where everything just works as expected without any hassle and without having to read a user&rsquo;s manual. If your device supports Pluto&rsquo;s DCE protocol, your customers can take your product out of the box, plug it in, and know that it will work as expected, controllable from their mobile phone or orbiter, without any configuration. All you need to do is add an Ethernet port, and implement DCE, which is very light, cross-platform compatible, and runs on a variety of very inexpensive microprocessors. Best of all, starting with version 2, the DCE platform will be available as open source under a GPL license. This means it is completely free to use, and Pluto does not charge any royalties for the technology. Plus, as an open source standard, you are not tied to anything proprietary, and are sure that this is a plug-and-play protocol that will last and gain mainstream acceptance. Pluto&rsquo;s in-house engineers can help you add DCE compatibility with minimal cost.</p>
    <p>If you are interested in working with Pluto, please complete the following inquiry form and a sales rep will contact you promptly:</p>

	</td>
  </tr>
</table>
';
	include('operations/contactForm.php');
	$out.=contactForm('manufacturer',$conn);	
		
	$output->setPressedButton(5);
	$output->setNavigationMenu(array('Get Pluto'=>'index.php?section=get_pluto','I\'m a manufacturer'=>'index.php?section=manufacturer'));
	$output->setTitle(APPLICATION_NAME." :: I'm a Manufacturer");		
	$output->setBody($out);
	$output->output();
 }
?>
