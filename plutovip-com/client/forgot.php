<?
function forgot($output)
{
	global $checkMasterUserUrl,$changePassMasterUserUrl;
	$out='';
	$out.='

<table width="100%" border="0" align="right" cellpadding="0" cellspacing="0" class="maintable">
  <tr>
    <td width="5%"></td>
    <td width="100%" rowspan="2" valign="top" align="left">
  <form action="index.php?section=forgot" method="POST" name="form1">		<div align="center">
			
			<h2>Forgot Password</h2>
						Enter your e-mail and a temporary password will be e-mailed to you.<br />
			You will be required to change your password once you log-in.<br /><br />
			<b>E-mail</b> <input type="text" name="Email" /> <input type="submit" name="go" value="Go" /><br /><br />
			<a href="index.php">Home</a>
		</div>
		</form>
           </td></tr></table>
           ';
	if(isset($_POST['go']))
	{
		// query MasterUsers table if the Email exists
		$queryMasterUsers=checkMasterUsers('', '',$checkMasterUserUrl,'&Email='.$_POST['Email']);

		if(!$queryMasterUsers[0]){
			$out='';
			$out.='

<table width="100%" border="0" align="right" cellpadding="0" cellspacing="0" class="maintable">
  <tr>
    <td width="5%"></td>
    <td width="100%" rowspan="2" valign="top" align="center">
			<br>This E-mail address is not in our database!<br> <a href="index.php?section=forgot">Go back</a>
	</td>
  </tr>
</table>';
		}
		else
		{
			parse_str($queryMasterUsers[1]);	// SET $MasterUsersID=PK_MasterUsers
			
			$ToEmail=$_POST['Email'];
			$subject="Your password for Plutovip";
			$pass=rand(0,10).$_POST['Email'][2].$_POST['Email'][3].rand(0,32);
			$masterPassMd5=md5($pass);
			// try to change password in MasterUsers table
			$queryMasterUsers=updateMasterUsersPassword($MasterUsersID,$masterPassMd5,$changePassMasterUserUrl);
			if($queryMasterUsers[0]){
				$message='Your new password is <b>'.$pass. '</b>  ! <a href="www.plutovip.com>click here</a> to login and change your password';
				$headers = "From: PlutoVIP <website@plutohome.com>\nX-Priority: 1\nContent-Type: text/html; charset=iso-8859-1\n";
				mail($ToEmail,$subject,$message,$headers);
				$out='';
			}else{
				$out=$queryMasterUsers[1];
			}
			$out.='

<table width="100%" border="0" align="right" cellpadding="0" cellspacing="0" class="maintable">
  <tr>
    <td width="5%"></td>
    <td width="100%" rowspan="2" valign="top" align="center">
     <br><br>The new password has been e-mailed to '.$ToEmail.'<br><a href="index.php">Home</a>
	</td>
  </tr>
</table>';

		}
	}

	$output->setScriptCalendar('null');
	$output->setScriptTRColor('null');
	$output->setBody($out);
	$output->setImgName("img.jpg");
	$output->setTitle(APPLICATION_NAME."::Forgot password");
	$output->setPageID(98); //butonul selectat
	$output->output();
}
?>