<?
function changePassword($output)
{
	global $changePassMasterUserUrl;
	$out='';
	if($_SESSION['userIsLogged']!="yes")
	{
		$out.="Your login session has expired";
		header("Location: index.php");
	}


	$out.='
    <form action="index.php?section=changePassword" method="POST" name="form1">			
      
		<table align="left" cellspacing="0" cellpadding="5" class="insidetable2">
        </tr>
			<tr>
				<td align="left" width="150"><B>Current Password</B></td>
				<td><input type="password" name="current" class="textfield"/> <a href="index.php?section=forgotPassword">Forgot password</a>?<br></td>
			</tr>
			<tr>
				<td align="left"><B>New Password</B></td>
				<td><input type="password" name="password" class="textfield"/><br></td>
			</tr>
			<tr>
				<td align="left"><B>Confirm New Password</B></td>
				<td><input type="password" name="confirm" class="textfield"/><br></td>
			</tr>
			<tr><td colspan="2" align="left"><input type="submit" name="changePasswordword" value="Change Password" /></td></tr>
		</table>
		</form>';

	if(isset($_POST['changePasswordword'])){
		$new_pass=$_POST['password'];
		if($_POST['current']!='' && $_POST['password']!='' && $_POST['password']==$_POST['confirm']){
			$updMasterUsers=updateMasterUsersPassword($_SESSION['userID'],$new_pass,$changePassMasterUserUrl,$_POST['current']);
			if($updMasterUsers[0]){
				$_SESSION['Password']=$new_pass;
				$out='';
				$out.='
				   <table width="100%" border="0" align="right" cellpadding="0" cellspacing="0" class="insidetable2">
				  		<tr>
				    		<td width="5%"></td>
				    		<td width="100%" rowspan="2" valign="top" align="center"><br>Your password has been changed!
				    	</td>
				    </tr>
				   </table>';
			}else{
				$out.='
				   <table width="100%" border="0" align="right" cellpadding="0" cellspacing="0" class="maintable">
				  		<tr>
				    		<td width="5%"></td>
				    		<td width="100%" rowspan="2" valign="top" align="center" class="err">'.$updMasterUsers[1].'</td>
				    </tr>
				   </table>';		
			
			}
		}elseif($_POST['current']==''){
			$out.='
				   <table width="100%" border="0" align="right" cellpadding="0" cellspacing="0" class="maintable">
				  		<tr>
				    		<td width="5%"></td>
				    		<td width="100%" rowspan="2" valign="top" align="center" class="err"><br>You must type old password in order to change it.</td>
				    </tr>
				   </table>';		
		}elseif($_POST['password']==''){
			$out.='
				   <table width="100%" border="0" align="right" cellpadding="0" cellspacing="0" class="maintable">
				  		<tr>
				    		<td width="5%"></td>
				    		<td width="100%" rowspan="2" valign="top" align="center" class="err"><br>New password cannot be empty.</td>
				    </tr>
				   </table>';		
		}else{
			$out.='
				   <table width="100%" border="0" align="right" cellpadding="0" cellspacing="0" class="maintable">
				  		<tr>
				    		<td width="5%"></td>
				    		<td width="100%" rowspan="2" valign="top" align="center" class="err"><br>Please type the same paswword in "New" and "Confirm New" fields.</td>
				    </tr>
				   </table>';		
		}
	}

	$output->setNavigationMenu(array("Login"=>"index.php?section=login",'Change Password'=>'index.php?section=changePassword'));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME." :: Change Password");
	$output->output();
}
?>