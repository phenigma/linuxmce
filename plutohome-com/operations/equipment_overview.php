<?php
 function equipment_overview($output) {
		$out = '';
		
$out.=<<<TEXT

<table width="100%" border="0" cellpadding="0" cellspacing="0" class="maintable">
  <tr>
    <td class="titletable"><img src="images/submenus/pluto_system_txt.gif" width="96" height="13"></td>
  </tr>
  <tr>
    <td class="insidetable"><table border="0" align="left" cellpadding="0" cellspacing="0">
      <tr>
        <td><a href="index.php?section=equipment_overview"><img src="images/submenus/Pluto%20system/equipment_overview_on.gif" width="154" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=core"><img src="images/submenus/Pluto%20system/core_off.gif" width="154" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=media_director"><img src="images/submenus/Pluto%20system/media_director_off.gif" width="154" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=orbiter"><img src="images/submenus/Pluto%20system/orbiter_off.gif" width="154" height="17" border="0"></a></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=mobile_orbiter"><img src="images/submenus/Pluto%20system/mobile_orbiter_off.gif" width="154" height="17" border="0"></a></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=accesories"><img src="images/submenus/Pluto%20system/accessories_off.gif" width="154" height="17" border="0"></a></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=software"><img src="images/submenus/Pluto%20system/software_off.gif" width="154" height="17" border="0"></a></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=special_offer"><img src="images/submenus/Pluto%20system/special_offer_off.gif" width="154" height="17" border="0"></a></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="javascript:open_popup('operations/flash.html','796','542')"><img src="images/flash_presentation.gif" width="154" height="17" border="0"></a></td>
        <td>&nbsp;</td>
      </tr>
    </table>
      <p><img src="images/submenus/equipment_overview_txt.gif" width="148" height="13"><br>
      A Pluto system consists of 4 pieces of equipment:</p>
      <p><strong>Pluto Core</strong> -- This is the brains of the system. It is a commercial-grade, Linux server, and it orchestrates all the activity on your Pluto system. Each home needs only 1 Pluto Core.<br>
          <br>
          <strong>Pluto Media Director</strong> -- This is what hooks up to your tv and a/v equipment. You will need a Pluto Media Director wherever you have a tv or entertainment area that you would like to be fully integrated into your Pluto system. Note that an inexpensive Interface Module is available for those entertainment areas where you do not wish to invest in a Media Director, and this Module allows you to control that equipment using the Orbiters or your mobile phone.<br>
          <br>
      <strong>Pluto Orbiter</strong> -- You control the Pluto system using either your mobile phone, or the Pluto Orbiter. The Pluto Orbiter is a color-touch screen tablet that comes with either a cradle for setting it on a table, or a wall mount.<br><br>
<strong>Pluto Mobile Orbiter</strong> -- Pluto makes your mobile phone the most simple and convenient universal remote for everything regardless of the brand—lights, tv, stereo, security, climate, etc. Carry your phone and Pluto will always know what room you’re in and your personal choices for tv, music, phone calls, lighting, climate, etc. follow you through the house automatically.	
</p>
  <table  border="0" align="center" cellpadding="0" cellspacing="0">
            <tr>
              <td><a href="javascript:open_popup('operations/poza24.html','532','465')"><img src="images/img%20system/core/c_01_m.jpg" width="141" height="112" border="0"></a></td>
              <td>&nbsp;</td>
              <td><a href="javascript:open_popup('operations/poza29.html','532','465')"><img src="images/img%20system/media_director/md_03_m.jpg" width="141" height="112" border="0"></a></td>
              <td>&nbsp;</td>
              <td><a href="javascript:open_popup('operations/poza32.html','532','465')"><img src="images/img%20system/orbiter/o_03_m.jpg" width="141" height="112" border="0"></a></td>
              <td>&nbsp;</td>
              <td><a href="javascript:open_popup('operations/poza33.html','532','465')"><img src="images/img%20system/telefon/t_02_m.jpg" width="141" height="112" border="0"></a></td>
        </tr>
            <tr>
              <td align="center" class="text">The Core</td>
              <td>&nbsp;</td>
              <td align="center" class="text">Media Director </td>
              <td>&nbsp;</td>
              <td align="center" class="text">Orbiter</td>
              <td>&nbsp;</td>
              <td align="center" class="text">Mobile Orbiter </td>
            </tr>
          </table>
      <p>In addition to the 4 main components, there exist many accessories, such as cameras, interface modules to control your existing a/v, lighting, etc.</p>
    </td>
  </tr>
</table>
	
TEXT;
		
		$output->setImgName("support.jpg"); //img that needs to be changed
        $output->setPageID(1); //butonul selectat
 		$output->setNavigationMenu(array("Sample page"=>"index.php?section=samplePage")); //not used yet
 		$output->setTitle(APPLICATION_NAME."::Equipment Overview");			//page title
 		
 		
 		
 		//don't touch them 		
		$output->setScriptCalendar('null');
		$output->setScriptTRColor('null');		
		$output->setBody($out);		
  		$output->output(); 
 }
?>
