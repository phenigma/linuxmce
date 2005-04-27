<?php
 function dealer($output,$conn) {
		$out = '';
		
$out.='
<table width="100%"  border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td align="center" class="insidetable"><table width="563"  border="0" cellpadding="0" cellspacing="0">
      <tr>
        <td align="left"><img src="images/titles/dealer.gif"></td>
      </tr>
    </table></td>
  </tr>
  <tr>
    <td align="left" class="insidetable2">
      <p>If you are interested in becoming a Pluto dealer, please fill out the form below and a rep will contact you shortly.</p>

	</td>
  </tr>
</table>
   
';
	include('operations/contactForm.php');
	$out.=contactForm('dealer',$conn);

	$output->setPressedButton(5);
	$output->setNavigationMenu(array('Get Pluto'=>'index.php?section=get_pluto','I\'m a dealer'=>'index.php?section=dealer'));
	$output->setTitle(APPLICATION_NAME." :: I'm a Dealer");			
	$output->setBody($out);
	$output->output();
 }
?>
