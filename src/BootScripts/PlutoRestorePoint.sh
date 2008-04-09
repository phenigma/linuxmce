#!/bin/bash
# Pluto System Restore

# vars
MASTERDIR="/home/backup"
BKPDIR=$(date +%Y-%m-%d)
FULLPATH="$MASTERDIR/download/$BKPDIR"

Usage () {
	echo "$0 [command]"
	echo "Commands :"
	echo "	--backup -> create a system backup"
	echo "	--restore -> restore system from a backup file"
	echo "	--restore --skip-md5 -> system restore without md5 control file"
	exit
}


if [[ -z $1 ]]; then
	Usage
fi

# create restore point
if [[ "$1" == "--backup" ]]; then 
	BackupDescription="$2"

	#rm -rf $MASTERDIR
	if [[ ! -d $MASTERDIR ]]; then
		mkdir -p $MASTERDIR/download
	fi
	
	if [[ ! -d $BKPDIR ]]; then
		mkdir -p $MASTERDIR/download/$BKPDIR
	fi
	
	mkdir -p "$FULLPATH"
	echo "$BackupDescription" > "$FULLPATH/README"
	
	# backup mysql 
	mkdir -p $FULLPATH/mysql
	dbtables=$(mysql -u root -D pluto_main -N -e "select Tablename from psc_local_tables")
	dbtables=$(echo $dbtables psc_local_batdet psc_local_bathdr psc_local_batuser psc_local_repset psc_local_schema psc_local_tables)
	
	for table in $dbtables
	do
		/usr/bin/mysqldump -e --quote-names --allow-keywords --add-drop-table -u root pluto_main $table  > $FULLPATH/mysql/$table-$BKPDIR.sql
	done

	mkdir -p $FULLPATH/asterisk
	/usr/bin/mysqldump -e --quote-names --allow-keywords --add-drop-table -u root asterisk > $FULLPATH/asterisk/asterisk.sql
	
	# backup pluto-admin
	mkdir -p $FULLPATH/usr/pluto/orbiter
	cp -Lr /usr/pluto/orbiter/floorplans $FULLPATH/usr/pluto/orbiter
	cp -Lr /usr/pluto/orbiter/users $FULLPATH/usr/pluto/orbiter
	cp -Lr /usr/pluto/orbiter/rooms $FULLPATH/usr/pluto/orbiter
	cp -Lr /usr/pluto/orbiter/scenarios $FULLPATH/usr/pluto/orbiter
	
	# backup conf files
	mkdir -p $FULLPATH/etc
	cp -r /etc/pluto.conf $FULLPATH/etc
	
	# make archive
	cd $MASTERDIR/download	
	tar cfj backup-$BKPDIR.tar.bz2 $BKPDIR
	md5sum backup-$BKPDIR.tar.bz2 > backup-$BKPDIR.md5

	echo "Restore point created."
	rm -rf $FULLPATH
fi  

# restore system from backup
if [[ "$1" == "--restore" ]]; then
	cd $MASTERDIR/upload
	backupfile=$(ls $MASTERDIR/upload | grep tar)
	
	if [[ "$backupfile" != "" ]]; then
		. /usr/pluto/bin/Config_Ops.sh

		tar xfj "$backupfile"
		BKPDIR=$(find $MASTERDIR/upload -maxdepth 1 -mindepth 1 -type d)
		# process data from backup
		cp -L -r $BKPDIR/etc/* /etc
		cp -L -r $BKPDIR/usr/* /usr
		chown -R  www-data /usr/pluto/orbiter/floorplans/
		# restore mysql
		cd $BKPDIR/mysql
		dbtables=$(ls)
		# restore old mysql data
		for table in $dbtables
		do
			/usr/bin/mysql -u root -D pluto_main < $table
		done
		
		# restore asterisk settings
		cd $BKPDIR/asterisk
		/usr/bin/mysql -u root asterisk < asterisk.sql
		/etc/init.d/asterisk restart

		## Reinstall this packages so we'll have an updates database schema
		apt-get -y --reinstall install pluto-system-database
		apt-get -y --reinstall install pluto-media-database
		apt-get -y --reinstall install pluto-local-database
		apt-get -y --reinstall install pluto-security-database
		apt-get -y --reinstall install pluto-telecom-database

		rm -rf $MASTERDIR/upload/*
		rmdir $MASTERDIR/upload

		ConfSet "Display" "$Display"
		ConfSet "AutostartCore" "$AutostartCore"
		ConfSet "AutostartMedia" "$AutostartMedia"

		/usr/pluto/bin/Network_Setup.sh
		/usr/pluto/bin/Diskless_Setup.sh
	fi


fi

