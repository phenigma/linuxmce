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
      Pluto is sold as a complete, turnkey appliance.  However, like other products in the smart home and high-end media market, Pluto's components are really modified PC computers inside custom cases.
	  What makes Pluto totally remarkable is that we decided to give away all the software for free.  That's right, you can make your own Pluto 
	  system using your PC computers together with our special software.  It will do everything our turnkey product does, and you don't have to pay us anything.  Not only do we promise not to prosecute you for copying our software, we'll actually
	  help you do it with a simple wizard that guides you through the process and provides step-by-step instructions.  The wizard will even give you all our source code if you want it.  To begin, just register or login, go to your My Pluto homepage, and choose "New Installation".</p>
	  <p><a href="index.php?section=letter">Why is Pluto doing this?  What's Pluto's open source strategy?</a></p>
	  <p><a href="index.php?section=developer">I'm a techie.  Tell me about the software.</a></p>
	  <p><a href="index.php?section=get_pluto">How can I get Pluto, either as a pre-configured appliance, or as software I install myself?</a></p>
    </td>
  </tr>
</table>
	
TEXT;
		
		$output->setImgName("support.jpg"); //img that needs to be changed
        $output->setPageID(1); //butonul selectat
 		$output->setNavigationMenu(array("Sample page"=>"index.php?section=samplePage")); //not used yet
 		$output->setTitle(APPLICATION_NAME."::Software");			//page title
 		
 		
 		
 		//don't touch them 		
		$output->setScriptCalendar('null');
		$output->setScriptTRColor('null');		
		$output->setBody($out);		
  		$output->output(); 
 }
?>
