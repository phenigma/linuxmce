<?
/* @var $dbADO ADOConnection */
$PK_Package=(isset($_GET['pkid'])&&$_GET['pkid']!='0')?$_GET['pkid']:$_SESSION['package'];

$queryPackage='SELECT * FROM Package WHERE PK_Package=?';
$resPackage=$dbADO->Execute($queryPackage,$PK_Package);
if($resPackage->RecordCount()!=0){
	$rowPackage=$resPackage->FetchRow();
	$packageName=$rowPackage['Description'];
	$isSource=$rowPackage['IsSource'];
	$sourceCodeID=$rowPackage['FK_Package_Sourcecode'];
}

$out='
<a href="javascript:history.back()">Back</a><br>
<h1><b>Download '.$packageName.'</b></h1>
<br>
<h2>'.(($isSource!=1)?'Binaries':'Source code').'</h2>';

$out.=displayPackageDownload($PK_Package,$dbADO);

if($isSource!=1){
	if($sourceCodeID!=''){
		$out.='<h2>Source code</h2>';
		$out.=displayPackageDownload($sourceCodeID,$dbADO);
	}
}else{
	$out.='<h4>Precompiled binaries</h4>';
	$binariesForSource=$dbADO->Execute('SELECT * FROM Package WHERE FK_Package_Sourcecode=?',$PK_Package);
	while($rowBinaries=$binariesForSource->FetchRow()){
		$out.='<h5>'.$rowBinaries['Description'].'<h5>';
		$out.=displayPackageDownload($rowBinaries['PK_Package'],$dbADO);
	}
}

$output->setBody($out);
$output->setTitle(APPLICATION_NAME);			
$output->output();  		


function displayPackageDownload($PK_Package,$dbADO)
{
	$out='';
	$dependanciesTxt=array();
	$selectDependancies='
			SELECT * FROM Package_Package 
		    	LEFT JOIN Package ON Package.PK_Package = Package_Package.FK_Package_DependsOn 
		    WHERE Package_Package.FK_Package=? ORDER BY Description ASC';
	$resDependancies=$dbADO->Execute($selectDependancies,$PK_Package);
	while($rowDependancies=$resDependancies->FetchRow()){
		$dependanciesTxt[]='<a href="index.php?section=packageDownload&pkid='.$rowDependancies['FK_Package_DependsOn'].'">'.$rowDependancies['Description'].'</a>';
	}
	$out.='
Dependencies: '.((count($dependanciesTxt)==0)?'none':join(', ',$dependanciesTxt)).'<br><br>';

	$selectRuns = 'select Package_Compat.FK_OperatingSystem,Package_Compat.FK_Distro,
								Distro.Description As Distro,
								OperatingSystem.Description As OperatingSystem,
								OS_Distro.Description As OS_Distro
						FROM Package_Compat
						LEFT JOIN OperatingSystem ON Package_Compat.FK_OperatingSystem=OperatingSystem.PK_OperatingSystem
						LEFT JOIN Distro ON FK_Distro=PK_Distro
						LEFT JOIN OperatingSystem As OS_Distro ON OS_Distro.PK_OperatingSystem=Distro.FK_OperatingSystem
						WHERE FK_Package=?';
	$resRuns = $dbADO->Execute($selectRuns,$PK_Package);
	$runsTxt = '';
	while ($rowRuns = $resRuns->FetchRow()) {
		if ($rowRuns['FK_Distro']!='') {
			switch($rowRuns['Distro']==1){
				case 1:
					$runsTxt.='<img src="include/images/debian.png" title="'.$rowRuns['OS_Distro'].' / '.$rowRuns['Distro'].'" align="middle">';
				break;
				case 3:
					$runsTxt.='<img src="include/images/redhat.png" title="'.$rowRuns['OS_Distro'].' / '.$rowRuns['Distro'].'" align="middle">';
				break;
				case 5:
					$runsTxt.='<img src="include/images/mandrake.png" title="'.$rowRuns['OS_Distro'].' / '.$rowRuns['Distro'].'" align="middle">';
				break;
				case 7:
					$runsTxt.='<img src="include/images/windows.png" title="'.$rowRuns['OS_Distro'].' / '.$rowRuns['Distro'].'" align="middle">';
				break;
				default:
					$runsTxt .= ' '.$rowRuns['OS_Distro'].' / '.$rowRuns['Distro'];				
				break;
			}
		}
		else if ($rowRuns['FK_OperatingSystem']!='') {
			if($rowRuns['FK_OperatingSystem']==1){
				$runsTxt.='<img src="include/images/linux.png" title="'.$rowRuns['OperatingSystem'].'(all distros)" align="middle">';
			}else{
				$runsTxt.='<img src="include/images/windows.png" title="'.$rowRuns['OperatingSystem'].'" align="middle">';
			}

		}
		else {
			$runsTxt .= ' All Operating Systems';
		}
	}
	$out.='Runs on: '.$runsTxt.'<br><br>';
	$selectSources='
	SELECT 
		Package_Source.*, RepositoryType.*,
		RepositoryType.Description AS Type,
		RepositorySource.Description AS RepositoryName, RepositorySource.Instructions As RepositoryInstructions
	FROM Package_Source
		INNER JOIN RepositorySource ON FK_RepositorySource=PK_RepositorySource
		INNER JOIN RepositoryType ON RepositorySource.FK_RepositoryType=RepositoryType.PK_RepositoryType
	WHERE FK_Package=? ORDER BY Name ASC';

	$resSources=$dbADO->Execute($selectSources,$PK_Package);
	$out.='<table cellspacing="0" cellpadding="2" border="0">';
	$sourcesCount=0;
	while($rowSources=$resSources->FetchRow()){
		$sourcesCount++;
		$out.='
	<tr>
		<td>Type</td>
		<td> <b>'.$rowSources['Type'].'</b></td>
		<td>Source:</td>
		<td colspan="5"> <b>'.$rowSources['RepositoryName'].'</b></td>
	</tr>
	<tr>
		<td>Name</td>
		<td> <b>'.$rowSources['Name'].'</b></td>
		<td>Repository</td>
		<td> <b>'.$rowSources['Repository'].'</b></td>
		<td>Version</td>
		<td> <b>'.$rowSources['Version'].'</b></td>
		<td>Parms</td>
		<td> <b>'.$rowSources['Parms'].'</b></td>
	</tr>';
		$selectRuns = 'SELECT Package_Source_Compat.FK_OperatingSystem,Package_Source_Compat.FK_Distro,
								Distro.Description As Distro,
								OperatingSystem.Description As OperatingSystem,
								OS_Distro.Description As OS_Distro
						FROM Package_Source_Compat
						LEFT JOIN OperatingSystem ON Package_Source_Compat.FK_OperatingSystem=OperatingSystem.PK_OperatingSystem
						LEFT JOIN Distro ON FK_Distro=PK_Distro
						LEFT JOIN OperatingSystem As OS_Distro ON OS_Distro.PK_OperatingSystem=Distro.FK_OperatingSystem
						WHERE FK_Package_Source=?';
		$resRuns = $dbADO->Execute($selectRuns,$rowSources['PK_Package_Source']);
		$runsTxt = '';
		while ($rowRuns = $resRuns->FetchRow()) {
			if ($rowRuns['FK_Distro']!='') {
				switch ($rowRuns['FK_Distro']){
					case 1:
						$runsTxt.='<img src="include/images/debian.png" title="'.$rowRuns['OS_Distro'].' / '.$rowRuns['Distro'].'" align="middle">';
					break;
					case 3:
						$runsTxt.='<img src="include/images/redhat.png" title="'.$rowRuns['OS_Distro'].' / '.$rowRuns['Distro'].'" align="middle">';
					break;
					case 3:
						$runsTxt.='<img src="include/images/mandrake.png" title="'.$rowRuns['OS_Distro'].' / '.$rowRuns['Distro'].'" align="middle">';
					break;
					case 7:
						$runsTxt.='<img src="include/images/windows.png" title="'.$rowRuns['OS_Distro'].' / '.$rowRuns['Distro'].' / '.$rowRuns['Distro'].'" align="middle">';
					break;
					default:
						$runsTxt .= ' '.$rowRuns['OS_Distro'].' / '.$rowRuns['Distro'];				
					break;
				}				
			}
			else if ($rowRuns['FK_OperatingSystem']!='') {
				if($rowRuns['OperatingSystem']==1){
					$runsTxt.='<img src="include/images/linux.png">';
				}elseif($rowRuns['OperatingSystem']==2)
					$runsTxt.='<img src="include/images/windows.png">';
				else
					$runsTxt .= ' '.$rowRuns['OperatingSystem'].'(all distros)';
			}
			else {
				$runsTxt .= ' All Operating Systems';
			}
		}
		$out.='
	<tr>
		<td colspan="8">Runs on: '.$runsTxt.'</td>
	</tr>';

		if($rowSources['FK_RepositorySource']==2 || in_array($rowSources['PK_RepositoryType'],$GLOBALS['HTTPorFTP'])){
			$queryRepositoryURLs='
			SELECT 
				RepositorySource_URL.*,
				Country.Description AS CountryName
			FROM RepositorySource_URL 
				LEFT JOIN Country ON FK_Country=PK_Country
			WHERE FK_RepositorySource =?';
			$resRepositoryURLs=$dbADO->Execute($queryRepositoryURLs,$rowSources['FK_RepositorySource']);
			while($rowRepositoryURLs=$resRepositoryURLs->FetchRow()){
				$out.='
		<tr>
			<td></td>';
			if( $rowSources['FK_RepositorySource']==2 )
			{
				$out.='<td><a href="http://debsarge.plutohome.com/download/debian/main/binary-i386/'.$rowSources['Name'].'_'.$rowSources['Version'].'_i386.deb"><B>DOWNLOAD</B></a></td>';
			}
			else
			{
				$out.='<td><a href="'.$rowRepositoryURLs['URL'].(($rowSources['Repository']!='')?$rowSources['Repository'].'/':'').$rowSources['Name'].'_'.$rowSources['Version'].$rowSources['Parms'].'"><B>DOWNLOAD</B></a></td>';
			}
			$out.='<td>Location: </td>
			<td>'.$rowRepositoryURLs['CountryName'].'</td>';
			if($rowRepositoryURLs['Username']!='' || $rowRepositoryURLs['Password']!=''){
				$out.='
				<td>Username: </td>
				<td>'.$rowRepositoryURLs['Username'].'</td>
				<td>Password: </td>
				<td>'.$rowRepositoryURLs['Password'].'</td>';
			}else{
				$out.='
				<td>&nbsp;</td>
				<td>&nbsp;</td>
				<td>&nbsp;</td>
				<td>&nbsp;</td>';
			}
		$out.='</tr>';
			}
		}
		$out.='
	<tr>
		<td colspan="8">'.(($rowSources['PathToFile']!='')?'Download link: <a href="'.$rowSources['PathToFile'].'" target="_blank">'.$rowSources['PathToFile'].'</a>':''.nl2br($rowSources['RepositoryInstructions'])).'</td>
	</tr>
	<tr>
		<td colspan="8"><hr></td>
	</tr>';
	}
	$out.='</table>';
	return $out;
}
?>