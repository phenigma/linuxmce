#!/bin/sh

#   Written by Tzafrir Cohen <tzafrir.cohen@xorcom.com>, 
#              Diego Iastrubni <diego.iastrubni@xorcom.com>
#   Copyright (C) 2005, Xorcom
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

# a small script to generate the needed SQL tables by AMPortal
# needs to be run as root

# quoted from the daily cron of mysql:
MYSQL="mysql --defaults-extra-file=/etc/mysql/debian.cnf -f"
MYSQLADMIN="mysqladmin --defaults-extra-file=/etc/mysql/debian.cnf -f"


exec_command()
{
	CMD=$1
	
	if [ "$DEBUG" == "1" ] ; then
        	echo "$CMD"
	fi
	$CMD
}

delete_db()
{
	DB=$1

	if [ -d /var/lib/mysql/$DB ]; then
		echo -n "Dropping $DB"
		exec_command "$MYSQLADMIN DROP $DB";
		echo "."
	fi
}


delete_db "asterisk"    
delete_db "asteriskcdrdb" 

echo
echo "The freepbx databases have been purged"
echo
