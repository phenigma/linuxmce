<?
function get_pluto($output,$conn){
	$out='
<table width="100%"  border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td align="center" class="insidetable"><table width="563"  border="0" cellpadding="0" cellspacing="0">
      <tr>
        <td align="left"><img src="images/titles/get_pluto.gif"></td>
      </tr>
    </table></td>
  </tr>
  <tr>
    <td valign="top" class="insidetable2"><table border="0">
		<tr>
			<td width="15">&nbsp;</td>
			<td width="100%">&nbsp;</td>
		</tr>	
		<tr>
			<td><img src="images/bullet_1.gif" width="11" height="11" align="absmiddle" /></td>
			<td><span class="title">Build a Pluto system for FREE!</span></td>
		</tr>
		<tr>
			<td colspan="2">Download the Pluto software for free to install on your existing PC\'s.</td>
		</tr>
		<tr>
			<td colspan="2" align="left"><a href="support/index.php?section=document&docID=1">Get the software </a>&nbsp;</td>
		</tr>	
		<tr>
			<td>&nbsp;</td>
			<td>&nbsp;</td>
		</tr>	
		<tr>
			<td>&nbsp;</td>
			<td>&nbsp;</td>
		</tr>	
	
		<tr>
			<td>&nbsp;</td>
			<td>Get useful information:<br>
			<a href="index.php?section=dealer">I\'m a dealer </a><br>
			<a href="index.php?section=manufacturer">I\'m a manufacturer</a><br>
			</td>
		</tr>	
	
	</table>
    </td>
  </tr>
</table>	
';

	$output->setPressedButton(5);
	$output->setNavigationMenu(array('Get Pluto'=>'index.php?section=get_pluto'));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME.' :: Get Pluto');
	$output->output();
}
?>
