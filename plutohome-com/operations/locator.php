<?php
 function get_pluto($output) {
		$out = '';
		
$out.='

<table width="100%" border="0" cellpadding="0" cellspacing="0" class="maintable">
  <tr>
    <td class="titletable"><img src="images/submenus/how_to_buy_txt.gif" width="82" height="13"></td>
  </tr>
  <tr>
    <td class="insidetable"><table border="0" align="left" cellpadding="0" cellspacing="0">
      <tr>
        <td width="154"><a href="index.php?section=get_pluto"><img src="images/submenus/How%20to%20buy/pluto_for_my_home_on.gif" width="200" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
      </tr>
      <tr>
        <td><a href="index.php?section=dealer"><img src="images/submenus/How%20to%20buy/a_dealer_off.gif" width="200" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
      </tr>
      <tr>
        <td><a href="index.php?section=manufacturer"><img src="images/submenus/How%20to%20buy/a_manufacturer_off.gif" width="200" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
      </tr>
      <tr>
        <td><a href="index.php?section=developer"><img src="images/submenus/How%20to%20buy/open_source_developer_off.gif" width="200" height="17" border="0"></a></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
      </tr>
      <tr>
        <td>&nbsp;</td>
        <td>&nbsp;</td>
      </tr>
    </table>
    <p><img src="images/submenus/pluto_for_my_home_txt.gif" width="165" height="13"><br>
      
      <p>Pluto has 2 types of dealers: Standard and Pro.  Both provide the complete Pluto system seen here.  
      But Pluto Pro dealers require special certification and must also sell compatible lighting control systems, security, climate and pool controls. 
      Most also sell high-end audio/video equipment, and work with contractors to offer built-in solutions, such as in-wall Orbiters. 
      Your Pluto Pro dealer offers a complete, custom installed, turn-key solution.  Also, only Pro dealers can sell Pluto Pro: our high-end line, made from the finest materials, available in a variety of finishes and colors to complement any d&eacute;cor.</p>
    
      <p>Pluto Series 2 is expected to be released for retail in March, 2005.  We are now signing up dealers.  The dealer locator service will be available in February.  Please check back then.</p>
      <p>If you wish to be contacted by a dealer in your area, you can complete the form below.  We will forward all inquiries to dealers in February.</p>
';
	include('operations/contactForm.php');
	$out.=contactForm('locator');


	$output->setImgName("support.jpg"); //img that needs to be changed
	$output->setPageID(5); //butonul selectat
	$output->setNavigationMenu(array("Sample page"=>"index.php?section=samplePage")); //not used yet
	$output->setTitle(APPLICATION_NAME."::Get Pluto for My Home");			//page title



	//don't touch them
	$output->setScriptCalendar('null');
	$output->setScriptTRColor('null');
	$output->setBody($out);
	$output->output();
 }
?>
