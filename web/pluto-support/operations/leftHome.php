<?
if($package!=0){
	$downloadLink='index.php?section=packageDownload&pkid='.$package;
	$queryParentDoc = '
		SELECT PK_Document,Title FROM Package
		INNER JOIN Document ON FK_Document=PK_Document
		WHERE PK_Package=?';
	$resParentDoc=$dbADO->Execute($queryParentDoc,$package);
	$documentLinks='';
	if($resParentDoc->RecordCount()>0){
		$rowParentDoc=$resParentDoc->FetchRow();
		$documentLinks.='<a href="right.php?section=documents/documentDisplay&docID='.$rowParentDoc['PK_Document'].'" target="basefrm">Documentation for '.$rowParentDoc['Title'].'</a><br>';
		
		$resChilds=$dbADO->Execute('SELECT PK_Document,Title FROM Document WHERE FK_Document_Parent=?',$rowParentDoc['PK_Document']);
		while($rowChilds=$resChilds->FetchRow()){
			$documentLinks.='&nbsp;&nbsp;&nbsp;&nbsp;<a href="right.php?section=documents/documentDisplay&docID='.$rowChilds['PK_Document'].'" target="basefrm">'.$rowChilds['Title'].'</a><br>';
		}
	}
	
}else{
	$downloadLink='index.php?section=mainDownload';
}
$out='
<p>Useful shortcuts for
**package**</p>

<p><a href="'.$downloadLink.'" target="_top">Download software</a></p>
<p>'.@$documentLinks.'</p>
<p><a href="index.php?section=forum" target="_top">Forum</a></p>
<p><a href="index.php?section=faq" target="_top">FAQ</a></p>
<p><a href="index.php?section=mail" target="_top">Mailing List</a></p>
<br><br>
<p><b><a href="index.php?section=home&package=0" target="_top">Pluto\'s Main Support Page</a></b></p>
';

$output->setBody($out);
$output->setTitle(APPLICATION_NAME);			
$output->output();  		
?>