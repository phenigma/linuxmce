<?php
 function orbiter($output) {
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
        <td><a href="index.php?section=orbiter"><img src="images/submenus/Pluto%20system/orbiter_on.gif" width="154" height="17" border="0"></a></td>
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
        <td><a href="index.php?section=accesories"><img src="images/submenus/Pluto%20system/accessories_off.gif" width="154" height="17" border="0"></a></td>
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
    <p><img src="images/submenus/orbiter_txt.gif" width="56" height="13"><br>
      </p>
      <p>The Pluto Orbiter has an 8.4&quot; color, touch-screen display, with hard buttons, including a 4 way rocker. It also has a built-in speaker and microphone.</p>
      <p>The built-in battery and wi-fi (802.11) allow you to carry the Orbiter anywhere. The table mount (shown) comes standard, and a wall mount is available. Either way, the Orbiter will charge while in the base.</p>
      <p><strong>How many Orbiters do you want?</strong><br>
        <br>
You do not need any Pluto Orbiters--you can control the entire Pluto system with your mobile phone.  However, most customers like to have at least 1 or 2 Orbiters, especially for the main theater room, since browsing catalogs of media is easier on the Orbiter, and the large, color touch-screen let's you control your whole house at a glance, with interactive maps.  You can even see clearly on a map all the media playing throughout the house, the status of all your security sensors, lighting, climate, pool, sprinkler, etc.</p>
    <p><center><a href="javascript:open_popup('operations/poza30.html','532','465')"><img src="images/img%20system/orbiter/o_01_m.jpg" width="141" height="112" border="0"></a> <a href="javascript:open_popup('operations/poza31.html','532','465')"><img src="images/img%20system/orbiter/o_02_m.jpg" width="141" height="112" border="0"></a> <a href="javascript:open_popup('operations/poza32.html','532','465')"><img src="images/img%20system/orbiter/o_03_m.jpg" width="141" height="112" border="0"></a>
    </center></p></td>
  </tr>
</table>
	
TEXT;
		
		$output->setImgName("support.jpg"); //img that needs to be changed
        $output->setPageID(1); //butonul selectat
 		$output->setNavigationMenu(array("Sample page"=>"index.php?section=samplePage")); //not used yet
 		$output->setTitle(APPLICATION_NAME."::Orbiter");			//page title
 		
 		
 		
 		//don't touch them 		
		$output->setScriptCalendar('null');
		$output->setScriptTRColor('null');		
		$output->setBody($out);		
  		$output->output(); 
 }
?>