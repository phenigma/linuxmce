<?
function contact($output,$conn){
	$out='
<table width="100%"  border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td align="center" class="insidetable"><table width="563"  border="0" cellpadding="0" cellspacing="0">
      <tr>
        <td align="left"><img src="images/titles/contact.gif"></td>
      </tr>
    </table></td>
  </tr>
  <tr>
    <td valign="top" class="insidetable2"><table>
		<tr>
			<td colspan="2">Choose a method listed below and a member from our staff support will answer to your questions</td>
		</tr>
		<tr>
			<td width="15">&nbsp;</td>
			<td width="100%">&nbsp;</td>
		</tr>	
		<tr>
			<td><img src="images/bullet_1.gif" width="11" height="11" align="absmiddle" /></td>
			<td><span class="title">Corporate office:</span></td>
		</tr>
		<tr>
			<td colspan="2">Pluto<br>
                  2699 Stirling Road, Building C<br>
                  Fort Lauderdale, FL 33312<br>
                  Phone: (877) pluto-4-u<br>
                  Office house (EST):</span> Mon-Fri 8:30am &ndash; 9:00pm Sat: 10:00am &ndash; 3:00pm
			</td>
		</tr>	
		<tr>
			<td>&nbsp;</td>
			<td>&nbsp;</td>
		</tr>	
		<tr>
			<td><img src="images/bullet_1.gif" width="11" height="11" align="absmiddle" /></td>
			<td><span class="title">Phone:</span> (877)pluto-4-u</td>
		</tr>
		<tr>
			<td>&nbsp;</td>
			<td>&nbsp;</td>
		</tr>	
		<tr>
			<td><img src="images/bullet_1.gif" width="11" height="11" align="absmiddle" /></td>
			<td><span class="title">Email:</span> <a href="mailto:support@plutohome.com">support@plutohome.com</a></td>
		</tr>
		<tr>
			<td>&nbsp;</td>
			<td>&nbsp;</td>
		</tr>	
	</table>
    </td>
  </tr>
</table>	
';

	$output->setPressedButton(4);
	$output->setNavigationMenu(array('Support'=>'index.php?section=support'));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}
?>