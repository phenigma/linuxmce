<?
$package=(isset($_SESSION['package']))?$_SESSION['package']:0;
if($package!=0){
	$out='
	<p>This is the main home page for the software module <b>**package**</b>.  The above menu has links to documentation, downloads, source code, forums, mailing lists and FAQ\'s specific to <b>**package**</b>.</p>

	<p>Many of Pluto\'s <a href=index.php?section=document&docID=7>plug-ins</a> and <a href=index.php?section=document&docID=6>DCE devices</a> have their own home pages.  
	For general help, visit the <a href=index.php?section=home&package=0>general support</a> page.</p>

	<p><b><u>**package**</b></u></p>';
	$queryDevices = 'SELECT PK_DeviceTemplate,Description FROM DeviceTemplate WHERE FK_Package=?';
	$resDevices = $dbADO->Execute($queryDevices,$package);
	$found = 0;
	$out1='This module includes the following DCE Devices: ';
	$out2 = '';
	while ($rowDevices = $resDevices->FetchRow()) {
		$found = 1;
		$out2.=' "'.$rowDevices['Description'].'" ';
	}
	if ($found == 0) {
		$out.='<center><b>**No DCE Devices**</b></center>';
	}
	else {
		$out.=$out1.'<b>'.$out2.'</b>';
	}
	$queryDocs = 'SELECT Contents FROM Package
					INNER JOIN Document ON FK_Document=PK_Document
					where PK_Package=?';
	$resDocs = $dbADO ->Execute($queryDocs,$package);
	$rowDocs = $resDocs->FetchRow();
	$out.='<br><br>Documentation:<br><br>'.$rowDocs['Contents'];
}
else {
	$out='<p>This is the main support page for Pluto.  Here you will find documentation, downloads, source code, forums, mailing lists and FAQ\'s for Pluto.</p>

	<p>Pluto is a very modular system, and every installation can have its own unique mix of modules, such as <a href=index.php?section=document&docID=7>plug-ins</a> and <a href=index.php?section=document&docID=6>DCE devices</a>.  The documentation contained here
	is of a general nature and assumes you are using Pluto\'s standard mix of devices.  If you want specific information about a particular software module, you should
	visit that module\'s own home page.  Each software module has it\'s own support page with documentation, forums, bug reports, etc.  A list of all the software modules
	and the respective home pages is <a href=index.php?section=modules>here</a>';
	$queryDocs = 'SELECT Contents FROM Document
					where PK_Document=1';
	$resDocs = $dbADO ->Execute($queryDocs,$package);
	$rowDocs = $resDocs->FetchRow();
	$out.='<br><br>Documentation:<br><br>'.$rowDocs['Contents'];
}

$output->setBody($out);
$output->setTitle(APPLICATION_NAME);			
$output->output();  		
?>