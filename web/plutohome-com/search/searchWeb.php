<?
function searchWeb($output,$conn){

$k=0;
$j=0;
$nr_link=15;
$nr_page=0;
error_reporting(0);
  $out='';
	
	$out.='
	<table width="100%" border="0" cellpadding="0" cellspacing="0" class="maintable">
		<tr>
			<td width="20"></td>
			<td class="insidetable">';
   $name_array=array();
   
  $search=0;
  $k=0;
  $l=0;
  $cuv=isset($_REQUEST['searchString'])?$_REQUEST['searchString']:$_REQUEST['cuv'];
  
  $sql="select PageContentID,PageContenttext,PageName from PageContent where PageContenttext like '%".$cuv."%'";
  $r=mysql_query($sql,$conn) or die("Can not grab from database"  .mysql_error());
  while($rows=mysql_fetch_array($r))
  { 
    
    $text=strip_tags($rows[1]);
    if(preg_match("/".$cuv."/i",$text))
    {
    $pos=strpos($text,$cuv);
    $desc=substr($text,$pos-80,$pos-($pos-80));
    $cut=explode(" ",$desc);
    $desc=$cut[1];
    for($i=2;$i<strlen($cut);$i++) $desc.=' '.$cut[$i];
    $desc1=substr($text,$pos+strlen($cuv),150);
    $cut1=explode(" ",$desc1);
    $desc1=$cut1[1];
    for($i=2;$i<strlen($cut1);$i++) $desc1.=' '.$cut1[$i];
    $search=1;
    }
    else $search=0;
    if($search==1)
    {
      $l++;
    $linkuri[$l]=$rows[2];
    $descriere[$l]='...'.$desc.' <b> '.$cuv.' </b> '.$desc1.'...';
    }
   }
  $sql="select faqID,faqFAQCategID from FAQ where faqQuestion like '%".$cuv."%' or faqAnswer like '%".$cuv."%' or faqDetails like '%".$cuv."%'";
  $r=mysql_query($sql,$conn) or die("Can not grab from database"  .mysql_error());;
  if(mysql_num_rows($r)!=0)
{ $search=1;
  $row=mysql_fetch_object($r);
  $text=strip_tags($row->faqAnswer);
  $l++;
  if(preg_match("/".$cuv."/i",$text))
     {
    $pos=strpos($text,$cuv);
    $desc=substr($text,$pos-80,$pos-($pos-80));
    $cut=explode(" ",$desc);
    $desc=$cut[1];
    for($i=2;$i<strlen($cut);$i++) $desc.=' '.$cut[$i];
    $desc1=substr($text,$pos+strlen($cuv),150);
    $cut1=explode(" ",$desc1);
    $desc1=$cut1[1];
    for($i=2;$i<strlen($cut1);$i++) $desc1.=' '.$cut1[$i];
     }
   else {
     $text=strip_tags($row->faqDetails);
     $pos=strpos($text,$cuv);
    $desc=substr($text,$pos-80,$pos-($pos-80));
    $cut=explode(" ",$desc);
    $desc=$cut[1];
    for($i=2;$i<strlen($cut);$i++) $desc.=' '.$cut[$i];
    $desc1=substr($text,$pos+strlen($cuv),150);
    $cut1=explode(" ",$desc1);
    $desc1=$cut1[1];
    for($i=2;$i<strlen($cut1);$i++) $desc1.=' '.$cut1[$i];
    
   }
  $descriere[$l]='...'.$desc.'<b> '.$cuv.' </b>'.$desc1.'...';
  $linkuri[$l]="faq_presentation";
  }
  $sql1="select newsID,newsNewsCategID,newsContent,newsTitle from News where newsContent like '%".$cuv."%' or newsTitle like '%".$cuv."%' ";
  $r1=mysql_query($sql1,$conn) or die("Can not grab from database!!!"  .mysql_error());
  if(mysql_num_rows($r1)!=0)
   { $search=1;
  $row1=mysql_fetch_object($r1);
  $text=strip_tags($row->newsContent);
  $l++;
  if(preg_match("/".$cuv."/i",$text))
     {
    $pos=strpos($text,$cuv);
    $desc=substr($text,$pos-80,$pos-($pos-80));
   $cut=explode(" ",$desc);
    $desc=$cut[1];
    for($i=2;$i<strlen($cut);$i++) $desc.=' '.$cut[$i];
    $desc1=substr($text,$pos+strlen($cuv),150);
    $cut1=explode(" ",$desc1);
    $desc1=$cut1[1];
    for($i=2;$i<strlen($cut1);$i++) $desc1.=' '.$cut1[$i];
     }
   else {
     $text=strip_tags($row->newsTitle);
     $pos=strpos($text,$cuv);
    $desc=substr($text,$pos-80,$pos-($pos-80));
   $cut=explode(" ",$desc);
    $desc=$cut[1];
    for($i=2;$i<strlen($cut);$i++) $desc.=' '.$cut[$i];
    $desc1=substr($text,$pos+strlen($cuv),150);
    $cut1=explode(" ",$desc1);
    $desc1=$cut1[1];
    for($i=2;$i<strlen($cut1);$i++) $desc1.=' '.$cut1[$i];
   }
   $descriere[$l]='...'.$desc.' <b> '.$cuv.' </b> '.$desc1.'...';
   $linkuri[$l]="newsPresentation";

  } 
  if($search==0) { $out.="No results!"; $k=0;}
 // $out.=$l.'<br>';
$max_page=($l/$nr_link)+1;
 //$out.='<span align="center" class="insidetable"><b>There are <u>'.$l.'</u> results for the keyword <u>'.$cuv.'</u>.</b>';
 //$out.=$max_page;
 if($l>$nr_link&& $l!=0 && !isset($_GET['page'])) 
      {
        $out.='<span align="center" class="insidetable"><b>Results 0 - '.$nr_link.' of about <u>'.$l.'</u> for keyword <u>'.$cuv.'</u>.</b>';
       for($i=0;$i<=$nr_link;$i++){
         $link=explode(".",$linkuri[$i]);
         $out.='<p><a href="index.php?section='.$link[0].'">http://Plutohome.com/'.$linkuri[$i].'</a><br></p>';
         $out.='<p>'.$descriere[$i].'<p>';
           }
           $out.='<tr><td></td><td align="center"> 1 ';
           for($i=2;$i<=$max_page;$i++) $out.='<a href="index.php?section=searchWeb&page='.$i.'&l='.$l.'&cuv='.$cuv.'"> ' .$i. ' </a>';
           $out.='</td></tr>';
           $max=$max_page;
      }
 if($l<=$nr_link && !isset($_GET['page'])){
   $out.='<span align="center" class="insidetable"><b>Results 0 - '.$l.' of about <u>'.$l.'</u> for keyword <u>'.$cuv.'</u>.</b>';
    for($i=1;$i<=$l;$i++){
      $link=explode(".",$linkuri[$i]);
         $out.='<p><a href="index.php?section='.$link[0].'">http://Plutohome.com/'.$linkuri[$i].'</a><br></p>';
        $out.='<p>'.$descriere[$i].'<p>';
    }
}
    if(isset($_GET['page']))
  {   
      $j=$_GET['page'];
      $l=$_GET['l'];
      $nr=$nr_link*$j;
      $max_page=($l/$nr_link)+1;
      $out.='<span align="center" class="insidetable"><b>Results '.$nr_link*($j-1).' - '.min($nr,$l).' of about <u>'.$l.'</u> for keyword <u>'.$cuv.'</u>.</b>';
     // $out='';
     // $out.='<table width="100%" border="0" cellpadding="0" cellspacing="0" class="maintable"><td width="20"></td><td class="insidetable">';
      for($i=$nr_link*($j-1);$i<min($nr,$l);$i++) 
      {
        $link=explode(".",$linkuri[$i]);
         $out.='<p><a href="index.php?section='.$link[0].'">http://Plutohome.com/'.$linkuri[$i].'</a><br></p>';
         $out.='<p>'.$descriere[$i].'<p>';
      }
       $out.='<tr><td></td><td align="center">';
           for($i=1;$i<$j;$i++) $out.='<a href="index.php?section=searchWeb&page='.$i.'&l='.$l.'&cuv='.$cuv.'"> ' .$i. ' </a>';
           $out.=$j;
           for($i=$j+1;$i<$max_page;$i++) $out.='<a href="index.php?section=searchWeb&page='.$i.'&l='.$l.'&cuv='.$cuv.'"> ' .$i. ' </a>';
           $out.='</td></tr>';
  }

	  $out.='</td>
	  		</tr>
	  	</table>';
         
	  $output->setScriptCalendar('null');
	  $output->setScriptTRColor('null');
	  $output->setBody($out);
	  $output->setTitle(APPLICATION_NAME."::Search");
	  $output->output();
}

?>

