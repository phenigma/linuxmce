<?
function register_b($output)
{
	global $addMasterUserUrl,$checkMasterUserUrl;
	$referrer = isset($_REQUEST['referrer'])?cleanString($_REQUEST['referrer']):'';
	$out='';
  	$out.='

<table width="100%" border="0" align="right" cellpadding="0" cellspacing="0" class="maintable">
  <tr>
    <td width="5%"></td>
    <td width="100%" rowspan="2" valign="top" align="left">
  <form action="index.php?section=register_b" method="POST" name="form1">		
	<br><b>Note: You have entered a secure site.<br>All information you provide is encrypted for your protection.<br><br>
		<b><i>Business</b></i> Sign up for Pluto VIP<br>
		If you are an individual, <a href="index.php?section=register">click here</a> to sign up
		<br><br>
<div align="center">		
<table cellspacing="0" cellpadding="3">
			<tr>
				<td align="right">E-mail</td>
				<td><input type="text" name="email" /></td>
				<td>&nbsp;</td>
			</tr>
			<tr>
				<td align="right">Username</td>
				<td><input type="text" name="username" /></td>
				<td class="formextrainfo">Pick a username that you will use to login to the system.<br />You can use your email address as your username if you wish</font></td>
			</tr>
			<tr>
				<td align="right">Referrer</td>
				<td><input type="text" name="referrer" value="'.$referrer.'" /></td>
				<td class="formextrainfo">Type the username who let you know about Pluto VIP.</td>
			</tr>
  			<tr>
				<td align="right">Password</td>
				<td><input type="password" name="password" value="" /></td>
				<td class="formextrainfo">Your password must be at least 6 characters long and must contain both letters and numbers.<br />This is for your own protection.</td>
			</tr>
			<tr>
				<td align="right">Confirm</td>
				<td><input type="password" name="password2" value="" /></td>
				<td class="formextrainfo">please retype your password</td>
			</tr>
			<tr>
				<td align="right">Business Name</td>
				<td><input type="text" name="Name" value="" /></td>
				<td>&nbsp;</td>
			</tr>
			<tr>
				<td align="right">Contact Name</td>
				<td><input type="text" name="Contact" value="" /></td>
				<td>&nbsp;</td>
			</tr>
			<tr>
				<td align="right">Category</td>
				<td><select name="FKID_EstablishmentCategory"><option value="1">Other</option><option value="12">Airline</option><option value="8">Bar</option><option value="4">Bookstore</option><option value="10">Clothing Store</option><option value="6">Coffee Shop</option><option value="5">Fitness Club</option><option value="11">Hotel</option><option value="7">Ice Cream Shop</option><option value="3">Night Club</option><option value="2">Restaurant</option></select></td>
				<td>&nbsp;</td>
			</tr>
			<tr>
				<td align="right">Website (optional) http://</td>
				<td><input type="text" name="Website" value="" /></td>
				<td>&nbsp;</td>
			</tr>
			<tr>
				<th colspan="3"><input type="submit"  name="register_b" value="Sign up" /></th>
			</tr>
		</table>
		<a href="index.php">I have already registered my Business</a>
		</div>
		</form>';
  	if(isset($_POST['register_b'])){
  		$errorsCount=0;
  		$mail=$_POST['email'];
      $pass=strlen($_POST['password']);
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

  		if($_POST['Name']=='' || $_POST['Contact']=='' ||  $_POST['FKID_EstablishmentCategory']==''){
  			$out.='<table align="center"><tr><td align="center" colspan="3"> Please enter all fields.</td></tr></table>';
  			$errorsCount++;
  		}
       if($pass<6) {
  			$out.='<table align="center"><tr><td align="center" colspan="3"> The password is too short.</td></tr></table>';
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

  		$typeUser=1;	// 1 mean his signup was business; later he can add his user account
  		if($errorsCount==0){
	  		$userAddedtoMasterUsers=addtoMasterUsers($typeUser,$mail,$_POST['username'],$referrerID,$pass,$addMasterUserUrl);
	  		if(!$userAddedtoMasterUsers[0]){
	  			$out.='<table align="center"><tr><td align="center" colspan="3"> '.$userAddedtoMasterUsers[1].'</td></tr></table>';
	  			$errorsCount++;
	  		}else{
	  			parse_str($userAddedtoMasterUsers[1]);	// extract $MasterUsersID from $userAddedtoMasterUsers[1]="MasterUsersID=1234"
	  			$insertEstablishment="INSERT INTO Establishment(Name,Contact,Website, FK_MasterUsers ,FKID_EstablishmentCategory ) values ('".$_POST['Name']."','".$_POST['Contact']."','".$_POST['Website']."','".$MasterUsersID."','".$_POST['FKID_EstablishmentCategory']."')";
	  			dbQuery($insertEstablishment);
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
  			}
  			header("Location: index.php?section=settings_b");
  		}
  	}


  	$out.='</td></tr></table>';
  	$output->setScriptCalendar('null');
  	$output->setScriptTRColor('null');
  	$output->setBody($out);
  	$output->setImgName("img.jpg");
  	$output->setTitle(APPLICATION_NAME."::Register");
  	$output->setPageID(98); //butonul selectat
  	$output->output();
}
?>