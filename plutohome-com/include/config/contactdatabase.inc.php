<?PHP


  $dbUser = 'leads';
  $dbPass = 'iasi';
  $dbDatabase = 'leads';
  $dbServer = 'localhost';
  $dbType = 'mysql';
   $conn=mysql_connect($dbServer,$dbUser,$dbPass) or die('could not connect to database');
  $db=mysql_selectdb($dbDatabase,$conn) or die("could not select $dbDatabase");
?>
