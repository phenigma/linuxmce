<?php

session_start();

require('include/config/config.inc.php');
require('include/utils.inc.php');
require('include/masterusers.inc.php');
require('include/template.class.inc.php');
require('include/package.inc.php');

$docID=(isset($_GET['docID']))?$_GET['docID']:0;
$edit=(isset($_GET['edit']))?$_GET['edit']:0;
$package=$_SESSION['package'];

$output = new Template($dbADO);
$output->setTemplateFileType('homeWithFrames');
$output->setLeftFrameSrc('index.php?section=leftMenu&docID='.$docID.'&package='.$package.'&edit='.$edit);
$output->setRightFrameSrc('index.php?section=documentDisplay');

$output->setScriptCalendar('null');
$output->setTitle(APPLICATION_NAME);			
$output->output(); 

?>