#!/bin/sh

#   Written by Diego Iastrubni <diego.iastrubni@xorcom.com>
#   Copyright (C) 2006, Xorcom
#  
#   All rights reserved.
#  
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#  
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#  
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

# a small script for generating SQLite tables for freepbx

DEF_ADMIN_USER="admin"
DEF_ADMIN_PASSWD="admin"

if [ ! -f /etc/amportal.conf ]; then
	echo "amportal configuration not found, please install amportal.conf"
	echo "	cp /usr/share/freepbx-common/amportal.conf /etc/amportal.conf"
	echo "Modify it to your needs and run this script again."
	exit 1
fi

. /etc/amportal.conf

if [ "$AMPDBENGINE" != "sqlite" ]; then
	echo "freePBX is not configured to work with SQLite"
	echo "Plese edit /etc/amportal.conf to your needs and run this script again"
	exit 1
fi

if [ "x$AMPDBFILE" == "x" ]; then
	echo "No database file is configured in /etc/amportal.conf"
	echo "Plese edit /etc/amportal.conf to your needs and run this script again"
	exit 1
fi

AMPDBDIR=`dirname $AMPDBFILE`

mkdir -p $AMPDBDIR
touch $AMPDBFILE
chown asterisk.asterisk $AMPDBFILE
chmod 660 $AMPDBFILE


echo "creating asterisk DB at $AMPDBFILE"
cat /usr/share/freepbx-common/sql/newinstall.sqlite.sql | sqlite $AMPDBFILE

