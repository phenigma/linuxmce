<?
function refer_a_business ($output,$typeReferral=0)
{
	global $addReferrerUrl,$https;
	$out='';
	if($_SESSION['userIsLogged']!="yes")
	{
		$out.="Your login session has expired";
		header("Location: index.php");
	}
	if(isset($_POST['refer'])){
		$queryReferrer=addNewReferrer($_SESSION['userID'],$typeReferral,$_POST['ContactName'],@$_POST['BusinessName'],$_POST['Email'],$_POST['ReferrerNotes'],$addReferrerUrl);
		$emailText="Hi, ".$_POST['ContactName']."<br><br>".$_SESSION['username']." has invited you to join Pluto VIP website.<br>";
		$emailText.="Here are his comments about the website: <hr>".nl2br($_POST['ReferrerNotes'])."<hr>";
		if($typeReferral==1)
			$emailText.='<br><a href="'.$https.'index.php?section=register_b&referrer='.$_SESSION['username'].'">Click here</a> to sign in with Pluto VIP.';
		else
			$emailText.='<br><a href="'.$https.'index.php?section=register&referrer='.$_SESSION['username'].'">Click here</a> to sign in with Pluto VIP.';
		$emailText.="<br>Best regards, Pluto VIP";
		if($queryReferrer==''){
			$referralMsg='Your informations has been saved!';
			mail($_POST['Email'], "From ".$_SESSION['username'].": Invitation to join Pluto VIP", $emailText, "From: Pluto VIP <noreply@plutovip.com>\nX-Priority: 1\nContent-Type: text/html; charset=iso-8859-1\n");
		}
		else 
			$referralMsg=$queryReferrer;
	}

  
  $out.='
  <table width="100%" border="0" align="right" cellpadding="0" cellspacing="0" class="maintable">
  	<tr>
	     <td width="100%" rowspan="2" valign="top" align="left" class="insidetable">
	    <form action="index.php?section='.(($typeReferral==0)?'refer_a_individual':'refer_a_business').'" method="POST" name="form1">
		  <table width="100%" height="100%" cellpadding="0" cellspacing="0" border="0" style="background-color:#ffffff;">
					<tr valign="top"><td>
						<table width="100%" height="100%" cellpadding="0" cellspacing="0" border="0" style="background-color:#ffffff;"><tr valign="top">
				<td align="left"> <img src="images/submenu_txt/refer_a_business_txt.gif" width="121" height="14"><br><br></td></tr>			 
				<tr><td align="center"><br><b>New Referral:<br>'.$referralMsg.'<hr></b></tr>
		      <tr>
				<table cellspacing="3" cellpadding="0" border="0" >
					<tr><th>Contact Name</th><td><input type="text" name="ContactName" value="" /></td></tr>';
  		if($typeReferral==1){
		  	$out.='<tr><th>Business Name</th><td><input type="text" name="BusinessName" value="" /></td></tr>';
  		}
			$out.='	<tr><th>E-mail</th><td><input type="text" name="Email" value="" /></td></tr>
					<tr><th>Notes</th><td><textarea name="ReferrerNotes"></textarea></td></tr>
					<tr><th colspan="2"><input type="submit" value="Refer '.(($typeReferral==0)?'Individual':'Business').'" name="refer" /></th></tr>
				</table>
		      </tr>
				</td>
		  	</tr>
		  </table>
	  </td>
	 </tr>
  </table>
  </form>';

	$output->setScriptCalendar('null');
	$output->setScriptTRColor('null');
	$output->setBody($out);
	$output->referIMG = 'images/home%20page%20img/a_refer_business_on.gif';
	$output->setImgName("img.jpg");
	$output->setTitle(APPLICATION_NAME."::Refer for business");
	$output->setPageID(98); //butonul selectat
	$output->output();
}
?>