<?
function changeEmail($output)
{
	global $checkMasterUserUrl,$updateMasterUserUrl;
	$out='';
	if($_SESSION['userIsLogged']!="yes"){
		header("Location: index.php");
		exit();
	}
	
	$display='';
	if(isset($_POST['changeEmail'])){
		
		$Email=$_POST['Email'];
		if (!ereg ("[0-9a-zA-Z]@[0-9a-zA-Z].[0-9a-zA-Z]", $Email))
		$display='Invalid e-mail entered<br><a href="index.php?section=changeEmail">Go back</a>';
		else{
			// query MasterUsers table if the Email exists
			$queryMasterUsers=checkMasterUsers('', '',$checkMasterUserUrl,'&Email='.$_POST['Email']);
			if($queryMasterUsers[0]){
				$display='<span class="err">This email address already exists!</span>';
			}else{
				$updMasterUsers=updateMasterUsers($_SESSION['userID'],'&Email='.$Email,$updateMasterUserUrl);
				if($updMasterUsers==true){
					$display='Your email has been changed!';
					$_SESSION['Email']=$Email;
				}
			}
		}
	}
	
	$out.='
   <style type="text/css">
	.email 
	{
		color:#00A;
	}
	</style>
	    <form action="index.php?section=changeEmail" method="POST" name="form1">		
		<table align="left" cellspacing="0" cellpadding="5" class="insidetable2">
			<tr>
				<td align="left" width="50%"><B>Current E-mail</B></td>
				<td width="50%"><span class="email">'.$_SESSION['Email'].'</span></td>
			</tr>
			<tr>
				<td align="left"><B>New E-mail</B></td>
				<td><input type="text" name="Email"  /></td>
			</tr>
			<tr><td colspan="2" align="left"><input type="submit" name="changeEmail" value="Change E-mail"/></td></tr>
		</table>
	 </form>';


	$out.='
	<table width="100%" border="0" align="right" cellpadding="0" cellspacing="0" class="insidetable2">
	  <tr>
	    <td width="5%"></td>
	    <td width="100%" rowspan="2" valign="top" align="center">'.$display.'</td>
	</tr>
</table>';

	$output->setNavigationMenu(array("Login"=>"index.php?section=login",'Change Email'=>'index.php?section=changeEmail'));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME." :: Change Email");
	$output->output();
}
?>