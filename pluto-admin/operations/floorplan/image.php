<?
header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
header("Last-Modified: " . gmdate("D, d M Y H:i:s") . " GMT");
header("Cache-Control: no-store, no-cache, must-revalidate");
header("Cache-Control: post-check=0, pre-check=0", false);
header("Pragma: no-cache");

$imagepath=$_REQUEST['imagepath'];
$imageinfo = getimagesize( $imagepath );
if ($imageinfo[2] == 3) {
	$imagetype = "png" ;
}
else {
	header( "HTTP/1.0 404 Not Found" );
	exit ;
}

header( "Content-type: image/$imagetype" );
@readfile( $imagepath );
?>