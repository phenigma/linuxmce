<?php
 function reliability($output) {
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
        <td colspan="2"><img src="images/spacer.gif" width="3" height="3"></td>
      </tr>
      <tr>
        <td><a href="index.php?section=benefits"><img src="images/submenus/Why%20Pluto/benefits_off.gif" width="278" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="images/spacer.gif" width="3" height="3"></td>
      </tr>
      <tr>
        <td><a href="index.php?section=reliability"><img src="images/submenus/Why%20Pluto/reliability_on.gif" width="278" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="images/spacer.gif" width="3" height="3"></td>
      </tr>
      <tr>
        <td><a href="index.php?section=cost"><img src="images/submenus/Why%20Pluto/cost_off.gif" width="278" height="17" border="0"></a></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="images/spacer.gif" width="3" height="3"></td>
      </tr>
      <tr>
        <td><a href="index.php?section=future"><img src="images/submenus/Why%20Pluto/future_proof_off.gif" width="278" height="17" border="0"></a></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="images/spacer.gif" width="3" height="3"></td>
      </tr>
      <tr>
        <td>&nbsp;</td>
        <td>&nbsp;</td>
      </tr>
    </table>
    <p><img src="images/submenus/reliability_txt.gif" width="78" height="13"><br>
      The more 'stuff' there is, the more stuff there is to break.</p>
      <p>Without Pluto you need a hodge-podge of assorted products, and probably none of them were built to be commercial-grade and fully redundant. It would just be prohibitive, for example, to build, say an answering machine, with backup power and storage and redundant components.</p>
      <p>However, with Pluto, everything is handled by a commercial-grade Linux-based server (the Core), which has redundancy, backup everything, and is built to last a lifetime. True, most homes would normally never consider installing such a high-end piece of equipment. But, since this one device handles everything, it becomes justifiable. It's much better to invest in 1 high-end server that is built to last and has a guaranteed response time of 24 hours in the event of any failure, as opposed to buying dozens of low-cost, stand-alone components that are only made to last a couple years.<br>
      </p></td>
  </tr>
</table>
	
TEXT;
		
		$output->setImgName("support.jpg"); //img that needs to be changed
        $output->setPageID(3); //butonul selectat
 		$output->setNavigationMenu(array("Sample page"=>"index.php?section=samplePage")); //not used yet
 		$output->setTitle(APPLICATION_NAME."::Reliability");			//page title
 		
 		
 		
 		//don't touch them 		
		$output->setScriptCalendar('null');
		$output->setScriptTRColor('null');		
		$output->setBody($out);		
  		$output->output(); 
 }
?>