<?PHP
	
  //-you must not modify anything bellow this line
  $conn=mysql_connect($dbMasterUserServer,$dbMasterUserUser,$dbMasterUserPass) or die('could not connect to database');
  $dbMasterUser=mysql_select_db($dbMasterUserDatabase,$conn) or die("could not select $dbMasterUserDatabase");

  $connPlutoVip=mysql_connect($dbPlutoVipServer,$dbPlutoVipUser,$dbPlutoVipPass,true) or die('could not connect to database');
  $dbPlutoVip=mysql_select_db($dbPlutoVipDatabase,$connPlutoVip) or die("could not select $dbPlutoVipDatabase");

  $connPlutoHome=mysql_connect($dbServer,$dbUser,$dbPass,true) or die('could not connect to database');
  $db=mysql_select_db($dbDatabase,$connPlutoHome) or die("could not select $dbDatabase");

  $connphpBB=mysql_connect($dbphpBBServer,$dbphpBBUser,$dbphpBBPass,true) or die('could not connect to database');
  $dbphpBB=mysql_select_db($dbphpBBDatabase,$connphpBB) or die("could not select $dbphpBBDatabase");

  $connMantis=mysql_connect($g_hostname,$g_db_username,$g_db_password,true) or die('could not connect to database');
  $dbMantis=mysql_select_db($g_database_name,$connMantis) or die("could not select $g_database_name");

  $connMain=mysql_connect($dbPlutoMainServer,$dbPlutoMainUser,$dbPlutoMainPass,true) or die('could not connect to database');
  $dbPlutoMain=mysql_select_db($dbPlutoMainDatabase,$connMain) or die("could not select $dbPlutoMainDatabase");
?>
