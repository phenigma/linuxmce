<?php

function action_approve_reject_batch($action,$rep_id_lists){
	global $dbSqlCVSworkServer,$dbSqlCVSworkUser,$dbSqlCVSworkPass,$dbSqlCVSworkDatabase,$cmdln;
	$rep_id_list=explode('~',$rep_id_lists);
	foreach($rep_id_list as $rep_id){
		list($rep,$id)=explode('_',$rep_id,2);
		if(empty($dbSqlCVSworkPass))
			$pass="";
		else
			$pass=" -p \"$dbSqlCVSworkPass\"";
		$cmd=$cmdln.' -h "'.$dbSqlCVSworkServer.'" -u "'.$dbSqlCVSworkUser.$pass.'" -D "'.$dbSqlCVSworkDatabase.'" -r "'.$rep.'" -U "'.$_POST['username'].'~'.$_POST['password'].'" -b "'.(int)$id.'" '.$action;
		exec($cmd,$retArray);
		$msg=array_pop($retArray);
	}
	return $msg;
}

?>