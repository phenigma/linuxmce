<?php
 function media_director($output) {
		$out = '';
		
$out.=<<<TEXT

<table width="100%"  border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td align="left" class="insidetable"><table width="438"  border="0" cellpadding="0" cellspacing="0">
      <tr>
        <td width="155"><a href="index.php?section=pluto_system_overview"><img src="images/buttons/pluto_system_overview_on.gif" width="155" height="11" border="0" /></a></td>
        <td width="155"><a href="index.php?section=how_the_system_works"><img src="images/buttons/how_the_system_works_off.gif" width="155" height="11" border="0" /></a></td>
        <td width="128"><a href="index.php?section=learn_how_to_use_it"><img src="images/buttons/learn_how_to_use_it_off.gif" width="128" height="11" border="0" /></a></td>
        </tr>
    </table></td>
  </tr>
  <tr>
    <td valign="top" background="images/back_insidetable.jpg" class="insidetable"><table width="100%"  border="0" cellpadding="0" cellspacing="0">
      <tr>
        <td width="89" valign="top"><img src="images/thumbs/img_overview.jpg" width="89" height="89" /></td>
        <td width="10"><img src="images/spacer.gif" width="10" height="10" /></td>
        <td valign="top"><table width="100%"  border="0" cellspacing="0" cellpadding="0">
            <tr>
              <td><img src="images/titles/pluto_system_overview.gif" width="200" height="13" /></td>
            </tr>
            <tr>
              <td><br />
            A Pluto system consists of 4 pieces of equipment: Pluto Core, Pluto Media Director, Pluto Orbiter, Pluto Mobile Orbiter.In addition to the 4 main components, there exist many accessories such as cameras, interface modules to control your existing A/V, lighting, etc.<br /></td>
            </tr>
            <tr>
              <td align="right">&nbsp;</td>
            </tr>
        </table></td>
      </tr>
    </table></td>
  </tr>
  <tr>
    <td valign="top" class="insidetable2"><p><img src="images/bullet_1.gif" width="11" height="11" align="absmiddle" /> <span class="title">Pluto Media Director </span> </p>
    <p>The MEDIA DIRECTOR renders the media on your TV/stereo. In addition to all the standard analog in/out connections and SPDIF digital audio, it supports the latest digital video connection: DVI. If you have a high def TV, you can enjoy perfect DVD movies, 100% digital from the Core, through the Media Director to the TV without ever converting to analog or losing 1 pixel. The Media Director up-samples the video to high-definition, and even beyond high-definition for the latest super-high resolution plasma TVs or projectors for the ardent audio/video-phile. The Media Director includes a built-in CD/DVD/DVD-RW/DVD+RW Player and Recorder, ports for digital camcorders (firewire) plus 6 types of digital film, an interface module to control your A/V equipment, and a PC computer with a wireless keyboard and mouse. It also includes Pluto's patent-pending 'follow-me' detection system so that it can detect who is in each room. Allow 1 Pluto Media Director for each TV or entertainment area. The Pluto Core works with popular multi-zone audio systems and digital audio players, providing an inexpensive solution for whole-house audio.</p>
    <p>
      <!--images media director--><table>
		<tr>
			<td><a href="javascript:open_popup('operations/image.php?img=md','width=530,height=430,scrollbars=0,toolbar=0,resizable=0');"><img src="images/thumbs/media_director_1.jpg" border="0" alt="Click to see larger image"></a></td>
			<td><a href="javascript:open_popup('operations/image.php?img=md&alt=1','width=530,height=430,scrollbars=0,toolbar=0,resizable=0');"><img src="images/thumbs/media_director_2.jpg" border="0" alt="Click to see larger image"></a></td>
		</tr>
	</table>
    </p>
    <p><strong>Technical specifications:<br />
      </strong><br />
        <strong>Dimensions/Mounting:</strong><br />
12.2&quot; x 3.3&quot; x 13.5&quot; (W x H x D)<br />
310mm x 84mm x 345mm (W x H X D)</p>
    <p> <strong>Capacities/Configuration:</strong><br />
  Gigabit Ethernet<br />
  CD/DVD -R/-RW and +R/+RW<br />
  6x USB 2.0 (4 front), 2x 1394 Firewire (1 front)<br />
  <strong>Audio: </strong>SPDIF Digital + analog<br />
  <strong>Video:</strong> DVI (Digital), VGA, S-Video, Composite, Component with an adapter<br />
  <strong>Pluto applications:</strong> diskless configuration with distributed processing throughout the network<br />
  <strong>Windows applications:</strong> 120GB Hard Drive, Intel Pentium Processor, 512MB RAM</p>
    <p> <strong>Installation considerations:</strong><br />
  Worldwide electrical standard: 110-230 volts, 50/60 Hz<br />
  Similar to a PVR or Media PC, the Pluto Media Director should not be placed in enclosed cabinet with no ventilation - it will overheat. An open shelf is recommended.</p></td>
  </tr>
  <tr>
    <td valign="top">&nbsp;</td>
  </tr>
</table>
	
TEXT;
		
	$output->setPressedButton(3);
	$output->setNavigationMenu(array('How Pluto works'=>'index.php?section=how_pluto_works','How Pluto works'=>'index.php?section=how_pluto_works','Pluto System Overview'=>'index.php?section=pluto_system_overview','Media Director'=>'index.php?section=media_director'));
	$output->setTitle(APPLICATION_NAME." :: Media Director");			//page title
	$output->setScriptCalendar('null');
	$output->setScriptTRColor('null');
	$output->setBody($out);
	$output->output();
 }
?>