<?
	// pluto Asterisk
	$dbPlutoAsteriskType = "MySQL";
	$dbPlutoAsteriskUser = "plutoasterisk";
	$dbPlutoAsteriskPass = "";
	$dbPlutoAsteriskServer = "localhost";
	$dbPlutoAsteriskDatabase = "asterisk";

	
  	$asteriskDSN = $dbPlutoAsteriskType.'://'.urlencode($dbPlutoAsteriskUser).':'.urlencode($dbPlutoAsteriskPass).'@'.$dbPlutoAsteriskServer.'/'.urlencode($dbPlutoAsteriskDatabase); 
  	$asteriskADO = &ADONewConnection($asteriskDSN);
?>
