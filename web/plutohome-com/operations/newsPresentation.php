<?php
 function newsPresentation($output,$conn) {
 		
		
 		$out='<table width="100%" border="0" cellpadding="0" cellspacing="0" class="maintable"><td  valign="top" align="center" >
     <table border="0" align="left" cellpadding="0" cellspacing="0">
     <tr>
    <td class="titletable"><img src="images/submenus/company_txt.gif" width="66" height="13"></td>
    </tr>
       </table></td>
      <tr><td  valign="top" align="center" class="insidetable">
     <table border="0" align="left" cellpadding="0" cellspacing="0">
      <tr>
        <td width="139"><a href="index.php?section=company"><img src="images/submenus/Company/company_overview_off.gif" width="139" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=contact"><img src="images/submenus/Company/contact_off.gif" width="139" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=newsPresentation"><img src="images/submenus/Company/news_on.gif" width="139" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=privacy_policy"><img src="images/submenus/Company/privacy_policy_off.gif" width="139" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td colspan="2"><img src="images/spacer.gif" width="3" height="3"></td>
        </tr>
      <tr>
        <td><a href="index.php?section=site_map"><img src="images/submenus/Company/site_map_off.gif" width="139" height="17" border="0"></a></td>
        <td width="20">&nbsp;</td>
      </tr>
      <tr>
        <td>&nbsp;</td>
        <td>&nbsp;</td>
      </tr>
    </table>
       </tr>
      </td><td width="100%" class="insidetable"><table width="95%" class="newstable"> <tr>
    <br><img src="images/submenus/news_txt.gif" width="40" height="13">
  </tr>';
      $sql="select * from News where markedDeleted!=1 order by newsDate desc";
   $r=mysql_query($sql,$conn) or die("Can not grab from database" .mysql_error());
   $nr_news=5;
   $l=0;
   while($row=mysql_fetch_object($r))
  {  
     $id[$l]=$row->newsID;
     $title[$l]=$row->newsTitle;
     $text[$l]=$row->newsContent;
     $str[$l]=substr($text[$l],0,390);
     $take=substr($text[$l],390,400);
     $t=explode(" ",$take);
     $str[$l]=$str[$l].$t[0];
     $date[$l]=substr($row->newsDate,0,10);
     $l++;
  }
  //$out.=$l;
  $max_page=($l/$nr_news)+1;
  //$out.='<br>'.$max_page;
  if($l>$nr_news&& $l!=0 && !isset($_GET['page'])) 
      {
      
       for($i=0;$i<$nr_news;$i++){
         $out.='
         	<tr>
         		<td><a href="index.php?section=full_story&id='.$id[$i].'" class="newstitle">'.$date[$i].' - <b>'.$title[$i].'</a></b><br>'.$str[$i].'...</td></tr><br>';
       }
           $out.='<tr><td align="center" valign="bottom"> 1 ';
           for($i=2;$i<=$max_page;$i++) $out.='<a href="index.php?section=newsPresentation&page='.$i.'&l='.$l.'"> ' .$i. ' </a>';
           $out.='</td></tr>';
      }
 if($l<=$nr_news && !isset($_GET['page'])){
    for($i=0;$i<$l;$i++){
         $out.='<tr><a href="index.php?section=full_story&id='.$id[$i].'" class="newstitle">'.$date[$i].' - <b>'.$title[$i].'</a></b><br>'.$str[$i].'...</tr><br>';
    }
}
    if(isset($_GET['page']))
  {   
      $j=$_GET['page'];
      $l=$_GET['l'];
      $nr=$nr_news*$j;
      $max_page=($l/$nr_news)+1;
     // $out='';
     // $out.='<table width="100%" border="0" cellpadding="0" cellspacing="0" class="maintable"><td width="20"></td><td class="insidetable">';
      for($i=$nr_news*($j-1);$i<min($nr,$l);$i++) 
      {
        $out.='<tr><a href="index.php?section=full_story&id='.$id[$i].'" class="newstitle">'.$date[$i].' - <b>'.$title[$i].'</a></b><br>'.$str[$i].'...</tr><br>';
      }
       $out.='<tr><td align="center">';
           for($i=1;$i<$j;$i++) $out.='<a href="index.php?section=newsPresentation&page='.$i.'&l='.$l.'"> ' .$i. ' </a>';
           $out.=$j;
           for($i=$j+1;$i<$max_page;$i++) $out.='<a href="index.php?section=newsPresentation&page='.$i.'&l='.$l.'"> ' .$i. ' </a>';
           $out.='</td></tr>';
  }
  $out.='</table></td></table>';
 		
 		$output->setNavigationMenu(array("News"=>"index.php?section=newsPresentation"));
 		
		$output->setImgName('img.jpg');
        $output->setPageID(6);
		$output->setScriptCalendar('null');
		$output->setScriptTRColor('null');		
		$output->setBody($out);
		$output->setTitle(APPLICATION_NAME."::News");			
  		$output->output(); 
 }
?>