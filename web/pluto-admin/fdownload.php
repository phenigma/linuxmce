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

$allowedDirectories=array(
	'/home/backup/download/'
);

$allowed=0;
foreach ($allowedFiles AS $file){
	if(strpos($_GET['filepath'],$file)!==false){
		$allowed=1;
	}
}



foreach ($allowedDirectories AS $directory){
	if(strpos($_GET['filepath'],$directory)!==false){
		$allowed=1;
	}
}

if($allowed==1){
	if(isset($_REQUEST['full'])){
		$filepath=@$_GET['filepath'];
		$filename = substr($_GET['filepath'],strrpos ($_GET['filepath'],'/')+1);
	}else{
		$filepath=$Path.@$_GET['filepath'];
		$filename = substr($_GET['filepath'],strrpos ($_GET['filepath'],'/')+1);
	}
	
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