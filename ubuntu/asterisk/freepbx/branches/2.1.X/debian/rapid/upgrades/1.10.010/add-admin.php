#! /usr/bin/php -q

<?php

//
// Add a default admin to the system if none found.
// The purpose of this update, is to add a default admin user to
// as existing Rapid installation of AMP. Altough AMP does give you
// the option of using the DB user/password as an extra user
// we fell that using a "system" account for such things is a huge risk
// 
//
//   Written by Diego Iastrubni <diego.iastrubni@xorcom.com>
//   Copyright (C) 2005, Xorcom
//
//   All rights reserved.
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


// this hack makes this update runable by hand as well...
// as an ugly hack to add a new admin to the system
if ($argv[1] == "--force")
{
	include('/usr/share/amportal/admin/functions.php');
	$amp_conf = parse_amportal_conf("/etc/amportal.conf");
	include('/usr/share/amportal/admin/common/db_connect.php');
}

$sql = "SELECT * FROM ampusers";
$results = $db->getAll($sql);
if(DB::IsError($results)) 
{
        die($results->getMessage());
}


if ( count($results) == 0 )
{
	$def_admin_user   = "admin";
	$def_admin_passwd = "admin";

	print "Adding default admin - user:'$def_admin_user', passowrd: '$def_admin_passwd' \n";	
	$sql = "INSERT INTO `ampusers` VALUES ('$def_admin_user', '$def_admin_passwd', '', '', '', '*')";
	$results = $db->query($sql);
	
	if(DB::IsError($results)) 
	{
    	        die($results->getMessage());
	}
}
else
{
	print "An administator is already defined in AMP, ignoring (". count($results)." admins found).\n";
}


?>
