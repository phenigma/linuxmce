<?php
 function download($output) {
		$out = '';
 		$out.='<table width="100%" border="0" cellpadding="0" cellspacing="0" class="maintable">
     <td width="10%" valign="top" align="left" >
   <table border="0" align="left" cellpadding="0" cellspacing="0">
     <tr>
    <td class="titletable"><img src="images/meniuri/plutovip_support_txt.gif" width="131" height="13"></td>
    </tr>
       </table></td>
      <tr><td width="20%" valign="top" align="center" class="insidetable">
      <table border="0" align="left" cellpadding="0" cellspacing="0">
      <tr>
        <td width="154"><a href="index.php?section=download"><img src="images/submeniuri/download_on.jpg" width="138" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=compatibility"><img src="images/submeniuri/compatibility_off.jpg" width="138" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=technical_support"><img src="images/submeniuri/technical_support_off.jpg" width="138" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=faq_presentation"><img src="images/submeniuri/faqs_off.jpg" width="138" height="17" border="0"></a></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
        </tr>
      
    </table>
       </tr>
      </td>
  <td width="100%" align="left" class="insidetable">
    <p><img src="images/submeniuri/download_txt.gif" width="79" height="13"><br>
        <br>
      Your Pluto system is self-configuring and self-updating. There is no software to download. When minor revisions are available, the Pluto Core will update itself automatically. If there is a major revision that would affect functionality, such as adding new features or changing the user interface, your Pluto Orbiter or mobile phone will notify you when such upgrades are available, explain the changes, and give you the option of upgrading.</p>
      <p>The only software that you may want to install is the Windows version of the Pluto Orbiter software to control the system from a Windows pc. For security reasons, this software must be used with a Windows pc on your internal network--if you want to control your Pluto system from outside your home you must use either the mobile phone or the Web interface. To install the Windows version of the Pluto Orbiter software, open your web browser (such as Internet explorer) on your Windows pc and go to: http://core/software.php (NOTE: That link will only work if you are using a computer in your home with a Pluto Core). Choose the link for &quot;Pluto Orbiter - Windows Version&quot;. The software will install itself automatically. Once installed, this software is also self-updating. You will not need to install it again or upgrade.</p></td>
  </tr>
</table>
	
';
		
		$output->setImgName("support.jpg"); //img that needs to be changed
        $output->setPageID(4); //butonul selectat
 		$output->setNavigationMenu(array("Sample page"=>"index.php?section=samplePage")); //not used yet
 		$output->setTitle(APPLICATION_NAME."::Download");			//page title
 		
 		
 		
 		//don't touch them 		
		$output->setScriptCalendar('null');
		$output->setScriptTRColor('null');		
		$output->setBody($out);		
  		$output->output(); 
 }
?>