<?
function full_story($output,$conn){
	$out = '';

	$id=$_GET['id'];
	$sql="SELECT UNIX_TIMESTAMP(newsDate) AS timestamp,newsTitle,newsContent FROM News where newsID='".$id."'";
	$r=mysql_query($sql,$conn) or die("Can not grab from database".mysql_error());
	$row=mysql_fetch_object($r);
	$out.='
<table width="100%"  border="0" cellspacing="0" cellpadding="0">
  <tr>
    <td align="center" class="insidetable"><table width="563"  border="0" cellpadding="0" cellspacing="0">
      <tr>
        <td align="left"><img src="images/titles/pluto_news.gif"></td>
      </tr>
    </table></td>
  </tr>
  <tr>
    <td valign="top" class="insidetable2"><table>
		<tr>
			<td width="15"><img src="images/bullet_1.gif" width="11" height="11" align="absmiddle" /></td>
			<td class="title2"><b>'.date('d.m.Y',$row->timestamp).'<br>'.$row->newsTitle.'</b><br /></td>
		</tr>
		<tr>
			<td>&nbsp;</td>
			<td>&nbsp;</td>
		</tr>	
		<tr>
			<td>&nbsp;</td>
			<td>'.nl2br($row->newsContent).'</td>
		</tr>
	</table>
    </td>
  </tr>
</table>	
';

	$output->setNavigationMenu(array('News'=>'index.php?section=newsPresentation'));
	$output->setBody($out);
	$output->setTitle(APPLICATION_NAME);
	$output->output();
}
?>