<?
function register($output,$conn){
	global $addMasterUserUrl,$checkMasterUserUrl;

	if(isset($_SESSION['userIsLogged']) && $_SESSION['userIsLogged']=="yes"){
		// the user is already logged
		header("Location: index.php?section=myPluto");
	}
	
  	$out='';
	$referrer = isset($_REQUEST['referrer'])?cleanString($_REQUEST['referrer']):'';
	
  	$out.='

   <form action="index.php?section=register" method="POST" name="register">	
<table cellspacing="0" cellpadding="3" align="center"  class="insidetable2">
     		<tr>
				<td align="left" colspan="3" class="insidetable"><img src="images/titles/register.gif" border="0"> <span class="err">'.(isset($_GET['error'])?strip_tags($_GET['error']):'').'</span></td>
			</tr>
			<tr>
				<td align="left"><B>E-mail</B> </td>
				<td><input type="text" class="textfield" name="email" value="" /></td>
				<td class="formextrainfo">Your email address is not verified.  However we would prefer a valid email address--we never share it, our database is secure, and we don\'t send newsletters unless you subscribe.</td>
			</tr>
			<tr>
				<td align="left"><B>Username</B> * </td>
				<td><input type="text" class="textfield" name="username" value="" /></td>
				<td class="formextrainfo">If you get a PlutoHome system, this will also be your username for messaging, VOIP and video phone calls<br/></td>
			</tr>
			<tr>
				<td align="left"><B>Referrer</B> </td>
				<td><input type="text" class="textfield" name="referrer" value="'.$referrer.'" /></td>
				<td class="formextrainfo">Type the username of the person who let you know about Pluto.  We offer generous bonuses and rewards to those who refer both retail and open source users, as well as dealers and manufacturers.</td>
			</tr>
  			<tr>
				<td align="left"><B>Password</B> </td>
				<td><input type="password" class="textfield" name="password" /></td>
				<td class="formextrainfo">&nbsp;</td>
			</tr>
			<tr>
				<td align="left"><B>Confirm</B> </td>
				<td><input type="password" class="textfield" name="password2" /></td>
				<td class="formextrainfo">Please confirm the password</td>
			</tr>
			<tr>
				<td colspan="3" align="left"><input type="image" name="register" src="images/buttons/buton_sign_up.gif" onsubmit="validateEmail(this);"/></th>
			</tr>
    	     <tr>
	    	     <td colspan="3" align="left"><a href="index.php?section=login">I am an Existing User</a></td>
       		</tr>
			<tr>
				<td colspan="3" align="left" class="formextrainfo">* When you register we automatically register you for our forums and bug reports.  You can use your email address as your username, but if you do, your email address will be visible in the forums.</th>
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
 

  	if(isset($_POST['username']))
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
  			parse_str($userAddedtoMasterUsers[1]);
  			$_SESSION['userID'] = $MasterUsersID;
  			$_SESSION['username'] = $_POST['username'];
  			$_SESSION['userIsLogged']="yes";
  			$_SESSION['categ']=$typeUser;
  			
  			$res=mysql_query("SELECT * FROM Users WHERE FK_MasterUsers='$MasterUsersID'",$conn);
			if(mysql_num_rows($res)!=0){
				$res=mysql_fetch_object($res);
				$_SESSION['extPassword']=$res->ExtPassword;
			}
  			header("Location: index.php?section=myPluto");
  		}
  	}

	$output->setNavigationMenu(array("Register"=>"index.php?section=register"));  	
	$output->setBody($out);
  	$output->setTitle(APPLICATION_NAME."::Register");
  	$output->output();
}
?>