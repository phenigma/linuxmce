<?
function news($output,$conn){
  $out = '';
		
$out.='

<table width="100%" border="0" align="right" cellpadding="0" cellspacing="0" class="maintable">
  <tr>
    <td class="titletable"><img src="images/take_a_pluto.gif" width="550" height="53"></td>
    <td width="251" rowspan="2" valign="top" bgcolor="#F2F3F7"><table width="100%" border="0" cellspacing="0" cellpadding="0">
<tr>
    <td>
	<img src="images/3ways.gif" width="251" height="23">
	</td>
</tr>

<tr>
    <td class="counter">1. Smart-home and hi-fi dealers offering custom installation and a turnkey solution<br></td>
</tr>

<tr>
    <td class="counter" align="left">
	<a href="index.php?section=locator"><b>dealer locator</b></a>
	</td>
</tr>

<tr>
    <td class="counter">2.  Third party products that work with Pluto<br></td>
</tr>

<tr>
    <td class="counter" align="left">
	<a href="index.php?section=get_pluto"><b>tell me more</b></a>
	</td>
</tr>

<tr>
    <td class="counter">3.  Use your own computers, and Pluto gives you the software <b>FREE</b>!<br></td>
</tr>

<tr>
    <td class="counter" align="left">
	<a href="support/index.php?section=document&docID=1"><b>get the software</b></a>
	</td>
</tr>

      <tr>
        <td><img src="images/m_news.gif" width="251" height="23"></td>
      </tr>';
   $sql="select * from News  where markedDeleted!=1 order by newsDate desc limit 0,2";
   $r=mysql_query($sql,$conn) or die("Can not grab from database" .mysql_error());
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
     $out.='<tr><td class="news"><a href="index.php?section=full_story&id='.$row->newsID.'" class="newstitle">'.$date[5].$date[6].'.'.$date[8].$date[9].'.'.$date[0].$date[1].$date[2].$date[3].' - <b>'.$title.'</a><b></td></tr><tr><td class="news">'.$str.'...<br></td></tr>';
  }

    $out.=<<<LOL
 </table>      </td>
  </tr>
  <tr>
    <td class="insidetable"><table width="100%"  border="0" cellpadding="0" cellspacing="0">
      <tr>
        <td><table width="100%" border="0" align="left" cellpadding="0" cellspacing="0" class="ftable">
          <tr>
            <td width="166"><a href="index.php?section=security"><img src="images/submenus/Pluto%20benefits/security_off.gif" width="166" height="17" border="0"></a></td>
            <td width="15">&nbsp;</td>
            <td>&bull; Protect your home and family 24/7 from anywhere in the world</td>
          </tr>
          <tr>
            <td colspan="3"><img src="images/spacer.gif" width="3" height="3"></td>
          </tr>
          <tr>
            <td><a href="index.php?section=telecom"><img src="images/submenus/Pluto%20benefits/telecom_off.gif" width="166" height="17" border="0"></a></td>
            <td width="15">&nbsp;</td>
            <td>&bull; Voice-over-ip, video conferencing, multiple homes joined as one</td>
          </tr>
          <tr>
            <td colspan="3"><img src="images/spacer.gif" width="3" height="3"></td>
          </tr>
          <tr>
            <td><a href="index.php?section=home_automation"><img src="images/submenus/Pluto%20benefits/home_automation_off.gif" width="166" height="17" border="0"></a></td>
            <td width="15">&nbsp;</td>
            <td width="508">&bull; A mobile phone turns into a remote for everything from lights to sprinklers. </td>
          </tr>
          <tr>
            <td colspan="3"><img src="images/spacer.gif" width="3" height="3"></td>
          </tr>
          <tr>
            <td><a href="index.php?section=media_entertainment"><img src="images/submenus/Pluto%20benefits/media_entertainment_off.gif" width="166" height="17" border="0"></a></td>
            <td width="15">&nbsp;</td>
            <td>&bull; PVR, whole-home music &amp; movie server, DVD recorder/player</td>
          </tr>
          <tr>
            <td colspan="3"><img src="images/spacer.gif" width="3" height="3"></td>
          </tr>
          <tr>
            <td valign="top"><a href="index.php?section=personal_computing"><img src="images/submenus/Pluto%20benefits/personal_computing_off.gif" width="166" height="17" border="0"></a></td>
            <td width="15">&nbsp;</td>
            <td class="ftable">&bull; Run Windows and Linux seamlessly, with a simple mode for the novice</td>
          </tr>
          <tr>
            <td colspan="3"><img src="images/spacer.gif" width="3" height="3"></td>
          </tr>
        </table></td>
      </tr>
      <tr>
        <td class="text"><br>
No other single product combines more than 2 of our 5 core functions.  Without Pluto you&rsquo;re stuck with a hodge-podge of incompatible &ldquo;stuff&rdquo; that all works differently and can never function as a cohesive whole.  With Pluto control everything effortlessly with our touch-screen tablets or your own mobile phone.  Pluto even knows how to make each family member's media and personal preferences follow him around the house.  There are many revolutionary benefits to Pluto&rsquo;s all in one approach.  Take a moment to see what Brilliant Living is all about. <br>
<br></td>
      </tr>
      <tr>
        <td><table width="100%" border="0" cellspacing="0" cellpadding="0">
          <tr>
            <td>
              <table width="100%"  border="0" cellspacing="0" cellpadding="0">
                <tr>
                  <td width="146"><img src="images/the_pluto_system.gif" width="146" height="22"></td>
                  <td><a href="javascript:open_popup('operations/flash.html','797','524')"><img src="images/view_flash_presentation.gif" width="160" height="22" border="0"></a></td>
                </tr>
              </table></td>
          </tr>
          <tr>
            <td align="center"><table width="150" border="0" cellspacing="0" cellpadding="0">
                <tr>
                  <td width="141"><a href="index.php?section=core"><img src="images/m_core.jpg" width="141" height="209" border="0"></a></td>
                  <td width="10"><img src="images/spacer.gif" width="10" height="3"></td>
                  <td width="141"><a href="index.php?section=media_director"><img src="images/m_media.jpg" width="141" height="209" border="0"></a></td>
                  <td width="10"><p><img src="images/spacer.gif" width="10" height="3"></p>
                      <p>&nbsp;</p></td>
                  <td><a href="index.php?section=orbiter"><img src="images/m_orbiter.jpg" width="141" height="209" border="0"></a></td>
                  <td><img src="images/spacer.gif" width="10" height="3"></td>
                  <td><a href="index.php?section=mobile_orbiter"><img src="images/m_remote.jpg" width="141" height="209" border="0"></a></td>
                </tr>
            </table></td>
          </tr>
        </table></td>
      </tr>
      
    </table>
    </td>
  </tr>
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
