<?php
function telecom($output) {
	$out = '
<table width="100%"  border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td align="center" class="insidetable"><table width="563"  border="0" cellpadding="0" cellspacing="0">
      <tr>
        <td width="72"><a href="index.php?section=security"><img src="images/buttons/security_off.gif" width="72" height="11" border="0" /></a></td>
        <td width="70"><a href="index.php?section=telecom"><img src="images/buttons/telecom_on.gif" width="70" height="11" border="0" /></a></td>
        <td width="124"><a href="index.php?section=home_automation"><img src="images/buttons/home_automation_off.gif" width="124" height="11" border="0" /></a></td>
        <td width="155"><a href="index.php?section=media_entertainment"><img src="images/buttons/media_entertainment_off.gif" width="155" height="11" border="0" /></a></td>
        <td width="132"><a href="index.php?section=personal_computing"><img src="images/buttons/personal_computing_off.gif" width="132" height="11" border="0" /></a></td>
      </tr>
    </table></td>
  </tr>
  <tr>
    <td valign="top" background="images/back_insidetable.jpg" class="insidetable"><table width="100%"  border="0" cellspacing="0" cellpadding="0">
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
              <td align="right">&nbsp;</td>
            </tr>
        </table></td>
      </tr>
    </table></td>
  </tr>
  <tr>
    <td valign="top" class="insidetable2">
      <p><img src="images/bullet_1.gif" width="11" height="11" align="absmiddle" /> Pluto automatically turns all your TVs into video phones. If somebody calls you from a video phone, it rings on the phones in the house just like normal. You can transfer the call to someone else, whether or not they have a video phone. You can still put the call on hold, and pick it up on your mobile phone or cordless.<br />
        <br />
        <img src="images/bullet_1.gif" width="11" height="11" align="absmiddle" /> With Pluto, whenever you\'re on the phone, the nearest Orbiter will automatically show you pictures of each family member. If you want to transfer, or conference, just touch the person\'s picture.<br />
        <br />
        <img src="images/bullet_1.gif" width="11" height="11" align="absmiddle" /> If you have multiple residences, just tell your Pluto rep to join them. Pluto will automatically sense what home you\'re in. Anytime someone calls any of your homes-anywhere in the world-the call is automatically put through, unless you\'re sleeping. Pluto uses voice-over-ip, so there are no long distance charges.<br />
          <br />
        <img src="images/bullet_1.gif" width="11" height="11" align="absmiddle" /> Pluto\'s \'follow-me\' keep track of what room in the house you\'re in, and your phone calls automatically ring on the extension nearest you.<br />
        <br />
        <img src="images/bullet_1.gif" width="11" height="11" align="absmiddle" /> Pluto remembers who normally calls for whom and route the call automatically, based on the caller ID. When your daughter\'s school friend calls, it will automatically ring in your daughter\'s room if that\'s where she is - without disturbing anyone else in the house. If she\'s out, it will go automatically to her mobile phone if you wish.<br />
        <br />
        <img src="images/bullet_1.gif" width="11" height="11" align="absmiddle" /> As you add names to your personal phone book, be sure to check off which ones are priority people, like friends and family. When you press the button on your mobile phone to go to sleep, Pluto adjusts your phone settings. Somebody who calls will automatically get voicemail, except for your priority callers who will be told that you are sleeping, and given the option of waking you. If they choose to do so, Pluto will put the call through to the phone by the bed so as not to disturb others in the house.</p>
    </td>
  </tr>
</table>
	';

	$output->setPressedButton(1);
	$output->setNavigationMenu(array('What is Pluto'=>'index.php?section=what_is_pluto','Telecom'=>'index.php?section=telecom'));
	$output->setTitle(APPLICATION_NAME."::Telecom");			//page title
	$output->setScriptCalendar('null');
	$output->setScriptTRColor('null');
	$output->setBody($out);
	$output->output();
}
?>
