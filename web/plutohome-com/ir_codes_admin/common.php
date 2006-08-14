<?php
define('main_path',dirname(__FILE__));
set_include_path(main_path.'/../');
require('include/config/config.inc.php');
require('include/template.class.inc.php');
require('include/masterusers.inc.php');
require('include/utils.inc.php');
require('checkLogin.php');
include('sqlCVSDB.inc.php');

function Head($Username,$error,$msg,$back='',$path='',$needLogin=true){
	$trBack=$back==''?'':'
			<tr>
				<td colspan="5"><a href="'.$back.'" class="normal_row">Back</a></td>
			</tr>';
	$trLogin=$needLogin?'
			<tr>
				<th colspan="5">Enter sqlCVS login and password</td>
			</tr>
			<tr>
				<th>Username</th>
				<td><input type="text" name="username" value=""></td>
			</tr>
			<tr>
				<th>Password</th>
				<td><input type="password" name="password" value=""></td>
			</tr>':'';
	return '<div align="center"><a href="'.$path.'ir_codes_admin.php?logout=1">Logout '.$Username.'</a></div>
	<div align="center"><h3>sqlCVS View Unaproved batches</h3></div>
	<div class="err" align="center">'.stripslashes($error).'</div>
	<div align="center" class="confirm"><B>'.$msg.'</B></div>
	<form action="'.$path.'ir_codes_admin.php" method="POST" name="webIR">
		<input type="hidden" name="action" value="">
		<input type="hidden" name="pk" value="">
		<input type="hidden" name="pkd" value="">
		<input type="hidden" name="groupIR" value="">
		<table cellpadding="3" cellspacing="0" class="normaltext" align="center">'.$trBack.$trLogin.'
			<tr>
				<td>&nbsp;</td>
			</tr>
		</table>
		<table cellpadding="3" cellspacing="0" class="normaltext" align="center">';
}

function mysql_to_unix_timestamp($mysql_timestamp){
	preg_match('/(\d{4})(\d{2})(\d{2})(\d{2})(\d{2})(\d{2})/',
	$mysql_timestamp,$pieces);
	$unix_timestamp = mktime($pieces[4], $pieces[5], $pieces[6],
	$pieces[2], $pieces[3], $pieces[1]);
	return($unix_timestamp);
}

function PrintOut($out){
/*echo "<html><title>".APPLICATION_NAME." :: IRCodes admin</title><body>
$out
</body></html>";*/
	$output = new Template();
	$output->setConn($conn);
	$output->setTemplateFileType('popup');
	$output->setTitle(APPLICATION_NAME." :: IRCodes admin");
	$output->setBody(@$out);
	$output->output();
}

function clone_record_by_psc_id( $tableNameFrom, $tableNameTo, $psc_id, $psc_batch="", $update=false, $targetPK="", $usePK_as_psc_id=false){
	// copying record
	$result = master_sql_query("SELECT * FROM  $tableNameFrom WHERE psc_id=$psc_id" . ($psc_batch==""?"":" AND psc_batch=$psc_batch"));
	if (!$result)
		return false;

	$rowInfo = $result->GetRowAssoc(2);
	if (!$rowInfo)
		return false;

	$row_fields = array();
	$row_values = array();

	foreach ($rowInfo as $k=>$v){
		// skipping PK_* and psc_*
		if (strncasecmp($k, "PK_", 3)==0 || strncasecmp($k, "psc_", 4)==0)
			continue;

		$row_fields []= $k;
		if (isset($v))
			$row_values []= "\"" . mysql_escape_string($v) . "\"";
		else
			$row_values []= "NULL";
	}

	if (sizeof($row_fields)==0)
		return false;

	if (!$update){
		$query = "INSERT INTO $tableNameTo (" . join(",", $row_fields) . ") VALUES (". join(",", $row_values) . ")";
	}else{
		$query = "UPDATE $tableNameTo SET ";
		$assignments=array();
		for ($i=0; $i<sizeof($row_fields); $i++)
			$assignments []= $row_fields[$i] . "=" .$row_values[$i];
		$query .= join(",", $assignments) . " WHERE ";
		if (!$usePK_as_psc_id){
			$query .= "PK_$tableNameTo=$targetPK";
		}else{
			$query .= "psc_id=$targetPK";
		}
	}
//	echo "Clone query: '" . $query . "'\n";
	if(!worker_sql_query($query))
		return false;

	if ($update)
		return -1;

	$row_id = worker_last_id();
	if (($row_id==0)||($row_id==FALSE))
		return false;

	return $row_id;
}

function master_sql_query($query){
	global $sqlCVSADO;
	return $sqlCVSADO->Execute($query);
}

function worker_sql_query($query){
	global $sqlCVSworkADO;
	return $sqlCVSworkADO->Execute($query);
}

function worker_last_id(){
	global $sqlCVSworkADO;
	return $sqlCVSworkADO->Insert_ID();
}

function master_get_single_value($query){
	$res = master_sql_query($query);
	if ($res){
		$row = $res->FetchRow();
		if ($row)
			return $row[0];
		else
			return FALSE;
	}else
		return FALSE;
}

function master_get_1d_array($query){
	$res = master_sql_query($query);
	if ($res){
		$result = array();
		while($row = $res->FetchRow()){
			$result [] = $row[0];
		}
		return $result;
	}else{
		return FALSE;
	}
}

function master_get_2d_array($query){
	$res = master_sql_query($query);
	if ($res){
		$result = array();
		while($row = $res->FetchRow()){
			$result [] = $row;
		}
		return $result;	
	}else{
		return FALSE;
	}
}

?>