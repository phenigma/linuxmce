<?
function forgotPassword($output)
{
	global $checkMasterUserUrl,$changePassMasterUserUrl;
	$out='';
	$out.='
  <form action="index.php?section=forgotPassword" method="POST" name="form1">
		<table width="100%">
     		<tr>
				<td align="left" colspan="2" class="insidetable"><img src="images/titles/login.gif" border="0"> <span class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</span></td>
			</tr>
     		<tr>
				<td align="left" colspan="2" class="insidetable2">&nbsp;</td>
			</tr>	
     		<tr>
				<td align="left" colspan="2" class="insidetable2">			
				<div class="title">Forgot Password ?</div><br><br>
				Enter your e-mail and a new password will be e-mailed to you.<br />
				<br /><br />
				<b>E-mail</b> <input type="text" name="Email"/> <input type="submit" name="go" value="Go" /><br /><br />
           		</td>
			</tr>
		</table>
	</form>';
	
	if(isset($_POST['go']))
	{
		// query MasterUsers table if the Email exists
		$queryMasterUsers=checkMasterUsers('', '',$checkMasterUserUrl,'&Email='.$_POST['Email']);

		if(!$queryMasterUsers[0]){
			$out='';
			$out.='

<table width="100%" border="0" align="right" cellpadding="0" cellspacing="0" class="maintable">
  <tr class="insidetable2">
    <td width="5%"></td>
    <td width="100%" rowspan="2" valign="top" align="center">
			<br>This E-mail address is not in our database!<br> <a href="index.php?section=forgotPassword">Go back</a>
	</td>
  </tr>
</table>';
		}
		else
		{
			parse_str($queryMasterUsers[1]);	// SET $MasterUsersID=PK_MasterUsers
			
			$ToEmail=$_POST['Email'];
			$subject="Your password for PlutoHome";
			$pass=str_makerand(8,8,1,0,0);
			// try to change password in MasterUsers table
			$queryMasterUsers=updateMasterUsersPassword($MasterUsersID,$pass,$changePassMasterUserUrl);
			if($queryMasterUsers[0]){
				$message='Hello,<br> Your new password for Plutohome Website is <b>'.$pass. '</b>  <br><br><a href="http://plutohome.com/index.php?section=login">Click here</a> to login and after that you can change the password by using the "Change Password" feature.<br><br>Best regards, <br>PlutoHome staff.';
				$headers = "From: PlutoHome <website@plutohome.com>\nX-Priority: 1\nContent-Type: text/html; charset=iso-8859-1\n";
				mail($ToEmail,$subject,$message,$headers);
				$out='';
			}else{
				$out=$queryMasterUsers[1];
			}
			$out.='

<table width="100%" border="0" align="right" cellpadding="0" cellspacing="0" class="maintable">
  <tr class="insidetable2">
    <td width="5%"></td>
    <td width="100%" rowspan="2" valign="top" align="center">
     <br><br>The new password has been e-mailed to '.$ToEmail.'<br><a href="index.php">Home</a>
	</td>
  </tr>
</table>';

		}
	}
	
	$output->setNavigationMenu(array("Login"=>"index.php?section=login",'Forgot Password'=>'index.php?section=forgotPassword'));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME." :: Forgot Password");
	$output->output();
}

function str_makerand ($minlength, $maxlength, $useupper, $usespecial, $usenumbers) 
{ 
/* 
Author: Peter Mugane Kionga-Kamau 
http://www.pmkmedia.com 

Description: string str_makerand(int $minlength, int $maxlength, bool $useupper, bool $usespecial, bool $usenumbers) 
returns a randomly generated string of length between $minlength and $maxlength inclusively. 

Notes: 
- If $useupper is true uppercase characters will be used; if false they will be excluded. 
- If $usespecial is true special characters will be used; if false they will be excluded. 
- If $usenumbers is true numerical characters will be used; if false they will be excluded. 
- If $minlength is equal to $maxlength a string of length $maxlength will be returned. 
- Not all special characters are included since they could cause parse errors with queries. 

Modify at will. 
*/ 
	$key='';
    $charset = "abcdefghijklmnopqrstuvwxyz"; 
    if ($useupper)   $charset .= "ABCDEFGHIJKLMNOPQRSTUVWXYZ"; 
    if ($usenumbers) $charset .= "0123456789"; 
    if ($usespecial) $charset .= "~@#$%^*()_+-={}|][";   // Note: using all special characters this reads: "~!@#$%^&*()_+`-={}|\\]?[\":;'><,./"; 
    if ($minlength > $maxlength) $length = mt_rand ($maxlength, $minlength); 
    else                         $length = mt_rand ($minlength, $maxlength); 
    for ($i=0; $i<$length; $i++) $key .= $charset[(mt_rand(0,(strlen($charset)-1)))]; 
    return $key; 
} 
?>