<?php
 function company($output) {
		$out = '';
		
$out.=<<<TEXT

<table width="100%" border="0" cellpadding="0" cellspacing="0" class="maintable">
  <tr>
    <td class="titletable"><img src="images/submenus/company_txt.gif" width="66" height="13"></td>
  </tr>
  <tr>
    <td class="insidetable"><table border="0" align="left" cellpadding="0" cellspacing="0">
      <tr>
        <td width="139"><a href="index.php?section=company"><img src="images/submenus/Company/company_overview_on.gif" width="139" height="17" border="0"></a></td>
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
        <td><a href="index.php?section=privacy_policy"><img src="images/submenus/Company/privacy_policy_off.gif" width="139" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=site_map"><img src="images/submenus/Company/site_map_off.gif" width="139" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td>&nbsp;</td>
        <td>&nbsp;</td>
      </tr>
    </table>
       <img src="images/submenus/company_overview_txt.gif" width="137" height="13">
                <p>Corporate office:<br>
                  Pluto<br>
                  2699 Stirling Road, Building C<br>
                  Fort Lauderdale, FL 33312<br>
                  Phone: (877) pluto-4-u<br>
                  Office house (EST): Mon-Fri 8:30am &ndash; 9:00pm Sat: 10:00am &ndash; 3:00pm</p>
                  
      <p> Pluto is a privately held company, founded in 2002 in Fort Lauderdale, Florida with the singular mission of offering a product that was more of a lifestyle than a technology. Pluto is a destination that allows our customers to focus on the important things in life, knowing that the myriads of devices and electronic gadgets we have come to rely on work together seamlessly, intuitively.</p>
      <p>A product this unique could only be developed by a team equally unique. Pluto recruited the best engineers around for its development team. But a product as ambitious as Pluto demanded even more. So Pluto heartily embraced the open source community and found the premier engineers from a variety of open source projects from all over the world. They too were brought to Florida to work side-by-side with our in-house teams, bringing with them years of experience and high-quality projects that were incorporated into the Pluto product.</p></td>
  </tr>
</table>

		
TEXT;
		
		$output->setImgName("support.jpg"); //img that needs to be changed
        $output->setPageID(6); //butonul selectat
 		$output->setNavigationMenu(array("Sample page"=>"index.php?section=samplePage")); //not used yet
 		$output->setTitle(APPLICATION_NAME."::Company Overview");			//page title
 		
 		
 		
 		//don't touch them 		
		$output->setScriptCalendar('null');
		$output->setScriptTRColor('null');		
		$output->setBody($out);		
  		$output->output(); 
 }
?>