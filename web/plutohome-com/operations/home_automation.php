<?php
function home_automation($output) {
	$out = '
<table width="100%"  border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td align="center" class="insidetable"><table width="563"  border="0" cellpadding="0" cellspacing="0">
      <tr>
        <td width="72"><a href="index.php?section=security"><img src="images/buttons/security_off.gif" width="72" height="11" border="0" /></a></td>
        <td width="70"><a href="index.php?section=telecom"><img src="images/buttons/telecom_off.gif" width="70" height="11" border="0" /></a></td>
        <td width="124"><a href="index.php?section=home_automation"><img src="images/buttons/home_automation_on.gif" width="124" height="11" border="0" /></a></td>
        <td width="155"><a href="index.php?section=media_entertainment"><img src="images/buttons/media_entertainment_off.gif" width="155" height="11" border="0" /></a></td>
        <td width="132"><a href="index.php?section=personal_computing"><img src="images/buttons/personal_computing_off.gif" width="132" height="11" border="0" /></a></td>
      </tr>
    </table></td>
  </tr>
  <tr>
    <td valign="top" background="images/back_insidetable.jpg" class="insidetable"><table width="100%"  border="0" cellpadding="0" cellspacing="0">
      <tr>
        <td width="89" valign="top"><img src="images/thumbs/img_home_automation.jpg" width="89" height="89" /></td>
        <td width="10"><img src="images/spacer.gif" width="10" height="10" /></td>
        <td valign="top"><table width="100%"  border="0" cellspacing="0" cellpadding="0">
            <tr>
              <td><img src="images/titles/home_automation.gif" width="200" height="13" /></td>
            </tr>
            <tr>
              <td><br />
            Your mobile phone turns into a remote control for lights, climate, etc.<br />
            Effortless, &quot;follow-me automatically&quot; automation<br />
            Self-configuring and maintaining. NO programming or tech skills needed<br /></td>
            </tr>
            <tr>
              <td align="right">&nbsp;</td>
            </tr>
        </table></td>
      </tr>
    </table></td>
  </tr>
  <tr>
    <td valign="top" class="insidetable2">
      <p><img src="images/bullet_1.gif" width="11" height="11" align="absmiddle" /> Home automation systems are usually controlled by a slew of expensive touch-screen tablets needed everywhere you want control. Only Pluto turns your mobile phone into a comfortable remote, making the tablets optional.<br />
        <br />
        <img src="images/bullet_1.gif" width="11" height="11" align="absmiddle" /> Your mobile phone not only becomes more comfortable and useable than any type of remote control on the market, it works anywhere in the world you have cellular data coverage. Control everything, monitor your cameras, accept deliveries, etc. It doesn\'t matter if you\'re in the living room or in Lithuania. When you\'re at home, the mobile uses Bluetooth to communicate. When you\'re away, it automatically switches to the cellular network.<br />
        <br />
        <img src="images/bullet_1.gif" width="11" height="11" align="absmiddle" /> Pluto can recognize each family member and automatically set up the room according to his preferences. Just keep your mobile phone in your pocket. It compares the relative strengths throughout the house to calculate exactly where each family member is. It works for your lighting, climate, media, as well as security settings.<br />
        <br />
        <img src="images/bullet_1.gif" width="11" height="11" align="absmiddle" /> With Pluto, everything is plug-and-play, self-configuring, and no programmer will ever come to your home. All you do is go to a simple web site, and answer multiple-choice questions about your lifestyle.<br />
        <br />
        <img src="images/bullet_1.gif" width="11" height="11" align="absmiddle" /> With Pluto, you get zero-touch automation. If you have a DVD you want to watch, just put the disc in the nearest media director. Pluto will analyze what type of disc it is, and setup all your TVs and stereo equipment automatically. Pluto will even feed the DVD Menu directly to the nearest mobile phone or Orbiter. You don\'t have to touch a single button. </p>
    </td>
  </tr>
</table>
	';



	$output->setPressedButton(1);
	$output->setNavigationMenu(array('What is Pluto'=>'index.php?section=what_is_pluto','Home automation'=>'index.php?section=home_automation'));
	//don't touch them
	$output->setScriptCalendar('null');
	$output->setScriptTRColor('null');
	$output->setBody($out);
	$output->output();
}
?>
