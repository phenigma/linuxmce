<?php
 function programGuidelines($output) {
		$out = '';
		
$out.='
<table width="100%" border="0" cellpadding="0" cellspacing="0" class="maintable">
<tr>
	<td class="titletable" align="center"><B>Pluto Home Pluto Authorized Dealer Program Guidelines</B></td>
</tr>
<tr>
  <td width="100%" class="insidetable" align="left">
	<div style="position:relative; top=0px; float:left; width=250px;">
	<table width="230" border="0" cellpadding="0" cellspacing="0">
		<tr>
	    	<td valign="top" class="normaltext"><a href="index.php?section=dealerProgram">Overview</a></td>
	  	</tr>
		<tr>
	    	<td valign="top" class="normaltext"><a href="index.php?section=programGuidelines">Program Guidelines</a></td>
	  	</tr>
		<tr>
	    	<td valign="top" class="normaltext"><a href="index.php?section=schedule&sch=1">Schedule 1</a></td>
	  	</tr>
		<tr>
	    	<td valign="top" class="normaltext"><a href="index.php?section=schedule&sch=2">Schedule 2</a></td>
	  	</tr>
		<tr>
	    	<td valign="top" class="normaltext"><a href="index.php?section=schedule&sch=3">Schedule 3</a></td>
	  	</tr>
		<tr>
	    	<td valign="top" class="normaltext"><a href="index.php?section=fundRequestApplication">Funds Request Form</a></td>
	  	</tr>
		<tr>
	    	<td valign="top" class="normaltext"><a href="index.php?section=reimbursementClaimForm">Reimbursement Claim Form</a></td>
	  	</tr>
		<tr>
	    	<td valign="top" class="normaltext"><a href="index.php?section=dealerTerms">Terms and conditions</a></td>
	  	</tr>
		<tr>
	    	<td valign="top" class="normaltext"><a href="index.php?section=dealerApplication">Application form</a></td>
	  	</tr>
	    <tr>
	        <td class="normaltext" height="10">&nbsp;</td>
	    </tr>
	</table>
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

	$output->setTitle(APPLICATION_NAME."::Dealer Program Guidelines");			//page title



	//don't touch them
	$output->setScriptCalendar('null');
	$output->setScriptTRColor('null');
	$output->setBody($out);
	$output->output();
 }
?>