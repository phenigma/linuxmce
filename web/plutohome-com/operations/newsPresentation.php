<?php
function newsPresentation($output,$conn) {

	$sql="SELECT newsID,newsTitle,newsContent,UNIX_TIMESTAMP(newsDate) AS timestamp FROM News WHERE markedDeleted!=1 ORDER BY newsDate DESC";
	$r=mysql_query($sql,$conn) or die("Can not grab from database" .mysql_error());
	$id=array();
	$title=array();
	$text=array();
	while($row=mysql_fetch_object($r)){
		$id[]=$row->newsID;
		$title[]=date('m.d.Y',$row->timestamp).'<br>'.$row->newsTitle;
		$text[]=$row->newsContent;
	}
	$from=(isset($_GET['page']))?((int)$_GET['page']-1):0;
	$itemsPerPage=5;
	$pages=ceil(mysql_num_rows($r)/$itemsPerPage);
	
	$out='
<table width="100%"  border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td align="center" class="insidetable"><table width="563"  border="0" cellpadding="0" cellspacing="0">
      <tr>
        <td align="left"><img src="images/titles/pluto_news.gif"></td>
      </tr>
    </table></td>
  </tr>
  <tr>
    <td valign="top" class="insidetable2"><table>';
	for($i=$from*$itemsPerPage;$i<$from+$itemsPerPage;$i++){
		if(isset($id[$i])){
			preg_match("/^(.*)\n/i",$text[$i],$matches);
		
			$firstParagraph='';
			if(isset($matches[0])){
				$firstParagraph=$matches[0];
			}			
		$out.='
		<tr>
			<td width="15"><img src="images/bullet_1.gif" width="11" height="11" align="absmiddle" /></td>
			<td class="title2"><b>'.$title[$i].'</b><br /></td>
		</tr>
		<tr>
			<td>&nbsp;</td>
			<td>'.(($firstParagraph!='')?$firstParagraph.'<br><a href="index.php?section=full_story&id='.$id[$i].'">More details</a>':nl2br($text[$i])).'</td>
		</tr>
		<tr>
			<td>&nbsp;</td>
			<td>&nbsp;</td>
		</tr>	
		';
		}
	}
	if($pages>1){
		$pageLinks=array();
		for($i=0;$i<$pages;$i++){
			$pageLinks[]=($i==$from)?($i+1):'<a href="index.php?section=newsPresentation&page='.($i+1).'">'.($i+1).'</a>';
		}
		$out.='
		<tr>
			<td colspan="2" class="insidetable">&nbsp;</td>
		</tr>	
		<tr>
			<td>&nbsp;</td>
			<td>'.join(' | ',$pageLinks).'</td>
		</tr>		
';
	}
	$out.='
	</table>
    </td>
  </tr>
</table>	
';

	$output->setNavigationMenu(array("News"=>"index.php?section=newsPresentation"));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME."::News");
	$output->output();
}
?>