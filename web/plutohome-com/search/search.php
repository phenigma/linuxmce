<?
function search($output,$dbADO,$conn){
	/* @var $dbADO ADOConnection */
	/* @var $rs ADORecordSet */

	$searchString=@$_REQUEST['cuvant'];
	
	$out='
	<table width="100%" border="0" cellpadding="0" cellspacing="0" class="maintable">
		<tr>
			<td class="insidetable">';
	$rs=$dbADO->Execute('SELECT * FROM Document WHERE Title LIKE ? OR Contents LIKE ?',array('%'.$searchString.'%','%'.$searchString.'%'));
	$docsFound=$rs->RecordCount();
	$out.='
		<table>
		<tr>
			<td colspan="2"><h3>Searching for: '.$searchString.'</h3></td>
		</tr>	
		<tr>
			<td colspan="2"><h4>Results found in DOCUMENTS: '.$docsFound.'</h4></td>
		</tr>
		<tr>
			<td width="20"></td>
			<td class="insidetable">';
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
			<td colspan="2"><div align="center" style="background:#CCCCCC;"><a href="index.php&section=searchDocs&searchString='.$searchString.'"><B>More results...</B></a></div>
			</td>
		</tr>';
	}

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
			<td colspan="2"><div align="center" style="background:#CCCCCC;"><a href="index.php?section=searchWeb&searchString='.$searchString.'"><B>More Results...</B></a></div></td>
		</tr>';
	}
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
		return $part;
	}
	else
		return nl2br($str);
}

function highlight($target,$str)
{
	return str_replace($target,'<font color="blue"><b>'.$target.'</b></font>',$str);
}
?>

