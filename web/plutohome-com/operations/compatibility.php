<?php
 function compatibility($output) {
		$out = '';
		
$out.=<<<TEXT

<table width="100%" border="0" cellpadding="0" cellspacing="0" class="maintable">
  <tr>
    <td align="center" class="insidetable"><table width="563"  border="0" cellpadding="0" cellspacing="0">
      <tr>
        <td align="left"><img src="images/titles/support.gif"></td>
      </tr>
    </table></td>
  </tr>
  <tr>
    <td align="left" class="insidetable2">
        Pluto can work with nearly everything that provides a means for external control. Pluto supports all standard interfaces: Ethernet, infrared, RS232, RS485, USB, and relay control.</p>
        <p>If you have a device that you think should be controllable, please tell us about it using our <a href="index.php?section=support">support form</a>.</p>
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
      </blockquote>	</td>
  </tr>
</table>

	
TEXT;
		
	$output->setPressedButton(4);
	$output->setNavigationMenu(array('Support'=>'index.php?section=support','Compatibility'=>'index.php?section=compatibility'));
	$output->setTitle(APPLICATION_NAME."::Compatibility");			//page title
	$output->setScriptCalendar('null');
	$output->setScriptTRColor('null');
	$output->setBody($out);
	$output->output();
 }
?>