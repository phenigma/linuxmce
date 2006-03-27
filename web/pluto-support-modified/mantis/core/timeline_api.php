<?php
	# Mantis - a php based bugtracking system
	# Copyright (C) 2000 - 2002  Kenzaburo Ito - kenito@300baud.org
	# Copyright (C) 2002 - 2004  Mantis Team   - mantisbt-dev@lists.sourceforge.net
	# This program is distributed under the terms and conditions of the GPL
	# See the README and LICENSE files for details

	### Timeline API ###

	$t_core_dir = dirname( __FILE__ ).DIRECTORY_SEPARATOR;

	require_once( $t_core_dir . 'collapse_api.php' );


	# --------------------
	function timeline_update( $p_bug_id, $p_date_todo, $p_id_after_todo, $p_hours_estimate, $p_hours_actual ,$update_hours_actual=0) {

		$t_bug_table = config_get( 'mantis_bug_table' );

		# Update timeline fields
		$query = "UPDATE $t_bug_table
				SET 
					date_todo='$p_date_todo',
					id_after_todo='$p_id_after_todo',
					hours_estimate='$p_hours_estimate'";
		$query.=($update_hours_actual==1)?", hours_actual='$p_hours_actual'":"";
		$query.="
				WHERE id='$p_bug_id'";
		db_query( $query );

	}



 	# --------------------
 	# print HTML relationship form
	function timeline_view_box( $p_bug_id ,$t_bug) {
?>
<br/>
<table class="width100" cellspacing="1">
<tr class="row-2" valign="top">
	<td width="15%" class="form-title" colspan="2">
		<?php
			echo 'Timeline';
		?>
	</td>
</tr>
<?php
		# bug not read-only and user authenticated
		if ( !bug_is_readonly( $p_bug_id ) ) {

			# user access level at least updater
			if ( access_has_bug_level( config_get( 'update_bug_threshold' ), $p_bug_id ) ) {
?>
<tr class="row-1">
	<td><?php echo lang_get( 'this_bug' ) ?>
		<form method="POST" action="bug_timeline_save.php">
		<input type="hidden" name="src_bug_id" value="<?php echo $p_bug_id ?>" size="4" />
		Date assigned: <input type="text" name="date_todo" value="<?=$t_bug->date_todo?>" size="17"/>
		do after task: <input type="text" name="id_after_todo" value="<?=$t_bug->id_after_todo?>" maxlength="7" size="4"/>
		hours - estimate: <input type="text" name="hours_estimate" value="<?=$t_bug->hours_estimate?>" maxlength="7" size="4"/>
		hours - actual: <input type="text" name="hours_actual" value="<?=$t_bug->hours_actual?>" maxlength="7" size="4"/> 
		<input type="submit" name="save_timeline" class="button" value="Save" size="4"/>
		</form>
	</td></tr>
<?php
			}
		}
?>
</table>


<?php
		
	}
?>
