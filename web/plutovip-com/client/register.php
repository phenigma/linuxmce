<?
function register($output){
	global $addMasterUserUrl,$checkMasterUserUrl;
  	$out='';
	$referrer = isset($_REQUEST['referrer'])?cleanString($_REQUEST['referrer']):'';
	
  	$out.='

<table width="100%" border="0" align="right" cellpadding="0" cellspacing="0" class="maintable">
  <tr>
    <td width="5%"></td>
    <td width="100%" rowspan="2" valign="top" align="left">
   <form action="index.php?section=register" method="POST" name="form1">	
  <br><b>Note: You have entered a secure site.<br>All information you provide is encrypted for your protection.<br><br><b><i>Individual</b></i> Sign up for Pluto VIP</b><br>
		<b>If you are a business, <a href="index.php?section=register_b">click here</a> to sign up</b><br>
		<b></b>
		<br><table cellspacing="0" cellpadding="3" align="center">
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
				<td align="right">Referrer</td>
				<td><input type="text" name="referrer" value="'.$referrer.'" /></td>
				<td class="formextrainfo">Type the username who let you know about Pluto VIP.</td>
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
         <td colspan="3" align="center"><a href="index.php">I am an Existing User</a></td>
       </tr>
		</table>
		
		</div>
		</form>';
 

  	if(isset($_POST['register']))
  	{
  		$errorsCount=0;
  		$mail=$_POST['email'];
      $pass=strlen($_POST['password']);
      $passw=$_POST['password'];
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
  		$pass=$_POST['password'];
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
  			$isMasterUsers=checkMasterUsers($_POST['username'], $pass,$checkMasterUserUrl,'&FirstAccount=&Email=&PlutoId=&Pin=');
			if($isMasterUsers[0]){
				parse_str($isMasterUsers[1]);
				$_SESSION['userID'] = $MasterUsersID;
				$_SESSION['PlutoId'] = $PlutoId;
				$_SESSION['Pin'] = $Pin;
				$_SESSION['username'] = $_POST['username'];
				$_SESSION['userIsLogged']="yes";
				$_SESSION['categ']=$FirstAccount;
				$_SESSION['Email']=$Email;
				$_SESSION['Password']=$password;

				$sql="INSERT INTO Users (FK_MasterUsers) VALUES ('$MasterUsersID')";
      			mysql_query($sql) or die("Can not insert into Users ".mysql_error());

  			}
  			header("Location: index.php?section=settings");
  		}
  	}

  	$out.='</td>
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