<?
$package=(isset($_SESSION['package']))?$_SESSION['package']:0;
$out='
<p><a href=index.php?section=document&docID=53>Explain Pluto\'s version numbers</a>&nbsp;&nbsp;&nbsp;&nbsp;<a href=index.php?section=platforms>Compatible platforms</a></p>
';
if ($package == 0) {
	$queryVers = 'SELECT VersionName,Date,Description,Comments,NextSteps,Repository FROM Version
					GROUP BY Repository
					ORDER BY Date';
	$resVers = $dbADO->Execute($queryVers);
	$out.='<center><table>';
	$out.='<tr bgcolor="lightblue">
			<td><center>Version Name</center><td>
		</tr>';
	$ind = 0;
	while ($rowVers = $resVers->FetchRow()) {
		//if ($ind%2 != 0) $col = ' bgcolor="lightblue"';
		//	else $col = ' bgcolor="lightred"';
		$col = ' bgcolor="#C0C0C0"';
		switch ($rowVers['Repository']) {
			case '0' : $text = 'Development'; break;
			case '1' : $text = 'Testing'; break;
			case '2' : $text = 'Release'; break;
			default : $text = ''; 
		}
		$out.='<tr>';
		$out.='<td'.$col.'><center>'.$rowVers['VersionName'].'</center></td>';
		$out.='<td>';
			$out.='<table><tr>';
				$out.='<td><table width="100%">';
					$out.='<tr bgcolor="lightblue"><td><center>Date</center></td>
							<td><center>Description</center></td>
							<td><center>Repository</center></td></tr>';
					$out.='<tr'.$col.'>';
						$out.='<td><center>'.$rowVers['Date'].'</center></td>';
						$out.='<td><center>'.$rowVers['Description'].'</center></td>';
						$out.='<td><center>'.$text.'</center></td>';
					$out.='</tr></table></td></tr>';
				$out.='<tr><td><table>';
					$out.='<tr><td bgcolor="lightblue"><center>Comments</center></td>';
					$out.='<td'.$col.'><center>'.$rowVers['Comments'].'</center></td></tr>';
					$out.='<tr><td bgcolor="lightblue"><center>Next Steps</center></td>';
					$out.='<td'.$col.'><center>'.$rowVers['NextSteps'].'</center></td></tr>';
				$out.='</table></td></tr>';
			$out.='</tr></table>';
		$out.='</td></tr>';
		$ind++;
	};
	$out.='</table></center>';
}
	if ($package != 0) {
		$out.='<br>';
		$out.='<p><b>**package**</b> version informations :</p>';
		$queryVers1 = ' SELECT VersionName,Date,Description,Package_Version.Comments,FK_Package 
							FROM Version
						LEFT JOIN Package_Version ON FK_Version=PK_Version AND FK_Package=?
						GROUP BY Repository
						ORDER BY Date';
		$resVers1 = $dbADO->Execute($queryVers1,$package);
		$out.='<center><table>';
		$out.='<tr bgcolor="lightblue">
				<td><center>Version Name</center></td>
				<td><center>Date</center></td>
				<td><center>Description</center></td>
				<td><center>Comments</center></td>
			</tr>';
		$ind = 0;
		while ($rowVers1 = $resVers1->FetchRow()) {
			if ($ind%2 != 0) $col = ' bgcolor="lightblue"';
				else $col = ' bgcolor="#C0C0C0"';
			if ($rowVers1['FK_Package']=='') $star=' * ';
				else $star='';
			$out.='<tr'.$col.'>';
			$out.='<td><center>'.$rowVers1['VersionName'].$star.'</center></td>';
			$out.='<td><center>'.$rowVers1['Date'].'</center></td>';
			$out.='<td><center>'.$rowVers1['Description'].'</center></td>';
			$out.='<td><center>'.$rowVers1['Comments'].'</center></td>';
			$out.='</tr>';
		};
		$out.='</table></center>';		
		$out.='
			<p>Each numbered version includes a release of all the software.  To keep things simple, there are no separate versions for modules.  
			When you see an * next to a version on the left, that means this software, <b>**package**</b> had no changes in that version.</p>
			';
	}

$output->setBody($out);
$output->setTitle(APPLICATION_NAME);			
$output->output();  		
?>