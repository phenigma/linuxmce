<?php
 function cost($output) {
		$out = '';
		
$out.=<<<TEXT

 <table width="100%" border="0" cellpadding="0" cellspacing="0" class="maintable">
  <tr>
    <td class="titletable"><img src="images/submenus/why_pluto_txt.gif" width="76" height="13"></td>
  </tr>
  <tr>
    <td class="insidetable"><table border="0" align="left" cellpadding="0" cellspacing="0">
      <tr>
        <td width="154"><a href="index.php?section=simplicity"><img src="images/submenus/Why%20Pluto/simplicity_off.gif" width="278" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
      </tr>
      <tr>
        <td><a href="index.php?section=benefits"><img src="images/submenus/Why%20Pluto/benefits_off.gif" width="278" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
      </tr>
      <tr>
        <td><a href="index.php?section=reliability"><img src="images/submenus/Why%20Pluto/reliability_off.gif" width="278" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
      </tr>
      <tr>
        <td><a href="index.php?section=cost"><img src="images/submenus/Why%20Pluto/cost_on.gif" width="278" height="17" border="0"></a></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="../images/spacer.gif" width="3" height="3"></td>
      </tr>
      <tr>
        <td><a href="index.php?section=future"><img src="images/submenus/Why%20Pluto/future_proof_off.gif" width="278" height="17" border="0"></a></td>
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
    <p><img src="images/submenus/cost_txt.gif" width="35" height="13"><br><br>
      The luxury and convenience offered by Pluto has been reserved for the ultra-rich. It's been widely publicized that a handful of billionaires spent millions just to have follow-me convenience like Pluto.  Pluto is the first mass-produced product that makes this available to the general public.  You would likely spend more on the mish-mash of stand-alone pieces currently on the market.</p>
      <p>Try to buy a decent security, phone, and home automation system, plus media + entertainment gear, and a computer for that price. It's impossible.<p></p> And no product, at any price, rivals the convenience and elegance of Pluto.</p>
      <p>Also, your Pluto system will likely run on your existing phone cable (cat 5), turning the same cable into both a voice-over-ip phone system plus a high-speed computer/internet network. You can plug your laptop into the back of any wall jack or Pluto phone to get high-speed internet, file sharing, printer sharing, etc. Normally, you would have to pay for this extra wiring--possibly even more than the cost of a complete Pluto system--but with Pluto it's an added &quot;bonus&quot;.<br>
      </p>
    </td>
  </tr>
</table>
	
TEXT;
		
		$output->setImgName("support.jpg"); //img that needs to be changed
        $output->setPageID(3); //butonul selectat
 		$output->setNavigationMenu(array("Sample page"=>"index.php?section=samplePage")); //not used yet
 		$output->setTitle(APPLICATION_NAME."::Cost");			//page title
 		
 		
 		
 		//don't touch them 		
		$output->setScriptCalendar('null');
		$output->setScriptTRColor('null');		
		$output->setBody($out);		
  		$output->output(); 
 }
?>