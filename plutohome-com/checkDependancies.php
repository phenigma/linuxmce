<?
require('../include/config/config.inc.php');
require('../include/utils.inc.php');

$device=$_GET['device'];
echo 'Here are all the software required for this device:';

echo '<pre>';

passthru ('/usr/pluto/bin/ConfirmDependencies -d '.$device.' list', $var);
//passthru ('ls -al',$var);

echo $var;
echo '</pre>';
?>