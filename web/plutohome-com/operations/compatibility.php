<?php
 function compatibility($output) {
		$out = '';
		
$out.=<<<TEXT

<table width="100%" border="0" cellpadding="0" cellspacing="0" class="maintable">
  <tr>
    <td class="titletable"><img src="images/submenus/support_txt.gif" width="60" height="13"></td>
  </tr>
  <tr>
    <td class="insidetable"><table border="0" align="left" cellpadding="0" cellspacing="0">
      <tr>
        <td width="154"><a href="index.php?section=installation"><img src="images/submenus/Support/installation_information_of.gif" width="207" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="images/spacer.gif" width="3" height="3"></td>
      </tr>
      <tr>
        <td><a href="index.php?section=project"><img src="images/submenus/Support/project_management_off.gif" width="207" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="images/spacer.gif" width="3" height="3"></td>
      </tr>
      <tr>
        <td><a href="index.php?section=warranty"><img src="images/submenus/Support/warranty_off.gif" width="207" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="images/spacer.gif" width="3" height="3"></td>
      </tr>
      <tr>
        <td><a href="index.php?section=download"><img src="images/submenus/Support/download_off.gif" width="207" height="17" border="0"></a></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="images/spacer.gif" width="3" height="3"></td>
      </tr>
      <tr>
        <td><a href="index.php?section=compatibility"><img src="images/submenus/Support/compatibility_on.gif" width="207" height="17" border="0"></a></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="images/spacer.gif" width="3" height="3"></td>
      </tr>
      <tr>
        <td><a href="index.php?section=technical_support"><img src="images/submenus/Support/technical_support_off.gif" width="207" height="17" border="0"></a></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="images/spacer.gif" width="3" height="3"></td>
      </tr>
      <tr>
        <td><a href="index.php?section=faq_presentation"><img src="images/submenus/Support/faqs_off.gif" width="207" height="17" border="0"></a></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="images/spacer.gif" width="3" height="3"></td>
      </tr>
      <tr>
        <td>&nbsp;</td>
        <td>&nbsp;</td>
      </tr>
    </table>
    <p><img src="images/submenus/compatibility_txt.gif" width="99" height="13"><br>
        Pluto can work with nearly everything that provides a means for external control. Pluto supports all standard interfaces: Ethernet, infrared, RS232, RS485, USB, and relay control.</p>
        <p>If you have a device that you think should be controllable, please tell us about it using our <a href="index.php?section=technical_support">tech support form</a>.</p>
        <p>If you are a programmer, you can easily make your own DCE Devices to do almost anything.  We even have a wizard for programmers that will build the whole project for you automatically.  <a href="/support/index.php?section=document&docID=15">Introduction for programmers and developers</a>
         The following are the devices known to be compatible at this time, although most of them have not yet been ported to our series 2 platform.</p>
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
</table>
	
TEXT;
		
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