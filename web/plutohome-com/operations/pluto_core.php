<?php
 function pluto_core($output) {
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
    <td valign="top" class="insidetable2"><p><img src="images/bullet_1.gif" width="11" height="11" align="absmiddle" /> <span class="title">Pluto Core</span> </p>
    <p>The CORE is a commercial-grade Linux server. It is entirely self-configuring. It does not even come with a mouse or a keyboard. Just plug it in.<br />
      Because the Core communicates with everything over the local area network, the connections are easy: just 2 standard network connectors, one for your internal network in the house, and one for your internet connection. Additionally, your Core will have jacks for whatever telephone lines you have. The core has a built-in voice-over-ip pbx switch that can route all incoming and outgoing calls over the internet. However, we recommend that you always have at least one 'normal' phone line as a backup in case there is a problem with your internet connection.</p>
    <p>
      <!--images core --><table>
		<tr>
			<td><a href="javascript:open_popup('operations/image.php?img=core','width=530,height=430,scrollbars=0,toolbar=0,resizable=0');"><img src="images/thumbs/core_1.jpg" border="0" alt="Click to see larger image"></a></td>
			<td><a href="javascript:open_popup('operations/image.php?img=core&alt=1','width=530,height=430,scrollbars=0,toolbar=0,resizable=0');"><img src="images/thumbs/core_2.jpg" border="0" alt="Click to see larger image"></a></td>
		</tr>
	</table>
    </p>
    <p><strong>Technical specifications:<br />
      </strong><br />
        <strong>Dimensions/Mounting:</strong> 16.9&quot; x 6.9&quot; x 23.8&quot; (W x H x D)<br />
        <strong>Rackmount:</strong> Standard 19&quot; 4U<br />
        <strong>Weight:</strong> Varies depending on drive capacity, up to 80 lbs.</p>
    <p> <strong>Capacities (maximum configuration):</strong><br />
      <strong>Storage: </strong>10 TB (10,000 GB) RAID 5 Array, configured as 60 200GB Drives contained in 5 cases with each case having a capacity of 12 drives, 2 TB total<br />
      <strong>Processor:</strong> Dual Intel Xeon with Hyperthreading<br />
      <strong>Memory:</strong> Dual-Channel DDR333, 4 GB<br />
      <strong>LAN:</strong> Primary (internal network) Intel Gigabit (1,000 Base T), Secondary (external/internet) 10/100 Base T<br />
      <strong>Audio:</strong> 6 channel digital audio for use with whole-house audio systems<br />
      <strong>Connections:</strong> 2x RS232, 2x 1394 Firewire, 8x USB 2.0</p>
    <p> <strong>Installation considerations:</strong><br />
  Worldwide electrical standard: 110-230 volts, 50/60 Hz<br />
  Good ventilation and cool environment are important for reliability. Recommend no more than 75 F/24 C</p></td>
  </tr>
  <tr>
    <td valign="top">&nbsp;</td>
  </tr>
</table>
	
TEXT;
		

	$output->setPressedButton(3);
	$output->setNavigationMenu(array('How Pluto works'=>'index.php?section=how_pluto_works','How Pluto works'=>'index.php?section=how_pluto_works','Pluto System Overview'=>'index.php?section=pluto_system_overview','Pluto Core'=>'index.php?section=pluto_core'));
	$output->setTitle(APPLICATION_NAME.' :: Pluto Core');	
	$output->setScriptCalendar('null');
	$output->setScriptTRColor('null');
	$output->setBody($out);
	$output->output();
}
?>
