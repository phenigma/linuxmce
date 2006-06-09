<?
$package=(isset($_SESSION['package']))?$_SESSION['package']:0;
	$out='<p>Here are some helpfull links :</p>
	<p><b>Documentation :</b></p>
	<table>
		<tr>
			<td><a href="'.$wikiHost.'index.php/'.wikiLink('Pluto Home').'" target="_blank">Pluto Home</a> : </td>
			<td><a href="'.$wikiHost.'index.php/'.wikiLink('Installation').'" target="_blank">Installation</a> : Equipment, Software...</td>
		</tr>
		<tr>
			<td></td>
			<td><a href="'.$wikiHost.'index.php/'.wikiLink('Pluto Admin Website').'" target="_blank">Pluto Admin Website</a> : Wizard Pages, Advanced Pages...</td>
		</tr>
		<tr>
			<td></td>
			<td><a href="'.$wikiHost.'index.php/'.wikiLink('Software').'">Software</a> : <a href="'.$wikiHost.'index.php/'.wikiLink('Core Programs').'">Core programs</a>, <a href="'.$wikiHost.'index.php/'.wikiLink('Pluto Libraries').'">Pluto Libraries</a>, <a href="'.$wikiHost.'index.php/'.wikiLink('Pluto Utilities').'">Pluto Utilities</a>, <a href="'.$wikiHost.'index.php/'.wikiLink('Pluto DCE Devices').'">DCE Devices</a>, <a href="'.$wikiHost.'index.php/'.wikiLink('Pluto Plug-ins').'">Pluto Plug-ins</a> </td>
		</tr>
		<tr>
			<td></td>
			<td><a href="'.$wikiHost.'index.php/'.wikiLink('Versions').'">Versions</a></td>
		</tr>
		<tr>
			<td><a href="'.$wikiHost.'index.php/'.wikiLink('PlutoVIP').'">PlutoVip</a> : </td>
			<td><a href="'.$wikiHost.'index.php/'.wikiLink('User\'s manual').'">User\'s manual</a></td>
		</tr>
		<tr>
			<td></td>
			<td><a href="'.$wikiHost.'index.php/'.wikiLink('VIP Design').'">VIP Design</a></td>
		</tr>
		<tr>
			<td></td>
			<td><a href="'.$wikiHost.'index.php/'.wikiLink('VIP Server').'">VIP Server</a></td>
		</tr>
		<tr>
			<td></td>
			<td><a href="'.$wikiHost.'index.php/'.wikiLink('Download').'">Download</a></td>
		</tr>
	</table>';
	
	$out.='<br><table>
			<tr>
				<td><b>Download :</b></td>
				<td><a href="index.php?section=mainDownload&package=0">Pluto\'s Main Download Page</td>
			</tr>
			<tr>
				<td></td>
				<td><a href="'.$wikiHost.'index.php/'.wikiLink('Windows').'">Windows Software Installers Page</td>
			</tr>';
	if ($package != 0) {
			$out.='<tr>
				<td></td>
				<td><a href="index.php?section=packageDownload&pkid='.$package.'"><b>**package**</b>Download Page</td>
			</tr>';
	}
	$out.='</table>';
	
$output->setBody($out);
$output->setTitle(APPLICATION_NAME);			
$output->output();  	
?>