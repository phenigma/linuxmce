<?
/* @var $dbADO ADOConnection */

$out='
<h3><b>Main download</b></h3>
<p>You can go to PlutoVip Download <b><a href="index.php?section=document&docID=104">here</a></b></p>
<p>Get to <b>Windows Software Installers</b> <a href="index.php?section=document&docID=106">here</a>.  <a href="index.php?section=document&docID=101">CVS/Sub-Version info</a></p>
Pluto comes in many modules, and most of those modules require several other pieces of software to function.  The easiest way to get Pluto is to register or login to plutohome.com, choose "My Pluto" and then "New Installation". 
An install wizard will ask a few questions and then compile a list of all the software you will need and build a custom installation script that will set everything up for you.
<br><br>If you want to download individual modules manually, this page lists all the software Pluto offers, as well as all the 3rd party software Pluto needs to run.  The software
modules, or packages, are separated by category: <a href="#binaries" target="_self">Binaries</a> (ie software), <a href="#sourcecode" target="_self">Source Code</a> and <a href="#nonexec" target="_self">non executable</a> (support files like graphics).
The packages are grouped by manufacturer and show the sources.  By sources we mean where you can get the software, and by what means.  For example, Pluto\'s Debian repository, FTP on Source Forge, or a Red Hat RPM are all considered sources.<br><br>
The dependencies are the other software that you will need to make it run.  Click on a software module for the links to all the download sources.<br><br><b>SPSC</b> stands for "separate package for source code".  That means the source
code is not included in this package, but is available in a separate package in the "Source code" section.
<br><br>
';




// packages display 

$out.='
	<script>
	function windowOpen(locationA,attributes) {
		window.open(locationA,\'\',attributes);
	}
	</script>

<a name="binaries"></a><b><h3><u>Binaries</u></h3></b><br>';
$selectBinaries='
	SELECT 
		Package.*,
		PackageType.Description AS TypeName,
		Manufacturer.Description AS ManufacturerName
	FROM Package 
		LEFT JOIN PackageType ON FK_PackageType=PK_PackageType
		LEFT JOIN Manufacturer ON FK_Manufacturer=PK_Manufacturer
	WHERE IsSource=0 AND NonExecutable=0
		ORDER BY ManufacturerName DESC, TypeName ASC, Package.Description ASC';
$resBinaries=$dbADO->Execute($selectBinaries);
$out.=formatOutput($resBinaries,$dbADO);

$out.='<a name="sourcecode"></a></a><b><h3><u>Source Code</u></h3></b>';
$selectSourceCode='
	SELECT 
		Package.*,
		PackageType.Description AS TypeName,
		Manufacturer.Description AS ManufacturerName
	FROM Package 
		LEFT JOIN PackageType ON FK_PackageType=PK_PackageType
		LEFT JOIN Manufacturer ON FK_Manufacturer=PK_Manufacturer
	WHERE IsSource=1 AND NonExecutable=0
		ORDER BY ManufacturerName DESC, TypeName ASC, Package.Description ASC';
$resSourceCode=$dbADO->Execute($selectSourceCode);
$out.=formatOutput($resSourceCode,$dbADO);

$out.='<a name="nonexec"></a></a><b><h3><u>Non Executibles</u></h3></b>';
$selectNonExec='
	SELECT 
		Package.*,
		PackageType.Description AS TypeName,
		Manufacturer.Description AS ManufacturerName
	FROM Package 
		LEFT JOIN PackageType ON FK_PackageType=PK_PackageType
		LEFT JOIN Manufacturer ON FK_Manufacturer=PK_Manufacturer
	WHERE IsSource=0 AND NonExecutable=1
		ORDER BY ManufacturerName DESC, TypeName ASC, Package.Description ASC';
$resNonExec=$dbADO->Execute($selectNonExec);
$out.=formatOutput($resNonExec,$dbADO);

$output->setBody($out);
$output->setTitle(APPLICATION_NAME);			
$output->output();  		

function formatOutput($res,$dbADO)
{
	$Manufacturer='start';
	$pkType='notype';
	$out='';
	$packageCount=0;
	while($row=$res->FetchRow()){
		$packageCount++;
		if($Manufacturer!=$row['ManufacturerName']){
			$out.=($row['ManufacturerName']!='')?'<br><span class="title1"><b>Manufacturer: '.$row['ManufacturerName'].'</b></span><br><br>':'<br><span class="title1"><b>No manufacturer listed</b></span><br><br>';
			$Manufacturer=$row['ManufacturerName'];
		}
		if($pkType!=$row['TypeName']){
			$out.=($row['TypeName']!='')?'<br><span class="title2">'.$row['TypeName'].'</span><br>':'<span class="title2">No type</span><br>';
			$pkType=$row['TypeName'];
		}
		$selectSources='
			SELECT * FROM Package_Source
				INNER JOIN RepositorySource ON FK_RepositorySource=PK_RepositorySource
			WHERE FK_Package=? ORDER BY Name ASC';
		$resSources=$dbADO->Execute($selectSources,$row['PK_Package']);
		$sourcesTxt=array();
		while($rowSources=$resSources->FetchRow()){
			$sourcesTxt[]=$rowSources['Description'];
		}
		
		$dependanciesTxt=array();
		$selectDependancies='
			SELECT * FROM Package_Package 
		    	LEFT JOIN Package ON Package.PK_Package = Package_Package.FK_Package_DependsOn 
		    WHERE Package_Package.FK_Package=? ORDER BY Description ASC';
		$resDependancies=$dbADO->Execute($selectDependancies,$row['PK_Package']);
		$selectRuns = 'select Package_Compat.FK_OperatingSystem,Package_Compat.FK_Distro,
								Distro.Description As Distro,
								OperatingSystem.Description As OperatingSystem,
								OS_Distro.Description As OS_Distro
						FROM Package_Compat
						LEFT JOIN OperatingSystem ON Package_Compat.FK_OperatingSystem=OperatingSystem.PK_OperatingSystem
						LEFT JOIN Distro ON FK_Distro=PK_Distro
						LEFT JOIN OperatingSystem As OS_Distro ON OS_Distro.PK_OperatingSystem=Distro.FK_OperatingSystem
						WHERE FK_Package=?';
		$resRuns = $dbADO->Execute($selectRuns,$row['PK_Package']);
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
		while($rowDependancies=$resDependancies->FetchRow()){
			$dependanciesTxt[]='<a href="#link_'.$rowDependancies['FK_Package_DependsOn'].'" target="_self">'.$rowDependancies['Description'].'</a>';
		}
		$out.='<table border="0" width="100%" bgcolor="'.(($packageCount%2==0)?'#F0F3F8':'#FFFFFF').'">
				<tr>
					<td valign="top" width="300"><a name="link_'.$row['PK_Package'].'"></a>&nbsp;&nbsp;&nbsp;&nbsp;<a href="index.php?section=packageDownload&pkid='.$row['PK_Package'].'"><b>'.$row['Description'].'</b></a></td>
					<td valign="top" width="200">Sources: '.join(', ',$sourcesTxt).'</td>
					<td valign="top">SPSC: '.(($row['FK_Package_Sourcecode'])?'<a href="#link_'.$row['FK_Package_Sourcecode'].'" target="_self">Yes</a>':'No').'</td>
				</tr>
				<tr>
					<td colspan="3">Dependancies: '.join(', ',$dependanciesTxt).'</td>
				</tr>
				<tr>
					<td colspan="3">Runs on : '.$runsTxt.'</td>
				</tr>
			</table>';
	}
	return $out;
}
?>