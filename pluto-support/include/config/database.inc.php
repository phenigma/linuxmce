<?PHP


 include($_SERVER['DOCUMENT_ROOT']."".$directory.'include/adodb/adodb-errorhandler.inc.php');
 require($_SERVER['DOCUMENT_ROOT']."".$directory.'include/adodb/adodb.inc.php');
 include($_SERVER['DOCUMENT_ROOT']."".$directory.'include/adodb/tohtml.inc.php');
 
  $ADODB_COUNTRECS=true; //for speed up, set it to false, but it will return -1 for RecordCount()
  
  $dsn = $dbPlutoMainType.'://'.urlencode($dbPlutoMainUser).':'.urlencode($dbPlutoMainPass).'@'.$dbPlutoMainServer.'/'.urlencode($dbPlutoMainDatabase); 
  $dbADO = &ADONewConnection($dsn);


  if (!$dbADO) {
	die("Connection failed");
  }
  
 
  $dbADO->SetFetchMode(ADODB_FETCH_ASSOC);
  $ADODB_FORCE_TYPE=3;
  
?>
