<?php
 function company($output) {
		$out = '';
		
$out.=<<<TEXT

<table width="100%"  border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td align="left" class="insidetable"><table width="438"  border="0" cellpadding="0" cellspacing="0">
      <tr>
        <td width="155"><img src="images/titles/company.gif" /></td>
        </tr>
    </table></td>
  </tr>
  <tr>
    <td class="insidetable2">
<p>Contact:<br>
                  <span class="title">Pluto<br>
                  20 Island Ave., #1112<br>
				  Miami Beach, FL 33139<br>
                  Phone:</span> <B>(877) pluto-4-u</B><br>
				  </p>
                  
      <p>Pluto is privately held.  Please use the contact form if you are interested in working with us.
	  </p></td>
  </tr>
</table>
	
TEXT;
		

	$output->setNavigationMenu(array('Company'=>'index.php?section=company')); 
	$output->setTitle(APPLICATION_NAME.' :: Company');
	$output->setScriptCalendar('null');
	$output->setScriptTRColor('null');
	$output->setBody($out);
	$output->output();
}
?>
