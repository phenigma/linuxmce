<?
function info_business($output)
{
  $out='';
  $i=0;
  if($_SESSION['userIsLogged']!="yes")
  {
        $out.="Your login session has expired";
        header("Location: index.php");
  }
   $sql1="select * from Users_EstablishmentCategory where FK_MasterUsers_User='".$_SESSION['userID']."' ";
      $r1=mysql_query($sql1);
     
  
  $out.='<table width="100%" border="0" align="right" cellpadding="0" cellspacing="0" class="maintable">
  <tr>
   
    <td width="100%" rowspan="2" valign="top" align="left" class="insidetable">
    <form action="index.php?section=info_business" method="POST" name="form1">	
    <table>
      <tr><td align="left"><img src="images/submenu_txt/info_businesses_txt.gif" width="253" height="14"></td></tr>
			<tr><th colspan="3"><br>Information to share with businesses by category.<br>List the things you would like each type of business  to know<br>about you when you enter (clothing sizes, favorite foods, etc.).</th></tr>
							<tr>';
     $sql1="select * from Users_EstablishmentCategory where FK_MasterUsers_User='".$_SESSION['userID']."' and FKID_EstablishmentCategory='1'";
      $r1=mysql_query($sql1);
      if(mysql_num_rows($r1)!=0)
      $row=mysql_fetch_object($r1);
					$out.='<td align="right">Other</td>
					<td><textarea name="Users_EstablishmentCategory1ShareText" >'.$row->ShareText.'</textarea></td>
				</tr>
								<tr>';
                $sql1="select * from Users_EstablishmentCategory where FK_MasterUsers_User='".$_SESSION['userID']."' and FKID_EstablishmentCategory='2'";
      $r1=mysql_query($sql1);
      if(mysql_num_rows($r1)!=0)
      $row=mysql_fetch_object($r1);
					$out.='<td align="right">Restaurant</td>
					<td><textarea name="Users_EstablishmentCategory2ShareText"  >'.$row->ShareText.'</textarea></td>
				</tr>
								<tr>';
                $sql1="select * from Users_EstablishmentCategory where FK_MasterUsers_User='".$_SESSION['userID']."' and FKID_EstablishmentCategory='3'";
      $r1=mysql_query($sql1);
      if(mysql_num_rows($r1)!=0)
      $row=mysql_fetch_object($r1);
					$out.='<td align="right">Night Club</td>
					<td><textarea name="Users_EstablishmentCategory3ShareText" >'.$row->ShareText.'</textarea></td>
				</tr>
								<tr>';
                $sql1="select * from Users_EstablishmentCategory where FK_MasterUsers_User='".$_SESSION['userID']."' and FKID_EstablishmentCategory='4'";
      $r1=mysql_query($sql1);
      if(mysql_num_rows($r1)!=0)
      $row=mysql_fetch_object($r1);
					$out.='<td align="right">Bookstore</td>
					<td><textarea name="Users_EstablishmentCategory4ShareText" >'.$row->ShareText.'</textarea></td>
				</tr>
								<tr>';
                $sql1="select * from Users_EstablishmentCategory where FK_MasterUsers_User='".$_SESSION['userID']."' and FKID_EstablishmentCategory='5'";
      $r1=mysql_query($sql1);
      if(mysql_num_rows($r1)!=0)
      $row=mysql_fetch_object($r1);
					$out.='<td align="right">Fitness Club</td>
					<td><textarea name="Users_EstablishmentCategory5ShareText" >'.$row->ShareText.'</textarea></td>
				</tr>
								<tr>';
                $sql1="select * from Users_EstablishmentCategory where FK_MasterUsers_User='".$_SESSION['userID']."' and FKID_EstablishmentCategory='6'";
      $r1=mysql_query($sql1);
      if(mysql_num_rows($r1)!=0)
      $row=mysql_fetch_object($r1);
					$out.='<td align="right">Coffee Shop</td>
					<td><textarea name="Users_EstablishmentCategory6ShareText" >'.$row->ShareText.'</textarea></td>
				</tr>
								<tr>';
                $sql1="select * from Users_EstablishmentCategory where FK_MasterUsers_User='".$_SESSION['userID']."' and FKID_EstablishmentCategory='7'";
      $r1=mysql_query($sql1);
      if(mysql_num_rows($r1)!=0)
      $row=mysql_fetch_object($r1);
					$out.='<td align="right">Ice Cream Shop</td>
					<td><textarea name="Users_EstablishmentCategory7ShareText" >'.$row->ShareText.'</textarea></td>
				</tr>
								<tr>';
                $sql1="select * from Users_EstablishmentCategory where FK_MasterUsers_User='".$_SESSION['userID']."' and FKID_EstablishmentCategory='8'";
      $r1=mysql_query($sql1);
      if(mysql_num_rows($r1)!=0)
      $row=mysql_fetch_object($r1);
					$out.='<td align="right">Bar</td>
					<td><textarea name="Users_EstablishmentCategory8ShareText" >'.$row->ShareText.'</textarea></td>
				</tr>
								<tr>';
            $sql1="select * from Users_EstablishmentCategory where FK_MasterUsers_User='".$_SESSION['userID']."' and FKID_EstablishmentCategory='10'";
      $r1=mysql_query($sql1);
      if(mysql_num_rows($r1)!=0)
      $row=mysql_fetch_object($r1);
					$out.='<td align="right">Clothing Store</td>
					<td><textarea name="Users_EstablishmentCategory10ShareText"  >'.$row->ShareText.'</textarea></td>
				</tr>
								<tr>';
                $sql1="select * from Users_EstablishmentCategory where FK_MasterUsers_User='".$_SESSION['userID']."' and FKID_EstablishmentCategory='11'";
      $r1=mysql_query($sql1);
      if(mysql_num_rows($r1)!=0)
      $row=mysql_fetch_object($r1);
					$out.='<td align="right">Hotel</td>
					<td><textarea name="Users_EstablishmentCategory11ShareText"  >'.$row->ShareText.'</textarea></td>
				</tr>
								<tr>';
                $sql1="select * from Users_EstablishmentCategory where FK_MasterUsers_User='".$_SESSION['userID']."' and FKID_EstablishmentCategory='12'";
      $r1=mysql_query($sql1);
      if(mysql_num_rows($r1)!=0)
      $row=mysql_fetch_object($r1);
					$out.='<td align="right">Airline</td>
					<td><textarea name="Users_EstablishmentCategory12ShareText"  >'.$row->ShareText.'</textarea></td>
				</tr>';

  $out.='
							<tr><td colspan="3" align="right"><input type="submit" value="update" name="update"/></td></tr>
		</table>
		</div>
		</td></tr></table></form>';
    if(isset($_POST['update']))
  {
      $sql1="select * from Users_EstablishmentCategory where FK_MasterUsers_User='".$_SESSION['userID']."'";
      $r1=mysql_query($sql1);
      if(mysql_num_rows($r1)==0)
    {
        for($i=1;$i<=12;$i++)
      {
          $sql="insert into Users_EstablishmentCategory (FK_MasterUsers_User,FKID_EstablishmentCategory,ShareText) values('".$_SESSION['userID']."','".$i."','".$_POST['Users_EstablishmentCategory'.$i.'ShareText']."')";
          mysql_query($sql) or die("Can not insert into Users_EstablishmentCategory ".mysql_error());
      }
    }
    else
    {
      for($i=1;$i<=12;$i++)
      {
      $sql="update Users_EstablishmentCategory set ShareText='".$_POST['Users_EstablishmentCategory'.$i.'ShareText']."' where FK_MasterUsers_User='".$_SESSION['userID']."' and FKID_EstablishmentCategory='".$i."'";
      mysql_query($sql) or die("Can not insert into Users_EstablishmentCategory ".mysql_error());
      }
    }
    $out='';
         $out.='<table width="100%" border="0" align="right" cellpadding="0" cellspacing="0" class="maintable">
  <tr>
    <td width="5%"></td>
    <td width="100%" rowspan="2" valign="top" align="center">Your informations has been changed!
    </td></tr></table>';
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