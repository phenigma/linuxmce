<?
function how_pluto_works($output,$conn){
		$out = <<<TEXT
		
<table width="100%"  border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td class="insidetable"><img src="images/titles/how_pluto_works.gif" width="200" height="13" /></td>
  </tr>
  <tr>
    <td valign="top" background="images/back_insidetable.jpg" class="insidetable"><table width="100%"  border="0" cellpadding="0" cellspacing="0">
      <tr>
        <td width="89" valign="top"><img src="images/thumbs/img_overview.jpg" width="89" height="89" /></td>
        <td width="10"><img src="images/spacer.gif" width="10" height="10" /></td>
        <td valign="top"><table width="100%"  border="0" cellspacing="0" cellpadding="0">
            <tr>
              <td><img src="images/titles/pluto_system_overview.gif" width="200" height="13" /></td>
            </tr>
            <tr>
              <td><br />
                A Pluto system consists of 4 pieces of equipment: Pluto Core, Pluto Media Director, Pluto Orbiter, Pluto Mobile Orbiter. In addition to the 4 main components, there exist many accessories such as cameras, interface modules to control your existing A/V, lighting, etc.<br /></td>
            </tr>
            <tr>
              <td align="right"><img src="images/bullet_1.gif" width="11" height="11" align="absmiddle" /> <strong><a href="index.php?section=pluto_system_overview">more </a></strong> &nbsp;</td>
            </tr>
          </table>
          </td>
      </tr>
    </table></td>
  </tr>
  <tr>
    <td valign="top" class="insidetable"><table width="100%"  border="0" cellspacing="0" cellpadding="0">
      <tr>
        <td width="89" valign="top"><img src="images/thumbs/img_how_system_works.jpg" width="89" height="89" /></td>
        <td width="10"><img src="images/spacer.gif" width="10" height="10" /></td>
        <td valign="top"><table width="100%"  border="0" cellspacing="0" cellpadding="0">
          <tr>
            <td><img src="images/titles/how_the_system_works.gif" width="200" height="13" /></td>
          </tr>
          <tr>
            <td><br />
      The Pluto Core is a media server for your whole CD/DVD collection. The Media Director renders the media from the Pluto Core. The Pluto Mobile Orbiter and the Pluto Orbiter are used to control your home.<br /></td>
          </tr>
          <tr>
            <td align="right"><img src="images/bullet_1.gif" width="11" height="11" align="absmiddle" /> <strong><a href="index.php?section=how_the_system_works">more </a></strong> &nbsp;</td>
          </tr>
        </table></td>
      </tr>
    </table></td>
  </tr>
  <tr>
    <td valign="top" background="images/back_insidetable.jpg" class="insidetable"><table width="100%"  border="0" cellspacing="0" cellpadding="0">
      <tr>
        <td width="89" valign="top"><img src="images/thumbs/img_how_to_use_it.jpg" width="89" height="89" /></td>
        <td width="10"><img src="images/spacer.gif" width="10" height="10" /></td>
        <td valign="top"><table width="100%"  border="0" cellspacing="0" cellpadding="0">
            <tr>
              <td><img src="images/titles/learn_how_to_use_it.gif" width="200" height="13" /></td>
            </tr>
            <tr>
              <td><br />
                <a href="index.php?section=learn_how_to_use_it#mobile">How to use the mobile phone</a><br />
                <a href="index.php?section=learn_how_to_use_it#orbiter">How to use the orbiter</a><br />
                 <a href="index.php?section=learn_how_to_use_it#follow">How pluto's follow me works</a><br /></td>
            </tr>
            <tr>
              <td align="right"><img src="images/bullet_1.gif" width="11" height="11" align="absmiddle" /> <strong><a href="index.php?section=learn_how_to_use_it">more </a></strong> &nbsp; </td>
            </tr>
        </table></td>
      </tr>
    </table></td>
  </tr>
  <tr>
    <td>&nbsp;</td>
  </tr>
</table>

TEXT;


	$output->setPressedButton(3);
	$output->setNavigationMenu(array('How Pluto works'=>'index.php?section=how_pluto_works'));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}
?>