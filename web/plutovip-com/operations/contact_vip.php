<?
function contact_vip($output)
{
	global $checkMasterUserUrl;
	$out='';
	if($_SESSION['userIsLogged']!="yes")
	{
		$out.="Your login session has expired";
		header("Location: index.php");
	}
	$hostname = gethostbyname($_SERVER['REMOTE_ADDR']);
	$out.='<table width="100%" border="0" align="right" cellpadding="0" cellspacing="0" class="maintable">
  <tr>
    
    <td width="100%" rowspan="2" valign="top" align="center" class="insidetable">
    <form action="index.php?section=contact_vip" method="POST" name="form1">		<table width="100%" height="100%" cellpadding="0" cellspacing="0" border="0" style="background-color:#ffffff;">
        <tr>
      <td align="left">
      <img src="images/submenu_txt/contact_vip_txt.gif" width="98" height="14"><br><br>
      </td></tr>
			<td align="center">
				<br><table width="100%" height="100%" cellpadding="0" cellspacing="0" border="0" style="background-color:#ffffff;" >
         
					<tr>
						<td align="center" ><b>From:</b></td>
						<td align="left">PlutoVIP '.$_SESSION['username'].' ['.$hostname.']</td>
					</tr>
					<tr>
						<td align="center"><b>To:</b></td>
						<td align="left"> PlutoVIP Username: <input type="text" name="username" value="" /></td>
					</tr>
					<tr><td colspan="2" align="center">
						<br><textarea name="message" cols="70" rows="30"></textarea>
					</td></tr>
					<tr><td colspan="2" align="center">
						<input type="submit" value="Send" name="send" />
					</td></tr>
				</table>
				</td></tr></table></td></tr></table></form>';
	if(isset($_POST['send']))
	{
		// query MasterUsers table for PK_MasterUsers value and Email
		$isMasterUsers=checkMasterUsers($_POST['username'], '',$checkMasterUserUrl,'&Email=');
		
		$errorCount=0;
		if($isMasterUsers[0]){
			parse_str($isMasterUsers[1]);	// now I set the $MasterUsersID and Email variables
			$PK_MasterUsers=$MasterUsersID;	// inconsistency because curl act weird when receive underscore in parameters name
		}
		else
			$errorCount++;	// user not found or MasterUsers Database error

		$sql="insert into MessageLog (FK_MasterUsers_From ,FK_MasterUsers_To , IpAddress, Message) values('".$_SESSION['userID']."','".$PK_MasterUsers."','".$hostname."','".$_POST['message']."')";
		mysql_query($sql) or die("Can not insert into MessageLog  ".mysql_error());

		$headers = "From: PlutoVIP <website@plutohome.com>\r\nX-Priority: 1\r\nContent-Type: text/html; charset=iso-8859-1\r\n";

		$message='Hi '.$_POST['username'].',<br>';
		$message.=$_SESSION['username'].' has posted an message for you.<br>';
		$message.='<hr>'.nl2br($_POST['message']).'</hr>';
		$message.='Message sent from PlutoVip Website';

		mail($Email,"Pluto VIP: New message from ".$_SESSION['username'],$message,$headers);
		$out='';
		$out.='Your message has been sent';
	}
	
	$output->setScriptCalendar('null');
	$output->setScriptTRColor('null');
	$output->contactIMG = 'images/home%20page%20img/a_contact_vip_on.gif';
	$output->setBody($out);
	$output->setImgName("img.jpg");
	$output->setTitle(APPLICATION_NAME."::Contact a VIP");
	$output->setPageID(98); //butonul selectat
	$output->output();
}
?>