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
<p>Corporate office:<br>
                  <span class="title">Pluto<br>
                  2699 Stirling Road, Building C<br>
                  Fort Lauderdale, FL 33312<br>
                  Phone:</span> <B>(877) pluto-4-u</B><br>
                  <span class="title">Office house (EST):</span> <B>Mon-Fri 8:30am &ndash; 9:00pm Sat: 10:00am &ndash; 3:00pm</B></p>
                  
      <p> Pluto is a privately held company, founded in 2002 in Fort Lauderdale, Florida with the singular mission of offering a product that was more of a lifestyle than a technology. Pluto is a destination that allows our customers to focus on the important things in life, knowing that the myriads of devices and electronic gadgets we have come to rely on work together seamlessly, intuitively.</p>
      <p>A product this unique could only be developed by a team equally unique. Pluto recruited the best engineers around for its development team. But a product as ambitious as Pluto demanded even more. So Pluto heartily embraced the open source community and found the premier engineers from a variety of open source projects from all over the world. They too were brought to Florida to work side-by-side with our in-house teams, bringing with them years of experience and high-quality projects that were incorporated into the Pluto product.</p></td>
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
