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
			<td><span class="title">Dealer locator</span></td>
		</tr>
		<tr>
			<td colspan="2">Pluto dealers offer the Pluto system by itself, or as part of a custom-installed whole-house smart home solution.</td>
		</tr>
		<tr>
			<td colspan="2" align="left"><a href="index.php?section=locator">Find a dealer </a>&nbsp;</td>
		</tr>		
		<tr>
			<td>&nbsp;</td>
			<td>&nbsp;</td>
		</tr>	
		<tr>
			<td><img src="images/bullet_1.gif" width="11" height="11" align="absmiddle" /></td>
			<td><span class="title">Powered By Pluto</span></td>
		</tr>
		<tr>
			<td colspan="2">Third party products that work with Pluto. Pluto version 2 is available as an embedded solution for incorporating into other consumer electronics products.</td>
		</tr>
		<tr>
			<td colspan="2" align="left"><a href="index.php?section=how_to_get">Tell me more </a>&nbsp;</td>
		</tr>	
		<tr>
			<td>&nbsp;</td>
			<td>&nbsp;</td>
		</tr>	
		<tr>
			<td><img src="images/bullet_1.gif" width="11" height="11" align="absmiddle" /></td>
			<td><span class="title">For developers and "techies"</span></td>
		</tr>
		<tr>
			<td colspan="2">Use your own computers, and Pluto gives you the software FREE!<br>
Download the same software powering our retail products for FREE, as open source, compatible with Linux and Windows.</td>
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
			<td>Get usefull information:<br>
			<a href="index.php?section=dealer">I\'m a dealer </a><br>
			<a href="index.php?section=manufacturer">I\'m a manufacturer</a><br>
			<a href="index.php?section=developer">I\'m an open source developer </a>
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