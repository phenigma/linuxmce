<?
function bchange_email($output)
{
	global $checkMasterUserUrl,$updateMasterUserUrl;
	$out='';
	if($_SESSION['userIsLogged']!="yes")
	{
		$out.="Your login session has expired";
		header("Location: index.php");
	}
	if(isset($_POST['bchange_email'])){
		$display='';
		$Email=$_POST['Email'];
		if (!ereg ("[0-9a-zA-Z]@[0-9a-zA-Z].[0-9a-zA-Z]", $Email))
		$display='Invalid e-mail entered<br><a href="index.php?section=bchange_email">Go back</a>';
		else{
			// query MasterUsers table if the Email exists
			$queryMasterUsers=checkMasterUsers('', '',$checkMasterUserUrl,'&Email='.$_POST['Email']);
			if($queryMasterUsers[0]){
				$display='This email address already exist!';
			}else{
				$updMasterUsers=updateMasterUsers($_SESSION['userID'],'&Email='.$Email,$updateMasterUserUrl);
				if($updMasterUsers){
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
   <table width="100%" border="0" align="right" cellpadding="0" cellspacing="0" class="maintable">
	  <tr>
	   
	    <td width="100%" rowspan="2" valign="top" align="left" class="insidetable">
	    <form action="index.php?section=bchange_email" method="POST" name="form1">		
		<table align="left" cellspacing="0" cellpadding="5">
	      <tr>
	        <td align="left">
	         <img src="images/submenu_txt/change_email_txt.gif" width="211" height="14">
	        </td>
	        </tr>
				<tr>
					<th align="right" width="50%">Current E-mail</th>
					<td width="50%"><span class="email">'.$_SESSION['Email'].'</span></td>
				</tr>
				<tr>
					<th align="right">New E-mail</th>
					<td><input type="text" name="Email"  /></td>
				</tr>
				<tr><td colspan="2" align="center"><input type="submit" name="bchange_email" value="Change E-mail"/></td></tr>
			</table>
			</td>
		   </tr>
		  </table>
	 </form>';


	$out.='
	<table width="100%" border="0" align="right" cellpadding="0" cellspacing="0" class="maintable">
	  <tr>
	    <td width="5%"></td>
	    <td width="100%" rowspan="2" valign="top" align="center">'.$display.'</td>
	</tr>
</table>';

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