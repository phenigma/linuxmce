<?
	// pluto Asterisk
	$dbEIBType = "MySQL";
	$dbEIBUser = "eib";
	$dbEIBPass = "";
	$dbEIBServer = "localhost";
	$dbEIBDatabase = "asterisk";

	
  	$eibDSN = $dbEIBType.'://'.urlencode($dbEIBUser).':'.urlencode($dbEIBPass).'@'.$dbEIBServer.'/'.urlencode($dbEIBDatabase); 
  	$eibADO = &ADONewConnection($eibDSN);
?>
