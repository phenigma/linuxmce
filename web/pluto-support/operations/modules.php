<?
	$out="<p>Here are the module's which have their own home pages, forums and bug tracking:</p>";
	$queryModules = 'Select PK_Package, Package.Description Pack, Comments,
							PackageType.Description as Type
					From Package
					left join PackageType on FK_PackageType = PK_PackageType
						where HomePage is not null and isSource=0
					order by Type';
	$resModules = $dbADO->Execute($queryModules);
	$ind = 0;
//	$out.='<center><table><tr bgcolor="lightblue">
//			<td><center>Package</center></td>
//			<td><center>    Description   </center></td>
//			<td><center>Package Type</center></td>
//		</tr>';
	while ($rowModules = $resModules->FetchRow()) {
		if ($ind%2 == 1) $col = ' bgcolor="#C0C0C0"'; else $col=' bgcolor="white"';
		// $out.='<tr'.$col.'>';
		$out .= '<p><a class="package-name" href="index.php?section=home&package='.$rowModules['PK_Package'].'">' . $rowModules['Pack'] . '</a></p>';
			// $out.='<td><center><a href="'.$rowModules['Pack'].'</a></center></td>';
		if ( trim($rowModules['Comments']) == "" ) 
		{
			$out .= '<p class="package-comments">A description is not yet available for this package.</p>';
		}
		else
		{
			$out.='<p class="package-comments">'.$rowModules['Comments'].'</p>';
		}
	
		$out .= '<p class="package-comments">This package is in the ' . $rowModules['Type'] . ' group.</p>';
	//	$out .= '<p class="package-subtitle">Package type: ' . $rowModules['Type'] . '</p>';
		//$out.='<td><center>'.$rowModules['Comments'].'</center></td>';
		//$out.='<td><center>'.$rowModules['Type'].'</center></td>';
			//
		//$out.='</tr>';
		$ind++;
	}
//	$out.='</table></center>';

$output->setBody($out);
$output->setTitle(APPLICATION_NAME);			
$output->output();  	
?>
