<?php
 function software($output) {
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
        <td><a href="index.php?section=accesories"><img src="images/submenus/Pluto%20system/accessories_off.gif" width="154" height="17" border="0"></a></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
      </tr>
      <tr>
        <td><a href="index.php?section=software"><img src="images/submenus/Pluto%20system/software_on.gif" width="154" height="17" border="0"></a></td>
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
    <p><img src="images/submenus/software_txt.gif" width="72" height="13"><br>
      Coming soon &ndash; Build your own Pluto system for free</p>
    <p>October, 2004 &ndash; Pluto version 2.0 beta software released as open source<br>
        Target: February, 2005 &ndash; Pluto released as a free download for Windows and Linux PC&rsquo;s</p>
    <p>At the moment Pluto is sold exclusively as a proprietary, closed box, turn-key solution. However, inside the Pluto Core, Media Director and Orbiters are specially-modified computers running Pluto&rsquo;s own software. This is quite normal. Most digital video recorders, high-end music &amp; movie servers, smart home solutions, pbx phone systems, etc. are all essentially computers inside running proprietary software. Even cars have computers inside.  But with version 2, Pluto will also run on standard, off-the-shelf PC computers.</p>
    <p>What is not so normal is that, starting with version 2, Pluto will be giving away all the software free and releasing it as open source. Read about this new strategy <a href="index.php?section=letter">here</a></p>
    <p><a href="index.php?section=developer">Learn about the software Pluto will be releasing as open source in October, 2004</a></p>
    <p><a href="index.php?section=get_pluto">How to get Pluto for your home</a></p>
    <p>We will send a newsletter when Pluto 2 has completed beta testing and is ready for the mainstream market as a FREE download for both Windows and Linux. The target release date is February, 2005. Be sure to sign up for our newsletter to be notified.</p>
    </td>
  </tr>
</table>
	
TEXT;
		
		$output->setImgName("support.jpg"); //img that needs to be changed
        $output->setPageID(1); //butonul selectat
 		$output->setNavigationMenu(array("Sample page"=>"index.php?section=samplePage")); //not used yet
 		$output->setTitle(APPLICATION_NAME."::Accesories");			//page title
 		
 		
 		
 		//don't touch them 		
		$output->setScriptCalendar('null');
		$output->setScriptTRColor('null');		
		$output->setBody($out);		
  		$output->output(); 
 }
?>
