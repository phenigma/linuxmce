<?php
 function security($output) {
		$out = '';
		
$out.=<<<TEXT

<table width="100%" border="0" cellpadding="0" cellspacing="0" class="maintable">
  <tr>
    <td class="titletable"><img src="images/submenus/pluto_benefits_txt.gif" width="106" height="13"></td>
  </tr>
  <tr>
    <td class="insidetable"><table width="186" border="0" align="left" cellpadding="0" cellspacing="0">
      <tr>
        <td><a href="index.php?section=security"><img src="images/submenus/Pluto%20benefits/security_on.gif" width="166" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=telecom"><img src="images/submenus/Pluto%20benefits/telecom_off.gif" width="166" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=home_automation"><img src="images/submenus/Pluto%20benefits/home_automation_off.gif" width="166" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=media_entertainment"><img src="images/submenus/Pluto%20benefits/media_entertainment_off.gif" width="166" height="17" border="0"></a></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=personal_computing"><img src="images/submenus/Pluto%20benefits/personal_computing_off.gif" width="166" height="17" border="0"></a></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=benefits_comparison"><img src="images/submenus/Pluto%20benefits/benefits_comparison_off.gif" width="166" height="17" border="0"></a></td>
        <td>&nbsp;</td>
      </tr>
      <tr>
        <td>&nbsp;</td>
        <td>&nbsp;</td>
      </tr>
    </table>
      <p><img src="images/submenus/security_txt.gif" width="63" height="13"><br>
      Protect your home and your family as never before possible</p>
      <p>&bull;<a href="#notification"><strong> Immediate notification of any alerts on your mobile phone with live video</strong></a> <br>
      &bull; <strong><a href="#arm">Auto arm/disarming of the alarm--saves you hassle and worry</a><br>
      &bull;<a href="#mark"> <strong>Mark rooms as off-limits to anyone but authorized persons</strong> </a><br>
      &bull; <strong><a href="#baby">Special baby sitter mode to protect your children</a></strong> <br>
</strong></strong></p>
      <p>&bull;<strong> <a name="notification"></a>Immediate notification of any alerts on your mobile phone with live video (patent pending)</strong><br>
      The instant anything important happens at your home, such as a security breach, fire or even someone ringing the doorbell, your mobile phone will beep and show you a live video feed. You are in full control. 1 button turns on the lights, tv's and stereo's, turning the whole house into a giant speakerphone so you can speak to the person. If it's a delivery, you can unlock the door, let him in, tell him where to leave the package, and lock up again when he's done. If it's an intruder, you can let him know you're watching him as you call the police.  Pluto makes a 911 call from the house phone--patching you in on the cell--so the police see a real emergency, not another monitored alarm call.</p>
      <p>&bull; <strong><a name="arm" id="arm"></a>Auto arm/disarming of the alarm--saves you hassle and worry (patent pending)</strong><br>
      Because Pluto is able to distinctly recognize each family member and determine who is coming and going, Pluto can automatically turn the alarm off and unlock the door when it detects you approaching the house, and automatically lock and arm when everyone has left.  You don't have to worry about forgetting to turn the alarm on.</p>
      <p>&bull; <strong><a name="mark" id="mark"></a>Mark rooms as off-limits to anyone but authorized persons (patent pending)</strong><br>
      Tell Pluto that only certain people are allows in certain rooms. When Pluto detects the presence of an authorized person it will automatically shut off the alarm sensors in that room.  It\'s fully automatic--come and go as you please.  If an unauthorized person enters, you will be notified on your mobile phone.</p>
      <p>&bull; <strong><a name="baby" id="baby"></a>Special baby sitter mode to protect your children (patent pending)</strong><br>
      Just tell Pluto if you leave the children at home with a baby sitter. Pluto will notify you whenever someone comes and goes from the house, and show you a picture so you can be sure there are no unauthorized visitors. Of course you can also push a button on your phone to turn on the surveillance cameras and check what's happening at any time. If you wish, Pluto can even automatically keep a video archive of everything that happens in your home. You just need to tell Pluto when to do it (all the time, only when you're away, and so on), and what cameras you want monitored.</p>
      <p>Pluto security is like nothing you have seen before.<br>
    Click <a href="javascript:open_popup('operations/security.html#security_features','500','400')">here</a> to view the security features sheet.</p></td>
  </tr>
</table>

TEXT;
		
		$output->setImgName("pic.jpg"); //img that needs to be changed
        $output->setPageID(0); //butonul selectat
 		$output->setNavigationMenu(array("Sample page"=>"index.php?section=samplePage")); //not used yet
 		$output->setTitle(APPLICATION_NAME."::Security");			//page title
 		
 		
 		
 		//don't touch them 		
		$output->setScriptCalendar('null');
		$output->setScriptTRColor('null');		
		$output->setBody($out);		
  		$output->output(); 
 }
?>
