<?php
 function programGuidelines($output) {
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
<tr class="insidetable2">
	<td class="title" align="center"><B>Pluto Home Pluto Authorized Dealer Program Guidelines</B></td>
</tr>
<tr>
  <td width="100%" class="insidetable2" align="left">
	<div style="position:relative; top=0px; float:left; width=250px;">
	';
		include('dealer/dealerMenu.php');
		$out.=dealerMenu();
		$out.='
	</div>

The requirements and benefits associated with the Pluto Home Pluto Authorized Dealer Program Guidelines (the “Guidelines”) are detailed herein. Pluto Home has the right to change, modify or discontinue these Guidelines in whole or in part at any time without notice. These Guidelines include Schedules 1-3 attached hereto, and are subject to the Pluto Home Pluto Authorized Dealer Program Terms (<a href="index.php?section=dealerTerms">the “Terms”</a>).
<br><br>
These Guidelines include the following Schedules, each of which is incorporated into and made an integral part of these Guidelines:
<ul>
	<li><a href="index.php?section=schedule&sch=1">Schedule 1</a> – Program Requirements</li>
	<li><a href="index.php?section=schedule&sch=2">Schedule 2</a> – Pluto Platform Marketing Program</li>
	For Pluto Pro dealers only:
	<li><a href="index.php?section=schedule&sch=3">Schedule 3</a> – Dealer Support roles & Service Responsibilities</li>
<ul>
  </td>
</tr>
</table>

     
';
	$output->setPressedButton(5);
	$output->setNavigationMenu(array('Get Pluto'=>'index.php?section=get_pluto','I\'m a dealer'=>'index.php?section=dealer','Dealer Program'=>'index.php?section=dealerProgram','Program Guidelines'=>'index.php?section=programGuidelines'));
	$output->setTitle(APPLICATION_NAME." :: Dealer Program Guidelines");			
	$output->setBody($out);
	$output->output();
 }
?>