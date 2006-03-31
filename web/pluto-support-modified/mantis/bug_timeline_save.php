<?php
	# Mantis - a php based bugtracking system
	# Copyright (C) 2000 - 2002  Kenzaburo Ito - kenito@300baud.org
	# Copyright (C) 2002 - 2004  Mantis Team   - mantisbt-dev@lists.sourceforge.net
	# This program is distributed under the terms and conditions of the GPL
	# See the README and LICENSE files for details

	# --------------------------------------------------------
	# $Id: bug_relationship_add.php,v 1.6 2005/07/25 16:34:10 thraxisp Exp $
	# --------------------------------------------------------

	# ======================================================================
	# Author: Marcello Scata' <marcelloscata at users.sourceforge.net> ITALY
	# ======================================================================

	require_once( 'core.php' );
	$t_core_path = config_get( 'core_path' );
	require_once( $t_core_path . 'timeline_api.php' );

	$f_bug_id = gpc_get_int( 'src_bug_id' );
	$f_date_todo = gpc_get_string( 'date_todo' );
	$f_id_after_todo = gpc_get_int( 'id_after_todo' );
	$f_hours_estimate = gpc_get_int( 'hours_estimate' );
	$f_hours_actual = gpc_get_int( 'hours_actual' );

	# user has access to update the bug...
	access_ensure_bug_level( config_get( 'update_bug_threshold' ), $f_bug_id );
	
	$update_hours_actual=0;
	# hardcoded to allow only Eugen or Aaron to modify it
	$current_user=auth_get_current_user_id();
	if($current_user==33129 || $current_user==37162 || $current_user==37294){
		$master=1;
	}

	timeline_update( $f_bug_id, $f_date_todo, $f_id_after_todo, $f_hours_estimate, $f_hours_actual,$master );


	print_header_redirect_view( $f_bug_id );
?>
