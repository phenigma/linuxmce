<?php
require_once('ir_codes_admin/common.php');

// accepts metabatch name and psc_id of the group to clone
function action_clone_ir_group( $groupID="", $metaBatchName=""){
	//cloning group itself
	$groupID_clone_PK_ID=clone_record_by_psc_id("InfraredGroup", "InfraredGroup", $groupID);
	$group_PK_ID=master_get_single_value("SELECT PK_InfraredGroup FROM InfraredGroup WHERE psc_id=$groupID");
	$group_Description=master_get_single_value("SELECT Description FROM InfraredGroup WHERE psc_id=$groupID");
	
	//cloning its details records
	$details_psc_ids=master_get_1d_array("SELECT psc_id FROM InfraredGroup_Command WHERE FK_InfraredGroup=$group_PK_ID");
	$details_clone_PK_ID=array();
	foreach($details_psc_ids as $psc_id){
		$clone_id=clone_record_by_psc_id("InfraredGroup_Command", "InfraredGroup_Command", $psc_id);
		if($clone_id)
			$details_clone_PK_ID[$psc_id]=$clone_id;
	}

//	print_r($details_clone_PK_ID);

	// getting batchID
	$batchID=master_get_single_value("SELECT PK_psc_ir_bathdr FROM psc_ir_bathdr WHERE comments=\"$metaBatchName\"");

//	echo "IR BatchID: $batchID\n";

	// we working only with non-applied batches
	if($batchID>0||$batchID==""){
//		echo "IR Batch is not useful\n";
		return;
	}

	//updating InfraredGroup from batch
	clone_record_by_psc_id("InfraredGroup_pschist","InfraredGroup",$groupID,$batchID,true,$groupID_clone_PK_ID);

	// updating InfraredGroup_Command from batch
	$history_details_psc_ids=master_get_1d_array("SELECT psc_id FROM InfraredGroup_Command_pschist WHERE psc_batch=$batchID");
	foreach($history_details_psc_ids as $history_psc_id){
		clone_record_by_psc_id("InfraredGroup_Command_pschist","InfraredGroup_Command",$history_psc_id,$batchID,true,$details_clone_PK_ID[$history_psc_id]);
	}

	// updating description
	worker_sql_query("UPDATE InfraredGroup SET Description=\"$group_Description (alternate #1)\"
		WHERE PK_InfraredGroup=$groupID_clone_PK_ID");

	// updating FK_*
	worker_sql_query("UPDATE InfraredGroup_Command SET FK_InfraredGroup=$groupID_clone_PK_ID
		WHERE PK_InfraredGroup_Command IN (" . join(",", $details_clone_PK_ID) . ")");
}

?>