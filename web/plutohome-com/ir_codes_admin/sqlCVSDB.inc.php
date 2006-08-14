<?
	// pluto sqlCVS
	$dbSqlCVSType = "MySQL";
	$dbSqlCVSUser = "root";
	$dbSqlCVSPass = "";
	$dbSqlCVSServer = "localhost";
	$dbSqlCVSDatabase = "pluto_server";

	// work copy of pluto sqlCVS database
	$dbSqlCVSworkType = "MySQL";
	$dbSqlCVSworkUser = "root";
	$dbSqlCVSworkPass = "";
	$dbSqlCVSworkServer = "localhost";
	$dbSqlCVSworkDatabase = "pluto_client";
	
	$sqlCVSADO = &ADONewConnection('mysql');
	$sqlCVSADO->NConnect($dbSqlCVSServer,urlencode($dbSqlCVSUser),urlencode($dbSqlCVSPass),urlencode($dbSqlCVSDatabase));
	
	$sqlCVSworkADO = &ADONewConnection('mysql');
	$sqlCVSworkADO->NConnect($dbSqlCVSworkServer,urlencode($dbSqlCVSworkUser),urlencode($dbSqlCVSworkPass),urlencode($dbSqlCVSworkDatabase));
	
	//command line
	$cmdln='/usr/pluto/bin/sqlCVS -R "3999" -H "sqlCVS.plutohome.com"';
?>
