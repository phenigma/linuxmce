<?
function how_the_system_works($output,$conn){
	$out = <<<TEXT
<table width="100%"  border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td align="left" class="insidetable"><table width="438"  border="0" cellpadding="0" cellspacing="0">
      <tr>
        <td width="155"><a href="index.php?section=pluto_system_overview"><img src="images/buttons/pluto_system_overview_off.gif" width="155" height="11" border="0" /></a></td>
        <td width="155"><a href="index.php?section=how_the_system_works"><img src="images/buttons/how_the_system_works_on.gif" width="155" height="11" border="0" /></a></td>
        <td width="128"><a href="index.php?section=learn_how_to_use_it"><img src="images/buttons/learn_how_to_use_it_off.gif" width="128" height="11" border="0" /></a></td>
        </tr>
    </table></td>
  </tr>
  <tr>
    <td valign="top" background="images/back_insidetable.jpg" class="insidetable"><table width="100%"  border="0" cellspacing="0" cellpadding="0">
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
              <td align="right"> &nbsp;</td>
            </tr>
        </table></td>
      </tr>
    </table></td>
  </tr>
  <tr>
    <td valign="top" class="insidetable2">The Core is the central server which runs the Pluto Software; it's a media server for your whole CD/DVD collection. You only need one in your house; you will not need to access it.<br />
      <br />
      Allow one Media Director for each TV or entertainment area because it renders the media from the Pluto Core. It works with popular multi-zone audio systems and digital audio players.<br />
      <br />
      The Pluto Orbiter lets you control the whole house through a very easy to use user interface. Browsing catalogs of media is easier with a wide touch-screen display.<br />
    <br />
    Pluto makes your mobile phone the simplest and most convenient universal remote for everything regardless of the brand&mdash;lights, TV, stereo, security, climate, etc.</td>
  </tr>
  <tr>
    <td valign="top"><img src="images/schema_how_the_system_works.jpg"></td>
  </tr>
  <tr>
    <td valign="top">&nbsp;</td>
  </tr>  
</table>
	
TEXT;


	$output->setPressedButton(3);
	$output->setNavigationMenu(array('How Pluto works'=>'index.php?section=how_pluto_works','How the system works'=>'index.php?section=how_the_system_works'));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}
?>