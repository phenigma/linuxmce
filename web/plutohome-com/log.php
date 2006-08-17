<?php
require('include/config/config.inc.php');
require('include/template.class.inc.php');
require('include/utils.inc.php');

// fixed variables
// path to save logs
$path='/home/users/vali/work/web/';

// interval between 2 submits (anti-flood)
$submitInterval=3;

$GLOBALS['db_in_debug']=1;
$ip=$_SERVER['REMOTE_ADDR']	;
$toLog=cleanString(@$_POST['reseted_codes_log']);
$now=time();


// save the log for reseted infrared codes 
if(isset($_POST['reseted_codes_log'])){
	
	$data=getFields('ResetedCodesLog','WHERE IPAddress=\''.$ip.'\'',$conn,'','id,IPAddress,UNIX_TIMESTAMP(LastSubmitted) AS LastSubmitted,Downloads');
	if(count($data)==0){
		// insert new record
		$saveLog=1;
	}else{
		$id=$data[0]['id'];
		$LastSubmitted=$data[0]['LastSubmitted'];
		
	
		if(($now-$LastSubmitted)<$submitInterval){
			$saveLog=0;
		}else{
			// update
			$saveLog=2;
		}
	}
	
	switch ($saveLog){
		case 0:
			die('Flood protection active, please wait ...');
		break;
		case 1:
			$isSaved=write_to_file($path.'reseted_codes.'.$now,$toLog);
			if($isSaved){
				db_query("INSERT INTO ResetedCodesLog (IPAddress,LastSubmitted,Downloads) VALUES ('$ip',FROM_UNIXTIME('$now'),1)",$conn);
				echo 'Saved';
			}else{
				echo 'Log not saved, check permission for '.$path;
			}
			
		break;
		case 2:
			$isSaved=write_to_file($path.'reseted_codes.'.$now,$toLog);
			if($isSaved){
				db_query("UPDATE ResetedCodesLog SET LastSubmitted=FROM_UNIXTIME('$now'),Downloads=Downloads+1 WHERE id=$id",$conn);
				echo 'Saved';
			}else{
				echo 'Log not saved, check permission for '.$path;
			}
		break;
		
	}
}else{
	die('Parameters not specified');
}


// generic function to extract rows from a table
function getFields($table,$where,$conn,$order='',$fields='*'){
	$res=db_query("SELECT $fields FROM $table $where $order",$conn);
	$data=array();
	while($row=mysql_fetch_assoc($res)){
		$data[]=$row;
	}
	return $data;
} 

function db_query($query,$conn)
{
	if(@$GLOBALS['db_in_debug']==1){
		print('<hr>'.$query.'<hr>');
	}
	if (empty($query)) {
		return FALSE;
	}
	$result=mysql_query($query,$conn);
	if(!$result)
		message_die ("Database request failed: <ul>"."<li>Error no: ".mysql_errno()."<li>Error: ".mysql_error()."<li>SQL query: ".$query."</ul>\n",$GLOBALS['action_on_error']);
	else
		return $result;
}

function write_to_file($filename,$content){
	if (!$handle = fopen($filename, 'w+')) {
		return "Cannot open file ($filename)";
	}


	if (fwrite($handle, $content) === FALSE) {
		return "Cannot write to file ($filename)";
	}

	fclose($handle);
	return true;

}
?>