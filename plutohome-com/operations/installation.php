<?php
 function installation($output) {
		$out = '';
		
$out.=<<<TEXT

<table width="100%" border="0" cellpadding="0" cellspacing="0" class="maintable">
  <tr>
    <td class="titletable"><img src="images/submenus/support_txt.gif" width="60" height="13"></td>
  </tr>
  <tr>
    <td class="insidetable"><table border="0" align="left" cellpadding="0" cellspacing="0">
      <tr>
        <td width="154"><a href="index.php?section=installation"><img src="images/submenus/Support/installation_information_on.gif" width="207" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=project"><img src="images/submenus/Support/project_management_off.gif" width="207" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=warranty"><img src="images/submenus/Support/warranty_off.gif" width="207" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=download"><img src="images/submenus/Support/download_off.gif" width="207" height="17" border="0"></a></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=compatibility"><img src="images/submenus/Support/compatibility_off.gif" width="207" height="17" border="0"></a></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=technical_support"><img src="images/submenus/Support/technical_support_off.gif" width="207" height="17" border="0"></a></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=faq_presentation"><img src="images/submenus/Support/faqs_off.gif" width="207" height="17" border="0"></a></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td>&nbsp;</td>
        <td>&nbsp;</td>
      </tr>
    </table>
      <p><img src="images/submenus/installation_information_tx.gif" width="187" height="13"></p>
      <p>Pluto has a comprehensive support web site with both general information about Pluto, and also detailed support for each of the plug-in modules.  
      To install the free software, just register or login, and choose "New Installation" from your My Pluto home page.
      Here are some useful shortcuts to our online support:<br>
      <br><a href="/support/index.php?section=document&docID=11">Installation Section</a>
      <br><a href="/support/index.php?section=document&docID=14">User's manual</a>
      <br><a href="/support/index.php?section=document&docID=15">Introduction for programmers and developers</a>
  </tr>
</table>
	
TEXT;
		
		$output->setImgName("support.jpg"); //img that needs to be changed
        $output->setPageID(4); //butonul selectat
 		$output->setNavigationMenu(array("Sample page"=>"index.php?section=samplePage")); //not used yet
 		$output->setTitle(APPLICATION_NAME."::Installation Information");			//page title
 		
 		
 		
 		//don't touch them 		
		$output->setScriptCalendar('null');
		$output->setScriptTRColor('null');		
		$output->setBody($out);		
  		$output->output(); 
 }
?>