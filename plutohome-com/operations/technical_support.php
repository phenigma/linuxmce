<?
function technical_support($output)
{
 
$out='';
$out.='<table width="100%" border="0" cellpadding="0" cellspacing="0" class="maintable"><td width="30%" valign="top" align="center" >
     <table border="0" align="left" cellpadding="0" cellspacing="0">
     <tr>
    <td class="titletable"><img src="images/submenus/support_txt.gif" width="60" height="13"></td>
    </tr>
       </table></td>
      <tr><td width="30%" valign="top" align="center" class="insidetable">
      <table border="0" align="left" cellpadding="0" cellspacing="0">
      <tr>
        <td width="154"><a href="index.php?section=installation"><img src="images/submenus/Support/installation_information_of.gif" width="207" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=project"><img src="images/submenus/Support/project_management_off.gif" width="207" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=warranty"><img src="images/submenus/Support/warranty_off.gif" width="207" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=download"><img src="images/submenus/Support/download_off.gif" width="207" height="17" border="0"></a></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=compatibility"><img src="images/submenus/Support/compatibility_off.gif" width="207" height="17" border="0"></a></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=technical_support"><img src="images/submenus/Support/technical_support_on.gif" width="207" height="17" border="0"></a></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=faq_presentation"><img src="images/submenus/Support/faqs_off.gif" width="207" height="17" border="0"></a></td>
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
       </tr>
      </td>
      <td width="70%" class="insidetable">
<p>Pluto has many support options.  You can click "Live Chat" at the top of the page for instant help.  
Our support web site also has documentation, forums and bug reports both for Pluto in general, and for each of Pluto\'s modules.  
You may want to try these useful links to pages in our support site:<br>
      <br><a href="/support/index.php?section=document&docID=1&package=0">Main Support Page</a>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href="/support/index.php?section=document&docID=11&package=0">Installation Section</a>
      <br><a href="/support/index.php?section=document&docID=14&package=0">User\'s manual</a>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href="/support/index.php?section=document&docID=15&package=0">Introduction for programmers and developers</a>
</p>      
<br>
You can also use the form below to contact tech support, or call (877) pluto-4-u.
<br>
For sales or general inquiries, <a href="index.php?section=contact">click here</a>.

<br><br>
';
	include('operations/contactForm.php');
	$out.=contactForm('technical_support');  
	
	$output->setNavigationMenu(array("Sample page"=>"index.php?section=samplePage"));
	$output->setScriptCalendar('null');
	$output->setScriptTRColor('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME."::Technical support");
	$output->setPageID(4);
	$output->output();
}
?>
