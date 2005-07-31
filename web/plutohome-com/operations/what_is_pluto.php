<?
function what_is_pluto($output,$conn){
	$out = '
<table width="100%"  border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td class="insidetable"><img src="images/titles/what_is_pluto.gif" width="200" height="13" /></td>
  </tr>
  <tr>
		<td>
	<table cellpadding="0" cellspacing="0" class="insidetable" width="100%">
	<tr>
		<td><IMG SRC="images/img_what_is_pluto.jpg"></td>
		<td style="padding-left: 8px;">Pluto is the only all-in-one solution for your home that seamlessly combines media &amp; entertainment, home automation, security, telecom and computing. <br />
      You can control your whole house with a mobile phone, a touch-screen tablet or a web-interface. A Pluto system is like an appliance - not a computer. It is self-configuring, maintaining and updating. No technical skills are required to use or install Pluto.<br />
      Pluto is above all simple. The devices are all plug and play. Pluto is also an open platform, offering unlimited expansion potential, and requiring no special cabling.<br />
    This is Pluto: a complete, comfortable and secure solution for your home. <br />
	See also: <a href="http://plutohome.org">Open source/techie page</a> and <a href="http://plutohome.com/support/index.php?section=document&docID=208">What can Pluto do?</a></td>
	</tr></table></td>
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
              <td align="right"><img src="images/bullet_1.gif" width="11" height="11" align="absmiddle" /> <strong><a href="index.php?section=security">more </a></strong> &nbsp;</td>
            </tr>
          </table>
          </td>
      </tr>
    </table></td>
  </tr>
  <tr>
    <td valign="top" class="insidetable"><table width="100%"  border="0" cellspacing="0" cellpadding="0">
      <tr>
        <td width="89" valign="top"><img src="images/thumbs/img_telecom.jpg" width="89" height="89" /></td>
        <td width="10"><img src="images/spacer.gif" width="10" height="10" /></td>
        <td valign="top"><table width="100%"  border="0" cellspacing="0" cellpadding="0">
          <tr>
            <td><img src="images/titles/telecom.gif" width="200" height="13" /></td>
          </tr>
          <tr>
            <td><br />
      Effortless video conferencing - answer and dial just like normal<br />
Tie all your homes together as one phone system<br />
Always routes your calls to the extension nearest you <br /></td>
          </tr>
          <tr>
            <td align="right"><img src="images/bullet_1.gif" width="11" height="11" align="absmiddle" /> <strong><a href="index.php?section=telecom">more </a></strong> &nbsp;</td>
          </tr>
        </table></td>
      </tr>
    </table></td>
  </tr>
  <tr>
    <td valign="top" background="images/back_insidetable.jpg" class="insidetable"><table width="100%"  border="0" cellpadding="0" cellspacing="0">
      <tr>
        <td width="89" valign="top"><img src="images/thumbs/img_home_automation.jpg" width="89" height="89" /></td>
        <td width="10"><img src="images/spacer.gif" width="10" height="10" /></td>
        <td valign="top">
          <table width="100%"  border="0" cellspacing="0" cellpadding="0">
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
              <td align="right"><img src="images/bullet_1.gif" width="11" height="11" align="absmiddle" /> <strong><a href="index.php?section=home_automation">more </a></strong> &nbsp;</td>
            </tr>
          </table></td>
      </tr>
    </table></td>
  </tr>
  <tr>
    <td valign="top" class="insidetable"><table width="100%"  border="0" cellspacing="0" cellpadding="0">
      <tr>
        <td width="89" valign="top"><img src="images/thumbs/img_media.jpg" width="89" height="89" /></td>
        <td width="10"><img src="images/spacer.gif" width="10" height="10" /></td>
        <td valign="top">
          <table width="100%"  border="0" cellspacing="0" cellpadding="0">
            <tr>
              <td><img src="images/titles/media_entertainment.gif" width="200" height="13" /></td>
            </tr>
            <tr>
              <td><br />
                Your media follows you around the house<br />
Each family member can have both public and private media<br />
Live video of favorite shows &amp; channel history<br /></td>
            </tr>
            <tr>
              <td align="right"><img src="images/bullet_1.gif" width="11" height="11" align="absmiddle" /> <strong><a href="index.php?section=media_entertainment">more </a></strong> &nbsp;</td>
            </tr>
          </table></td>
      </tr>
    </table></td>
  </tr>
  <tr>
    <td valign="top" background="images/back_insidetable.jpg" class="insidetable"><table width="100%"  border="0" cellpadding="0" cellspacing="0">
      <tr>
        <td width="89" valign="top"><img src="images/thumbs/img_computing.jpg" width="89" height="89" /></td>
        <td width="10"><img src="images/spacer.gif" width="10" height="10" /></td>
        <td valign="top">
          <table width="100%"  border="0" cellspacing="0" cellpadding="0">
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
              <td align="right"><img src="images/bullet_1.gif" width="11" height="11" align="absmiddle" /> <strong><a href="index.php?section=personal_computing">more </a></strong> &nbsp;</td>
            </tr>
          </table></td>
      </tr>
    </table></td>
  </tr>
  <tr>
    <td>&nbsp;</td>
  </tr>
</table>
	';


	$output->setPressedButton(1);
	$output->setNavigationMenu(array('What is Pluto'=>'index.php?section=what_is_pluto'));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}
?>
