<?php
 function benefits($output) {
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
        <td><a href="index.php?section=benefits"><img src="images/submenus/Why%20Pluto/benefits_on.gif" width="278" height="17" border="0"></a></td>
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
    <p><img src="images/submenus/benefits_txt.gif" width="264" height="13"><br>
      Pluto brings many significant benefits that are not possible with stand-alone systems. You can explore them using the buttons on the left.</p>
      <p>Consider one very significant benefit: Is anything more important to you than your family and your home? Yet without Pluto, every time you leave your house you are putting your confidence in a monitored alarm system. Police get 32 million calls a year for false monitored alarms, and the average response time nationwide approaches 1 hour!</p>
      <p>With Pluto, the instant anything happens in your home, your mobile phone alerts you with a live video feed--anywhere in the world. 1 button turns on the tv's and stereo's, turning the whole house into a giant speakerphone, so you can speak to the person--live. If it's a delivery, you can let him in and lock up when he goes. But, if it's an intruder, you can let him know you're watching as you call the police. There's even a baby sitter mode to protect your children. What a significant benefit! Only Pluto can offer this because all 5 aspects must be integrated for this to happen. The security sensors, the telecom system, the home automation system which controls the lights and tv's, the media system, which pipes your voice from the cell through the house, and the computer network which archives the video.</p>
    </td>
  </tr>
</table>
	
TEXT;
		
		$output->setImgName("support.jpg"); //img that needs to be changed
        $output->setPageID(3); //butonul selectat
 		$output->setNavigationMenu(array("Sample page"=>"index.php?section=samplePage")); //not used yet
 		$output->setTitle(APPLICATION_NAME."::Benefits");			//page title
 		
 		
 		
 		//don't touch them 		
		$output->setScriptCalendar('null');
		$output->setScriptTRColor('null');		
		$output->setBody($out);		
  		$output->output(); 
 }
?>