<?PHP


  $dbUser = 'root';
  $dbPass = '';
  $dbDatabase = 'MasterUsers';
  $dbServer = '10.0.0.150';
  $dbType = 'mysql';

  $dbPlutoVipUser = 'root';
  $dbPlutoVipPass = '';
  $dbPlutoVipDatabase = 'plutovip';
  $dbPlutoVipServer = '10.0.0.150';
  $dbPlutoVipType = 'mysql';

  $dbPlutoHomeUser = 'root';
  $dbPlutoHomePass = '';
  $dbPlutoHomeDatabase = 'pluto_website';
  $dbPlutoHomeServer = '10.0.0.150';
  $dbPlutoHomeType = 'mysql'; 
  
  $dbphpBBUser = 'root';
  $dbphpBBPass = '';
  $dbphpBBDatabase = 'phpbb2';
  $dbphpBBServer = '10.0.0.150';
  $dbphpBBType = 'mysql';
  
  $dbMantisUser = 'root';
  $dbMantisPass = '';
  $dbMantisDatabase = 'mantis';
  $dbMantisServer = '10.0.0.150';
  $dbMantisType = 'mysql';
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

?>
