<?
function business_register($output){
  $out='';
  $out.='

<table width="100%" border="0" align="right" cellpadding="0" cellspacing="0" class="maintable">
  <tr>
    <td width="5%"></td>
    <td width="100%" rowspan="2" valign="top" align="left">
  <br><b>Note: You have entered a secure site.<br>All information you provide is encrypted for your protection.<br><br><b><i>Individual</b></i> Sign up for Pluto VIP</b><br>
		<b>If you are a business, <a href="client/business_register.php">click here</a> to sign up</b><br>
		<b></b>
		<br><table cellspacing="0" cellpadding="3">
			<tr>
				<td align="right">E-mail</td>
				<td><input type="text" name="email" value="" /></td>
				<td>&nbsp;</td>
			</tr>
			<tr>
				<td align="right">Username</td>
				<td><input type="text" name="username" value="" /></td>
				<td class="formextrainfo">Pick a username that you will use to login to the system.<br />You can use your email address as your username if you wish</font></td>
			</tr>
			<tr>
				<td align="right">Password</td>
				<td><input type="password" name="password" /></td>
				<td class="formextrainfo">Your password must be at least 6 characters long and must contain both letters and numbers.<br />This is for your own protection.</td>
			</tr>
			<tr>
				<td align="right">Confirm</td>
				<td><input type="password" name="password2" /></td>
				<td class="formextrainfo">please retype your password</td>
			</tr>
			<tr>
				<td colspan="3" align="center"><input type="submit" value="Sign up" /></th>
			</tr>
         <tr>
         <td colspan="3" align="center"><a href="index.php">I am an Existing User</a></td>
       </tr>
		</table>
		
		</div>
		</form>
  </td>
  </tr>
  </table>';
$output->setScriptCalendar('null');
		$output->setScriptTRColor('null');		
		$output->setBody($out);
		$output->setImgName("img.jpg");
		$output->setTitle(APPLICATION_NAME."::Register");			
      $output->setPageID(98); //butonul selectat
  		$output->output();
}
?>