<?
$package=(isset($_SESSION['package']))?$_SESSION['package']:0;
	$out='<p>The easiest way to get Pluto\'s source code is to register or login, create a new installation, and specify that you want source code.  This will install the source code automatically.</p>
		<p>You can also get all Pluto\'s source code with CVS or SVN. **instructions with . for all or **package** for this***</p>
		<p>Following are archives of the source code for this module, <b>**package**</b>.  For other modules, click <a href=index.php?section=modules>here</a>.  <a href=index.php?section=platforms>Compatible platforms & compilers</a></p>';

	$out.='<br>Source links: ';
	if ($package == 0) {
		$queryPaths = 'SELECT Path FROM Package_Directory 
				WHERE FK_Directory=3';
		$resPaths = $dbADO->Execute($queryPaths);
	}
	else {
		$queryPaths = 'SELECT Path FROM Package_Directory 
				WHERE FK_Directory=3 AND FK_Package=?';
		$resPaths = $dbADO->Execute($queryPaths,$package);
	}
	$found = 0 ;
	$out1='<center><table>';
	$out2='';
	$ind=0;
	while ($rowPaths = $resPaths->FetchRow()) {
		if ($rowPaths['Path']=='') continue;
		$found = 1;
		if ($ind%2 == 1) $col = ' bgcolor="#C0C0C0"'; else $col = ' bgcolor="lightblue"';
		$out2.='<tr'.$col.'><td><center><a href="doxygen/'.$rowPaths['Path'].'/html/index.php">'
			.$rowPaths['Path'].'</a></center></td></tr>';
		$ind++;
	}
	$out3='</table></center>';
	if ($found == 0) {
		$out.='<br><center><b>*** No source links found ***<b></center>';
	}
	else {
		$out.=$out1.$out2.$out3;
	}
	
$output->setBody($out);
$output->setTitle(APPLICATION_NAME);			
$output->output();  		
?>