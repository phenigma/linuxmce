<?
function why_you_need_pluto($output,$conn){
	$out = '
<table width="100%"  border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td class="insidetable"><img src="images/titles/why_you_need_pluto.gif" width="200" height="13" /></td>
  </tr>
  <tr>
    <td valign="top" background="images/back_insidetable.jpg" class="insidetable"><table width="100%"  border="0" cellpadding="0" cellspacing="0">
      <tr>
        <td width="89" valign="top"><img src="images/thumbs/img_simplicity.jpg" width="89" height="89" /></td>
        <td width="10"><img src="images/spacer.gif" width="10" height="10" /></td>
        <td valign="top"><table width="100%"  border="0" cellspacing="0" cellpadding="0">
            <tr>
              <td><img src="images/titles/simplicity.gif" width="200" height="13" /></td>
            </tr>
            <tr>
              <td><br />
                Throw away all your remote controls.  Yes, all of them.                 <br />
You use your mobile phone to control your whole house. <br />
Yes, all of it.<br /></td>
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
    <td valign="top" class="insidetable"><table width="100%"  border="0" cellspacing="0" cellpadding="0">
      <tr>
        <td width="89" valign="top"><img src="images/thumbs/img_safety.jpg" width="89" height="89" /></td>
        <td width="10"><img src="images/spacer.gif" width="10" height="10" /></td>
        <td valign="top"><table width="100%"  border="0" cellspacing="0" cellpadding="0">
          <tr>
            <td><img src="images/titles/safety.gif" width="200" height="13" /></td>
          </tr>
          <tr>
            <td><br />
      Statistics show the police takes an hour to respond to monitored alarms. That\'s not safe.<br />
Pluto gives you a live video feed on the mobile phone the instant someone shows at your doorsteps. Call 911 if it\'s an intruder. That\'s safe.<br /></td>
          </tr>
          <tr>
            <td align="right">&nbsp;</td>
          </tr>
        </table></td>
      </tr>
    </table></td>
  </tr>
  <tr>
    <td valign="top" background="images/back_insidetable.jpg" class="insidetable"><table width="100%"  border="0" cellspacing="0" cellpadding="0">
      <tr>
        <td width="89" valign="top"><img src="images/thumbs/img_excitement.jpg" width="89" height="89" /></td>
        <td width="10"><img src="images/spacer.gif" width="10" height="10" /></td>
        <td valign="top"><table width="100%"  border="0" cellspacing="0" cellpadding="0">
            <tr>
              <td><img src="images/titles/excitement.gif" width="200" height="13" /></td>
            </tr>
            <tr>
              <td><br />
                Enjoy an entertainment extravaganza - all your media anywhere in the house at any time. <br />
                Just wallow in it.<br /></td>
            </tr>
            <tr>
              <td align="right"> &nbsp;</td>
            </tr>
        </table></td>
      </tr>
    </table></td>
  </tr>
  <tr>
    <td valign="top" class="insidetable"><table width="100%"  border="0" cellpadding="0" cellspacing="0">
      <tr>
        <td width="89" valign="top"><img src="images/thumbs/img_power.jpg" width="89" height="89" /></td>
        <td width="10"><img src="images/spacer.gif" width="10" height="10" /></td>
        <td valign="top">
          <table width="100%"  border="0" cellspacing="0" cellpadding="0">
            <tr>
              <td><img src="images/titles/power.gif" width="200" height="13" /></td>
            </tr>
            <tr>
              <td><br />
      Indulge the control freak within. <br />
      Check out the screen shots. <br />
      Enough said.<br />      <br />        </td>
            </tr>
            <tr>
              <td align="right">&nbsp;</td>
            </tr>
          </table></td>
      </tr>
    </table></td>
  </tr>
  <tr>
    <td valign="top" background="images/back_insidetable.jpg" class="insidetable"><table width="100%"  border="0" cellspacing="0" cellpadding="0">
      <tr>
        <td width="89" valign="top"><img src="images/thumbs/img_value.jpg" width="89" height="89" /></td>
        <td width="10"><img src="images/spacer.gif" width="10" height="10" /></td>
        <td valign="top">
          <table width="100%"  border="0" cellspacing="0" cellpadding="0">
            <tr>
              <td><img src="images/titles/value.gif" width="200" height="13" /></td>
            </tr>
            <tr>
              <td><br />
                A commercial-grade server that does it all with redundancy costs less than a slew of disposable devices like a PVR, phone system, DVD player, media PC, etc. <br /></td>
            </tr>
            <tr>
              <td align="right">&nbsp;</td>
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


	$output->setPressedButton(2);
	$output->setNavigationMenu(array('Why you need Pluto'=>'index.php?section=why_you_need_pluto'));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}
?>