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
      <p>As a dealer you have 2 options for offering Pluto.  The easiest is to buy equipment from a Pluto manufacturing partner.  
	  The Pluto platform will be installed already--just take it out of the box and plug it in.  Plus the manuals and accessories are 
	  included and you do not need to worry about compatibility issues.  However, as an open platform, Pluto also gives you the option
	  of buying or building your own equipment and installing the Pluto platform yourself.  A license is required to sell such a system.
	  The license fee is as follows (in US$):</p>
<p>	  <b>STB:</b> $95 &nbsp;&nbsp <b>Hybrid:</b> $250 &nbsp;&nbsp <b>Media Dir:</b> $150 &nbsp;&nbsp <b>Core:</b> $395 &nbsp;&nbsp <b>Orbiter:</b> $95</p>
	</td>
  </tr>
</table>

<table width="100%"  border="0" cellspacing="0" cellpadding="0" class="insidetable">
 <tr> <td><br><b>Definitions:</b></td><td colspan="5"></td></tr> 
 <tr> <td>STB</td><td colspan="5">A set-top box is a stand-alone device with a retail price under $600.  The software functionality is essentially the same as with a hybrid, but scaled down for a low-power processor.</td></tr> 
 <tr> <td>-----</td><td colspan="5"></td></tr> 
 <tr> <td>Hybrid</td><td colspan="5">A hybrid is a media PC designed to be sold as a stand-alone solution.  This includes all the Core software, plus media director software</td></tr> 
 <tr> <td>-----</td><td colspan="5"></td></tr> 
 <tr> <td>Media Dir</td><td colspan="5">The traditional Pluto media director sold as part of a whole-house solution.  The media director software is stored on the Core, and the media director PC does a network boot when used as a Pluto system.  A hard drive can host another o/s.</td></tr> 
 <tr> <td>-----</td><td colspan="5"></td></tr> 
 <tr> <td>Core</td><td colspan="5">The traditional Pluto Core sold as part of a whole-house solution.  Typically this is a server with large internal storage.</td></tr> 
 <tr> <td>-----</td><td colspan="5"></td></tr> 
 <tr> <td>Orbiter</td><td colspan="5">The remote control software for PDA\'s, webpads, and similar devices</td></tr> 

</table>
';

	$output->setPressedButton(5);
	$output->setNavigationMenu(array('Get Pluto'=>'index.php?section=get_pluto','I\'m a dealer'=>'index.php?section=dealer'));
	$output->setTitle(APPLICATION_NAME." :: I'm a Dealer");			
	$output->setBody($out);
	$output->output();
 }
?>
