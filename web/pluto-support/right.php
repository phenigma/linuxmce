<?php
	session_name('Pluto');
	session_start();


	require('include/config/config.inc.php');
	require('include/utils.inc.php');
	require('include/masterusers.inc.php');
	require('include/template.class.inc.php');
	require('include/package.inc.php');
	
	$section=$_REQUEST['section'];
	$output = new Template($dbADO);
	$output->setTemplateFileType('large');
	include_once($section.'.php');
	
?>
