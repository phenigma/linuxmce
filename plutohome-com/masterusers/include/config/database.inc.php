<?PHP


  $dbUser = 'plutovip';
  $dbPass = 'bogata41';
  $dbDatabase = 'MasterUsers';
  $dbServer = 'localhost';
  $dbType = 'mysql';

  $dbPlutoVipUser = 'plutovip';
  $dbPlutoVipPass = 'bogata41';
  $dbPlutoVipDatabase = 'plutovip';
  $dbPlutoVipServer = 'localhost';
  $dbPlutoVipType = 'mysql';

  $dbPlutoHomeUser = 'plutovip';
  $dbPlutoHomePass = 'bogata41';
  $dbPlutoHomeDatabase = 'pluto_website';
  $dbPlutoHomeServer = 'localhost';
  $dbPlutoHomeType = 'mysql'; 
  
  $dbphpBBUser = 'plutovip';
  $dbphpBBPass = 'bogata41';
  $dbphpBBDatabase = 'phpbb2';
  $dbphpBBServer = 'localhost';
  $dbphpBBType = 'mysql';
  
  $dbMantisUser = 'plutovip';
  $dbMantisPass = 'bogata41';
  $dbMantisDatabase = 'mantis';
  $dbMantisServer = 'localhost';
  $dbMantisType = 'mysql';
  
  $dbMainUser = 'plutovip';
  $dbMainPass = 'bogata41';
  $dbMainDatabase = 'pluto_main';
  $dbMainServer = 'localhost';
  $dbMainType = 'mysql';
/*
 include('include/adodb/adodb-errorhandler.inc.php');
 require('include/adodb/adodb.inc.php');
 include('include/adodb/tohtml.inc.php');
 
  $ADODB_COUNTRECS=true; //for speed up, set it to false, but it will return -1 for RecordCount()

  define('ADODB_ERROR_LOG_TYPE',3);
  define('ADODB_ERROR_LOG_DEST','C:/mysql_errors.log');


  $dsn = $dbType.'://'.urlencode($dbUser).':'.urlencode($dbPass).'@'.$dbServer.'/'.urlencode($dbDatabase); 
  $dbADO = &ADONewConnection($dsn);
  
  //for sqlite
  //$db = &ADONewConnection('sqlite');
  //$db->NConnect($GLOBALS['appRoot'].'sqlite.db'); # sqlite will create if does not exist

  //@var $db ADOConnection

  if (!$dbADO) {
	die("Connection failed");
  }
 
  $dbADO->SetFetchMode(ADODB_FETCH_ASSOC);
  $dbADO->debug=true;*/
  
  //-you must not modify anything bellow this line
  $conn=mysql_connect($dbServer,$dbUser,$dbPass) or die('could not connect to database');
  $db=mysql_select_db($dbDatabase,$conn) or die("could not select $dbDatabase");
  
  $connPlutoVip=mysql_connect($dbPlutoVipServer,$dbPlutoVipUser,$dbPlutoVipPass,true) or die('could not connect to database');
  $dbPlutoVip=mysql_select_db($dbPlutoVipDatabase,$connPlutoVip) or die("could not select $dbPlutoVipDatabase");

  $connPlutoHome=mysql_connect($dbPlutoHomeServer,$dbPlutoHomeUser,$dbPlutoHomePass,true) or die('could not connect to database');
  $dbPlutoHome=mysql_select_db($dbPlutoHomeDatabase,$connPlutoHome) or die("could not select $dbPlutoHomeDatabase");

  $connphpBB=mysql_connect($dbphpBBServer,$dbphpBBUser,$dbphpBBPass,true) or die('could not connect to database');
  $dbphpBB=mysql_select_db($dbphpBBDatabase,$connphpBB) or die("could not select $dbphpBBDatabase");

  $connMantis=mysql_connect($dbMantisServer,$dbMantisUser,$dbMantisPass,true) or die('could not connect to database');
  $dbMantis=mysql_select_db($dbMantisDatabase,$connMantis) or die("could not select $dbMantisDatabase");

  $connMain=mysql_connect($dbMainServer,$dbMainUser,$dbMainPass,true) or die('could not connect to database');
  $dbMain=mysql_select_db($dbMainDatabase,$connMain) or die("could not select $dbMainDatabase");

?>
