<?php
 function accesories($output) {
		$out = '';
		
$out.=<<<TEXT

<table width="100%" border="0" cellpadding="0" cellspacing="0" class="maintable">
  <tr>
    <td class="titletable"><img src="images/submenus/pluto_system_txt.gif" width="96" height="13"></td>
  </tr>
  <tr>
    <td class="insidetable"><table border="0" align="left" cellpadding="0" cellspacing="0">
      <tr>
        <td><a href="index.php?section=equipment_overview"><img src="images/submenus/Pluto%20system/equipment_overview_off.gif" width="154" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
      </tr>
      <tr>
        <td><a href="index.php?section=core"><img src="images/submenus/Pluto%20system/core_off.gif" width="154" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
      </tr>
      <tr>
        <td><a href="index.php?section=media_director"><img src="images/submenus/Pluto%20system/media_director_off.gif" width="154" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
      </tr>
      <tr>
        <td><a href="index.php?section=orbiter"><img src="images/submenus/Pluto%20system/orbiter_off.gif" width="154" height="17" border="0"></a></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
      </tr>
      <tr>
        <td><a href="index.php?section=mobile_orbiter"><img src="images/submenus/Pluto%20system/mobile_orbiter_off.gif" width="154" height="17" border="0"></a></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
      </tr>
      <tr>
        <td><a href="index.php?section=accesories"><img src="images/submenus/Pluto%20system/accessories_on.gif" width="154" height="17" border="0"></a></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
      </tr>
      <tr>
        <td><a href="index.php?section=software"><img src="images/submenus/Pluto%20system/software_off.gif" width="154" height="17" border="0"></a></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
      </tr>
      <tr>
        <td><a href="index.php?section=special_offer"><img src="images/submenus/Pluto%20system/special_offer_off.gif" width="154" height="17" border="0"></a></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td>&nbsp;</td>
        <td>&nbsp;</td>
      </tr>
    </table>
    <p><img src="images/submenus/accessories_txt.gif" width="89" height="13"><br>
        <br>
        Pick your optional accessories!<br><br>
Pluto offers a wide array of accessories, such as telephones, cameras, and so on. All the Pluto sells are self-configuring. Just plug them 
into any jack, and they work automatically without configuring anything.
<br><br>
For example, you can plug a Pluto phone into any network jack. The phones have extra plugs in there, so you can plug a security camera 
into the phone, or your laptop, or a Pluto interface module to control the TV. Plug anything into anything else--that's the elegance of 
Pluto's IP based, plug-and-play system.<br><br><br><br>
      Here are some of the accessories which Pluto System uses:<br><br> 
      Video Conferencing Camera <br>
      Video Conferencing Microphone<br>
      IP Telephone<br>
      DVD/CD Jukebox<br>
      8 port Gigabit Ethernet Switch<br>
      Wireless Access Point (wi-fi) with built-in 4 port 10/100 Switch<br>
      Interface modules</p>
      <p>Here are some details about <strong>Interface Modules</strong><br>
          <br>
      The Interface Modules offer the flexibility of controlling virtually any device anywhere. It has RS232, Infrared outputs, Relays, and sensor inputs. The Interface Modules communicate with your Pluto system via an Ethernet connection. So you can put one anywhere, whether or not you have other Pluto equipment in that location. </p>
      <p>If, for example, you have a room with some tv and stereo equipment that you want to control, you only need to put an interface module in that room. Plug it into the wall jack, and then stick IR emitters on your equipment. <br>
        Many sub-systems, like lighting, alarm panels, and so on, communicate with an RS232 port. If these devices are near the Core, you can plug them in directly to the Core's RS232 port. But, if they're not, just plug an interface module into the nearest Ethernet jack, and then plug the device into the Interface Module's RS232 port.</p>
      <p>Each of the IR emitter jacks is also a TTL input. This means you can hook up sensors, like motion detectors or contact switches, and other devices like intercoms and push buttons. The Pluto Core will fire an event whenever one of the inputs changes, and using the web interface, you can easily tell the Core how you want it to respond. For example, if you hook a doorbell to a sensor input, the Core can flash lights in addition to sounding the bell when the doorbell is pushed.</p>
      <p><strong>TOAD devices</strong><br>
      The sensors can also be used to make it easier to turn TOAD devices. For an explanation of TOAD devices vs. discrete inputs, click here. There are AC sensors and Video sensors that determine if a piece of equipment is on, based on whether or not it is drawing power or outputting a video signal. This would allow Pluto to determine whether or not the tv is already on, for example, so that it knows whether to send a power command to it. Refer to the aforementioned link if you do not understand what this means.</p>
      <p><strong>Capacities</strong><br>
        The Interface Module is available in 3 sizes: 6&quot;, which has 1 RS232 port and 3 Infrared emitters/TTL inputs. 12&quot;, which has 2 RS232 ports, 3 Relay Outputs, and 6 Infrared emitters/TTL inputs. And 18&quot;, which comes standard with the same inputs/outputs as the 12&quot; module, but has room to add several more modules and can be rack-mounted.</p></td>
  </tr>
</table>
	
TEXT;
		
		$output->setImgName("support.jpg"); //img that needs to be changed
        $output->setPageID(1); //butonul selectat
 		$output->setNavigationMenu(array("Sample page"=>"index.php?section=samplePage")); //not used yet
 		$output->setTitle(APPLICATION_NAME."::Accessories");			//page title
 		
 		
 		
 		//don't touch them 		
		$output->setScriptCalendar('null');
		$output->setScriptTRColor('null');		
		$output->setBody($out);		
  		$output->output(); 
 }
?>
