<?php
function security($output) {
	$out = '
<table width="100%"  border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td align="center" class="insidetable"><table width="563"  border="0" cellpadding="0" cellspacing="0">
      <tr>
        <td width="72"><a href="index.php?section=security"><img src="images/buttons/security_on.gif" width="72" height="11" border="0" /></a></td>
        <td width="70"><a href="index.php?section=telecom"><img src="images/buttons/telecom_off.gif" width="70" height="11" border="0" /></a></td>
        <td width="124"><a href="index.php?section=home_automation"><img src="images/buttons/home_automation_off.gif" width="124" height="11" border="0" /></a></td>
        <td width="155"><a href="index.php?section=media_entertainment"><img src="images/buttons/media_entertainment_off.gif" width="155" height="11" border="0" /></a></td>
        <td width="132"><a href="index.php?section=personal_computing"><img src="images/buttons/personal_computing_off.gif" width="132" height="11" border="0" /></a></td>
      </tr>
    </table></td>
  </tr>
  <tr>
    <td valign="top" background="images/back_insidetable.jpg" class="insidetable"><table width="100%"  border="0" cellpadding="0" cellspacing="0">
      <tr>
        <td width="89" valign="top"><img src="images/thumbs/img_security.jpg" width="89" height="89" /></td>
        <td width="10"><img src="images/spacer.gif" width="10" height="10" /></td>
        <td valign="top"><table width="100%"  border="0" cellspacing="0" cellpadding="0">
            <tr>
              <td><img src="images/titles/security.gif" width="200" height="13" /></td>
            </tr>
            <tr>
              <td><br />
                Immediate notification of any alerts on your mobile phone with live video<br />
Auto arming/disarming of the alarm<br />
Mark rooms as off-limits to anyone but authorized persons<br /></td>
            </tr>
            <tr>
              <td align="right">&nbsp;</td>
            </tr>
          </table>
          </td>
      </tr>
    </table></td>
  </tr>
  <tr>
    <td valign="top" class="insidetable2">
      <p><img src="images/bullet_1.gif" width="11" height="11" align="absmiddle" /> The instant anything important happens at your home, such as a security breach, fire or even someone ringing the doorbell, your mobile phone will beep and show you a live video feed.<br />
        <br />
        <img src="images/bullet_1.gif" width="11" height="11" align="absmiddle" /> Because Pluto is able to distinctly recognize each family member and determine who is coming and going, Pluto can automatically turn the alarm off and unlock the door when it detects you approaching the house, and automatically lock and arm when everyone has left. You don\'t have to worry about forgetting to turn the alarm on.          <br />
        <br />
        <img src="images/bullet_1.gif" width="11" height="11" align="absmiddle" /> Tell Pluto that only certain people are allowed in certain rooms. When Pluto detects the presence of an authorized person it will automatically shut off the alarm sensors in that room. It\'s fully automatic. If an unauthorized person enters, you will be notified on your mobile phone.
          <br />
          <br />
        <img src="images/bullet_1.gif" width="11" height="11" align="absmiddle" /> Tell Pluto if you leave the children at home with a baby sitter. Pluto will notify you whenever someone comes and goes from the house, and show you a picture so you can be sure there are no unauthorized visitors. You can push a button on your phone to turn on the surveillance cameras and check what\'s happening at any time.</p>
    </td>
  </tr>
</table>
	';
	$output->setPressedButton(1);
	$output->setNavigationMenu(array('What is Pluto'=>'index.php?section=what_is_pluto','Security'=>'index.php?section=security'));
	$output->setTitle(APPLICATION_NAME."::Security");			//page title
	$output->setScriptCalendar('null');
	$output->setScriptTRColor('null');
	$output->setBody($out);
	$output->output();
}
?>
