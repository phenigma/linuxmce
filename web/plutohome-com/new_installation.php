<?php
require('include/config/config.inc.php');
require('include/utils.inc.php');


$dbADO->Execute('INSERT INTO Installation (Description) VALUES (?)',$_SERVER['REMOTE_ADDR']);

print $dbADO->Insert_ID();
?>
