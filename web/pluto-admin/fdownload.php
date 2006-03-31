<?
$Path='/usr/pluto/';
$allowedFiles=array(
	'installers/OrbiterInstaller.msi',
	'installers/Orbiter_CeNet4_XScale.CAB',
	'installers/Orbiter_CeNet4_x86.CAB',
	'installers/ImportContacts.zip',
	'binaries/AYGSHELL.DLL',
	'binaries/PthreadsCE.dll',
	'binaries/logo.gif',
	'binaries/Orbiter.MD5',
	'binaries/OrbiterCE.exe'
);


if(in_array($_GET['filepath'],$allowedFiles)){
	$filepath=$Path.@$_GET['filepath'];
	$filename = substr($_GET['filepath'],strrpos ($_GET['filepath'],'/')+1);
	
	if(file_exists($filepath)){
		header('Content-type: application/force-download');
		header('Content-Transfer-Encoding: Binary');
		header('Content-disposition: attachment; filename='.$filename);
		readfile($filepath);
	}
}else{
	print 'Not allowed';
}
?>