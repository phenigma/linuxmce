<?
$Path='/usr/pluto/bin/';

$filepath=$Path.@$_GET['filepath'];
if(file_exists($filepath)){
	header('Content-type: application/force-download');
	header('Content-Transfer-Encoding: Binary');
	header('Content-disposition: attachment; filename='.$_GET['filepath']);
}
?>