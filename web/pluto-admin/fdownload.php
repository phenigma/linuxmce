<?
$Path='/usr/pluto/';

$filepath=$Path.@$_GET['filepath'];
$filename = substr($_GET['filepath'],strrpos ($_GET['filepath'],'/')+1);

if(file_exists($filepath)){
	header('Content-type: application/force-download');
	header('Content-Transfer-Encoding: Binary');
	header('Content-disposition: attachment; filename='.$filename);
	readfile($filepath);
}
?>