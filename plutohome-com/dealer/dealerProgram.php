<?
function dealerProgram($output) {
	/* @var $dbADO ADOConnection */
	

		
	$out = '
	<table>
		<tr>
			<td align="center" class="titletable"><B>Welcome to the Pluto Authorized Dealer Program</B></td>
		</tr>
		<tr>
			<td class="insidetable">
			<div style="position:relative; top=0px; float:left; width=200px;">
			<table width="180" border="0" cellpadding="0" cellspacing="0">
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
	
				<p>Pluto Home is extending the opportunity to apply to become an Authorized Dealer. As part of the qualification process, Pluto Home requires all potential Dealers to submit an <a href="index.php?section=dealerApplication">online application</a> and accept its Program Terms and Guidelines.
				<br><br><br>
				<p>The benefits of becoming a Pluto Authorized Dealer include:
				<ul>
					<li>The ability to procure and sell Pluto products from Pluto Home and Pluto’s partners</li>
					<li>Very generous marketing reimbursements up to 1/3 of your purchases from Pluto Home</li>
					<li>Referrals to potential customers from Pluto Corporate</li>
					<li>Technical training on the Pluto platform</li>
					<li>Access to the Pluto brand/image gallery and other promotional resources</li>
					<li>Information on upcoming Pluto products as available</li>
				</ul><br>
				Pluto authorizes 2 levels of dealers: “Standard” and “Pro”.  The requirements for “Standard” dealers are typical of any retail dealer.  However, the “Pro” dealers are expected to have the capability to sell, install and support a complete, turnkey, custom-installed smart home solution including security, home automation, and hi-fi.  “Pro” dealers must have either in-house capabilities or working relationships with electricians, contractors etc. to provide a complete solution.  All dealers can sell Pluto’s standard product line, however only the “Pro” dealers are authorized to sell the Pluto’s premium line, “Pluto Pro”.
				<br><br>
				The requirements for “Pro” dealers are far greater than for “Standard” dealers.  The documents that follow will designate which requirements apply only to “Pro” dealers.
				<br>
				For additional information or if you have any questions regarding the Pluto Authorized Dealer Program, please e-mail us at: <a href="mailto:dealers@plutohome.com">dealers@plutohome.com</a>.
			</td>
		</tr>
	</table>
';



	$output->setScriptCalendar('null');
	$output->setScriptTRColor('null');
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME."::Client");
	$output->output();
}
?>