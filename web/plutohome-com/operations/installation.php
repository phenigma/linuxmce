<?php
 function installation($output) {
 	global $wikiHost;
 	
 	$installationUrl=$wikiHost.'index.php/'.wikiLink('Installation');
 	$usersManualUrl=$wikiHost.'index.php/'.wikiLink('User\'s Manual');
 	$introductionUrl=$wikiHost.'index.php/'.wikiLink('Programmer\'s Guide');
 	
		$out = '';
		
$out.=<<<TEXT

<table width="100%" border="0" cellpadding="0" cellspacing="0" class="maintable">
  <tr>
    <td align="center" class="insidetable"><table width="563"  border="0" cellpadding="0" cellspacing="0">
      <tr>
        <td align="left"><img src="images/titles/support.gif"></td>
      </tr>
    </table></td>
  </tr>
  <tr>
    <td align="left" class="insidetable">
      <p>Pluto has a comprehensive support web site with both general information about Pluto, and also detailed support for each of the plug-in modules.  
      To install the free software, just register or login, and choose "New Installation" from your My Pluto home page.
      Here are some useful shortcuts to our online support:<br>
      <br><a href="$installationUrl">Installation Section</a>
      <br><a href="$usersManualUrl">User's manual</a>
      <br><a href="$introductionUrl">Introduction for programmers and developers</a>
	</td>
  </tr>
</table>
	
TEXT;
		
	$output->setPressedButton(4);
	$output->setNavigationMenu(array('Support'=>'index.php?section=support','Installation Information'=>'index.php?section=installation'));
	$output->setTitle(APPLICATION_NAME."::Installation Information");			//page title
	$output->setBody($out);
	$output->output();
 }
?>