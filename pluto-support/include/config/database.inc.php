<?PHP


  $dbUser = 'plutovip';
  $dbPass = 'bogata41';
  $dbDatabase = 'pluto_main';
  $dbServer = 'localhost';
  $dbType = 'mysql';

 
 include($_SERVER['DOCUMENT_ROOT']."".$directory.'include/adodb/adodb-errorhandler.inc.php');
 require($_SERVER['DOCUMENT_ROOT']."".$directory.'include/adodb/adodb.inc.php');
 include($_SERVER['DOCUMENT_ROOT']."".$directory.'include/adodb/tohtml.inc.php');
 
  $ADODB_COUNTRECS=true; //for speed up, set it to false, but it will return -1 for RecordCount()
  

  //define('ADODB_ERROR_LOG_TYPE',3);
  //define('ADODB_ERROR_LOG_DEST','C:/mysql_errors.log');

  $dsn = $dbType.'://'.urlencode($dbUser).':'.urlencode($dbPass).'@'.$dbServer.'/'.urlencode($dbDatabase); 
  $dbADO = &ADONewConnection($dsn);

  //for sqlite
  //$db = &ADONewConnection('sqlite');
  //$db->NConnect($GLOBALS['appRoot'].'sqlite.db'); # sqlite will create if does not exist

  /* @var $db ADOConnection */

  if (!$dbADO) {
	die("Connection failed");
  }
  
 
  $dbADO->SetFetchMode(ADODB_FETCH_ASSOC);
  $ADODB_FORCE_TYPE=3;
  
  //$dbMUADO->debug=true;
  //-you must not modify anything bellow this line
  /*$conn=mysql_connect($dbServer,$dbUser,$dbPass) or die('could not connect to database');
  $db=mysql_selectdb($dbDatabase,$conn) or die("could not select $dbDatabase");*/   
?>
