<?
$package=(isset($_SESSION['package']))?$_SESSION['package']:0;	
$out='
<p>Pluto is proud to support the open source movement and believes it is the way to deliver a superior product.  
All Pluto\'s source code is released under either the GPL or Pluto licenses.</p>
<p>Software that may be useful apart from the Pluto system, such as sqlCVS, BD, RA, etc., is released under the GPL license.  
Also, Pluto\'s underlying protocol, DCE, and our DCE Devices are GPL.</p>
<p>Most of the core modules that are unique to Pluto and our target market are released under the Pluto license.  
The Pluto license is essentially the same as GPL when the software is 
distributed without hardware (ie a pure software distribution).  If a hard goods manufacturer wants to include Pluto embedded in their product, 
then the Pluto license cannot be used and a
separate license must be obtained from Pluto.  Pluto believes this arrangement is a mutual win-win.  By sacrificing the revenue from pure software sales, offering the software
royalty free, and opening the source, Pluto hopes to build an active community of users.  This benefits the community who can get Pluto for free, and it benefits the company
by making the product more sellable as an embedded solution to hard goods manufacturers and smart home dealers.</p>';

	$out.='<p>The license texts are available here:</p>';
	$queryLicenseTxt = 'SELECT Description,Summary,URL FROM License';
	$resLicenseTxt = $dbADO->Execute($queryLicenseTxt);
	$out.='<center><table><tr bgcolor="lightblue">
			<td><center>Desciption</center></td>
			<td><center>Summary</center></td>
			<td><center>License</center></td>
		</tr>';
	$ind = 0;
	while ($rowLicenseTxt = $resLicenseTxt->FetchRow()) {
		if ($ind%2 == 1) $col = ' bgcolor="#C0C0C0"';
			else $col = '';		
		$out.='<tr'.$col.'>';
			$out.='<td><center>'.$rowLicenseTxt['Description'].'</center></td>';
			$out.='<td><center>'.$rowLicenseTxt['Summary'].'</center></td>';
			$out.='<td><center><a href="'.$rowLicenseTxt['URL'].'">full text</a></center></td>';
		$out.='</tr>';
		$ind++;
	}
	$out.='</table></center></p><br><br>';

if ($package != 0 ) {
	//echo 'sadfasdf';
	$queryLicense = 'SELECT PK_License,URL,License.Description,Manufacturer.Description as Manufact
							FROM Package
						JOIN License on FK_License=PK_License
						JOIN Manufacturer on FK_Manufacturer=PK_Manufacturer
						WHERE PK_Package=?';
	$resLicense = $dbADO->Execute($queryLicense,$package);
	$found = 0;
	$out1='<center><table><tr bgcolor="lightblue">
			<td><center>License Description</center></td>
			<td><center>Manufacturer Description</center></td>
		</tr>';
	$out2=''; $ind=0;
	while ($rowLicense = $resLicense->FetchRow()) {
		$found = 1;
		if ($ind%2 == 1) $col = ' bgcolor="lightgray"';
			else $col = '';
		$ind++;
		$out2.='<tr'.$col.'>';
			$out2.='<td><center>'.$rowLicense['Description'].'</center></td>';
			$out2.='<td><center>'.$rowLicense['Manufact'].'</center></td>';
		$out2.='</tr>';
	}
	$out3.='</table></center>';
	if ($found == 0) {
		$out.='<center><b>**There is no license info**</b></center>';
	} else {
		$out.='<p>This module, <b>**package**</b>, is released under the license:</p>';
		$out.=$out1.$out2.$out3;
		$out.='<p>A list of all the software modules in Pluto and the licenses they are released under can be found in Pluto\'s **main support page**.</p>';
	}
}
else { // package is null
	$queryLicense0 = 'SELECT PackageType.Description as PackTypeDesc,PK_Package,
								Package.Description as PackDesc,Manufacturer.Description as ManufactDesc,
								License.Description as LicenseDesc, License.URL
							FROM Package
						INNER JOIN PackageType ON FK_PackageType=PK_PackageType
						INNER JOIN License ON FK_License=PK_License
						INNER JOIN Manufacturer ON FK_Manufacturer=PK_Manufacturer
							WHERE IsSource=0 AND FK_Manufacturer=1
							ORDER BY PK_PackageType,Package.Description';
	$resLicense0 = $dbADO->Execute($queryLicense0);
	$out1='<center><table><tr bgcolor="lightblue">
			<td><center>Package Type</center></td>
			<td><center>Package Description</center></td>
			<td><center>License</center></td>
			<td><center>Manufacturer</center></td>
		</tr>';
	$found = 0; $ind = 0; $out2 = '';
	while ($rowLicense0 = $resLicense0->FetchRow()) {
		if ($found == 0)	$lastType = $rowLicense0['PackTypeDesc'];
		$newType = $rowLicense0['PackTypeDesc'];
		if ($lastType != $newType) {
			$out2.='<tr bgcolor="lightblue"><td></td><td></td><td></td><td></td><td></td></tr>';
			$lastType = $newType;
		}
		if ($ind%2 == 1) $col = ' bgcolor="#C0C0C0"';
			else $col = '';
		$out2.='<tr'.$col.'>';
			$out2.='<td><center>'.$rowLicense0['PackTypeDesc'].'</center></td>';
			$out2.='<td><center><a href="/support/index.php?section=home&package='.$rowLicense0['PK_Package'].'">'.$rowLicense0['PackDesc'].'</a></center></td>';
			$out2.='<td><center>'.$rowLicense0['LicenseDesc'].'</center></td>';
			$out2.='<td><center>'.$rowLicense0['ManufactDesc'].'</center></td>';
		$out2.='</tr>';
		$ind++;
		$found = 1;
	}
	$out3 = '</table></center>';
	if ($found == 1) {
		$out.=$out1.$out2.$out3;
	}
	else {
		$out.='<center><b>**There is no license info**</b></center>';
	}
}
	
$output->setBody($out);
$output->setTitle(APPLICATION_NAME);			
$output->output();  		
?>