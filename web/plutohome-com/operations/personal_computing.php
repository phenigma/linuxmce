<?php
function personal_computing($output) {
	$out = '
<table width="100%"  border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td align="center" class="insidetable"><table width="563"  border="0" cellpadding="0" cellspacing="0">
      <tr>
        <td width="72"><a href="index.php?section=security"><img src="images/buttons/security_off.gif" width="72" height="11" border="0" /></a></td>
        <td width="70"><a href="index.php?section=telecom"><img src="images/buttons/telecom_off.gif" width="70" height="11" border="0" /></a></td>
        <td width="124"><a href="index.php?section=home_automation"><img src="images/buttons/home_automation_off.gif" width="124" height="11" border="0" /></a></td>
        <td width="155"><a href="index.php?section=media_entertainment"><img src="images/buttons/media_entertainment_off.gif" width="155" height="11" border="0" /></a></td>
        <td width="132"><a href="index.php?section=personal_computing"><img src="images/buttons/personal_computing_on.gif" width="132" height="11" border="0" /></a></td>
        </tr>
    </table></td>
  </tr>
  <tr>
    <td valign="top" background="images/back_insidetable.jpg" class="insidetable"><table width="100%"  border="0" cellpadding="0" cellspacing="0">
      <tr>
        <td width="89" valign="top"><img src="images/thumbs/img_computing.jpg" width="89" height="89" /></td>
        <td width="10"><img src="images/spacer.gif" width="10" height="10" /></td>
        <td valign="top"><table width="100%"  border="0" cellspacing="0" cellpadding="0">
            <tr>
              <td><img src="images/titles/personal_computing.gif" width="200" height="13" /></td>
            </tr>
            <tr>
              <td><br />
            Perform basic computing without a mouse or keyboard<br />
            Powerful computing platform for advanced users <br />
            Automatically joins multiple homes together as one network <br /></td>
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
      <p><img src="images/bullet_1.gif" width="11" height="11" align="absmiddle" /> Every Pluto Media Director has a full computer built-in, and comes with a wireless keyboard &amp; mouse so you can run all your favorite software. Today\'s high-def TVs are actually great for computing because of the fantastic picture quality. So, if you want to check the weather or the latest news, just hit a button on your mobile phone and the page will show up on the TV. </p>
      <p><img src="images/bullet_1.gif" width="11" height="11" align="absmiddle" /> If you are a PC power user, imagine having a server running multi Intel Xeon processors and a 10 terabyte (10,000 GB) RAID 5 array. Plus, each Media Director incorporates a high-powered PC inside with the latest hardware, such as a 200GB hard drive, up to an Intel Pentium 4 at 3.06Ghz Processor with 2 GB RAM. And you just press 1 button to switch between Linux and Windows. The Media Director also has video outputs for computer monitors, such as VGA and DVI, and the latest audio/video sub-system for use as a high-end gaming platform.</p>
      <p><img src="images/bullet_1.gif" width="11" height="11" align="absmiddle" /> When you have multiple homes with Pluto systems, they can all be joined as one. Every Pluto Core will automatically create a VPN (virtual private network) connection to the others. It\'s completely safe and fully encrypted. The benefit is that you can go from one house to another and know that you will still have access to all your files no matter where you are without the hassle of copying them.</p>
      <p><img src="images/bullet_1.gif" width="11" height="11" align="absmiddle" /> The RAID array in the Pluto Core has a capacity of up to 10 TB (that\'s 10,000 GB). Pluto automatically creates a network share for each family member so you can put all your normal computer files there, too. You will never lose your files due to a hard drive crash (the RAID array has built-in redundancy). You can access any of your files anytime, anywhere - Pluto automatically creates a password-protected personal web page for each family member.</p>
    </td>
  </tr>
</table>
	';

	$output->setPressedButton(1);
	$output->setNavigationMenu(array('What is Pluto'=>'index.php?section=what_is_pluto','Personal computing'=>'index.php?section=personal_computing'));
	$output->setTitle(APPLICATION_NAME."::Personal Computing");			//page title
	$output->setScriptCalendar('null');
	$output->setScriptTRColor('null');
	$output->setBody($out);
	$output->output();
}
?>