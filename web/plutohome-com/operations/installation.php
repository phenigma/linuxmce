<?php
 function installation($output) {
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
      <br><a href="/support/index.php?section=document&docID=11">Installation Section</a>
      <br><a href="/support/index.php?section=document&docID=14">User's manual</a>
      <br><a href="/support/index.php?section=document&docID=15">Introduction for programmers and developers</a>
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