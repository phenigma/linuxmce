<?
	// pluto Asterisk
	$dbPlutoAsteriskType = "MySQL";
	$dbPlutoAsteriskUser = "asterisk";
	$dbPlutoAsteriskPass = "";
	$dbPlutoAsteriskServer = "localhost";
	$dbPlutoAsteriskDatabase = "asterisk";

	
  	$asteriskDSN = $dbPlutoAsteriskType.'://'.urlencode($dbPlutoAsteriskUser).':'.urlencode($dbPlutoAsteriskPass).'@'.$dbPlutoAsteriskServer.'/'.urlencode($dbPlutoAsteriskDatabase); 
  	$asteriskADO = &ADONewConnection($asteriskDSN);
?>
