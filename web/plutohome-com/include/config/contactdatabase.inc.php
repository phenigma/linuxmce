<?PHP
  include('/var/www/globalconfig/globalconfig.inc.php');

  $connLeads=mysql_connect($dbLeadsServer,$dbLeadsUser,$dbLeadsPass,true) or die('could not connect to database');
  $dbLeads=mysql_selectdb($dbLeadsDatabase,$connLeads) or die("could not select $dbLeadsDatabase");
?>
