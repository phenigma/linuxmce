<?PHP


 include(APPROOT.'include/adodb/adodb-errorhandler.inc.php');
 require(APPROOT.'include/adodb/adodb.inc.php');
 include(APPROOT.'include/adodb/tohtml.inc.php');
 
  $ADODB_COUNTRECS=true; //for speed up, set it to false, but it will return -1 for RecordCount()
  
  $dsn = $dbPlutoMainType.'://'.urlencode($dbPlutoMainUser).':'.urlencode($dbPlutoMainPass).'@'.$dbPlutoMainServer.'/'.urlencode($dbPlutoMainDatabase); 
  $dbADO = &ADONewConnection($dsn);

  $webdsn = $dbType.'://'.urlencode($dbUser).':'.urlencode($dbPass).'@'.$dbServer.'/'.urlencode($dbDatabase); 
  $webADO = &ADONewConnection($webdsn);

  if (!$dbADO) {
	die("Connection failed");
  }
  
 
  $dbADO->SetFetchMode(ADODB_FETCH_ASSOC);
  $ADODB_FORCE_TYPE=3;
  
?>
