<?PHP


  $dbUser = 'root';
  $dbPass = '';
  $dbDatabase = 'plutovip';
  $dbServer = '10.0.0.150';
  $dbType = 'mysql';
   $conn=mysql_connect($dbServer,$dbUser,$dbPass) or die('could not connect to database');
  $db=mysql_selectdb($dbDatabase,$conn) or die("could not select $dbDatabase");
?>