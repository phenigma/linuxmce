<?php
	include_once('/var/www/globalconfig/globalconfig.inc.php');
	# Mantis - a php based bugtracking system
	# Copyright (C) 2000 - 2002  Kenzaburo Ito - kenito@300baud.org
	# Copyright (C) 2002 - 2004  Mantis Team   - mantisbt-dev@lists.sourceforge.net
	# This program is distributed under the terms and conditions of the GPL
	# See the README and LICENSE files for details

	# --------------------------------------------------------
	# $Id: config_inc.php.sample,v 1.14 2004/09/12 12:23:35 vboctor Exp $
	# --------------------------------------------------------
	
	# This sample file contains the essential files that you MUST
	# configure to your specific settings.  You may override settings
	# from config_defaults_inc.php by assigning new values in this file

	# Rename this file to config_inc.php after configuration.

	###########################################################################
	# CONFIGURATION VARIABLES
	###########################################################################

	# In general the value OFF means the feature is disabled and ON means the
	# feature is enabled.  Any other cases will have an explanation.

	# Look in http://www.mantisbt.org/manual or config_defaults_inc.php for more
	# detailed comments.

	# --- database variables ---------

	# set these values to match your setup

	# --- email variables -------------
	$g_administrator_email  = 'webmaster@plutohome.com';
	$g_webmaster_email      = 'webmaster@plutohome.com';

	# the "From: " field in emails
	$g_from_email           = 'noreply@plutohome.com';

	# the return address for bounced mail
	$g_return_path_email    = 'admin@example.com';

	# --- file upload settings --------
	# This is the master setting to disable *all* file uploading functionality
	#
	# The default value is ON but you must make sure file uploading is enabled
	#  in PHP as well.  You may need to add "file_uploads = TRUE" to your php.ini.
	$g_allow_file_upload	= ON;
	
	# Specify where the user should be sent after logging out.
	$g_logout_redirect_page			= 'plutoLogout.php';

	$g_short_date_format    = 'Y-m-d';
	$g_normal_date_format   = 'Y-m-d H:i';
	$g_complete_date_format = 'Y-m-d H:i T';
	
	# --- enum strings ----------------
	$g_status_enum_string				= '50:open,75:reopen,80:fixed,90:closed';
	$g_severity_enum_string				= '20:trivial,40:normal,50:minor,60:major,70:crash,80:block';
	$g_resolution_enum_string 			= '5:pending,10:assigned,15:feedback,20:fixed,30:validation failed,40:unable to reproduce,60:duplicate,80:deferred,85:as designed,90:not a bug,95:on hold';


	$g_status_colors		= array( 'open'			=> '#c8c8ff', # red,
									 'fixed'		=> '#cceedd', # blue
									 're-open'		=> '#FFFFB0', # buish-green
									 'closed'		=> '#e8e8e8'); # light gray
	
?>
