<?
$package=(isset($_SESSION['package']))?$_SESSION['package']:0;
//$package=0; // Fix this.  The query isn't working
	$out='<p>The easiest way to get Pluto\'s source code is to register or login, create a new installation, and specify that you want source code.  This will install the source code automatically.</p>
		<p>You can also get all Pluto\'s source code with CVS or SVN. <a href="/support/index.php?section=document&docID=101">details</a></p>
		<br><p><b>Doxygen class documentation</b>';

	if ($package == 0) {
		$out .= '<p>Here are all the projects for which we have generated doxygen class documentation.  Use this to view the class structure and hierarchy, and browse the member functions and variables.</p>';
	}
	else
	{
		$out .= '<p>Here are all the projects for which we have generated doxygen class documentation which are part of **package**.  Use this to view the class structure and hierarchy, and browse the member functions and variables.</p>';
	}		
	
	if ($package == 0) {
		$queryPaths = 'SELECT Path as InputPath FROM Package_Directory 
				WHERE FK_Directory=3 AND GenerateDoxygen=1 ORDER BY InputPath';
		$resPaths = $dbADO->Execute($queryPaths);
	}
	else {
		$queryPaths = '
			SELECT Path as InputPath FROM Package
			JOIN Package_Directory ON FK_Package=FK_Package_Sourcecode 
			WHERE FK_Directory=3 AND GenerateDoxygen=1 AND PK_Package=?
			ORDER BY InputPath';
		$resPaths = $dbADO->Execute($queryPaths,$package);
	}
	$found = 0 ;
	$out1='<center><table>';
	$out2='<tr>';
	$ind=0;
	
	while ($rowPaths = $resPaths->FetchRow()) {
		if ($rowPaths['InputPath']=='') continue;
		$found = 1;
		$out2 .= '<td><a href="doxygen/'.$rowPaths['InputPath'].'/html/index.php">'.$rowPaths['InputPath'].'</a></td>';
		if ($ind%4 == 3) $out2.='</tr><tr>';

//		if ($ind%2 == 1) $col = ' bgcolor="#C0C0C0"'; else $col = ' bgcolor="lightblue"';
//		$out2.='<tr'.$col.'><td><center><a href="doxygen/'.$rowPaths['InputPath'].'/html/index.php">'
//			.$rowPaths['InputPath'].'</a></center></td></tr>';
		$ind++;
	}
	$out2 .= '</td></tr>';
	$out3='</table></center>';
//	if ($found == 0) {
//		$out.='<br><center><b>*** No source links found ***<b></center>';
//	}
//	else {
		$out.=$out1.$out2.$out3;
//	}
	
$output->setBody($out);
$output->setTitle(APPLICATION_NAME);			
$output->output();  		
?>