<?PHP
  include('/var/www/globalconfig/globalconfig.inc.php');

  $conn=mysql_connect($dbLeadsServer,$dbLeadsUser,$dbLeadsPass) or die('could not connect to database');
  $dbLeads=mysql_selectdb($dbLeadsDatabase,$conn) or die("could not select $dbLeadsDatabase");
?>
