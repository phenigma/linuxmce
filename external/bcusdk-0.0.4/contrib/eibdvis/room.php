<?php
/*
    EIBD client library examples
    Copyright (C) 2005-2008 Martin Koegler <mkoegler@auto.tuwien.ac.at>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
include('common.php');

$no = $_GET["no"];
$room = $config->rooms[$no];

if(!isset($room))
{
	html_header($config->title);
	html_end();
	die("");
}

$con = $config->connect ();

html_header($room->title." - ".$config->title, 60);

printf("<h1>%s</h1>\n", htmlentities($room->title));

foreach($room->ent as $ent)
$ent->display($con);

html_end();
?>