<?
function search($output,$dbADO,$conn){
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */

	$searchString=@$_REQUEST['cuvant'];
	
	$out='
	<table width="100%" border="0" cellpadding="0" cellspacing="0" class="maintable">
		<tr>
			<td align="left" class="insidetable"><img src="images/titles/search.gif"></td>
		</tr>	
		<tr>
			<td class="insidetable2">';
	$rs=$dbADO->Execute('SELECT * FROM Document WHERE Title LIKE ? OR Contents LIKE ?',array('%'.$searchString.'%','%'.$searchString.'%'));
	$docsFound=$rs->RecordCount();
	$out.='
		<table>
		<tr>
			<td colspan="2"><span class="title">Searching for: '.$searchString.'</span></td>
		</tr>	
		<tr>
			<td colspan="2"><span class="title">Results found in DOCUMENTS: '.$docsFound.'</span></td>
		</tr>
		<tr>
			<td width="20"></td>
			<td class="insidetable2">';
	// search in documents
	$pos=0;
	while($rowDocs=$rs->FetchRow()){
		$pos++;
		$out.='<B>'.highlight($searchString,$rowDocs['Title']).'</B><br>'.getPartialContent($searchString,$rowDocs['Contents']).'<br>';
		$out.='<div align="right" style="background:#EEEEEE;"><a href="support/index.php?section=document&docID='.$rowDocs['PK_Document'].'">Link</a></div>';
		if($pos==5)
			break;
	}		
	$out.='		</td>
	  		</tr>';
	if($docsFound>5){
		$out.='
		<tr>
			<td colspan="2"><div align="left"><a href="index.php?section=searchDocs&searchString='.$searchString.'">More results in documents ...</a></div>
			</td>
		</tr>';
	}
/*
	// search in Web pages content
	$sql="SELECT PageContentID,PageContenttext,PageName FROM PageContent WHERE PageContenttext LIKE '%".$searchString."%'";
  	$resPages=mysql_query($sql,$conn) or die("Can not grab from database"  .mysql_error());
  	
  	$webFound=mysql_num_rows($resPages);
	
	$out.='
			<tr>
				<td colspan="2"><h4>Results found in WEBSITE: '.$webFound.'</h4></td>
			</tr>	
			<tr>
				<td width="20"></td>
				<td class="insidetable">';
	$pos=0;
  	while($rowWeb=mysql_fetch_assoc($resPages)){
  		$pos++;
  		$out.='...'.getPartialContent($searchString,$rowWeb['PageContenttext']).' ...';
  		$out.='<div align="right" style="background:#EEEEEE;"><a href="index.php?section='.substr($rowWeb['PageName'],0,-4).'">Link</a></div>';
  		if($pos==5)
  			break;
  	}
	$out.='		</td>
			</tr>';
	if($webFound>5){
		$out.='
		<tr>
			<td colspan="2"><div align="center" style="background:#CCCCCC;"><a href="index.php?section=searchWeb&searchString='.$searchString.'"><B>More Results in website ...</B></a></div></td>
		</tr>';
	}
*/
	// search in phpbb
	global $dbphpBBServer,$dbphpBBUser,$dbphpBBPass,$dbphpBBDatabase;
	$connphpBB=mysql_connect($dbphpBBServer,$dbphpBBUser,$dbphpBBPass,true) or die('could not connect to database');
  	$dbphpBB=mysql_select_db($dbphpBBDatabase,$connphpBB) or die("could not select $dbphpBBDatabase");
	$queryWords="
		SELECT m.post_id 
		FROM phpbb_search_wordlist w, phpbb_search_wordmatch m 
		WHERE w.word_text LIKE '".$searchString."' AND m.word_id = w.word_id AND w.word_common <> 1";	
	$resWords=mysql_query($queryWords,$connphpBB);
	$wordsArray=array();
	while($rowWords=mysql_fetch_assoc($resWords)){
		$wordsArray[]=$rowWords['post_id'];
	}

	if(count($wordsArray==0))
		$wordsArray[]='0';
	$queryTopics="
		SELECT phpbb_posts.topic_id, phpbb_topics.topic_title 
		FROM phpbb_posts 
		INNER JOIN phpbb_topics ON phpbb_posts.topic_id=phpbb_topics.topic_id
		WHERE post_id IN (".join(',',$wordsArray).") 
		GROUP BY topic_id
		ORDER BY topic_time DESC";
	$resForum=mysql_query($queryTopics,$connphpBB);
	
	$forumFound=mysql_num_rows($resForum);
	$out.='
			<tr>
				<td colspan="2" class="insidetable2"><span class="title">Results found in FORUM: '.$forumFound.'</span></td>
			</tr>	
			<tr>
				<td width="20"></td>
				<td class="insidetable2">';
	$pos=0;
	while($rowForum=mysql_fetch_assoc($resForum)){
  		$pos++;
  		$out.=$rowForum['topic_title'];
  		$out.='<div align="right" style="background:#EEEEEE;"><a href="support/phpbb2/viewtopic.php?t='.$rowForum['topic_id'].'&highlight='.$searchString.'">Link</a></div>';
  		if($pos==5)
  			break;
  	}
	$out.='		</td>
			</tr>';
	if($forumFound>5){
		$out.='
		<tr>
			<td colspan="2"><div align="left"><a href="support/phpbb2/search.php?search_keywords='.$searchString.'&show_results=topics&search_fields=all">More Results in forum ...</a></div></td>
		</tr>';
	}
	
	mysql_close($connphpBB);

	// search in mantis
	global $g_hostname,$g_db_username,$g_db_password,$g_database_name;
  	$connMantis=mysql_connect($g_hostname,$g_db_username,$g_db_password,true) or die('could not connect to database');
  	$dbMantis=mysql_select_db($g_database_name,$connMantis) or die("could not select $g_database_name");

  $queryMantis="
		SELECT summary,mantis_bug_table.id 
		FROM mantis_bug_table
		INNER JOIN mantis_bug_text_table ON mantis_bug_table.id=mantis_bug_text_table.id   
		WHERE description LIKE '%".$searchString."%' OR summary LIKE '%".$searchString."%'";	
	$resMantis=mysql_query($queryMantis,$connMantis);

	$mantisFound=mysql_num_rows($resMantis);
	$out.='
			<tr>
				<td colspan="2" class="insidetable2"><span class="title">Results found in bugtracking application: '.$mantisFound.'</span></td>
			</tr>	
			<tr>
				<td width="20"></td>
				<td class="insidetable2">';
	$pos=0;
	while($rowMantis=mysql_fetch_assoc($resMantis)){
  		$pos++;
  		$out.=$rowMantis['summary'];
  		$out.='<div align="right" style="background:#EEEEEE;"><a href="support/mantis/view.php?id='.$rowMantis['id'].'">Link</a></div>';
  		if($pos==5)
  			break;
  	}
	$out.='		</td>
			</tr>';
	if($mantisFound>5){
		$out.='
		<tr>
			<td colspan="2"><div align="left"><a href="support/mantis/view_all_set.php?search='.$searchString.'&type=1">More Results in bugtracking application ...</a></div></td>
		</tr>';
	}
	
	mysql_close($connMantis);
	
	
	$out.='
	  	</table>
		</td>
	</tr>
</table>';
         
	  $output->setScriptCalendar('null');
	  $output->setScriptTRColor('null');
	  $output->setBody($out);
	  $output->setTitle(APPLICATION_NAME."::Search");
	  $output->output();
}

function getPartialContent($target,$str)
{
	preg_match("/\n(.*)$target(.*)\./i",$str,$matches);
	
	if(isset($matches[0])){
		$part=(ereg('support/',$matches[0]))?$matches[0]:highlight($target,$matches[0]);
		return strip_tags($part,'<a>');
	}
	else
		return nl2br(strip_tags($str,'<a>'));
}

function highlight($target,$str)
{
	return str_replace($target,'<font color="blue"><b>'.$target.'</b></font>',$str);
}
?>

