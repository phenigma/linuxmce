<?
$package=(isset($_SESSION['package']))?$_SESSION['package']:0;
	$out='<p>Here are some helpfull links :</p>
	<p><b>Documentation :</b></p>
	<table>
		<tr>
			<td><a href="index.php?section=document&docID=1">Pluto Home</a> : </td>
			<td><a href="index.php?section=document&docID=11">Installation</a> : Equipment, Software...</td>
		</tr>
		<tr>
			<td>.</td>
			<td><a href="index.php?section=document&docID=11">Pluto Admin Website</a> : Wizard Pages, Advanced Pages...</td>
		</tr>
		<tr>
			<td>.</td>
			<td><a href="index.php?section=document&docID=48">Software</a> : <a href="index.php?section=document&docID=3">Core programs</a>, <a href="index.php?section=document&docID=4">Pluto Libraries</a>, <a href="index.php?section=document&docID=5">Pluto Utilities</a>, <a href="index.php?section=document&docID=6">DCE Devices</a>, <a href="index.php?section=document&docID=7">Pluto Plug-ins</a>, </td>
		</tr>
		<tr>
			<td>.</td>
			<td><a href="index.php?section=document&docID=53">Versions</a></td>
		</tr>
		<tr>
			<td><a href="index.php?section=document&docID=2">PlutoVip</a> : </td>
			<td><a href="index.php?section=document&docID=36">User\'s manual</a></td>
		</tr>
		<tr>
			<td>.</td>
			<td><a href="index.php?section=document&docID=37">VIP Design</a></td>
		</tr>
		<tr>
			<td>.</td>
			<td><a href="index.php?section=document&docID=38">VIP Server</a></td>
		</tr>
		<tr>
			<td>.</td>
			<td><a href="index.php?section=document&docID=104">Download</a></td>
		</tr>';
	if ($package!=0) {
		$seleDocs = 'SELECT * FROM Package
			WHERE PK_Package=?';
		$resDocs = $dbADO->Execute($seleDocs,$package);
		$rowDocs = $resDocs->FetchRow(); 
	$out.='<tr>
	<td><a href="index.php?section=document&docID='.$rowDocs['FK_Document'].'"><b>**package**</b></a> : </td>
			<td><a href="index.php?section=document&docID='.$rowDocs['FK_Document_UsersManual'].'">User\'s manual</a></td>
		</tr>
		<tr>
			<td>.</td>
			<td><a href="index.php?section=document&docID='.$rowDocs['FK_Document_ProgrammersGuide'].'">Programmer\'s Guide</a></td>
		</tr>';
	}
	$out.='</table>';
	
	$out.='<br><table>
			<tr>
				<td><b>Download :</b></td>
				<td><a href="index.php?section=mainDownload&package=0">Pluto\'s Main Download Page</td>
			</tr>
			<tr>
				<td><b>.</b></td>
				<td><a href="index.php?section=document&docID=104">PlutoVip Download Page</td>
			</tr>
			<tr>
				<td><b>.</b></td>
				<td><a href="index.php?section=document&docID=106">Windows Software Installers Page</td>
			</tr>';
	if ($package != 0) {
			$out.='<tr>
				<td>.</td>
				<td><a href="index.php?section=packageDownload&pkid='.$package.'"><b>**package**</b>Download Page</td>
			</tr>';
	}
	$out.='</table>';
	
$output->setBody($out);
$output->setTitle(APPLICATION_NAME);			
$output->output();  	
?>