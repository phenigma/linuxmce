#!/bin/bash

# Don't do an import if the local database is already installed.  We'll test if the Users table exists
if ! echo 'SELECT PK_Users FROM Users limit 1;' | mysql pluto_main; then
	cd /usr/pluto/database/
	/usr/pluto/bin/sqlCVS -D pluto_main -r local import
else
	#if the table was already modified, this won't hurt anything
	echo 'alter table `pluto_main`.`Users` ,add column `Password_Unix` varchar (100)   NULL  after `Staff`, add column `Password_Samba` varchar (100)   NULL  after `Password_Unix`;' | mysql pluto_main
	# Don't want an error code
	exit 0
fi
