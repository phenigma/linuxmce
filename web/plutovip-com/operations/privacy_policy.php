<?php
 function privacy_policy($output) {
		$out = '';
		
$out.=<<<TEXT

<table width="100%" border="0" cellpadding="0" cellspacing="0" class="maintable" align="left">
  <tr>
    <td class="titletable" align="left"><img src="images/submenus/company_txt.gif" width="66" height="13"></td>
  </tr>
  <tr>
    <td class="insidetable" valign="top"><table border="0" align="left" cellpadding="0" cellspacing="0">
      <tr>
        <td width="139"><a href="index.php?section=company"><img src="images/submenus/Company/company_overview_off.gif" width="139" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=contact"><img src="images/submenus/Company/contact_off.gif" width="139" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=newsPresentation"><img src="images/submenus/Company/news_off.gif" width="139" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=privacy_policy"><img src="images/submenus/Company/privacy_policy_on.gif" width="139" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
        </tr>
     
    </table>
    </td>
  <td width="100%" align="left" class="insidetable">
                <p><img src="images/submenus/privacy_policy_txt.gif" width="106" height="13"><br><br>
                Pluto does not share any information you provide with third parties unless you specifically request it.</p>
                <p>If you choose to have the plutohome.com web-site remember your login, this information will be stored in a cookie. The cookie is not used for any other purpose, and does not track your internet activity. Payment information, such as credit card numbers, that you provide to make a purchase are used only for that 1 transactions, and are then purged from our systems.</p>
                <p>The Site, all creative and technical design, texts, graphics, interfaces, interactive elements are &copy; PLUTO HOME, ALL RIGHTS RESERVED</p>
                <p>Please contact us, if have questions about our privacy policy.</p>
 
    </td>
  </tr>
</table>
		
TEXT;
		
		$output->setImgName("pic.jpg"); //img that needs to be changed
        $output->setPageID(6); //butonul selectat
 		$output->setNavigationMenu(array("Sample page"=>"index.php?section=samplePage")); //not used yet
 		$output->setTitle(APPLICATION_NAME."::Privacy Policy");			//page title
 		
 		
 		
 		//don't touch them 		
		$output->setScriptCalendar('null');
		$output->setScriptTRColor('null');		
		$output->setBody($out);		
  		$output->output(); 
 }
?>