<?php
require_once('ir_codes_admin/common.php');

// accepts metabatch name
function action_truncate_batch( $metaBatchName=""){
	$repositories=array('dce', 'ir', 'local');
	$excluded_tables=array('InfraredGroup', 'InfraredGroup_Command');
	$result=array();

	foreach($repositories as $repo){
//		echo "Repository: $repo\n";
		$batchID=master_get_single_value("SELECT PK_psc_${repo}_bathdr FROM psc_${repo}_bathdr WHERE comments=\"$metaBatchName\"");
		if ($batchID==""||$batchID>0)
			continue;
		$result[]=$repo."_".$batchID;

		$tables=master_get_1d_array("SELECT Tablename FROM  psc_${repo}_batdet WHERE FK_psc_${repo}_bathdr=$batchID");
//		print_r($tables);

		foreach($tables as $table){
			if(in_array($table, $excluded_tables))
				continue;

			$table_has_PK = ( master_get_single_value("DESCRIBE $table PK_${table} ") != FALSE );

			$changes=master_get_2d_array("SELECT " . ($table_has_PK?"PK_${table}, ":"") . "psc_id, psc_toc FROM ${table}_pschist WHERE psc_batch=$batchID");

			foreach($changes as $change){
				switch ($change['psc_toc']){
				// new record
				case 0:
					 clone_record_by_psc_id("${table}_pschist", "$table", $change['psc_id'], $batchID);
					break;

				// modified record
				case 1:
					 clone_record_by_psc_id("${table}_pschist", "$table", $change['psc_id'], $batchID, true, ($table_has_PK?$change["PK_${table}"]:$change["psc_id"]), !$table_has_PK);
					break;

				// deleted record
				case 2:
					worker_sql_query("DELETE FROM $table WHERE PK_${table}=" . $change["PK_${table}"]);
					break;
				}
			}
//			print_r($changes);
		}
	}
	return $result;
}

?>