<?php
 function accesories($output) {
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
    <td valign="top" class="insidetable2"><p><img src="images/bullet_1.gif" width="11" height="11" align="absmiddle" /> <span class="title">Accesories</span></p>
      <p>In addition to the 4 main components, there exist many accessories such as cameras, interface modules to control your existing A/V, lighting, etc. </p>
      <p><strong>Accesories:</strong><br />
      Video Conferencing Camera<br />
      Video Conferencing Microphone<br />
      IP Telephone<br />
      DVD/CD Jukebox<br />
      8 port Gigabit Ethernet Switch<br />
      Wireless Access Point (wi-fi) with built-in 4 port 10/100 Switch<br />
      Interface modules</p>
    <p>All the Pluto sells are self-configuring. Just plug them into any jack and they work automatically without configuring anything. You can plug a Pluto phone into any network jack. The phones have extra plugs in there, so you can plug a security camera into the phone, or your laptop, or a Pluto interface module to control the TV. Plug anything into anything else - that's the elegance of Pluto's IP based, plug-and-play system.</p>
    </td>
  </tr>
  <tr>
    <td valign="top">&nbsp;</td>
  </tr>
</table>
	
TEXT;

	$output->setPressedButton(3);
	$output->setNavigationMenu(array('How Pluto works'=>'index.php?section=how_pluto_works','How Pluto works'=>'index.php?section=how_pluto_works','Pluto System Overview'=>'index.php?section=pluto_system_overview','Accesories'=>'index.php?section=accesories'));
		
 	$output->setTitle(APPLICATION_NAME." :: Accessories");			//page title
	$output->setBody($out);		
  	$output->output(); 
 }
?>
