<?
$package=(isset($_SESSION['package']))?$_SESSION['package']:0;
if ($package == 0) {
	$out='<br><p>This is the general news for Pluto and all the software modules.  You can access News for a particular module by visiting its homepage.</p>';
	$queryNews = 'SELECT * FROM News
					LEFT JOIN Package ON FK_Package=PK_Package
					ORDER BY Date Desc';
	$resNews = $dbADO->Execute($queryNews);
	$out1='<center><table><tr bgcolor="lightblue">
			<td><center>Title</center></td>
			<td><center>Date</center></td>
			<td><center>Short Summary</center></td>
			<td><center>Full Text</center></td>
		</tr>';
	$ind = 0; $found = 0;
	$out2 = '';
	while ($rowNews = $resNews->FetchRow()) {
		$found = 1;
		if ($ind%2 == 0) $col='';
			else $col=' bgcolor="#C0C0C0"';
		$out2.='<tr'.$col.'>';
			$out2.='<td><center>'.$rowNews['Title'].'</center></td>';
			$out2.='<td><center>'.$rowNews['Date'].'</center></td>';
			$out2.='<td><center>'.$rowNews['ShortSummary'].'</center></td>';
			$out2.='<td><center>'.$rowNews['FullText'].'</center></td>';
		$out2.='</tr>';
	}
	$out3='</table></center>';
	if ($found == 0) {
		$out.='<center><b>**There is no news**</b></center>';
	}
	else $out.=$out1.$out2.$out3;	
}

else { // $package is not null
	$out='<br>Package: <b>**package**</b> : ';
	$out.='<p>This is news for the module **package** only.  For all news about Pluto, see Pluto\'s **main support page**.</p>';
	$queryNews1 = 'SELECT * FROM News
					WHERE FK_Package = ?
					ORDER BY Date Desc';
	$resNews1 = $dbADO->Execute($queryNews1,$package);
	$out1='<center><table><tr bgcolor="lightblue">
			<td><center>Title</center></td>
			<td><center>Date</center></td>
			<td><center>Short Summary</center></td>
			<td><center>Full Text</center></td>
		</tr>';	$ind = 0; $found = 0;
	$out2 = '';
	while ($rowNews1 = $resNews1->FetchRow()) {
		$found = 1;
		if ($ind%2 == 0) $col='';
			else $col=' bgcolor="#C0C0C0"';
		$out2.='<tr'.$col.'>';
			$out2.='<td><center>'.$rowNews1['Title'].'</center></td>';
			$out2.='<td><center>'.$rowNews1['Date'].'</center></td>';
			$out2.='<td><center>'.$rowNews1['ShortSummary'].'</center></td>';
			$out2.='<td><center>'.$rowNews1['FullText'].'</center></td>';
		$out2.='</tr>';
	}
	$out3='</table></center>';
	if ($found == 0) {
		$out.='<center><b>**There is no news**</b></center>';
	}
	else $out.=$out1.$out2.$out3;		
}

$output->setBody('There is no news yet.  Please check back later.');
//$output->setBody($out);
$output->setTitle(APPLICATION_NAME);			
$output->output();  		
?>