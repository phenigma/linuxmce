<?php
 function locator($output,$conn) {
		$out = '';
		
$out.='

<table width="100%"  border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td align="center" class="insidetable"><table width="563"  border="0" cellpadding="0" cellspacing="0">
      <tr>
        <td align="left"><img src="images/titles/dealer_locator.gif"></td>
      </tr>
    </table></td>
  </tr>
  <tr>
    <td align="left" class="insidetable2">
      <p>Pluto has 2 types of dealers: Standard and Pro.  Both provide the complete Pluto system seen here.  
      But Pluto Pro dealers require special certification and must also sell compatible lighting control systems, security, climate and pool controls. 
      Most also sell high-end audio/video equipment, and work with contractors to offer built-in solutions, such as in-wall Orbiters. 
      Your Pluto Pro dealer offers a complete, custom installed, turn-key solution.  Also, only Pro dealers can sell Pluto Pro: our high-end line, made from the finest materials, available in a variety of finishes and colors to complement any d&eacute;cor.</p>
    
      <p>Pluto Series 2 is expected to be released for retail in March, 2005.  We are now signing up dealers.  The dealer locator service is available from February.</p>
      <p>If you wish to be contacted by a dealer in your area, you can complete the form below.</p>
	</td>
  </tr>
</table>
';
	include('operations/contactForm.php');
	$out.=contactForm('locator',$conn);

	$output->setPressedButton(5);
	$output->setNavigationMenu(array('Get Pluto'=>'index.php?section=get_pluto','Dealer locator'=>'index.php?section=locator'));
	$output->setTitle(APPLICATION_NAME." :: Dealer locator");			
	$output->setBody($out);
	$output->output();
 }
?>
