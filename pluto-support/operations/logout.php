<?
/* @var $dbADO ADOConnection */
/* @var $rs ADORecordSet */
$out='';

$_SESSION['userIsLogged']='';
session_destroy();

$out = '<div class="text" align="center" style="height:100px;vertical-align:middle;"><h3>Thank you for your visit</h3></div>';

$output->setBody($out);
$output->setTitle(APPLICATION_NAME);
$output->output();

?>