<?
function contact($output)
{
$out='';
$out.='<table width="100%" border="0" cellpadding="0" cellspacing="0" class="maintable"><td  valign="top" align="center" >
     <table border="0" align="left" cellpadding="0" cellspacing="0">
     <tr>
    <td class="titletable"><img src="images/submenus/company_txt.gif" width="66" height="13"></td>
    </tr>
       </table></td>
      <tr><td  valign="top" align="center" class="insidetable">
     <table border="0" align="left" cellpadding="0" cellspacing="0">
      <tr>
        <td width="139"><a href="index.php?section=company"><img src="images/submenus/Company/company_overview_off.gif" width="139" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=contact"><img src="images/submenus/Company/contact_on.gif" width="139" height="17" border="0"></a></td>
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
       </tr>
      </td>
      <td width="100%" class="insidetable">
       <img src="images/submenus/contact_txt.gif" width="62" height="13"><br><p>
	Our administrative office is open Monday-Friday, 8am-5pm EST, at the location below.<br><br>
	For technical support, please <a href="index.php?section=technical_support">click here</a>.<br>
	Tech support is available 24 hours a day, 7 days a week.  Sales and general inquiries will be answered within 1 business day.<br><br>
	Our mailing address:<br>
	Pluto<br>
	2699 Stirling Road, Building C<br>
	Fort Lauderdale, FL 33312<br>
	(877) pluto-4-u<br><br>
	For sales or general inquiries, please submit your message here:
</p>

';
	include('operations/contactForm.php');
	$out.=contactForm('contact');

	$output->setScriptCalendar('null');
	$output->setPageID(6); //butonul selectat
    $output->setScriptTRColor('null');		
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME."::Contact");				
  	$output->output();
}
