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
}


if (!array_key_exists("HIDDEN_MENUS",$amp_conf)) 
{
        out("Adding HIDDEN_MENUS option to amportal.conf - using AMP default (read documentation for more information)");
        $amp_conf["HIDDEN_MENUS"] = "";
}

if (!array_key_exists("ZAP_FIRST_CHANNEL",$amp_conf)) {
        out("Adding ZAP_FIRST_CHANNEL  option to amportal.conf - using AMP default (read documentation for more information)");
        $amp_conf["ZAP_FIRST_CHANNEL"] = "401";
}

// write amportal.conf
write_amportal_conf( "/etc/amportal.conf", $amp_conf);

?>
