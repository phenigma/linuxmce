<?php
 function compatibility($output) {
	$out = '';
 		$out.='<table width="100%" border="0" cellpadding="0" cellspacing="0" class="maintable">
      <td width="10%" valign="top" align="left" >
   <table border="0" align="left" cellpadding="0" cellspacing="0">
     <tr>
    <td class="titletable"><img src="images/meniuri/plutovip_support_txt.gif" width="131" height="13"></td>
    </tr>
       </table></td>
      <tr><td width="20%" valign="top" align="center" class="insidetable">
      <table border="0" align="left" cellpadding="0" cellspacing="0">
      <tr>
        <td width="154"><a href="index.php?section=download"><img src="images/submeniuri/download_off.jpg" width="138" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=compatibility"><img src="images/submeniuri/compatibility_on.jpg" width="138" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=technical_support"><img src="images/submeniuri/technical_support_off.jpg" width="138" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=faq_presentation"><img src="images/submeniuri/faqs_off.jpg" width="138" height="17" border="0"></a></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
        </tr>
      
    </table>
       </tr>
      </td>
  <td width="100%" align="left" class="insidetable">
    <p><img src="images/submeniuri/compatibility_txt.gif" width="102" height="13"><br>
        <p>Pluto can work with nearly everything that provides a means for external control. Pluto supports all standard interfaces: Ethernet, infrared, RS232, RS485, USB, and relay control.</p>
      <p>If you have a device that you think should be controllable, please tell us about it using our tech support form or by calling (877) pluto-4-u. Our goal is to support the widest array of devices, so if you have a normal, commercially-available device that is not on our list, we have a whole department that is ready to prepare a driver so that it will work with your Pluto system. There is never a charge. The following are the devices known to be compatible at this time.</p>
      <blockquote>
        <p> <br>
          <strong>AV Equipment: </strong><br>
        Pluto can control all your existing AV equipment using normal infrared. However, read this to understand some important things about successfully using IR.</p>
        <p><strong>Security panels:</strong> <br>
          Apex/Ademco Destiny 6100, Vista 128BP, 250BP <br>
          First Alert FA1660C <br>
          GE Interlogix/Caddx NetworX NX-4, NX-6, NX-8, NX-8E <br>
          HAI Omni series<br>
          DSC MAXSYS P4020 </p>
        <p><strong>Climate control: </strong><br>
          Aprilaire 8870/Statnet Thermostat <br>
          HAI Omni<br>
          X10 Thermostats (RCS TX15R/B)<br>
          Honeywell EnviraCon</p>
        <p><strong>Lighting control: </strong><br>
          ALC (HAI, OnQ) <br>
          Leviton DHC<br>
          Lightolier Compose PLC / Multiset<br>
          X-10<br>
          Vantage <br>
          Lutron Homeworks, RadioRa &amp; GrafikEye<br>
          LiteTouch 5000LC</p>
        <p><strong>Audio streamers/players: </strong><br>
          Barix Exstreamer <br>
        Turtle Beach AutioTron </p>
        <p><strong>Multi-zone audio systems: </strong><br>
          Russound CAV6.6 <br>
          Audioaccess PX-700<br>
          Vaux Electronics<br>
          Xantech<br>
        Nuvo</p>
        <p><strong>Surveillance cameras: </strong><br>
          Panasonic IP Cameras<br>
          Dlink IP Camera<br>
          Veo Observer,<br>
        Axis IP Camera</p>
        <p><strong>Pool: </strong><br>
          Jandy Aqualink</p>
        <p><strong>Sprinkler and irrigation: </strong><br>
          X10<br>
          Rain8PC<br>
          The Pluto interface module that comes with all Media Directors also has relay controls that can be used for sprinkler/irrigation systems</p>
        <p><strong>Miscellaneous devices and sensors</strong>: <br>
          Applied Digital (light, temperature, humidity)<br>
          JDS</p>
      </blockquote></td>
  </tr>
</table>';
	

		
		$output->setImgName("support.jpg"); //img that needs to be changed
        $output->setPageID(4); //butonul selectat
 		$output->setNavigationMenu(array("Sample page"=>"index.php?section=samplePage")); //not used yet
 		$output->setTitle(APPLICATION_NAME."::Compatibility");			//page title
 		
 		
 		
 		//don't touch them 		
		$output->setScriptCalendar('null');
		$output->setScriptTRColor('null');		
		$output->setBody($out);		
  		$output->output(); 
 }
?>