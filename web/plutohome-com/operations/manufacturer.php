<?php
 function manufacturer($output,$conn) {
$out = '';
		
$out.='
<table width="100%"  border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td align="center" class="insidetable"><table width="563"  border="0" cellpadding="0" cellspacing="0">
      <tr>
        <td align="left"><img src="images/titles/manufacturer.gif"></td>
      </tr>
    </table></td>
  </tr>
  <tr>
    <p>If you are interested in working with Pluto, please complete the following inquiry form and a sales rep will contact you promptly:</p>

	</td>
  </tr>
</table>
';
	include('operations/contactForm.php');
	$out.=contactForm('manufacturer',$conn);	
		
	$output->setPressedButton(5);
	$output->setNavigationMenu(array('Get Pluto'=>'index.php?section=get_pluto','I\'m a manufacturer'=>'index.php?section=manufacturer'));
	$output->setTitle(APPLICATION_NAME." :: I'm a Manufacturer");		
	$output->setBody($out);
	$output->output();
 }
?>
