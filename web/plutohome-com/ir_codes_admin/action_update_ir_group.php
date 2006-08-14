<?php
require_once('ir_codes_admin/common.php');

// accepts metabatch name and psc_id of the group to update in worker DB
function action_update_ir_group( $groupID="", $metaBatchName=""){
	// getting batchID
	$batchID=master_get_single_value("SELECT PK_psc_ir_bathdr FROM psc_ir_bathdr WHERE comments=\"$metaBatchName\"");

//	echo "IR BatchID: $batchID\n";

	// we working only with non-applied batches
	if($batchID>0||$batchID==""){
//		echo "IR Batch is not useful\n";
		return;
	}

	//updating InfraredGroup from batch
	clone_record_by_psc_id("InfraredGroup_pschist","InfraredGroup",$groupID,$batchID,true,$groupID,true);

	// updating InfraredGroup_Command from batch
	$history_details_psc_ids=master_get_1d_array("SELECT psc_id FROM InfraredGroup_Command_pschist WHERE psc_batch=$batchID");
	foreach($history_details_psc_ids as $history_psc_id){
		clone_record_by_psc_id("InfraredGroup_Command_pschist","InfraredGroup_Command",$history_psc_id,$batchID,true,$history_psc_id,true);
	}
}

?>