<?
function news($output){
  $out = '';
		
$out.='

<table width="100%" border="0" align="right" cellpadding="0" cellspacing="0" class="maintable">
  <tr>
    <td class="titletable" align="left">
    </td>
    <td width="224" rowspan="2" valign="top" bgcolor="#F2F3F7" align="left" ><table width="100%" border="0" cellspacing="0" cellpadding="0">
    <tr>
        <td><img src="images/home page img/login.jpg" width="251" height="23"></td>
      </tr>
    <tr><td width="244" class="td_main"><table width="224"  border="0" cellpadding="0" cellspacing="0" bgcolor="#F1F3F6">';
    session_start();
    if($_SESSION['userIsLogged']=="yes")
  {
      $out.='<tr><table><td><img src="images/home page img/logged_in_as.gif" align="middle"></td><td> <b>'.$_SESSION['username'].'</b></td></td></tr></table>';
  }
   else  
  {
   $out.='   
      <tr>
      <form name="form1" id="form1" action="'.$https.'index.php" method="POST">
     <input type="hidden" name="section" value="login">
      <td><table width="100%"  border="0" cellspacing="0" cellpadding="0">
            <tr>
              <td align="right"><input type="text" name="username" class="inputlogin"></td>
              <td width="71"><img src="images/home page img/l_username.gif" width="71" height="16"></td>
            </tr>
        </table></td>
      </tr>
     <tr>
        <td><img src="images/home page img/spacer_1.gif" width="4" height="4"></td>
      </tr
     <tr>
      <td><table width="100%"  border="0" cellspacing="0" cellpadding="0">
            <tr>
              <td align="right"><input type="password" name="password" class="inputlogin"></td>
              <td width="71"><img src="images/home page img/l_password.gif" width="71" height="16"></td>
            </tr>
        </table></td>
     </tr>
     <tr>
        <td><img src="images/home page img/spacer_1.gif" width="4" height="4"></td>
      </tr>
     <tr>
     <td ><table width="100%"  border="0" cellspacing="0" cellpadding="0">
            <tr>
              <td width="50"><input type="image" src="images/home page img/l_sign_in.gif"  name="submitX"></td>
              <td><a href="'.$https.'index.php?section=forgot"><img src="images/home page img/l_forgot_pass.gif" width="114" height="14" border="0"></a></td>
            </tr>
        </table></td>
     </tr>
     <tr>
        <td><img src="images/home page img/spacer_1.gif" width="4" height="4"></td>
      </tr>
     <tr>
        <td><table width="100%"  border="0" cellspacing="0" cellpadding="0">
            <tr>
              <td width="62"><img src="images/home page img/l_sign_up.gif" width="62" height="17"></td>
              <td width="87"><a href="'.$https.'index.php?section=register"><img src="images/home page img/l_individuals.gif" width="87" height="17" border="0"></a></td>
              <td><a href="'.$https.'index.php?section=register_b"><img src="images/home page img/l_business.gif" width="76" height="17" border="0"></a></td>
            </tr>
        </table></td>
      </tr>
        
    </table>
  </td>
  </tr> ';
  }
      $out.=' <tr>
        <td align="left">
        <embed src="flash/default.swf" quality="high" pluginspage="http://www.macromedia.com/go/getflashplayer" type="application/x-shockwave-flash" width="100%" height="90" align="center"></embed>
        </td>
        </tr>
      <tr>
        <td><img src="images/home page img/news.jpg" width="251" height="23"></td>
      </tr></form>';
  
   $sql="select * from News  where markedDeleted!=1 order by newsDate desc limit 0,1";
   $r=mysql_query($sql) or die("Can not grab from database" .mysql_error());
   while($row=mysql_fetch_object($r))
  {
     $text=$row->newsContent;
     $str=substr($text,0,699);
     $take=substr($text,699,700);
     $t=explode(" ",$take);
     $str=$str.$t[0];
     $date=substr($row->newsDate,0,10);
     //$str=strip_tags($str);
     $title=strip_tags($row->newsTitle);
     $out.='<tr><td class="td_main"><table><tr><td><a href="index.php?section=full_story&id='.$row->newsID.'" class="newstitle">'.$date[5].$date[6].'.'.$date[8].$date[9].'.'.$date[0].$date[1].$date[2].$date[3].' - <b>'.$title.'</a><b></td></tr><tr><td >'.$str.'...<br></td></tr></table></td></tr>';
  }

    $out.=<<<LOL
      <tr>
        <td><img src="images/home page img/www_plutohome_com.jpg" width="251" height="23"></td>
      </tr>
      <tr>
      <td class="td_main">Simplify your life with a Pluto system!
       Use your mobile phone as a remote 
       control for everything in your home!<br><br>

        The only system with functions that
        allows you to:<br>
        > Protect your home and family 24/7 
        from anywhere in the world<br>
        > Have Voice-over-ip communications, 
         video conferencing, multiple homes 
         joined as one<br>
         > Turn your mobile phone into a remote<br>
         > Record and play PVR, whole-home
         music & movie server, DVD\'s<br>
        > Run Windows and Linux seamlessly, 
        with a simple mode for the novice<br><br>
  
        More details at  www.plutohome.com 
        Take a moment to see what Brilliant 
        Living is all about!
      </td></tr>
 </table>      </td>
  </tr>
  <tr>
    <td  class="td_main" valign="top"><table width="100%"  border="0" cellspacing="0" cellpadding="0" >
  <tr>
    <td align="left"><img src="images/home%20page%20img/simplify_your_life_txt.gif" width="502" height="49"></td>
  </tr>
  <tr>
    <td>&nbsp;</td>
  </tr>
  <tr>
    <td align="left">Just carry your mobile phone...<BR>
      As you enter a business, they recognize you without a member card, and you can conduct secure transactions on your bluetooth mobil phone.<BR>
It\'s effortles, safe, and anonymous. </td>
  </tr>
  <tr>
    <td>&nbsp;</td>
  </tr>
  <tr>
    <td><table width="100%"  border="0" cellspacing="0" cellpadding="0">
      <tr>
        <td width="387" valign="top"><table width="387"  border="0" cellspacing="0" cellpadding="0">
          <tr>
            <td width="387"><a href="index.php?section=individuals"><img src="images/home%20page%20img/benefits_for_individuals.jpg" width="387" height="133" border="0"></a></td>
          </tr>
          <tr>
            <td><a href="index.php?section=businesses"><img src="images/home%20page%20img/benefits_for_business.jpg" width="387" height="134" border="0"></a></td>
          </tr>
        </table></td>
        <td width="16">&nbsp;</td>
        <td align="left" valign="top"><table width="197"  border="0" cellspacing="0" cellpadding="0">
          <tr>
            <td><img src="images/home%20page%20img/what_u_need.jpg" width="197" height="17"></td>
          </tr>
          <tr>
            <td><img src="images/home%20page%20img/h_table_1.gif" width="197" height="9"></td>
          </tr>
          <tr>
            <td class="table_small"><span >As an individual, you only need a mobil phone that supports Bluetooth--a common feature.<br>
                <br>
A business need a PlutoVIP software and a blutooth compatible cash register, or a sheap adapter that works with most existing once. It takes about 10 minutes to setup.&nbsp;<br>
Both you and the business need to register at plutovip.com, which takes about 2 minutes.<br>
<br>
</span></td>
          </tr>
          <tr>
            <td><img src="images/home%20page%20img/h_table_2.gif" width="197" height="9"></td>
          </tr>
        </table></td>
      </tr>
    </table></td>
  </tr>
  <tr>
    <td>&nbsp;</td>
  </tr>
  <tr>
    <td align="left"><img src="images/home%20page%20img/plutovip_is_a_service_txt.gif" width="255" height="13"></td>
  </tr>
  <tr>
    <td>&nbsp;</td>
  </tr>
  <tr>
    <td><table width="100%"  border="0" cellspacing="0" cellpadding="0">
      <tr>
        <td width="141" valign="top"><img src="images/home%20page%20img/main_menu.jpg" width="142" height="112"></td>
        <td width="841" align="left"><table width="448"  border="0" cellspacing="0" cellpadding="0">
          <tr>
            <td><img src="images/home%20page%20img/h_table_3.gif" width="458" height="13"></td>
          </tr>
          <tr>
            <td class="table_small"><img src="images/home%20page%20img/t_total_control.gif" width="90" height="12"></td>
          </tr>
          <tr>
            <td class="table_small">Control everything in your home from anywhere in the world with your mobile phone. When you're home, Pluto even knows which family members are in which rooms, so the menu on the mobile phone is the simplest, clearest, possible. Example: Just press 1 button to always get the lights in whatever room you're in.</td>
          </tr>
          <tr>
            <td><img src="images/home%20page%20img/h_table_4.gif" width="458" height="13"></td>
          </tr>
        </table></td>
      </tr>
      <tr>
        <td width="141"><img src="images/home%20page%20img/spacer.gif" width="5" height="5"></td>
        <td><img src="images/home%20page%20img/spacer.gif" width="5" height="5"></td>
      </tr>
      <tr>
        <td width="141"><img src="images/home%20page%20img/alert.jpg" width="142" height="112"></td>
        <td align="left" valign="top"><table width="448"  border="0" cellspacing="0" cellpadding="0">
          <tr>
            <td><img src="images/home%20page%20img/h_table_3.gif" width="458" height="13"></td>
          </tr>
          <tr>
            <td class="table_small"><img src="images/home%20page%20img/t_security.gif" width="93" height="12"></td>
          </tr>
          <tr>
            <td class="table_small">From anywhere in the world, your mobile phone alerts you with a live video feed the instant anything happens at home, and even turns your whole house into a giant speakerphone so you can speak with anybody there. You are always in full control, and can personally protect your home and family.<br>
              <br></td>
          </tr>
          <tr>
            <td><img src="images/home%20page%20img/h_table_4.gif" width="458" height="13"></td>
          </tr>
        </table></td>
      </tr>
      <tr>
        <td width="141"><img src="images/home%20page%20img/spacer.gif" width="5" height="5"></td>
        <td><img src="images/home%20page%20img/spacer.gif" width="5" height="5"></td>
      </tr>
      <tr>
        <td width="141"><img src="images/home%20page%20img/dvd.jpg" width="142" height="112"></td>
        <td align="left" valign="top"><table width="448"  border="0" cellspacing="0" cellpadding="0">
          <tr>
            <td><img src="images/home%20page%20img/h_table_3.gif" width="458" height="13"></td>
          </tr>
          <tr>
            <td class="table_small"><img src="images/home%20page%20img/t_dvd.gif" width="93" height="12"></td>
          </tr>
          <tr>
            <td class="table_small">Your mobile phone becomes the most comfortable, uncomplicated, reliable and intelligent remote control. Pluto can even direct your media, lighting + climate preferences, phone calls, etc. to follow you as you move through your house--automatically, without you doing a thing.<br>
              <br></td>
          </tr>
          <tr>
            <td><img src="images/home%20page%20img/h_table_4.gif" width="458" height="13"></td>
          </tr>
        </table></td>
      </tr>
    </table></td>
  </tr>
  
  <tr>
    <td align="left">Details about Pluto Home at<a href="http://www.plutohome.com"> <B>www.plutohome.com</B></a></td>
  </tr>
</table>
    </table>
        
LOL;


		$output->setScriptCalendar('null');
		$output->setScriptTRColor('null');		
		$output->setBody($out);
		$output->setImgName("img.jpg");
		$output->setTitle(APPLICATION_NAME);			
      $output->setPageID(98); //butonul selectat
  		$output->output(); 
      
}
        ?>
