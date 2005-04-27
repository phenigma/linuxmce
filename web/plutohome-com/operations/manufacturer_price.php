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
    <td align="left" class="insidetable2">Most of Pluto\'s software is available for free.  However, a license is required to offer the following products with Pluto\'s platform.</td>
  </tr>
</table>
<table width="100%"  border="0" cellspacing="0" cellpadding="0" class="insidetable">
 <tr style="font-weight: bold;"> <td>Annual Qty</td><td>STB</td><td>Hybrid</td><td>Media Dir</td><td>Core</td><td>Orbiter</td> </tr>
 <tr> <td>Dealer</td><td>95</td><td>250</td><td>150</td><td>395</td><td>95</td></tr> 
 <tr> <td>100-500</td><td>71</td><td>188</td><td>113</td><td>296</td><td>71</td></tr> 
 <tr> <td>500-1500</td><td>61</td><td>159</td><td>96</td><td>252</td><td>61</td></tr> 
 <tr> <td>1500-5000</td><td>51</td><td>135</td><td>81</td><td>214</td><td>51</td></tr> 
 <tr> <td>5000-10000</td><td>44</td><td>115</td><td>69</td><td>182</td><td>44</td></tr> 
 <tr> <td>10000+</td><td>37</td><td>98</td><td>59</td><td>155</td><td>37</td></tr> 
 <tr> <td colspan="6"><br>All prices shown in US$.</td></tr>
 <tr> <td colspan="6"><br>The Dealer price is shown for comparisson.  Dealers are not required to purchase equipment with Pluto pre-installed.  They can buy off-the-shelf computer hardware and install the Pluto platform themselves.  However, they are required to pay the license fee shown in the Dealer row.  As a manufacturer, you get lower licensing costs, and can therefore offer Pluto dealers equipment that costs less and saves them the hassle of installing the software themselves.</td> </tr>
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
	$output->setNavigationMenu(array('Get Pluto'=>'index.php?section=get_pluto','I\'m a manufacturer'=>'index.php?section=manufacturer'));
	$output->setTitle(APPLICATION_NAME." :: I'm a Manufacturer");		
	$output->setBody($out);
	$output->output();
 }
?>
