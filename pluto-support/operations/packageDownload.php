<?
/* @var $dbADO ADOConnection */
$PK_Package=(isset($_GET['pkid'])&&$_GET['pkid']!='0')?$_GET['pkid']:$_SESSION['package'];

$queryPackage='SELECT * FROM Package WHERE PK_Package=?';
$resPackage=$dbADO->Execute($queryPackage,$PK_Package);
if($resPackage->RecordCount()!=0){
	$rowPackage=$resPackage->FetchRow();
	$packageName=$rowPackage['Description'];
}

$dependanciesTxt=array();
$selectDependancies='
			SELECT * FROM Package_Package 
		    	LEFT JOIN Package ON Package.PK_Package = Package_Package.FK_Package_DependsOn 
		    WHERE Package_Package.FK_Package=? ORDER BY Description ASC';
$resDependancies=$dbADO->Execute($selectDependancies,$PK_Package);
while($rowDependancies=$resDependancies->FetchRow()){
	$dependanciesTxt[]='<a href="index.php?section=packageDownload&pkid='.$rowDependancies['FK_Package_DependsOn'].'">'.$rowDependancies['Description'].'</a>';
}

$out='
<a href="javascript:history.back()">Back</a><br>
<h3><b>Download options for "'.$packageName.'"</b></h3>
<br>
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
			$runsTxt .= ' '.$rowRuns['OS_Distro'].' / '.$rowRuns['Distro'];
		}
		else if ($rowRuns['FK_OperatingSystem']!='') {
			$runsTxt .= ' '.$rowRuns['OperatingSystem'].'(all distros)';
		}
		else {
			$runsTxt .= ' All Operating Systems';
		}
	}
$out.='Runs on : '.$runsTxt.'<br><br>';	
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
$out.='<b>Download options:</b><table>';
$sourcesCount=0;
while($rowSources=$resSources->FetchRow()){
	$sourcesCount++;
	$out.='
	<tr bgcolor="'.(($sourcesCount%2==0)?'#F0F3F8':'#FFFFFF').'">
		<td>Type</td>
		<td> <b>'.$rowSources['Type'].'</b></td>
		<td>Source:</td>
		<td colspan="5"> <b>'.$rowSources['RepositoryName'].'</b></td>
	</tr>
	<tr bgcolor="'.(($sourcesCount%2==0)?'#F0F3F8':'#FFFFFF').'">
		<td>Name</td>
		<td> <b>'.$rowSources['Name'].'</b></td>
		<td>Repository</td>
		<td> <b>'.$rowSources['Repository'].'</b></td>
		<td>Version</td>
		<td> <b>'.$rowSources['Version'].'</b></td>
		<td>Parms</td>
		<td> <b>'.$rowSources['Parms'].'</b></td>
	</tr>';
	if(in_array($rowSources['PK_RepositoryType'],$GLOBALS['CVSorSVN'])){
		$queryDirectories='
			SELECT 
				Distinct Directory.Description
			FROM Package_Directory 
				INNER JOIN Directory ON FK_Directory=PK_Directory
			WHERE FK_Package=?';
		$resDirectories=$dbADO->Execute($queryDirectories,$PK_Package);
		$directoriesList=array();
		while($rowDirectories=$resDirectories->FetchRow()){
			$directoriesList[]=$rowDirectories['Description'];
		}
		$out.='
		<tr>
			<td colspan="8">Directories: '.@join(', ',$directoriesList).'</td>
		</tr>';
	}
	if(in_array($rowSources['PK_RepositoryType'],$GLOBALS['HTTPorFTP'])){
		$out.='
		<tr bgcolor="'.(($sourcesCount%2==0)?'#F0F3F8':'#FFFFFF').'">
			<td colspan="8"><B>Download links:</B></td>
		</tr>';
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
		<tr bgcolor="'.(($sourcesCount%2==0)?'#F0F3F8':'#FFFFFF').'">
			<td></td>
			<td><a href="'.$rowRepositoryURLs['URL'].(($rowSources['Repository']!='')?$rowSources['Repository'].'/':'').$rowSources['Name'].'_'.$rowSources['Version'].$rowSources['Parms'].'">'.$rowRepositoryURLs['URL'].'</a></td>
			<td>Location: </td>
			<td>'.$rowRepositoryURLs['CountryName'].'</td>
			<td>Username</td>
			<td>'.(($rowRepositoryURLs['Username']=='')?'none':$rowRepositoryURLs['Username']).'</td>
			<td>Password</td>
			<td>'.(($rowRepositoryURLs['Password']=='')?'none':$rowRepositoryURLs['Password']).'</td>
		</tr>';
		}
	}
	$out.='	
	<tr bgcolor="'.(($sourcesCount%2==0)?'#F0F3F8':'#FFFFFF').'">
		<td colspan="8">'.(($rowSources['PathToFile']!='')?'Download link: <a href="'.$rowSources['PathToFile'].'" target="_blank">'.$rowSources['PathToFile'].'</a>':''.nl2br($rowSources['RepositoryInstructions'])).'</td>
	</tr>
	<tr>
		<td colspan="8">&nbsp;</td>
	</tr>';
}
$out.='</table>';



$output->setBody($out);
$output->setTitle(APPLICATION_NAME);			
$output->output();  		
?>