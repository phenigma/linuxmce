<?php
 function simplicity($output) {
		$out = '';
		
$out.=<<<TEXT

   <table width="100%" border="0" cellpadding="0" cellspacing="0" class="maintable">
  <tr>
    <td class="titletable"><img src="images/submenus/why_pluto_txt.gif" width="76" height="13"></td>
  </tr>
  <tr>
    <td class="insidetable"><table border="0" align="left" cellpadding="0" cellspacing="0">
      <tr>
        <td width="154"><a href="index.php?section=simplicity"><img src="images/submenus/Why%20Pluto/simplicity_on.gif" width="278" height="17" border="0"></a></td>
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
        <td><a href="index.php?section=cost"><img src="images/submenus/Why%20Pluto/cost_off.gif" width="278" height="17" border="0"></a></td>
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
      <p><img src="images/submenus/simplicity_txt.gif" width="191" height="13"></p>
      <p>Enjoy 1 common, universal way to control everything in your house. Whether you want to turn on the sprinklers, the lights, or the stereo, it is done exactly the same way, using either a touch-screen tablet, a PDA, a web page, or easiest of all, your mobile phone--and the mobile phone controls your home from anywhere in the world.</p>
      <p>The greatest convenience, though, comes from Pluto's ability to accurately and instantly recognize each family member personally, know whether he is in the home or not, and what room he is in. So, without doing a thing, when you enter a room Pluto can set the lights just the way you like, adjust the climate to your favorite setting, direct whatever media you were enjoying (tv, music, etc.) to follow you, route your phone calls to the extension nearest you, and automatically bypass the alarm in that room. It's a stable, worry-free system that you can effortlessly customize to your tastes, and turn on or off just by pressing 1 button on your mobile phone. This is the ultimate convenience. Before Pluto's new, patent-pending technology, a handful of super-rich individuals spent millions on custom systems to enjoy this luxury. And now it's a standard feature on all Pluto systems.</p>
      </td>
  </tr>
</table>
	
TEXT;
		
		$output->setImgName("support.jpg"); //img that needs to be changed
        $output->setPageID(3); //butonul selectat
 		$output->setNavigationMenu(array("Sample page"=>"index.php?section=samplePage")); //not used yet
 		$output->setTitle(APPLICATION_NAME."::Simplicity");			//page title
 		
 		
 		
 		//don't touch them 		
		$output->setScriptCalendar('null');
		$output->setScriptTRColor('null');		
		$output->setBody($out);		
  		$output->output(); 
 }
?>