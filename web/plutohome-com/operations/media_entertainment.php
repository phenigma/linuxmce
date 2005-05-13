<?php
function media_entertainment($output) {
	$out = '
<table width="100%"  border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td align="center" class="insidetable"><table width="563"  border="0" cellpadding="0" cellspacing="0">
      <tr>
        <td width="72"><a href="index.php?section=security"><img src="images/buttons/security_off.gif" width="72" height="11" border="0" /></a></td>
        <td width="70"><a href="index.php?section=telecom"><img src="images/buttons/telecom_off.gif" width="70" height="11" border="0" /></a></td>
        <td width="124"><a href="index.php?section=home_automation"><img src="images/buttons/home_automation_off.gif" width="124" height="11" border="0" /></a></td>
        <td width="155"><a href="index.php?section=media_entertainment"><img src="images/buttons/media_entertainment_on.gif" width="155" height="11" border="0" /></a></td>
        <td width="132"><a href="index.php?section=personal_computing"><img src="images/buttons/personal_computing_off.gif" width="132" height="11" border="0" /></a></td>
      </tr>
    </table></td>
  </tr>
  <tr>
    <td valign="top" background="images/back_insidetable.jpg" class="insidetable"><table width="100%"  border="0" cellspacing="0" cellpadding="0">
      <tr>
        <td width="89" valign="top"><img src="images/thumbs/img_media.jpg" width="89" height="89" /></td>
        <td width="10"><img src="images/spacer.gif" width="10" height="10" /></td>
        <td valign="top"><table width="100%"  border="0" cellspacing="0" cellpadding="0">
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
              <td align="right">&nbsp;</td>
            </tr>
        </table></td>
      </tr>
    </table></td>
  </tr>
  <tr>
    <td valign="top" class="insidetable2">
      <p><img src="images/bullet_1.gif" width="11" height="11" align="absmiddle" /> Pluto always knows what room each family member is in, and learns his favorite media (TV shows, music, etc.). Press the \'TV\' button on your mobile phone. The nearest TV - no matter what brand - will automatically come on to your favorite show. 90% of the time you will not need to use the program guide or channel surf.<br />
        <br />
        <img src="images/bullet_1.gif" width="11" height="11" align="absmiddle" /> When you move around the house, all you do is hold down the media button on your mobile phone for 1 second, and a red &quot;F&quot; for \'follow me\' appears. Now just wander around the house and whatever media you were enjoying - TV, DVD movie, music, radio, etc. - will follow you. It works individually for each family member. This is the ultimate convenience, and only Pluto has it. To date, only a handful of super-rich individuals have spent millions for this luxury.<br />
        <br />
        <img src="images/bullet_1.gif" width="11" height="11" align="absmiddle" /> In case there are two people in the same room, Pluto sets up the room for whoever entered first. If that person leaves the room, then Pluto sets it up for the next person who is still in the room.<br />
        <br />
        <img src="images/bullet_1.gif" width="11" height="11" align="absmiddle" /> Whenever you choose to record or save any media - TV shows, DVD movies, music, etc. - you are given the option of making it public, so the whole family can enjoy it, or private, so only you can access it. When you first get your Pluto system, just be sure to tell Pluto who are the heads of the house. These people can view everybody\'s private media.<br />
        <br />
        <img src="images/bullet_1.gif" width="11" height="11" align="absmiddle" /> Pluto learns each family member\'s favorite shows; when anybody presses the \'TV\' button, Pluto turns on the nearest TV, scans the current running programs, and automatically tunes to that person\'s favorite show. There\'s a full EPG too, but this is far more comfortable and convenient.<br />
        <br />
        <img src="images/bullet_1.gif" width="11" height="11" align="absmiddle" /> All media - music, movies, TV shows, home videos, etc. are stored on the Pluto Core - the central server. The Core is powerful enough to handle dozens of simultaneous media streams. You\'ll never have to worry about a hard drive crash and losing your media. You have virtually unlimited capacity, and everyone in the family can enjoy any media anywhere, anytime.<br />
        <br />
        <img src="images/bullet_1.gif" width="11" height="11" align="absmiddle" /> The Pluto Core distributes all the media digitally. This means you do not have to tear open your walls to run extra cables, plus the media can go from anywhere to anywhere with absolutely no loss of quality.</p>
      </td>
  </tr>
</table>
';

	$output->setPressedButton(1);
	$output->setNavigationMenu(array('What is Pluto'=>'index.php?section=what_is_pluto','Media & entertainment'=>'index.php?section=media_entertainment'));

	$output->setTitle(APPLICATION_NAME."::Media and Entertainment");			//page title
	//don't touch them
	$output->setScriptCalendar('null');
	$output->setScriptTRColor('null');
	$output->setBody($out);
	$output->output();
}
?>
