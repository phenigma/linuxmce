<?
function change_pass($output)
{
	global $changePassMasterUserUrl;
	$out='';
	if($_SESSION['userIsLogged']!="yes")
	{
		$out.="Your login session has expired";
		header("Location: index.php");
	}

	  
   $out.='<table width="100%" border="0" align="right" cellpadding="0" cellspacing="0" class="maintable">
  <tr>
    
    <td width="100%" rowspan="2" valign="top" align="left" class="insidetable">
    <form action="index.php?section=change_pass" method="POST" name="form1">			
      
		<table align="left" cellspacing="0" cellpadding="5" >
        <tr>
        <td align="left"><img src="images/submenu_txt/change_password_txt.gif" width="240" height="14"><br><br></td>
        </tr>
			<tr>
				<th align="right" width="50%">Current Password</th>
				<td width="50%"><input type="password" name="current" /><br></td>
			</tr>
			<tr>
				<th align="right">New Password</th>
				<td><input type="password" name="password" /><br></td>
			</tr>
			<tr>
				<th align="right">Confirm New Password</th>
				<td><input type="password" name="confirm" /><br></td>
			</tr>
			<tr><td colspan="2" align="center"><input type="submit" name="change_password" value="Change Password" /></td></tr>
		</table>
		</td></tr></table></form>';

	   if(isset($_POST['change_password'])){
	   	$new_pass=$_POST['password'];
	   	$updMasterUsers=updateMasterUsersPassword($_SESSION['userID'],$new_pass,$changePassMasterUserUrl,$_SESSION['Password']);
	   	if($updMasterUsers[0]){
	   		$_SESSION['Password']=$new_pass;
	   		$out='';
	   		$out.='
			   <table width="100%" border="0" align="right" cellpadding="0" cellspacing="0" class="maintable">
			  		<tr>
			    		<td width="5%"></td>
			    		<td width="100%" rowspan="2" valign="top" align="center"><br>Your password has been changed!
			    	</td>
			    </tr>
			   </table>';
	   	}
	   }
	
	   $output->setScriptCalendar('null');
	   $output->setScriptTRColor('null');
	   $output->settingsIMG = 'images/home%20page%20img/a_my_settings_on.gif';
	   $output->setBody($out);
	   $output->setImgName("img.jpg");
	   $output->setTitle(APPLICATION_NAME."::Contact a VIP");
	   $output->setPageID(98); //butonul selectat
	   $output->output();
}
?>