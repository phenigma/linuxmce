<?
function register($output){
	global $addMasterUserUrl,$checkMasterUserUrl;

	if(isset($_SESSION['userIsLogged']) && $_SESSION['userIsLogged']=="yes"){
		// the user is already logged
		header("Location: index.php?section=myPluto");
	}
	
  	$out='';
	$referrer = isset($_REQUEST['referrer'])?cleanString($_REQUEST['referrer']):'';
	
  	$out.='

<table width="100%" border="0" align="right" cellpadding="0" cellspacing="0" class="maintable">
  <tr>
    <td width="5%"></td>
    <td width="100%" rowspan="2" valign="top" align="left">
   <form action="index.php?section=register" method="POST" name="register">	
  <br><center><b>Register</b></center><br>	
<table cellspacing="0" cellpadding="3" align="center"  class="insidetable">
			<tr>
				<td align="right">E-mail</td>
				<td><input type="text" name="email" value="" /></td>
				<td class="formextrainfo">Please provide an valid email address.</td>
			</tr>
			<tr>
				<td align="right">Username</td>
				<td><input type="text" name="username" value="" /></td>
				<td class="formextrainfo">Pick a username that you will use to login to the system.<br/></td>
			</tr>
			<tr>
				<td align="right">Referrer</td>
				<td><input type="text" name="referrer" value="'.$referrer.'" /></td>
				<td class="formextrainfo">Type the username who let you know about Pluto.</td>
			</tr>
  			<tr>
				<td align="right">Password</td>
				<td><input type="password" name="password" /></td>
				<td class="formextrainfo">&nbsp;</td>
			</tr>
			<tr>
				<td align="right">Confirm</td>
				<td><input type="password" name="password2" /></td>
				<td class="formextrainfo">please retype your password</td>
			</tr>
			<tr>
				<td colspan="3" align="center"><input type="submit" name="register" value="Sign up" onsubmit="validateEmail(this);"/></th>
			</tr>
         <tr>
         <td colspan="3" align="center">We never reveal this information and all personal information is encrypted.
  	<br><br><a href="index.php?section=clientLoginForm">I am an Existing User</a></td>
       </tr>
		</table>
		
		</div>
		</form>
  		<script>
		 	var frmvalidator = new formValidator("register");
  			frmvalidator.addValidation("email","req","Please enter an email address.");
 			frmvalidator.addValidation("username","req","Please enter an username.");			
	 		frmvalidator.addValidation("password","req","Please enter your password.");
  			frmvalidator.addValidation("password2","req","Please confirm your password.");			
		</script>
  	';
 

  	if(isset($_POST['register']))
  	{
  		$errorsCount=0;
  		$mail=$_POST['email'];

  		if (!ereg ("[0-9a-zA-Z]@[0-9a-zA-Z].[0-9a-zA-Z]", $mail)){
  			$out.='<table align="center"><tr><td align="center" colspan="3"> Invalid e-mail entered</td></tr></table>';
  			$errorsCount++;
  		}

  		if($_POST['username']==''){
  			$out.='<table align="center"><tr><td align="center" colspan="3"> Invalid username entered</td></tr></table>';
  			$errorsCount++;
  		}
  		if($_POST['password']=='' || $_POST['password2']=='' || $_POST['password']!=$_POST['password2']){
  			$out.='<table align="center"><tr><td align="center" colspan="3"> Invalid password entered</td></tr></table>';
  			$errorsCount++;
  		}
  		$pass=md5($_POST['password']);
  		if($referrer!=''){
  			// query MasterUsers table for PK_MasterUsers value
  			$isMasterUsers=checkMasterUsers($referrer, '',$checkMasterUserUrl);

  			if($isMasterUsers[0]){
  				parse_str($isMasterUsers[1]);
  				$referrerID = $MasterUsersID;
  			}
  			else
  			$referrerID=0;	// user not found or MasterUsers Database error
  		}
  		else
  		$referrerID=0;

  		$typeUser=0;	// 0 mean his signup was individual
  		if($errorsCount==0){
	  		$userAddedtoMasterUsers=addtoMasterUsers($typeUser,$mail,$_POST['username'],$referrerID,$pass,$addMasterUserUrl);
	  		if(!$userAddedtoMasterUsers[0]){
	  			$out.='<table align="center"><tr><td align="center" colspan="3"> '.$userAddedtoMasterUsers[1].'</td></tr></table>';
	  			$errorsCount++;
	  		}
  		}

  		if($errorsCount==0){
  			parse_str($userAddedtoMasterUsers[1]);
  			$_SESSION['userID'] = $MasterUsersID;
  			$_SESSION['username'] = $_POST['username'];
  			$_SESSION['userIsLogged']="yes";
  			$_SESSION['categ']=$typeUser;
  			
  			$res=mysql_query("SELECT * FROM Users WHERE FK_MasterUsers='$MasterUsersID'");
			if(mysql_num_rows($res)!=0){
				$res=mysql_fetch_object($res);
				$_SESSION['extPassword']=$res->ExtPassword;
			}
  			header("Location: index.php?section=myPluto");
  		}
  	}

  	$out.='</td>
	  </tr>
	  </table>';
  	$output->setScriptCalendar('null');
  	$output->setScriptTRColor('null');
  	$output->setBody($out);
  	$output->setTitle(APPLICATION_NAME."::Register");
  	$output->output();
}
?>