<?php
 function future($output) {
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
        <td><a href="index.php?section=reliability"><img src="images/submenus/Why%20Pluto/reliability_off.gif" width="278" height="17" border="0"></a></td>
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
        <td><a href="index.php?section=future"><img src="images/submenus/Why%20Pluto/future_proof_on.gif" width="278" height="17" border="0"></a></td>
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
    <p><img src="images/submenus/future_proof_txt.gif" width="310" height="13"><br>
      We are constantly improving Pluto, always adding more benefits and features.  The Core--the brains behind Pluto--is a self-upgrading, self-configuring, Linux server.  When a new feature becomes available, your Pluto Core will automatically update itself--at no charge.</p>
      <p>Further, with Pluto, every single device is IP-based and runs on a standard Ethernet network over normal cat 5 cabling--which is already in most recent homes. You can plug just about any IP-based device into the network. The Pluto Core uses a special technology that allows it to automatically detect new devices that you plug in, automatically install any drivers it needs to work with them, configure them, and use them. Imagine the convenience of being able to buy an IP based camera, telephone, security sensor, audio player, etc., and just plug it in, and without doing a single thing, it is instantly, seamlessly merged into the Pluto network and fully controllable from your tablet or mobile phone.</p>
      <p>This is another first that you can only find on Pluto.</p>
    </td>
  </tr>
</table>
	
TEXT;
		
		$output->setImgName("support.jpg"); //img that needs to be changed
        $output->setPageID(3); //butonul selectat
 		$output->setNavigationMenu(array("Sample page"=>"index.php?section=samplePage")); //not used yet
 		$output->setTitle(APPLICATION_NAME."::Future Proof");			//page title
 		
 		
 		
 		//don't touch them 		
		$output->setScriptCalendar('null');
		$output->setScriptTRColor('null');		
		$output->setBody($out);		
  		$output->output(); 
 }
?>