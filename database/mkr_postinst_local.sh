#!/bin/bash

# Don't do an import if the local database is already installed.  We'll test if the Users table exists
if ! echo 'SELECT PK_Users FROM Users limit 1;' | mysql pluto_main; then
	cd /usr/pluto/database/
	/usr/pluto/bin/sqlCVS -D pluto_main -r local import
else
	#This is a temporary patch to chagne the schema until sqlCVS can propagate schema changes
	#if the table was already modified, this won't hurt anything
	if ! $(echo 'alter table `pluto_main`.`Users` ,add column `Password_Unix` varchar (100)   NULL  after `Staff`, add column `Password_Samba` varchar (100)   NULL  after `Password_Unix`;' | mysql -f pluto_main); then
		echo 'Database already updated'
	fi
	echo 'A mysql error above this line is okay'
	# Don't want an error code
	exit 0
fi
