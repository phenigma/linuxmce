<?
$package=(isset($_SESSION['package']))?$_SESSION['package']:0;
$pkid=(isset($_GET['pkid'])&&$_GET['pkid']!='0'&&$_GET['pkid']!='')?$_GET['pkid']:$package;

	$GLOBALS['out1']='';
	$GLOBALS['ind']=0;
	$GLOBALS['found']=0;
	$GLOBALS['makeLink']=0;


//instead of **package**, use description

function getPacks ($pack,$dbADO) {
	$queryPacks = 	'SELECT PK_Package,Package.Description,OnlyToBuild
						FROM Package_Package
					JOIN Package ON FK_Package_DependsOn=PK_Package
						WHERE FK_Package=?';
	$resPacks = $dbADO->Execute($queryPacks,$pack);
	//$ind = 0;	
	while ($rowPacks = $resPacks->FetchRow()) {
		$GLOBALS['found']=1;
		if ($GLOBALS['ind']%2 != 0 ) $col=' bgcolor="#C0C0C0"';
			else $col=' bgcolor="lightblue"';
		if ($rowPacks['OnlyToBuild']=='1') { $itb='<i><b>'; $ite='</b></i>'; }
			else { $itb=''; $ite=''; }
		if ($GLOBALS['makeLink']==0) {
			$GLOBALS['out1'].='<center><tr'.$col.'><td><center>'.$itb.
				$rowPacks['Description'].$ite.'</center></td></tr></center>';
		} else {
			$GLOBALS['out1'].='<center><tr'.$col.'><td><center>'.$itb.
				'<a href=index.php?section=platforms&pkid='.$rowPacks['PK_Package'].'>'
				.$rowPacks['Description'].$ite.'</a></center></td></tr></center>';
		}
		$GLOBALS['ind']++;
		//$GLOBALS['out1'].='intro';
		getPacks($rowPacks['PK_Package'],$dbADO);
		//$GLOBALS['out1'].='out';
	}
}

function getPacks2 ($pack,$dbADO) {
	$queryPacks = 	'SELECT PK_Package,Package.Description,OnlyToBuild
						FROM Package_Package
					JOIN Package ON FK_Package_DependsOn=PK_Package
						WHERE OnlyToBuild=1 and FK_Package=?';
	$resPacks = $dbADO->Execute($queryPacks,$pack);
	//$ind = 0;	
	while ($rowPacks = $resPacks->FetchRow()) {
		$GLOBALS['found']=1;
		if ($GLOBALS['ind']%2 != 0 ) $col=' bgcolor="#C0C0C0"';
			else $col=' bgcolor="lightblue"';
		$GLOBALS['out1'].='<center><tr'.$col.'><td><center>'.
			$rowPacks['Description'].'</center></td></tr></center>';
		$GLOBALS['ind']++;
		//$GLOBALS['out1'].='intro';
		getPacks($rowPacks['PK_Package'],$dbADO);
		//$GLOBALS['out1'].='out';
	}
}
if($pkid==0){
	$out='<p>Pluto is designed to be completely cross-platform, and run on a mix of Windows, Unix/Linux and other operating systems that can compile ANSI C++ code.  You can even mix different platforms within the same installation and they will all work together.</p>
		<p>Some of the software modules are specific to a certain platform, however.  The following table lists all the platforms the current development version has been tested on.
		This does not mean that every module will run on these platforms.</p>';
	$queryOS0 = 'SELECT Distro.*, OperatingSystem.Description AS OS FROM Distro
					INNER JOIN OperatingSystem ON FK_OperatingSystem=PK_OperatingSystem';
	$resOS0 = $dbADO->Execute($queryOS0);
	$out.='<center><table>
			<tr bgcolor="lightblue">
				<td><center><B>Operating System</B></center></td>
				<td><center><B>Distro</B></center></td>
				<td><center><B>Kick Start CD</B></center></td>
				<td><center><B>Installer</B></center></td>
				<td><center><B>Binaries</B></center></td>
				<td><center><B>Source Code</B></center></td>
				<td><center><B>Confirmed</B></center></td>
			</tr>';
	$ind = 0;
	while ($rowOS0 = $resOS0->FetchRow()){
		if ($ind%2 != 0 )
			$col = ' bgcolor="#C0C0C0"';
		else $col = '';
		$ind++;
		$out.='<tr'.$col.'>
			<td><center>'.$rowOS0['OS'].'</center></td>
			<td><center>'.$rowOS0['Description'].'</center></td>';
		if ($rowOS0['KickStartCD']=='' || $rowOS0['KickStartCD']=='0') 
			$col=''; else $col='x';
		$out.='<td><center>'.$col.'</center></td>';
		if ($rowOS0['Installer']=='' || $rowOS0['Installer']=='0') 
			$col=''; else $col='x';
		$out.='<td><center>'.$col.'</center></td>';
		if ($rowOS0['Binaries']=='' || $rowOS0['Binaries']=='0') 
			$col=''; else $col='x';
		$out.='<td><center>'.$col.'</center></td>';
		if ($rowOS0['SourceCode']=='' || $rowOS0['SourceCode']=='0') 
			$col=''; else $col='x';
		$out.='<td><center>'.$col.'</center></td>';
		if ($rowOS0['Confirmed']=='' || $rowOS0['Confirmed']=='0') 
			$col=''; else $col='x';
		$out.='<td><center>'.$col.'</center></td>';
		$out.='</tr>';
	}
	$out.='</table></center>';
	}
	
else{ //package is not 0
	$queryDesc = 'select Description From Package WHERE PK_Package=?';
	$resDesc = $dbADO->Execute($queryDesc,$pkid);
	$rowDesc = $resDesc->FetchRow();
	$packageNewName = $rowDesc['Description'];
	
	$out='<p>Pluto is designed to be completely cross-platform, and run on a mix of Windows, Unix/Linux and other operating systems that can compile ANSI C++ code.  You can even mix different platforms within the same installation and they will all work together.</p>
		<p><b> ** </b>
		Some of the software modules are specific to a certain platform, however.  This package, <b>'.$packageNewName.'</b>, is available from the following sources for the indicated platforms:</p>';
	$queryOSn0 = 'SELECT Package_Source_Compat.FK_OperatingSystem,Package_Source_Compat.FK_Distro,
						Distro.Description As Distro,OperatingSystem.Description as OperatingSystem,
						RepositorySource.Description as RepositorySource,
						RepositoryType.Description as RepositoryType
					FROM Package_Source_Compat
				JOIN Package_Source ON FK_Package_Source=PK_Package_Source
				JOIN RepositorySource ON FK_RepositorySource=PK_RepositorySource
				JOIN RepositoryType ON FK_RepositoryType=PK_RepositoryType
				LEFT JOIN Distro ON Package_Source_Compat.FK_Distro=PK_Distro
				LEFT JOIN OperatingSystem ON Package_Source_Compat.FK_OperatingSystem=PK_OperatingSystem
					WHERE FK_Package=?';
	$resOSn0 = $dbADO->Execute($queryOSn0,$pkid);
	$out.='<center><table>
			<tr bgcolor="lightblue">
				<td><center><B>Operating System</B></center></td>
				<td><center><B>Distro</B></center></td>
				<td><center><B>Source</B></center></td>
				<td><center><B>Type</B></center></td>
			</tr>';
	$ind = 0;
	while ($rowOSn0 = $resOSn0->FetchRow()){
		if ($ind%2 != 0) $col=' bgcolor="#C0C0C0"';
		else $col='';
		$ind++;
		$out.='<tr '.$col.'>';
		if ($rowOSn0['OperatingSystem']=='') $col='**any**';
			else $col = $rowOSn0['OperatingSystem'];
		$out.='<td><center>'.$col.'</center></td>';
		if ($rowOSn0['Distro']=='') $col='**any**';
			else $col = $rowOSn0['Distro'];
		$out.='<td><center>'.$col.'</center></td>';
		$out.='<td><center>'.$rowOSn0['RepositorySource'].'</center></td>';
		$out.='<td><center>'.$rowOSn0['RepositoryType'].'</center></td>';
		$out.='</tr>';
	}
	$out.='</table></center>';
	$out.='<br><b> ** '.$packageNewName.'</b> requires the following software to run: <br>';
	$out.='<center><table>';
	$GLOBALS['found']=0;
	$GLOBALS['makeLink']=1;
	getPacks($pkid,$dbADO);
	$GLOBALS['makeLink']=0;
	$out.=$GLOBALS['out1'];
	$out.='</table></center>';
	if ($GLOBALS['found']==0) {
		$out.='<center><b>**No software is needed**</b></center>';
	}
	$out.='<br>Software in <i>italics</i> are only required if you want to build this from source
<br><br><b>  **  </b>The source code for <b>'.$packageNewName.'</b> package is available from the following sources:';
	$querySources = 'SELECT FK_Package_Sourcecode
						FROM Package
						WHERE PK_Package=?';
	$resSources = $dbADO->Execute($querySources,$pkid);
	$rowSources = $resSources->FetchRow();
	if ($rowSources['FK_Package_Sourcecode']=='') {
		$out.='<center><b>**No source code is available**</b></center>';
	}
	else  {
		$resOSn0 = $dbADO->Execute($queryOSn0,$rowSources['FK_Package_Sourcecode']);
		$out.='<center><table>
				<tr bgcolor="lightblue">
					<td><center><B>Operating System</B></center></td>
					<td><center><B>Distro</B></center></td>
					<td><center><B>Source</B></center></td>
					<td><center><B>Type</B></center></td>
				</tr>';
		$ind = 0;
		while ($rowOSn0 = $resOSn0->FetchRow()){
			if ($ind%2 != 0) $col=' bgcolor="#C0C0C0"';
			else $col='';
			$ind++;
			$out.='<tr '.$col.'>';
			if ($rowOSn0['OperatingSystem']=='') $col='**any**';
				else $col = $rowOSn0['OperatingSystem'];
			$out.='<td><center>'.$col.'</center></td>';
			if ($rowOSn0['Distro']=='') $col='**any**';
				else $col = $rowOSn0['Distro'];
			$out.='<td><center>'.$col.'</center></td>';
			$out.='<td><center>'.$rowOSn0['RepositorySource'].'</center></td>';
			$out.='<td><center>'.$rowOSn0['RepositoryType'].'</center></td>';
			$out.='</tr>';
		}
		$out.='</table></center>';	
	}
	$out.='<br><b>  **  </b>To compile the source code for <b>'.$packageNewName.'</b> requires the following software: ';
	$GLOBALS['out1']='';
	$GLOBALS['ind']=0;
	$GLOBALS['found']=0;
	$out.='<center><table>';
	getPacks2($pkid,$dbADO);
	$out.=$GLOBALS['out1'];
	$out.='</table></center>';
	
	if ($rowSources['FK_Package_Sourcecode']!='') {
		$out.='<br>';
		$GLOBALS['out1']='';
		$GLOBALS['ind']=0;
		$out.='<center><table>';
		getPacks2($rowSources['FK_Package_Sourcecode'],$dbADO);
		$out.=$GLOBALS['out1'];
		$out.='</table></center>';
	}
	if ($GLOBALS['found']==0){
		$out.='<center><b>**No software is needed**</b></center>';
	}	
}

$output->setBody($out);
$output->setTitle(APPLICATION_NAME);			
$output->output();  		
?>