<?
$package=(isset($_SESSION['package']))?$_SESSION['package']:0;
$out='
<p><a href=index.php?section=document&docID=53>Explain Pluto\'s version numbers</a>&nbsp;&nbsp;&nbsp;&nbsp;<a href=index.php?section=platforms>Compatible platforms</a></p>
';
if ($package == 0) {
	$queryVers = '
		SELECT PK_Version,VersionName, Date, Description, Comments, NextSteps, Repository 
		FROM Version 
		WHERE PK_Version!=1
		ORDER BY Date DESC';
	$resVers = $dbADO->Execute($queryVers);
	$out.='<center>
		<table width="100%" border="0">';
	$out.='<tr bgcolor="lightblue">
			<td><center><B>Version Name</B></center></td>
			<td>&nbsp;</td>
		</tr>';
	$ind = 0;
	while ($rowVers = $resVers->FetchRow()) {
		//if ($ind%2 != 0) $col = ' bgcolor="lightblue"';
		//	else $col = ' bgcolor="lightred"';
		$col = ' bgcolor="#E0E0E0"';
		switch ($rowVers['Repository']) {
			case '0' : $text = 'Development'; break;
			case '1' : $text = 'Testing'; break;
			case '2' : $text = 'Release'; break;
			default : $text = ''; 
		}
		$out.='
			<tr>
				<td'.$col.'><center>'.$rowVers['VersionName'].'</center></td>
				<td>
					<table width="100%" border="0">
						<tr bgcolor="lightblue">
							<td width="100"><center><B>Date</B></center></td>
							<td><center><B>Description</B></center></td>
							<td><center><B>Repository</B></center></td></tr>
						<tr'.$col.'>
							<td><center>'.$rowVers['Date'].'</center></td>
							<td><center>'.$rowVers['Description'].'</center></td>
							<td><center>'.$text.'</center></td>
						</tr>
						<tr>
							<td bgcolor="lightblue"><center>Comments</center></td>
							<td'.$col.' colspan="2">'.$rowVers['Comments'].'</td>
						</tr>
						<tr>
							<td bgcolor="lightblue"><center>Next Steps</center></td>
							<td'.$col.' colspan="2">'.$rowVers['NextSteps'].'</td>
						</tr>
					</table>
				</td>
			</tr>';
		$ind++;
	};
	$out.='</table></center>';
}
	if ($package != 0) {
		$out.='<br>';
		$out.='<p><b>**package**</b> version informations :</p>';
		$queryVers1 = '
			SELECT PK_Version,VersionName, Date, Description, Package_Version.Comments, FK_Package 
			FROM Version 
			LEFT JOIN Package_Version ON FK_Version=PK_Version AND FK_Package=?
			ORDER BY Date DESC';
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
				else $col = ' bgcolor="#E0E0E0"';
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