<?
function support($output,$conn){
	$out='
<table width="100%"  border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td align="center" class="insidetable"><table width="563"  border="0" cellpadding="0" cellspacing="0">
      <tr>
        <td align="left"><img src="images/titles/support.gif"></td>
      </tr>
    </table></td>
  </tr>
  <tr>
    <td valign="top" class="insidetable2"><table>
		<tr>
			<td width="15">&nbsp;</td>
			<td>Choose a method listed below and a member from our staff support will answer to your questions</td>
		</tr>
		<tr>
			<td>&nbsp;</td>
			<td>&nbsp;</td>
		</tr>	
		<tr>
			<td><img src="images/bullet_1.gif" width="11" height="11" align="absmiddle" /></td>
			<td><span class="title">Corporate office:</span></td>
		</tr>
		<tr>
			<td>&nbsp;</td>
			<td>Pluto<br>
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
		<tr>
			<td><img src="images/bullet_1.gif" width="11" height="11" align="absmiddle" /></td>
			<td><span class="title">Live support:</span> contact us and we will answer to you online.</td>
		</tr>	
		<tr>
			<td>&nbsp;</td>
			<td><table style="border-bottom: 1px solid #9EBCF8;border-top: 1px solid #9EBCF8;border-left: 1px solid #9EBCF8;border-right: 1px solid #9EBCF8;" cellpadding="3">
				<tr>
					<td><!-- BEGIN PHP Live! code, (c) OSI Codes Inc. -->
<script language="JavaScript" src="http://plutohome.com/phplive/js/status_image.php?base_url=http://plutohome.com/phplive&l=radu&x=1&deptid=3&btn=1"><a href=""></a></script>
<!-- END PHP Live! code : (c) OSI Codes Inc. --></td></tr></table></td>
		</tr>	
		<tr>
			<td>&nbsp;</td>
			<td>&nbsp;</td>
		</tr>	
		<tr>
			<td><img src="images/bullet_1.gif" width="11" height="11" align="absmiddle" /></td>
			<td><span class="title">Forum:</span> post your questions <a href="support/phpbb2/index.php">here</a> depending on your interests and a forum administrator will answer to them.</td>
		</tr>	
		<tr>
			<td>&nbsp;</td>
			<td>&nbsp;</td>
		</tr>	
		<tr>
			<td><img src="images/bullet_1.gif" width="11" height="11" align="absmiddle" /></td>
			<td><span class="title">Technical support:</span> complete this form to send an email to whatever department you want.</td>
		</tr>	
		<tr>
			<td>&nbsp;</td>
			<td>&nbsp;</td>
		</tr>	
		<tr>
			<td>&nbsp;</td>
			<td>';
		include('operations/contactForm.php');
		$out.=contactForm('support',$conn);
		$out.='</td>
		</tr>	
		<tr>
			<td>&nbsp;</td>
			<td>Get usefull information:<br>
			<a href="index.php?section=installation">Installation information</a><br>
			<a href="index.php?section=compatibility">Compatibility</a>
			</td>
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